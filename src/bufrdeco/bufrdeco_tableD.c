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
 \file bufrdeco_tableD.c
 \brief file with the code to read table D data (code and flag tables)
 */
#include "bufrdeco.h"

/*!
  \fn int bufr_read_tableD ( struct bufrdeco *b )
  \brief Reads a file with table D content according with WMO csv format
  \param b pointer to a target struct \ref bufrdeco
  \return If succeded return 0, otherwise 1
*/
int bufr_read_tableD ( struct bufrdeco *b )
{
  char aux[32], *c;
  char *tk[16];
  int nt;
  FILE *t;
  buf_t ix;
  buf_t i = 0;
  int j0, nj = 0;
  char oldkey[8];
  char caux[256];
  char laux[CSV_MAXL];
  struct bufr_descriptor desc;
  struct bufr_tableD *td;

  //bufrdeco_assert (b != NULL);

  td = & ( b->tables->d );
  if ( td->path[0] == 0 )
    {
      return 1;
    }

  // Check if we've already readed this table.
  if ( strcmp ( td->path, td->old_path ) == 0 )
    {
#ifdef __DEBUG
      printf ( "# Reused table %s\n", td->path );
#endif
      return 0; // all done
    }

  strcpy_safe ( caux, td->path );
  memset ( td, 0, sizeof ( struct bufr_tableD ) );
  strlcpy ( td->path, caux, sizeof (td->path) );
  if ( ( t = fopen ( td->path, "r" ) ) == NULL )
    {
      snprintf ( b->error, sizeof ( b->error ),"Unable to open table D file '%s'\n", td->path );
      return 1;
    }

  // read first line, it is ignored
  fgets ( laux, CSV_MAXL, t );

  oldkey[0] = 0;
  j0 = 0;

  while ( fgets ( laux, CSV_MAXL, t ) != NULL && i < BUFR_MAXLINES_TABLED )
    {
      // Parse line
      //printf("%s\n",laux);
      if ( parse_csv_line ( &nt, tk, laux ) < 0 || ( nt != 2 && nt != 4 ) )
        {
          snprintf ( b->error, sizeof ( b->error ),"Error parsing csv line from table D file '%s' found %d tokens in line %u\n", td->path, nt, i );
          return 1;
        }

      // item fields
      strlcpy ( td->item[i].key, tk[0], sizeof ( td->item[i].key ) );
      strlcpy ( td->item[i].key2, tk[1], sizeof ( td->item[i].key2 ) );

      if ( nt == 4 && tk[2][0] )
        strlcpy ( td->item[i].description, tk[2], sizeof (td->item[i].description) );
      else
        td->item[i].description[0] = 0;

      if ( nt == 4 && tk[3][0] )
        strlcpy ( td->item[i].description2, tk[3], sizeof (td->item[i].description2) );
      else
        td->item[i].description2[0] = 0;

      if ( strcmp ( oldkey, tk[0] ) )
        {
          if ( oldkey[0] )
            {
              // write number of descriptors in emulated ECMWF line
              snprintf ( aux, sizeof (aux), "%s%3d", oldkey, nj );
              td->l[j0][1] = aux[0];
              td->l[j0][2] = aux[1];
              td->l[j0][3] = aux[2];
              td->l[j0][4] = aux[3];
              td->l[j0][5] = aux[4];
              td->l[j0][6] = aux[5];
              td->l[j0][7] = aux[6];
              td->l[j0][8] = aux[7];
              td->l[j0][9] = aux[8];
            }
          j0 = i;
          nj = 1;
        }
      else
        {
          nj++;
        }
      strlcpy ( oldkey, tk[0], sizeof (oldkey) );

      ix = strtoul ( tk[0], &c, 10 );
      uint32_t_to_descriptor ( &desc, ix );

      if ( td->num[desc.x] == 0 )
        {
          td->x_start[desc.x] = i;  // marc the start
        }
      ( td->num[desc.x] ) ++;

      // Now emule ECMWF format
      snprintf ( td->l[i], sizeof (td->l[i]) , "           %s", tk[1] );
      i++;
    }
  fclose ( t );

  // Last sequence
  snprintf ( aux, sizeof (aux), "%s%3d", oldkey, nj );
  td->l[j0][1] = aux[0];
  td->l[j0][2] = aux[1];
  td->l[j0][3] = aux[2];
  td->l[j0][4] = aux[3];
  td->l[j0][5] = aux[4];
  td->l[j0][6] = aux[5];
  td->l[j0][7] = aux[6];
  td->l[j0][8] = aux[7];
  td->l[j0][9] = aux[8];

  td->nlines = i;
  td->wmo_table = 1;
  strlcpy ( td->old_path, td->path, sizeof (td->old_path) ); // store latest path
  return 0;
}

/*!
 \fn  int bufr_find_tableD_index ( size_t *index, struct bufr_tableD *td, const char *key )
 \brief Find the index of a line in table D for a given key of a descriptor
 \param index pointer where to set the result
 \param td pointer to a struct \ref bufr_tableD where all table data is stored
 \param key string in the form FXXYYY which is the key of descriptor we want to find out
 \return If the descriptor has been found with success then returns 0, othewise returns 1
*/
int bufr_find_tableD_index ( buf_t *index, struct bufr_tableD *td, const char *key )
{
  buf_t i, i0;
  buf_t ix = 0;
  char *c, aux[8];

  //bufrdeco_assert ( td != NULL && key != NULL && index != NULL);

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


/*!
 \fn int bufrdeco_tableD_get_descriptors_array ( struct bufr_sequence *s, struct bufrdeco *b, const char *key )
 \brief get the descriptors array for a descriptor sequence defined in table D with F = 3
 \param s target struct \ref bufr_sequence
 \param b pointer to the basic container struct \ref bufrdeco
 \param key string in the form FXXYYY which is the key of descriptor we want to find out, F = 3
 \return If the sequence has been filled with success then returns 0, otherwise returns 1
*/
int bufrdeco_tableD_get_descriptors_array ( struct bufr_sequence *s, struct bufrdeco *b, const char *key )
{
  buf_t i, j;
  uint32_t nv, v;
  char *c;
  struct bufr_tableD *td;

  bufrdeco_assert ( b != NULL );

  td = & ( b->tables->d );

  // Reject wrong arguments
  if ( s == NULL || key == NULL )
    {
      snprintf ( b->error, sizeof ( b->error ),"%s(): Wrong entry arguments\n", __func__ );
      return 1;
    }

  // here the calling b item learn where to find table C line

  if ( bufr_find_tableD_index ( &i, td, key ) )
    {
      snprintf ( b->error, sizeof ( b->error ), "%s(): descriptor '%s' not found in table D\n", __func__, key );
      return 1; // descritor not found
    }

  // Get the name of common sequence
  if ( td->item[i].description[0] )
    strlcpy ( s->name, td->item[i].description, sizeof (s->name) );
  else
    s->name[0] = 0;

  // reads the amount of possible values
  nv = strtoul ( &td->l[i][7], &c, 10 );

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
