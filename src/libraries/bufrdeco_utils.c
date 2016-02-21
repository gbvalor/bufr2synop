/***************************************************************************
 *   Copyright (C) 2013-2016 by Guillermo Ballester Valor                  *
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

uint8_t bitf[8] = {0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};

size_t get_bits_as_char_array ( char *target, uint8_t *has_data, uint8_t *source, size_t *bit0_offset, size_t bit_length )
{
  int i, j;
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

size_t get_bits_as_uint32_t ( uint32_t *target, uint8_t *has_data, uint8_t *source, size_t *bit0_offset, size_t bit_length )
{
  int i;
  size_t r, d;
  uint8_t *c;

  if ( bit_length > 32 || bit_length == 0 )
    return 0;

  r = bit_length;
  d = 0;
  *target = 0;
  *has_data = 0; // marc if no missing data is present
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

// most significant of bits is the sign 
int get_table_b_reference_from_uint32_t(int32_t *target, uint8_t bits, uint32_t source)
{
  uint32_t mask = 1;
  if (bits > 32 || bits == 0)
    return 1;
  
  if (bits > 1)
    mask = ((uint32_t)1 << (bits - 1));
  
  if (mask & source)
  { // case of negative number
    *target = -(int32_t)(source - mask);
  }
  else
    *target = (int32_t)(source);
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


int init_bufr ( struct bufr *b, size_t l )
{
  memset ( b, 0, sizeof ( struct bufr ) );
  if ( ( b->sec4.raw = ( uint8_t * ) calloc ( 1, l ) ) == NULL )
    return 1;
  b->sec4.allocated = l;
  if ( ( b->table = ( struct bufr_tables * ) calloc ( 1, sizeof ( struct bufr_tables ) ) ) == NULL )
    {
      free ( ( void * ) b->sec4.raw );
      b->sec4.allocated = 0;
      return 1;
    }
  if ( ( b->tree = ( struct bufr_expanded_tree * ) calloc ( 1, sizeof ( struct bufr_expanded_tree ) ) ) == NULL )
    {
      free ( ( void * ) b->sec4.raw );
      free ( ( void * ) b->table );
      return 1;
    }

  return 0;
}

int clean_bufr ( struct bufr *b )
{
  if ( b->sec4.allocated > 0 &&  b->sec4.raw != NULL )
    free ( ( void * ) b->sec4.raw );
  if ( b->table != NULL )
    free ( ( void * ) b->table );
  if ( b->tree != NULL )
    free ( ( void * ) b->tree );
  memset ( b, 0, sizeof ( struct bufr ) );
  return 0;
}

