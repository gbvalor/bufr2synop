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
  uint32_t ix;
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
  escale = strtol ( &tb->l[i][97], &c, 10);

  // reference
  reference = strtol ( &tb->l[i][102], &c, 10);

  // bits
  nbits = strtol ( &tb->l[i][115], &c, 10);

  return 0;
}
