/***************************************************************************
 *   Copyright (C) 2013-2026 by Guillermo Ballester Valor                  *
 *   gbv@ogimet.com                                                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
/*!
 \file bufrdeco_offsets.c
 \brief This file has the code for binary serialization of subset bit offsets
*/
#include "bufrdeco.h"
#include <sys/stat.h>

/*!
 * \fn static uint32_t bufr_u32_from_be_bytes ( const uint8_t b[4] )
 * \brief Reconstruct a uint32_t from four bytes stored in big-endian (network) order
 * \param [in] b Array of four bytes where b[0] is the most significant byte
 * \return The resulting uint32_t value
 */
static uint32_t bufr_u32_from_be_bytes ( const uint8_t b[4] )
{
  return ( ( uint32_t ) b[0] << 24 ) | ( ( uint32_t ) b[1] << 16 ) | ( ( uint32_t ) b[2] << 8 ) | ( uint32_t ) b[3];
}

/*!
 * \fn static uint32_t bufr_u32_from_le_bytes ( const uint8_t b[4] )
 * \brief Reconstruct a uint32_t from four bytes stored in little-endian order
 * \param [in] b Array of four bytes where b[0] is the least significant byte
 * \return The resulting uint32_t value
 */
static uint32_t bufr_u32_from_le_bytes ( const uint8_t b[4] )
{
  return ( ( uint32_t ) b[3] << 24 ) | ( ( uint32_t ) b[2] << 16 ) | ( ( uint32_t ) b[1] << 8 ) | ( uint32_t ) b[0];
}

/*!
 * \fn static void bufr_u32_to_be_bytes ( uint32_t v, uint8_t b[4] )
 * \brief Decompose a uint32_t into four bytes in big-endian (network) order
 * \param [in]  v The uint32_t value to decompose
 * \param [out] b Array of four bytes where b[0] will hold the most significant byte
 */
static void bufr_u32_to_be_bytes ( uint32_t v, uint8_t b[4] )
{
  b[0] = ( uint8_t ) ( ( v >> 24 ) & 0xFFu );
  b[1] = ( uint8_t ) ( ( v >> 16 ) & 0xFFu );
  b[2] = ( uint8_t ) ( ( v >> 8 ) & 0xFFu );
  b[3] = ( uint8_t ) ( v & 0xFFu );
}

/*!
 * \fn static int bufr_get_file_size ( FILE *f, long *size )
 * \brief Get the total size in bytes of an open file without altering its current position
 * \param [in]     f    File pointer opened by caller
 * \param [out]    size Receives the file size in bytes
 * \return 0 on success, 1 on any seek or tell error
 *
 * The function saves the current file position, seeks to the end to get the size,
 * then restores the original position.
 */
static int bufr_get_file_size ( FILE *f, long *size )
{
  long p;

  // Validate input parameters
  bufrdeco_assert_with_return_val ( f != NULL && size != NULL, 1 );

  // Save current position
  p = ftell ( f );
  
  // Check for error in ftell
  bufrdeco_assert_with_return_val ( p >= 0, 1 );
  
  // Seek to end to determine file size
  bufrdeco_assert_with_return_val ( fseek ( f, 0L, SEEK_END ) == 0, 1 );
  *size = ftell ( f );
  bufrdeco_assert_with_return_val ( *size >= 0, 1 );

  // Restore original position
  bufrdeco_assert_with_return_val ( fseek ( f, p, SEEK_SET ) == 0, 1 );
  return 0;
}

/*!
 * \fn static size_t bufr_subset_offset_bits_size_from_nr ( uint32_t nr )
 * \brief Calculate the size in bytes required to store a given number of subset offset bits
 * \param [in] nr Number of subset offset bits
 * \return Size in bytes required to store the subset offset bits
 */
static size_t bufr_subset_offset_bits_size_from_nr ( uint32_t nr )
{
  return 4u + 4u * ( size_t ) nr;
}

