/***************************************************************************
 *   Copyright (C) 2013-2022 by Guillermo Ballester Valor                  *
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
 \file bufrdeco_read.c
 \brief This file has the code to read bufr files
*/
#include "bufrdeco.h"


/*!
  \fn int bufrdeco_read_bufr ( struct bufrdeco *b,  char *filename )
  \brief Read bufr file and does preliminary and first decode pass
  \param b pointer to struct \ref bufrdeco
  \param filename complete path of BUFR file


  This function does the folowing tasks:
  - Read the file and checks the marks at the begining and end to see wheter is a BUFR file
  - Init the structs and allocate the needed memory if not done previously
  - Splits and parse the BUFR sections (without expanding descriptors nor parsing data)
  - Reads the needed Table files and store them in memory.

  Returns 0 if all is OK, 1 otherwise
 */
int bufrdeco_read_bufr ( struct bufrdeco *b,  char *filename )
{
  int aux, res;
  uint8_t *bufrx = NULL; /*!< pointer to a memory buffer where we write raw bufr file */
  buf_t n = 0;
  FILE *fp;
  struct stat st;

  bufrdeco_assert ( b != NULL );

  /* Stat input file */
  if ( stat ( filename, &st ) < 0 )
    {
      snprintf ( b->error, sizeof ( b->error ), "%s(): cannot stat file '%s'\n", __func__, filename );
      return 1;
    }

  /* Alloc nedeed memory for bufr */
  if ( S_ISREG ( st.st_mode ) || S_ISLNK ( st.st_mode ) )
    {
      if ( ( bufrx = ( uint8_t * ) calloc ( 1, st.st_size + 4 ) ) == NULL )
        {
          snprintf ( b->error, sizeof ( b->error ), "%s(): cannot alloc memory for file '%s'\n", __func__, filename );
          return 1;
        }
    }
  else
    {
      snprintf ( b->error, sizeof ( b->error ), "%s(): '%s' is not a regular file nor symbolic link\n", __func__, filename );
      return 1;
    }

  /* Inits bufr struct */
  if ( ( st.st_size + 4 ) >= BUFR_LEN )
    {
      snprintf ( b->error, sizeof ( b->error ), "%s(): File '%s' too large. Consider increase BUFR_LEN\n", __func__, filename );
      free ( ( void * ) bufrx );
      return 1;
    }

  // Open and read the file
  if ( ( fp = fopen ( filename, "r" ) ) == NULL )
    {
      snprintf ( b->error, sizeof ( b->error ), "%s(): cannot open file '%s'\n", __func__, filename );
      free ( ( void * ) bufrx );
      return 1;
    }

  while ( ( aux = fgetc ( fp ) ) != EOF && ( int ) n < st.st_size )
    bufrx[n++] = ( uint8_t ) aux;

  //n = fread(&bufrx[0], 1, st.st_size, fp);
  //bufrdeco_assert(n == (size_t)st.st_size);

  // close the file

  fclose ( fp );

  res = bufrdeco_read_buffer ( b, bufrx, n );
  free ( bufrx );
  return res;
}


/*!
  \fn int bufrdeco_extract_bufr ( struct bufrdeco *b,  char *filename )
  \brief Read file and try to find a bufr report inserted in. Once found do the same that bufrdeco_read_file()
  \param b pointer to struct \ref bufrdeco
  \param filename complete path of BUFR file


  This function does the folowing tasks:
  - Try to find first buffer of bytes begining with 'BUFR' chars and ending with '7777'. This will be considered as a bufr file.
  - Read the file and checks the marks at the begining and end to see wheter is a BUFR file
  - Init the structs and allocate the needed memory if not done previously
  - Splits and parse the BUFR sections (without expanding descriptors nor parsing data)
  - Reads the needed Table files and store them in memory.

  Returns 0 if all is OK, 1 otherwise
 */
