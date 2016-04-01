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
 \file bufrdeco_tabled.c
 \brief file with the code to read table D data (code and flag tables)
 */
#include "bufrdeco.h"
int bufr_read_tabled ( struct bufr_tabled *td, char *error )
{
  char aux[8], *c;
  FILE *t;
  size_t ix = 0;
  size_t i = 0;
  char caux[256];

  if ( td->path == NULL )
    {
      return 1;
    }

  // Check if we've already readed this table.
  if ( strcmp ( td->path, td->old_path ) == 0)
    {
      return 0; // all done
    }

  strcpy ( caux, td->path );
  memset ( td, 0, sizeof ( struct bufr_tabled ) );
  strcpy ( td->path,caux );
  if ( ( t = fopen ( td->path, "r" ) ) == NULL )
    {
      sprintf ( error,"Unable to open table D file '%s'\n", td->path );
      return 1;
    }

  while ( fgets ( td->l[i], 124, t ) != NULL && i < BUFR_MAXLINES_TABLEC )
    {
      // supress the newline
      if ( ( c = strrchr ( td->l[i],'\n' ) ) != NULL )
        {
          *c = '\0';
        }
      if ( td->l[i][1] != ' ' && td->l[i][2] != ' ' )
        {
          aux[0] = td->l[i][2];
          aux[1] = td->l[i][3];
          aux[2] = '\0';
          ix = strtoul ( aux, &c, 10 );
          if ( td->x_start[ix] == 0 )
            {
              td->x_start[ix] = i;  // marc the start
            }
        }
      ( td->num[ix] ) ++;
      i++;
    }
  fclose ( t );
  td->nlines = i;
  strcpy ( td->old_path, td->path ); // store latest path
  return 0;
}

int bufr_find_tabled_index ( size_t *index, struct bufr_tabled *td, const char *key )
{
  size_t i, i0;
  size_t ix = 0;
  char *c, aux[8];

  aux[0] = key[1];
  aux[1] = key[2];
  aux[2] = '\0';
  ix = strtoul ( aux, &c, 10 );
  i0 = td->x_start[ix];
  for ( i = i0 ; i < i0 + td->num[ix] ; i++ )
    {
      if ( td->l[i][1] != key[0] ||
           td->l[i][2] != key[1] ||
           td->l[i][3] != key[2] ||
           td->l[i][4] != key[3] ||
           td->l[i][5] != key[4] ||
           td->l[i][6] != key[5] )
        {
          continue;
        }
      else
        {
          *index = i;
          return 0;
        }
    }
  return 1; // not found
}


int bufrdeco_tabled_get_descriptors_array ( struct bufr_sequence *s, struct bufrdeco *b, const char *key )
{
  size_t i, j;
  uint32_t nv, v;
  char *c;
  struct bufr_tabled *td;

  td = & ( b->table->d );

  // Reject wrong arguments
  if ( s == NULL || b == NULL || key == NULL )
    {
      sprintf ( b->error,"bufrdeco_tabled_get_descritors_array(): Wrong entry arguments\n" );
      return 1;
    }

  // here the calling b item learn where to find table C line

  if ( bufr_find_tabled_index ( &i, td, key ) )
    {
      sprintf ( b->error, "bufrdeco_tabled_val(): descriptor '%s' not found in table D\n", key );
      return 1; // descritor not found
    }

  // reads the amount of possible values
  if ( td->l[i][7] == ' ' )
    {
      nv = strtoul ( &td->l[i][7], &c, 10 );
    }
  else
    {
      sprintf ( b->error, "bufrdeco_tabled_get_descritors_array(): Error when parsing provided table D\n" );
      return 1;
    }
  // s->level must be set by caller
  // s->father must be set by caller

  s->ndesc = 0;

  // read all descriptors
  for ( j = 0; j < nv && ( i + j ) < td->nlines ; j++ )
    {
      v = strtoul ( & ( td->l[i+j][11] ), &c, 10 );
      uint32_t_to_descriptor ( & ( s->lseq[j] ), v );
      ( s->ndesc ) ++;
    }
  // s->sons are not set here
  return 0;
}