/*!
 * \fn static int bufr_subset_offset_bits_get_format ( const uint8_t *buffer, size_t buffer_size, uint32_t *nr, int *format_be )
 * \brief Determine the format (big-endian or little-endian) of a subset offset bits binary representation and extract the number of subsets
 * \param [in]  buffer      Pointer to the binary data containing the subset offset bits
 * \param [in]  buffer_size Size of the \a buffer in bytes
 * \param [out] nr          Receives the number of subsets extracted from the buffer
 * \param [out] format_be   Receives 1 if the format is big-endian, 0 if little-endian
 * \return 0 on success, 1 if the format cannot be determined or if the data is invalid
 *
 * The function reads the first four bytes of the buffer as both big-endian and little-endian uint32_t values,
 * then uses heuristics based on valid ranges and expected sizes to determine which format is correct.
*/
static int bufr_subset_offset_bits_get_format ( const uint8_t *buffer, size_t buffer_size, uint32_t *nr, int *format_be )
{
  uint32_t nr_be;
  uint32_t nr_le;
  size_t expected_size_be;
  size_t expected_size_le;

  // Validate input parameters
  bufrdeco_assert_with_return_val ( buffer != NULL && nr != NULL && format_be != NULL, 1 );
  bufrdeco_assert_with_return_val ( buffer_size >= 4u, 1 );

  // Read the first four bytes as both big-endian and little-endian uint32_t values
  nr_be = bufr_u32_from_be_bytes ( buffer );
  // Read the same bytes as little-endian
  nr_le = bufr_u32_from_le_bytes ( buffer );
  // Calculate the expected sizes for both interpretations
  expected_size_be = bufr_subset_offset_bits_size_from_nr ( nr_be );
  expected_size_le = bufr_subset_offset_bits_size_from_nr ( nr_le );

  // Heuristic checks to determine the correct format
  if ( nr_be <= BUFR_MAX_SUBSETS && nr_le > BUFR_MAX_SUBSETS )
    {
      *format_be = 1; // big-endian format is valid, little-endian is not
      *nr = nr_be;
      return 0;
    }

  // If both are valid, use the expected size to determine the format
  if ( nr_le <= BUFR_MAX_SUBSETS && nr_be > BUFR_MAX_SUBSETS )
    {
      *format_be = 0; // little-endian format is valid, big-endian is not
      *nr = nr_le;
      return 0;
    }

  // If both are valid, check which expected size matches the buffer size
  if ( nr_be <= BUFR_MAX_SUBSETS && nr_le <= BUFR_MAX_SUBSETS )
    {
      if ( expected_size_be == buffer_size && expected_size_le != buffer_size )
        {
          *format_be = 1; // big-endian format is valid, little-endian is not
          *nr = nr_be;
          return 0;
        }

      if ( expected_size_le == buffer_size && expected_size_be != buffer_size )
        {
          *format_be = 0; // little-endian format is valid, big-endian is not
          *nr = nr_le;
          return 0;
        }

      if ( expected_size_be == buffer_size )
        {
          *format_be = 1; // big-endian format is valid, little-endian is not
          *nr = nr_be;
          return 0;
        }
    }

  return 1;
}

/*!
 * \fn int bufr_write_subset_offset_bits (FILE *f , struct bufrdeco_subset_bit_offsets *off)
 * \brief Write offset bit array for subsets in a non-compressed bufr
 * \param [in] f file pointer opened by caller
 * \param [in] off pointer to the struct \ref bufrdeco_subset_bit_offsets with the data to write into file
 * \return if success return 0, otherwise 1
 */
int bufr_write_subset_offset_bits ( FILE *f, const struct bufrdeco_subset_bit_offsets *off )
{
  size_t wrote;

  bufrdeco_assert (off != NULL && f != NULL );

  wrote = fwrite ( & ( off->nr ), sizeof ( buf_t ), 1, f );
  bufrdeco_assert_with_return_val ( wrote == 1, 1 );

  wrote = fwrite ( & ( off->ofs[0] ), sizeof ( buf_t ), off->nr, f );
  bufrdeco_assert_with_return_val ( wrote == off->nr, 1 );

  return 0;
}

/*!
 * \fn int bufr_read_subset_offset_bits (FILE *f , struct bufrdeco_subset_bit_offsets *off)
 * \brief Write offset bit array for subsets in a non-compressed bufr
 * \param [in] f file pointer opened by caller
 * \param [out] off pointer to the struct \ref bufrdeco_subset_bit_offsets with the data to write into file
 * \return if success return 0, otherwise 1
 */
int bufr_read_subset_offset_bits ( FILE *f, struct bufrdeco_subset_bit_offsets *off )
{
  size_t readed;

  bufrdeco_assert (off != NULL && f != NULL );

  readed = fread ( & ( off->nr ), sizeof ( buf_t ), 1, f );
  bufrdeco_assert_with_return_val ( readed == 1, 1 );

  readed = fread ( & ( off->ofs[0] ), sizeof ( buf_t ), off->nr, f );
  bufrdeco_assert_with_return_val ( readed == off->nr, 1 );

  return 0;
}

