/***************************************************************************
 *   Copyright (C) 2013-2017 by Guillermo Ballester Valor                  *
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
 \file bufrdeco_utils.c
 \brief This file has the code of useful routines for library bufrdeco
*/
#include "bufrdeco.h"

uint8_t bitf[8] = {0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01}; /*!< Mask a single bit of a byte */
uint8_t biti[8] = {0xFF,0x7f,0x3f,0x1F,0x0F,0x07,0x03,0x01}; /*!< Mask remaining bits in a byte (less significant) */
uint8_t bitk[8] = {0x80,0xc0,0xe0,0xf0,0xf8,0xfc,0xfe,0xff}; /*!< Mask first bits in a byte (most significant) */

/*!
  \fn size_t get_bits_as_char_array ( char *target, uint8_t *has_data, uint8_t *source, size_t *bit0_offset, size_t bit_length )
  \brief Get a string from an array of bits
  \param target string as result
  \param has_data Output flags to check whether is missing data. If 0 then data is missing, othewise has data
  \param source array of uint8_t elements. Most significant bit of first element is the bit offest reference
  \param bit0_offset Bit offset
  \param bit_length Lenght (in bits) for the chunck to extract

  If returns the amount of bits readed. 0 if problems. It also update bits_offset with the new bits.
*/
size_t get_bits_as_char_array2 ( char *target, uint8_t *has_data, uint8_t *source, size_t *bit0_offset, size_t bit_length )
{
  size_t i, j;
  size_t r, d, nc;
  uint8_t *c;

  if ( bit_length % 8 )
    return 0; // bit_length needs to be divisible by 8

  nc = bit_length / 8;
  *has_data = 0; // marc if no missing data is present
  for ( j = 0; j < nc ; j++ )
    {
      r = 8;
      d = 0;
      * ( target + j ) = 0;
      do
        {
          c = source + ( *bit0_offset + d ) / 8;
          i = ( *bit0_offset + d ) % 8;
          if ( *c & bitf[i] )
            * ( target + j ) += ( 1U << ( r - 1 ) );
          else
            *has_data = 1;
          d += 1;
          r -= 1;
        }
      while ( r > 0 );
      *bit0_offset += 8; // update bit0_offset
    }
  * ( target + nc ) = '\0';
  return bit_length;
}

/*!
  \fn size_t get_bits_as_char_array ( char *target, uint8_t *has_data, uint8_t *source, size_t *bit0_offset, size_t bit_length )
  \brief get a sequence of bits in data section 4 from a BUFR reports to get an array of chars
  \param target string with the resulting array
  \param has_data if 1 then has data, if 0 what we got is missing data
  \param source buffer of uint8_t which is actually the data section 4.
  \param bit0_offset bit offset of first bit of first char
  \param bit_length number of bits to extract. Obviously this will be divisible by 8

  If returns the amount of bits readed. 0 if problems. It also update bit0_offset with the new bits.
*/
size_t get_bits_as_char_array ( char *target, uint8_t *has_data, uint8_t *source, size_t *bit0_offset, size_t bit_length )
{
  size_t i, j, k;
  size_t nc;
  uint8_t *c;

  if ( bit_length % 8 )
    return 0; // bit_length needs to be divisible by 8

  //printf("bit_length=%lu\n", bit_length);

  nc = bit_length / 8;
  i = ( *bit0_offset ) % 8;
  k = 8 - i;
  *has_data = 0; // marc if no missing data is present
  for ( j = 0; j < nc ; j++ )
    {
      c = source + ( *bit0_offset )  / 8;
      * ( target + j ) = ( *c & biti[i] );
      if ( i )
        {
          * ( target + j ) <<= i;
          * ( target + j ) |= ( ( * ( c + 1 ) & bitk[i - 1] ) >> k );
        }
      if ( * ( target + j ) != -1 )
        *has_data = 1;
      //printf("%c", * ( target + j ) );
      *bit0_offset += 8; // update bit0_offset
    }
  * ( target + nc ) = '\0';
  return bit_length;
}

