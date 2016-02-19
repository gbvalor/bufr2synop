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
 \file bufrdeco_tableb.c
 \brief file with the code to read table B data (code and flag tables)
 */
#include "bufrdeco.h"

const double pow10pos[8]= {1.0, 10.0, 100.0, 1000.0, 10000.0, 100000.0, 1000000.0, 10000000.0};
const double pow10neg[8]= {1.0,  0.1,  0.01,  0.001,  0.0001,  0.00001,  0.000001,  0.0000001};

int bufr_read_tableb ( struct bufr_tableb *tb, char *error )
{
  char *c;
  FILE *t;
  size_t i = 0;

  if ( tb->path == NULL )
    return 1;

  tb->nlines = 0;
  if ( ( t = fopen ( tb->path, "r" ) ) == NULL )
    {
      sprintf ( error,"Unable to open table B file '%s'\n", tb->path );
      return 1;
    }

  while ( fgets ( tb->l[i], 180, t ) != NULL && i < BUFR_MAXLINES_TABLEC )
    {
      // supress the newline
      if ( ( c = strrchr ( tb->l[i],'\n' ) ) != NULL )
        *c = '\0';
      i++;
    }
  fclose ( t );
  tb->nlines = i;
  return 0;
}

int bufrdeco_tableb_val ( struct bufr_atom_data *a, struct bufr *b, char *needle )
{
  size_t i, nbits;
  uint32_t ix, ival;
  uint8_t has_data;
  char *c;
  char name[80], type[32];
  int32_t escale, reference;
  struct bufr_tableb *tb;

  tb = & ( b->table->b );

  // Reject wrong arguments
  if ( a == NULL || b == NULL || needle == NULL )
    return 1;

  // search source descriptor on data
  // Find first line for descriptor
  for ( i = 0; i <  tb->nlines ; i++ )
    {
      if ( tb->l[i][0] != ' ' ||
           tb->l[i][1] != needle[0] ||
           tb->l[i][2] != needle[1] ||
           tb->l[i][3] != needle[2] ||
           tb->l[i][4] != needle[3] ||
           tb->l[i][5] != needle[4] ||
           tb->l[i][6] != needle[5] )
        continue;
      else
        break;
    }

  if ( i == tb->nlines )
    {
      sprintf(b->error, "bufrdeco_tableb_val(): descriptor '%s' not found in table B\n", needle);
      return 1; // descritor not found
    }

  // First we build the descriptor
  ix = strtoul ( needle, &c, 10 );
  uint32_t_to_descriptor ( &a->desc, ix );

  // detailed name
  bufr_charray_to_string ( name,&tb->l[i][8], 64 );
  bufr_adjust_string ( name ); // supress trailing blanks

  // tyoe
  bufr_charray_to_string ( type,&tb->l[i][73], 24 );
  bufr_adjust_string ( type );

  // escale
  escale = strtol ( &tb->l[i][97], &c, 10 );

  // reference
  reference = strtol ( &tb->l[i][102], &c, 10 );

  // bits
  nbits = strtol ( &tb->l[i][115], &c, 10 );


  if (get_bits_as_uint32_t ( &ival, &has_data, &b->sec4.raw[0], &b->sec4.bit_offset, nbits ) == 0)
  {
    sprintf(b->error, "bufrdeco_tableb_val(): Cannot get bits from '%s'\n", needle);
    return 1;
  }

  if ( has_data )
    {
      if ( escale >= 0 && escale < 8 )
        a->val = ( double ) ( ( int32_t ) ival + reference ) * pow10neg[ ( size_t ) escale];
      else if ( escale < 0 && escale > -8 )
        a->val = ( double ) ( ( int32_t ) ival + reference ) * pow10pos[ ( size_t ) ( -escale )];
      else
        a->val = ( double ) ( ( int32_t ) ival + reference ) * pow10 ( ( double ) ( -escale ) );
    }
  else
    a->val = MISSING_REAL;

  ival = ( uint32_t ) ( a->val + 0.5 );
  a->mask = 0;
  if ( a->val != MISSING_REAL )
    {
      if ( strstr ( a->unit, "CCITTIA5" ) != NULL )
        a->mask |= DESCRIPTOR_HAVE_STRING_VALUE;
      else if ( strstr ( a->unit, "CODE TABLE" ) == a->unit )
        {
          a->mask |= DESCRIPTOR_IS_CODE_TABLE;
          if ( bufrdeco_explained_table_val ( a->ctable, 256, & ( b->table->c ), & ( a->desc ), ival ) != NULL )
            {
              a->mask |= DESCRIPTOR_HAVE_CODE_TABLE_STRING;
            }
        }
      else if ( strstr ( a->unit,"FLAG" ) == a->unit )
        {
          a->mask |= DESCRIPTOR_IS_FLAG_TABLE;
          if ( bufrdeco_explained_flag_val ( a->ctable, 256, & ( b->table->c ), & ( a->desc ), ival ) != NULL )
            {
              a->mask |= DESCRIPTOR_HAVE_FLAG_TABLE_STRING;
            }
        }
    }
    else
      a->mask |= DESCRIPTOR_VALUE_MISSING;

  return 0;
}