int bufrdeco_extract_bufr ( struct bufrdeco *b,  char *filename )
{
  int aux, res;
  uint8_t *bufrx = NULL, *pbeg = NULL; /*!< pointer to a memory buffer where we write raw bufr file */
  buf_t n = 0, beg, end, size = 0;
  FILE *fp;
  struct stat st;

  bufrdeco_assert ( b != NULL );

  /* Stat input file */
  if ( stat ( filename, &st ) < 0 )
    {
      snprintf ( b->error, sizeof ( b->error ), "%s(): cannot stat file '%s'\n", __func__, filename );
      return 1;
    }

  /* Alloc nedeed memory for bufr */
  if ( S_ISREG ( st.st_mode ) || S_ISLNK ( st.st_mode ) )
    {
      if ( ( bufrx = ( uint8_t * ) calloc ( 1, st.st_size + 4 ) ) == NULL )
        {
          snprintf ( b->error, sizeof ( b->error ), "%s(): cannot alloc memory for file '%s'\n", __func__, filename );
          return 1;
        }
    }
  else
    {
      snprintf ( b->error, sizeof ( b->error ), "%s(): '%s' is not a regular file nor symbolic link\n", __func__, filename );
      return 1;
    }

  /* Inits bufr struct */
  if ( ( st.st_size + 4 ) >= BUFR_LEN )
    {
      snprintf ( b->error, sizeof ( b->error ), "%s(): File '%s' too large. Consider increase BUFR_LEN\n", __func__, filename );
      free ( ( void * ) bufrx );
      return 1;
    }

  // Open and read the file
  if ( ( fp = fopen ( filename, "r" ) ) == NULL )
    {
      snprintf ( b->error, sizeof ( b->error ), "%s(): cannot open file '%s'\n", __func__, filename );
      free ( ( void * ) bufrx );
      return 1;
    }

  beg = end = st.st_size;
  while ( ( aux = fgetc ( fp ) ) != EOF && ( int ) n < st.st_size )
    {
      bufrx[n++] = ( uint8_t ) aux;

      // check if first 'BUFR' item has been found
      if ( n >= 4 &&
           bufrx[n - 4] == 'B' &&
           bufrx[n - 3] == 'U' &&
           bufrx[n - 2] == 'F' &&
           bufrx[n - 1] == 'R' )
        {
          beg = n - 4;
          pbeg = &bufrx[ n - 4];
        }

      // check if first '7777' has been found after first 'BUFR'
      if ( n > 4 &&
           beg < ( ( size_t ) st.st_size - 4 ) &&
           bufrx[n - 4] == '7' &&
           bufrx[n - 3] == '7' &&
           bufrx[n - 2] == '7' &&
           bufrx[n - 1] == '7' )
        {
          end = n - 1;
          size = end - beg + 1;
        }
    }


  // close the file
  fclose ( fp );

  if ( size < 8 )
    {
      snprintf ( b->error, sizeof ( b->error ), "%s(): not found a '7777' item after 'BUFR'\n", __func__ );
      return 1;
    }
  res = bufrdeco_read_buffer ( b, pbeg, size );
  free ( bufrx );
  return res;
}


/*!
  \fn int bufrdeco_read_buffer ( struct bufrdeco *b, uint8_t *bufrx, size_t size  )
  \brief Read a memory buffer and does preliminary and first decode pass
  \param b pointer to struct \ref bufrdeco
  \param bufrx buffer already allocated by caller
  \param size size of BUFR in buffer

  This function does the folowing tasks:
  - Splits and parse the BUFR sections (without expanding descriptors nor parsing data)
  - Reads the needed Table files and store them in memory.

  Returns 0 if all is OK, 1 otherwise
 */