/*!
  \fn size_t get_bits_as_uint32_t2 ( uint32_t *target, uint8_t *has_data, uint8_t *source, size_t *bit0_offset, size_t bit_length )
  \brief Read bits from an array of uint8_t and set them as an uint32_t
  \param target uint32_t pointer where to set the result
  \param has_data Output flags to check whether is missing data. If 0 then data is missing, othewise has data
  \param source array of uint8_t elements. Most significant bit of first element is the bit offset reference
  \param bit0_offset Bit offset
  \param bit_length Lenght (in bits) for the chunck to extract

  This is a version which extract bit a bit. For more than about 8 bits should be used the algorithm in
  \ref get_bits_as_uint32_t

  If returns the amount of bits readed. 0 if problems. It also update bits_offset with the new bits.
*/
size_t get_bits_as_uint32_t2 ( uint32_t *target, uint8_t *has_data, uint8_t *source, size_t *bit0_offset, size_t bit_length )
{
  int i;
  size_t r, d;
  uint8_t *c;

  if ( bit_length > 32 || bit_length == 0 )
    return 0;

  r = bit_length;
  d = 0;
  *target = 0;
  *has_data = 0;

  do
    {
      c = source + ( *bit0_offset + d ) / 8;
      i = ( *bit0_offset + d ) % 8;
      if ( *c & bitf[i] )
        *target += ( 1U << ( r - 1 ) );
      else
        *has_data = 1;
      d += 1;
      r -= 1;
    }
  while ( r > 0 );
  *bit0_offset += bit_length; // update bit0_offset

  return bit_length;
}


/*!
  \fn size_t get_bits_as_uint32_t ( uint32_t *target, uint8_t *has_data, uint8_t *source, size_t *bit0_offset, size_t bit_length )
  \brief Read bits from an array of uint8_t and set them as an uint32_t
  \param target uint32_t pointer where to set the result
  \param has_data Output flags to check whether is missing data. If 0 then data is missing, othewise has data
  \param source array of uint8_t elements. Most significant bit of first element is the bit offset reference
  \param bit0_offset Bit offset
  \param bit_length Lenght (in bits) for the chunck to extract

  If bil_length is less than 8 then it uses version 2 \ref get_bits_as_uint32_t2

  If returns the amount of bits readed. 0 if problems. It also update bits_offset with the new bits.
*/
size_t get_bits_as_uint32_t ( uint32_t *target, uint8_t *has_data, uint8_t *source, size_t *bit0_offset, size_t bit_length )
{
  int i;
  uint8_t *c;
  uint64_t x;

  if ( bit_length > 32 || bit_length == 0 )
    return 0;

  if ( bit_length < 8 )
    return get_bits_as_uint32_t2 ( target, has_data, source, bit0_offset, bit_length );

  *target = 0;
  *has_data = 0; // marc if no missing data is present
  c = source + ( *bit0_offset ) / 8;
  i = ( *bit0_offset ) % 8;
  x = ( ( uint64_t ) ( *c & biti[i] ) << 32 ) + ( ( uint64_t ) ( * ( c + 1 ) ) << 24 ) + ( ( uint64_t ) ( * ( c + 2 ) ) << 16 ) +
      ( ( uint64_t ) ( * ( c + 3 ) ) << 8 ) + ( uint64_t ) ( * ( c + 4 ) ); // 40 - i bits
  x >>= ( 40 - i - bit_length );
  *target = ( uint32_t ) x;
  if ( ( 1UL << bit_length ) != ( x + 1UL ) )
    *has_data = 1;

  *bit0_offset += bit_length; // update bit0_offset
  return bit_length;
}


/*!
 \fn int get_table_b_reference_from_uint32_t ( int32_t *target, uint8_t bits, uint32_t source )
 \brief Get an int32_t from bits according with bufr criteria to change the reference of a descritor. Most significant bit in source is sign
 \param target int32_t as result
 \param bits number of bits to consider
 \param source uint32_T with the data to transform

 If success return 0, 1 otherwise
*/
// most significant of bits is the sign
int get_table_b_reference_from_uint32_t ( int32_t *target, uint8_t bits, uint32_t source )
{
  uint32_t mask = 1;
  if ( bits > 32 || bits == 0 )
    return 1;

  if ( bits > 1 )
    mask = ( ( uint32_t ) 1 << ( bits - 1 ) );

  if ( mask & source )
    {
      // case of negative number
      *target = - ( int32_t ) ( source - mask );
    }
  else
    *target = ( int32_t ) ( source );
  return 0;
}

/*!
  \fn uint32_t two_bytes_to_uint32(const uint8_t *source)
  \brief returns the uint32_t value from an array of two bytes, most significant first
*/
uint32_t two_bytes_to_uint32 ( const uint8_t *source )
{
  return ( ( uint32_t ) source[1] + ( uint32_t ) source[0] * 256 );
}