/*!
 * \fn size_t bufr_subset_offset_bits_binary_size(const struct bufrdeco_subset_bit_offsets *off)
 * \brief Return the number of bytes needed to store a \ref bufrdeco_subset_bit_offsets in binary form
 * \param [in] off Pointer to the struct \ref bufrdeco_subset_bit_offsets to size
 * \return Number of bytes required, or 0 if \a off is NULL or \a off->nr exceeds \ref BUFR_MAX_SUBSETS
 *
 * The returned size is valid for both the legacy native-endian format and the portable big-endian format,
 * because both store one uint32_t count followed by \a nr uint32_t offsets.
 */
size_t bufr_subset_offset_bits_binary_size ( const struct bufrdeco_subset_bit_offsets *off )
{
  // Validate input parameters
  if ( off == NULL || off->nr > BUFR_MAX_SUBSETS )
    return 0u;

  // The size is 4 bytes for the count plus 4 bytes for each offset
  return bufr_subset_offset_bits_size_from_nr ( off->nr );
}

/*!
 * \fn int bufr_write_subset_offset_bits_to_buffer(uint8_t *buffer, size_t buffer_size, size_t *used_size,
 *        const struct bufrdeco_subset_bit_offsets *off)
 * \brief Write a \ref bufrdeco_subset_bit_offsets to a caller-provided binary buffer in legacy native-endian format
 * \param [out] buffer     Target buffer where the binary representation will be stored
 * \param [in]  buffer_size Available size of \a buffer in bytes
 * \param [out] used_size  Receives the number of bytes written; may be NULL
 * \param [in]  off        Pointer to the struct \ref bufrdeco_subset_bit_offsets with the data to write
 * \return 0 on success, 1 if arguments are invalid or the buffer is too small
 *
 * This function preserves the historical layout used by \ref bufr_write_subset_offset_bits, so the resulting
 * bytes depend on the host byte order. It is intended for compatibility with existing persisted data.
 */
int bufr_write_subset_offset_bits_to_buffer ( uint8_t *buffer, size_t buffer_size, size_t *used_size,
    const struct bufrdeco_subset_bit_offsets *off )
{

  // Validate input parameters
  bufrdeco_assert_with_return_val ( buffer != NULL && off != NULL, 1 );
  bufrdeco_assert_with_return_val ( off->nr <= BUFR_MAX_SUBSETS, 1 );

  // Calculate the required size and check if the buffer is large enough
  size_t need = bufr_subset_offset_bits_binary_size ( off );
  bufrdeco_assert_with_return_val ( need > 0 && buffer_size >= need, 1 );

  // Copy the count and offsets to the buffer
  memcpy ( buffer, &( off->nr ), sizeof ( buf_t ) );
  memcpy ( buffer + sizeof ( buf_t ), &( off->ofs[0] ), ( size_t ) off->nr * sizeof ( buf_t ) );

  // Set the used size if requested
  if ( used_size != NULL )
    *used_size = need;

  return 0;
}

/*!
 * \fn int bufr_read_subset_offset_bits_from_buffer(const uint8_t *buffer, size_t buffer_size,
 *        struct bufrdeco_subset_bit_offsets *off)
 * \brief Read a \ref bufrdeco_subset_bit_offsets from a legacy native-endian binary buffer
 * \param [in]  buffer      Source buffer with the binary representation
 * \param [in]  buffer_size Size of \a buffer in bytes
 * \param [out] off         Pointer to the struct \ref bufrdeco_subset_bit_offsets where data will be stored
 * \return 0 on success, 1 if the buffer is invalid, incomplete or inconsistent with \ref BUFR_MAX_SUBSETS
 *
 * This function expects the same native-endian layout produced by \ref bufr_write_subset_offset_bits_to_buffer
 * and by the historical file writer \ref bufr_write_subset_offset_bits.
 */
int bufr_read_subset_offset_bits_from_buffer ( const uint8_t *buffer, size_t buffer_size,
    struct bufrdeco_subset_bit_offsets *off )
{
  size_t need;

  // Validate input parameters
  bufrdeco_assert_with_return_val ( buffer != NULL && off != NULL, 1 );
  bufrdeco_assert_with_return_val ( buffer_size >= sizeof ( buf_t ), 1 );

  // Copy the count and offsets from the buffer
  memcpy ( &( off->nr ), buffer, sizeof ( buf_t ) );
  bufrdeco_assert_with_return_val ( off->nr <= BUFR_MAX_SUBSETS, 1 );