int bufrdeco_read_buffer ( struct bufrdeco *b,  uint8_t *bufrx, buf_t size )
{
  uint8_t *c;
  buf_t ix, ud;

  bufrdeco_assert ( b != NULL );

  // Some fast checks
  if ( ( size + 4 ) >= BUFR_LEN )
    {
      snprintf ( b->error, sizeof ( b->error ), "%s(): Buffer provided too large. Consider increase BUFR_LEN\n", __func__ );
      return 1;
    }

  if ( size < 8 )
    {
      snprintf ( b->error, sizeof ( b->error ), "%s(): Too few bytes for a bufr\n", __func__ );
      return 1;
    }

  // check if begins with BUFR
  if ( bufrx[0] != 'B' || bufrx[1] != 'U' || bufrx[2] != 'F' || bufrx[3] != 'R' )
    {
      snprintf ( b->error, sizeof ( b->error ), "%s(): bufr file does not begin with 'BUFR' chars\n", __func__ );
      return 1;
    }

  // check if end with '7777'
  if ( bufrx[size - 4] != '7' || bufrx[size - 3] != '7' || bufrx[size - 2] != '7' || bufrx[size - 1] != '7' )
    {
      snprintf ( b->error, sizeof ( b->error ), "%s(): bufe file does not end with '7777' chars\n", __func__ );
      return 1;
    }

  /******************* section 0  *****************************/

  // we have already checked about begin and end

  // length of whole message
  b->sec0.bufr_length = three_bytes_to_uint32 ( &bufrx[4] );
  // check if length is correct
  if ( b->sec0.bufr_length != ( uint32_t ) size )
    {
      snprintf ( b->error, sizeof ( b->error ), "%s(): bufr file have %u bytes and it says %u\n", __func__,
                 ( uint32_t ) size, b->sec0.bufr_length );
      return 1;
    }

  // set bufr edition number
  b->sec0.edition = bufrx[7];

  if ( b->sec0.edition < 3 )
    {
      snprintf ( b->error, sizeof ( b->error ), "%s(): Bufr edition must be 3 or superior and this file is coded with version %u\n", __func__, b->sec0.edition );
      return 1;
    }

  // raw
  memcpy ( &b->sec0.raw[0], &bufrx[0], 8 );

  /******************* section 1 *****************************/
  c = &bufrx[8]; // pointer to begin of sec1
  switch ( b->sec0.edition )
    {
    case 3:
      b->sec1.length = three_bytes_to_uint32 ( c );
      b->sec1.master = c[3];
      b->sec1.subcentre = c[4];
      b->sec1.centre = c[5];
      b->sec1.update = c[6];
      b->sec1.options = c[7];
      b->sec1.category = c[8];
      b->sec1.subcategory = c[9];
      b->sec1.subcategory_local = 0; // not in version 3
      b->sec1.master_version = c[10];
      b->sec1.master_local = c[11];
      if ( c[12] > 80 )
        b->sec1.year = 1900 + c[12];
      else
        b->sec1.year = 2000 + c[12];
      b->sec1.month = c[13];
      b->sec1.day = c[14];
      b->sec1.hour = c[15];
      b->sec1.minute = c[16];
      b->sec1.second = 0; // not in version 3
      break;
    case 4:
      b->sec1.length = three_bytes_to_uint32 ( c );
      b->sec1.master = c[3];
      b->sec1.centre = two_bytes_to_uint32 ( &c[4] );
      b->sec1.subcentre = two_bytes_to_uint32 ( &c[6] );
      b->sec1.update = c[8];
      b->sec1.options = c[9];
      b->sec1.category = c[10];
      b->sec1.subcategory = c[11];
      b->sec1.subcategory_local = c[12];
      b->sec1.master_version = c[13];
      b->sec1.master_local = c[14];
      b->sec1.year = two_bytes_to_uint32 ( &c[15] );
      b->sec1.month = c[17];
      b->sec1.day = c[18];
      b->sec1.hour = c[19];
      b->sec1.minute = c[20];
      b->sec1.second = c[21];
      break;
    default:
      snprintf ( b->error, sizeof ( b->error ), "%s(): This file is coded with version %u and is not supported\n", __func__, b->sec0.edition );
      free ( ( void * ) bufrx );
      return 1;
    }
  memcpy ( b->sec1.raw, c, b->sec1.length ); // raw data
  c += b->sec1.length;
  //print_sec1_info(b);

  /******************* section 2 (Optional) ******************/
  if ( b->sec1.options & 0x80 )
    {
      b->sec2.length = three_bytes_to_uint32 ( c );
      memcpy ( b->sec2.raw, c, b->sec2.length );
      c += b->sec2.length;
    }

  /******************* section 3 *****************************/
  b->sec3.length = three_bytes_to_uint32 ( c );
  b->sec3.subsets = two_bytes_to_uint32 ( &c[4] );
  if ( c[6] & 0x80 )
    b->sec3.observed = 1;
  else
    b->sec3.observed = 0;
  if ( c[6] & 0x40 )
    b->sec3.compressed = 1;
  else
    b->sec3.compressed = 0;

  // loop of unexpanded descriptors
  for ( ix = 7, ud = 0; ( ix + 1 ) < b->sec3.length && ud < BUFR_LEN_UNEXPANDED_DESCRIPTOR; ix += 2 )
    {
      two_bytes_to_descriptor ( &b->sec3.unexpanded[ud], &c[ix] );
      if ( b->sec3.unexpanded[ud].f || b->sec3.unexpanded[ud].x || b->sec3.unexpanded[ud].y )
        ud++;
    }
  b->sec3.ndesc = ud;
  memcpy ( b->sec3.raw, c, b->sec3.length );
  c += b->sec3.length;

  /******************* section 4 *****************************/
  b->sec4.length = three_bytes_to_uint32 ( c );
  // we copy 4 byte more without danger because of latest '7777' and to use fastest exctracting bits algorithm
  memcpy ( b->sec4.raw, c, b->sec4.length + 4 );

  b->sec4.bit_offset = 32; // the first bit in byte 4

#ifdef DEBUG_TIME
  bufrdeco_clock_start = clock();
#endif

  if ( bufr_read_tables ( b ) )
    {
      return 1;
    }
#ifdef DEBUG_TIME
  bufrdeco_clock_end = clock();
  print_timing ( bufrdeco_clock_start, bufrdeco_clock_end,bufr_read_tables_wmo() );
#endif

  return 0;
}