/*!
  \fn uint32_t three_bytes_to_uint32(const uint8_t *source)
  \brief returns the uint32_t value from an array of three bytes, most significant first
*/
uint32_t three_bytes_to_uint32 ( const uint8_t *source )
{
  return ( ( uint32_t ) source[2] + ( uint32_t ) source[1] * 256 + ( uint32_t ) source[0] * 65536 );
}

/*!
  \fn int uint32_t_to_descriptor(struct bufr_descriptor *d, uint32_t id)
  \brief parse an integer with a descriptor fom bufr ECWMF libary
  \param d pointer to a struct \ref bufr_descriptor where to set the result on output
  \param id integer with the descriptor from ewcwf
*/
int uint32_t_to_descriptor ( struct bufr_descriptor *d, uint32_t id )
{
  if ( d == NULL )
    return 1;
  d->f = id / 100000;
  d->x = ( id % 100000 ) / 1000;
  d->y = id % 1000;
  sprintf ( d->c, "%06u", id );
  return 0;
}

/*!
  \fn int two_bytes_to_descriptor (struct bufr_descriptor *d, const uint8_t *source)
  \brief set a struct \ref bufr_descriptor from two consecutive bytes in bufr file
  \param source pointer to first byte (most significant)
  \param d pointer to the resulting descriptor

  It resturns 0 if all is OK. 1 otherwise
 */
int two_bytes_to_descriptor ( struct bufr_descriptor *d, const uint8_t *source )
{
  d->y = source[1];
  d->x = source[0] & 0x3f;
  d->f = ( source[0] >> 6 ) & 0x03;
  sprintf ( d->c, "%u%02u%03u", d->f, d->x, d->y );
  return 0;
}

/*!
  \fn char * bufr_charray_to_string(char *s, char *buf, size_t size)
  \brief get a null termitated c-string from an array of unsigned chars
  \param s resulting string
  \param buf pointer to first element in array
  \param size number of chars in array
*/
char * bufr_charray_to_string ( char *s, char *buf, size_t size )
{
  // copy
  memcpy ( s , buf, size );
  // add final string mark
  s[size] = '\0';
  return s;
}

/*!
  \fn uint8_t * adjust_string(char *s)
  \brief Supress trailing blanks of a string
  \param s string to process
*/
char * bufr_adjust_string ( char *s )
{
  size_t l;
  l = strlen ( s );
  while ( l && s[--l] == ' ' )
    s[l] = '\0';
  return s;
}

/*!
  \fn int is_a_delayed_descriptor ( struct bufr_descriptor *d )
  \brief check if a descriptor is a delayed descriptor
  \param d pointer to a struct \ref bufr_descriptor to check

  If is a delayed desccriptor return 1, 0 otherwise.
*/
int is_a_delayed_descriptor ( struct bufr_descriptor *d )
{
  if ( ( d->f == 0 ) &&
       ( d->x == 31 ) &&
       ( d->y == 1 || d->y == 2 || d->y == 11 || d->y == 12 ) )
    return 1;
  else
    return 0;
}

/*!
  \fn int is_a_short_delayed_descriptor ( struct bufr_descriptor *d )
  \brief check if a descriptor is a short delayed descriptor
  \param d pointer to a struct \ref bufr_descriptor to check

  If is a delayed desccriptor return 1, 0 otherwise.
*/
int is_a_short_delayed_descriptor ( struct bufr_descriptor *d )
{
  if ( ( d->f == 0 ) &&
       ( d->x == 31 ) &&
       ( d->y == 0 ) )
    return 1;
  else
    return 0;
}


/*!
  \fn int is_a_local_descriptor ( struct bufr_descriptor *d )
  \brief check if a descriptor is a local descriptor
  \param d pointer to a struct \ref bufr_descriptor to check

  If is a local desccriptor return 1, 0 otherwise.
*/
int is_a_local_descriptor ( struct bufr_descriptor *d )
{
  if ( ( d->f == 0 ) &&
       ( d->x >= 48 ) &&
       ( d->x <= 63 ) )
    return 1;
  else
    return 0;
}

/*!
   \fn char *get_formatted_value_from_escale ( char *fmt, int32_t escale, double val )

*/
char *get_formatted_value_from_escale ( char *fmt, int32_t escale, double val )
{
  char aux[16];
  if ( escale >= 0 )
    {
      sprintf ( aux,"%%17.%dlf " , escale );
      sprintf ( fmt, aux, val );
    }
  else
    sprintf ( fmt, "%17.0lf " , val );
  return fmt;
}