  // Calculate the required size and check if the buffer contains enough data
  need = bufr_subset_offset_bits_binary_size ( off );
  bufrdeco_assert_with_return_val ( need > 0u && buffer_size >= need, 1 );

  // Copy the offsets from the buffer
  memcpy ( &( off->ofs[0] ), buffer + sizeof ( buf_t ), ( size_t ) off->nr * sizeof ( buf_t ) );
  return 0;
}

/*!
 * \fn int bufr_write_subset_offset_bits_be_to_buffer(uint8_t *buffer, size_t buffer_size, size_t *used_size,
 *        const struct bufrdeco_subset_bit_offsets *off)
 * \brief Write a \ref bufrdeco_subset_bit_offsets to a caller-provided binary buffer in portable big-endian format
 * \param [out] buffer      Target buffer where the binary representation will be stored
 * \param [in]  buffer_size Available size of \a buffer in bytes
 * \param [out] used_size   Receives the number of bytes written; may be NULL
 * \param [in]  off         Pointer to the struct \ref bufrdeco_subset_bit_offsets with the data to write
 * \return 0 on success, 1 if arguments are invalid or the buffer is too small
 *
 * The resulting buffer can be stored in a binary blob and later decoded on any architecture with
 * \ref bufr_read_subset_offset_bits_universal_from_buffer.
 */
int bufr_write_subset_offset_bits_be_to_buffer ( uint8_t *buffer, size_t buffer_size, size_t *used_size,
    const struct bufrdeco_subset_bit_offsets *off )
{
  size_t need;
  uint8_t *p;
  buf_t i;

  // Validate input parameters
  bufrdeco_assert_with_return_val ( buffer != NULL && off != NULL, 1 );
  bufrdeco_assert_with_return_val ( off->nr <= BUFR_MAX_SUBSETS, 1 );

  // Calculate the required size and check if the buffer is large enough
  need = bufr_subset_offset_bits_binary_size ( off );
  bufrdeco_assert_with_return_val ( need > 0u && buffer_size >= need, 1 );

  // Write the count and offsets to the buffer in big-endian format
  p = buffer;
  bufr_u32_to_be_bytes ( off->nr, p );
  p += 4;

  // Write each offset in big-endian format
  for ( i = 0; i < off->nr; i++ )
    {
      bufr_u32_to_be_bytes ( off->ofs[i], p );
      p += 4;
    }

  // Set the used size if requested  
  if ( used_size != NULL )
    *used_size = need;

  return 0;
}

/*!
 * \fn int bufr_read_subset_offset_bits_universal_from_buffer(const uint8_t *buffer, size_t buffer_size,
 *        struct bufrdeco_subset_bit_offsets *off)
 * \brief Read a \ref bufrdeco_subset_bit_offsets from a binary buffer, auto-detecting big-endian or legacy native layout
 * \param [in]  buffer      Source buffer with the binary representation
 * \param [in]  buffer_size Size of \a buffer in bytes
 * \param [out] off         Pointer to the struct \ref bufrdeco_subset_bit_offsets where data will be stored
 * \return 0 on success, 1 if the format cannot be determined or the buffer is inconsistent
 *
 * The detection heuristic is the same as in \ref bufr_read_subset_offset_bits_universal: the first 4 bytes are
 * interpreted as both BE and LE, constrained by \ref BUFR_MAX_SUBSETS, and ambiguous cases are resolved using the
 * exact expected total buffer size. If both interpretations remain valid, BE is preferred.
 */
int bufr_read_subset_offset_bits_universal_from_buffer ( const uint8_t *buffer, size_t buffer_size,
    struct bufrdeco_subset_bit_offsets *off )
{
  uint32_t nr;
  int format_be;
  const uint8_t *p;
  buf_t i;

  // Validate input parameters
  bufrdeco_assert_with_return_val ( buffer != NULL && off != NULL, 1 );
  bufrdeco_assert_with_return_val ( bufr_subset_offset_bits_get_format ( buffer, buffer_size, &nr, &format_be ) == 0, 1 );

  // Copy the count and offsets from the buffer
  off->nr = nr;
  p = buffer + 4;

  // Read each offset according to the detected format
  if ( format_be )
    {
      for ( i = 0; i < off->nr; i++ )
        {
          off->ofs[i] = bufr_u32_from_be_bytes ( p );
          p += 4;
        }
    }
  else
    {
      for ( i = 0; i < off->nr; i++ )
        {
          off->ofs[i] = bufr_u32_from_le_bytes ( p );
          p += 4;
        }
    }

  return 0;
}

/*!
 * \fn int bufr_write_subset_offset_bits_be (FILE *f , const struct bufrdeco_subset_bit_offsets *off)
 * \brief Write a struct \ref bufrdeco_subset_bit_offsets to a file in portable big-endian format
 * \param [in] f   File pointer opened for binary writing by caller
 * \param [in] off Pointer to the struct \ref bufrdeco_subset_bit_offsets with the data to write
 * \return 0 on success, 1 on any write error or if off->nr exceeds \ref BUFR_MAX_SUBSETS
 *
 * The format written is: one uint32_t (BE) with the count \a nr, followed by \a nr uint32_t (BE)
 * values with the individual bit offsets.  This format is independent of the host byte order
 * and can be read back on any architecture with \ref bufr_read_subset_offset_bits_universal.
 */
int bufr_write_subset_offset_bits_be ( FILE *f, const struct bufrdeco_subset_bit_offsets *off )
{
  uint8_t buffer[ 4 * ( 1 + BUFR_MAX_SUBSETS )];
  size_t used_size;

  // Validate input parameters
  bufrdeco_assert (off != NULL && f != NULL );

  // Write the data to the buffer in big-endian format
  bufrdeco_assert_with_return_val ( bufr_write_subset_offset_bits_be_to_buffer ( buffer, sizeof ( buffer ), &used_size, off ) == 0, 1 );

  // Write the buffer to the file
  bufrdeco_assert_with_return_val ( fwrite ( buffer, 1, used_size, f ) == used_size, 1 );

  return 0;
}

/*!
 * \fn int bufr_read_subset_offset_bits_universal (FILE *f , struct bufrdeco_subset_bit_offsets *off)
 * \brief Read a struct \ref bufrdeco_subset_bit_offsets from a file, auto-detecting the byte order
 * \param [in]  f   File pointer opened for binary reading by caller
 * \param [out] off Pointer to the struct \ref bufrdeco_subset_bit_offsets where the data will be stored
 * \return 0 on success, 1 if the format cannot be determined or a read error occurs
 *
 * The function reads the first 4 bytes and interprets them both as a big-endian (BE) and as a
 * little-endian (LE) uint32_t candidate for the count field \a nr.  The byte order is determined
 * by the following rules, applied in order:
 *
 *  -# If only one candidate is <= \ref BUFR_MAX_SUBSETS that format is used.
 *  -# If both candidates are valid, the expected file size (4 + 4*nr bytes) is compared with the
 *     actual file size to break the tie.
 *  -# If the file size is consistent with both, the portable BE format is preferred.
 *  -# If neither candidate yields a consistent file, the function returns 1 (error).
 *
 * This allows reading both legacy files written in native (LE on x86) byte order by
 * \ref bufr_write_subset_offset_bits and portable files written by \ref bufr_write_subset_offset_bits_be.
 */
int bufr_read_subset_offset_bits_universal ( FILE *f, struct bufrdeco_subset_bit_offsets *off )
{
  long file_size;

  // The maximum size needed to read the subset offset bits is when nr == BUFR_MAX_SUBSETS, which is 4 + 4*BUFR_MAX_SUBSETS bytes
  uint8_t buffer[ 4 * ( 1 + BUFR_MAX_SUBSETS )];

  // Validate input parameters
  bufrdeco_assert (off != NULL && f != NULL );

  // Read the file size
  bufrdeco_assert_with_return_val ( bufr_get_file_size ( f, &file_size ) == 0, 1 );
  // Validate that the file size is within the expected range for subset offset bits data
  bufrdeco_assert_with_return_val ( file_size >= 4 && ( size_t ) file_size <= sizeof ( buffer ), 1 );

  // Read the data from the file
  bufrdeco_assert_with_return_val ( fseek ( f, 0L, SEEK_SET ) == 0, 1 );
  bufrdeco_assert_with_return_val ( fread ( buffer, 1, ( size_t ) file_size, f ) == ( size_t ) file_size, 1 );
  // Parse the buffer to fill the struct, auto-detecting the format
  bufrdeco_assert_with_return_val ( bufr_read_subset_offset_bits_universal_from_buffer ( buffer, ( size_t ) file_size, off ) == 0, 1 );

  return 0;
}

/*!
 * \fn int bufrdeco_write_subset_offset_bits ( struct bufrdeco *b, char *filename )
 * \brief Write offset bit array for subsets in a non-compressed bufr
 * \param [in] filename complete path of output file to open
 * \param [in] b pointer to the struct \ref bufrdeco
 * \return 1 if problem, 0 otherwise
 */
int bufrdeco_write_subset_offset_bits(struct bufrdeco* b, const char* filename)
{
    FILE* f;

    // assert nice args
    bufrdeco_assert_with_return_val(b != NULL && filename != NULL, 1);

    // open the file
    f = fopen(filename, "w");
    bufrdeco_assert_with_return_val(f != NULL, 1);

    // Read the offsets
    if (bufr_write_subset_offset_bits(f, &(b->offsets))) {
        fclose(f);
        return 1;
    }
    fclose(f);
    return 0;
}

/*!
 * \fn int bufrdeco_read_subset_offset_bits ( struct bufrdeco *b, char *filename )
 * \brief Write offset bit array for subsets in a non-compressed bufr
 * \param [in] filename complete path of input file to open
 * \param [in,out] b pointer to the struct \ref bufrdeco
 * \return 1 if problem, 0 otherwise
 */
int bufrdeco_read_subset_offset_bits(struct bufrdeco* b, char* filename)
{
    struct stat st;
    FILE* f;

    // assert nice args
    bufrdeco_assert_with_return_val(b != NULL && filename != NULL, 1);

    // silently return if cannot stat the file
    if (stat(filename, &st) < 0) {
        return 1;
    }

    // open the file
    f = fopen(filename, "r");
    bufrdeco_assert_with_return_val(f != NULL, 1);

    // Read the offsets
    if (bufr_read_subset_offset_bits(f, &(b->offsets))) {
        fclose(f);
        return 1;
    }
    fclose(f);
    return 0;
}

/*!
 * \fn int bufrdeco_write_subset_offset_bits_be ( struct bufrdeco *b, const char *filename )
 * \brief Write the subset bit-offset array of a non-compressed BUFR to a file in portable big-endian format
 * \param [in] b        Pointer to a struct \ref bufrdeco whose member \a offsets contains the data to write
 * \param [in] filename Complete path of the output binary file to create or overwrite
 * \return 0 on success, 1 if the file cannot be opened or a write error occurs
 *
 * This is the high-level wrapper around \ref bufr_write_subset_offset_bits_be.
 * The file produced is portable across architectures and can be read back with
 * \ref bufrdeco_read_subset_offset_bits_universal or \ref bufrdeco_read_subset_offset_bits.
 */
int bufrdeco_write_subset_offset_bits_be(struct bufrdeco* b, const char* filename)
{
    FILE* f;

    bufrdeco_assert_with_return_val(b != NULL && filename != NULL, 1);

    f = fopen(filename, "wb");
    bufrdeco_assert_with_return_val(f != NULL, 1);

    if (bufr_write_subset_offset_bits_be(f, &(b->offsets))) {
        fclose(f);
        return 1;
    }

    fclose(f);
    return 0;
}

/*!
 * \fn int bufrdeco_read_subset_offset_bits_universal ( struct bufrdeco *b, const char *filename )
 * \brief Read the subset bit-offset array from a file, auto-detecting whether it was written in
 *        big-endian (portable) or native little-endian (legacy) format
 * \param [in,out] b        Pointer to the struct \ref bufrdeco where the offsets will be stored
 * \param [in]     filename Complete path of the binary file to read
 * \return 0 on success, 1 if the file does not exist, cannot be opened, the format is unrecognisable,
 *         or a read error occurs
 *
 * This is the high-level wrapper around \ref bufr_read_subset_offset_bits_universal.
 * It transparently handles files created by both the legacy \ref bufrdeco_write_subset_offset_bits
 * (native endian) and the portable \ref bufrdeco_write_subset_offset_bits_be (big-endian).
 * The detection heuristic uses \ref BUFR_MAX_SUBSETS as the upper bound for the count field
 * and compares the expected file size against the actual file size to resolve ambiguous cases.
 */
int bufrdeco_read_subset_offset_bits_universal(struct bufrdeco* b, const char* filename)
{
    struct stat st;
    FILE* f;

    bufrdeco_assert_with_return_val(b != NULL && filename != NULL, 1);

    if (stat(filename, &st) < 0) {
        return 1;
    }

    f = fopen(filename, "rb");
    bufrdeco_assert_with_return_val(f != NULL, 1);

    if (bufr_read_subset_offset_bits_universal(f, &(b->offsets))) {
        fclose(f);
        return 1;
    }

    fclose(f);
    return 0;
}
