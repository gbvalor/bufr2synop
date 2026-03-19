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
 \file bufrdeco_tableD.c
 \brief file with the code to read table D data (code and flag tables)
 */
#include "bufrdeco.h"

struct bufr_tableD_csv_row {
    char key[8];
    char key2[8];
    char description[BUFR_EXPLAINED_LENGTH];
    char description2[BUFR_EXPLAINED_LENGTH];
};

static int bufr_tableD_read_next_csv_row ( FILE *f, const char *path, struct bufr_tableD_csv_row *row, struct bufrdeco *b )
{
  char laux[CSV_MAXL];
  char *tk[16];
  int nt;

  while ( fgets ( laux, CSV_MAXL, f ) != NULL )
    {
      if ( parse_csv_line ( &nt, tk, laux ) < 0 || ( nt != 2 && nt != 4 ) )
        {
          snprintf ( b->error, sizeof ( b->error ),"Error parsing csv line from table D file '%s'\n", path );
          return -1;
        }

      strncpy_safe ( row->key, tk[0], sizeof ( row->key ) );
      strncpy_safe ( row->key2, tk[1], sizeof ( row->key2 ) );

      if ( nt == 4 && tk[2][0] )
        {
          strncpy_safe ( row->description, tk[2], sizeof ( row->description ) );
        }
      else
        {
          row->description[0] = 0;
        }

      if ( nt == 4 && tk[3][0] )
        {
          strncpy_safe ( row->description2, tk[3], sizeof ( row->description2 ) );
        }
      else
        {
          row->description2[0] = 0;
        }

      return 1;
    }

  if ( ferror ( f ) )
    {
      snprintf ( b->error, sizeof ( b->error ),"Error reading from table D file '%s'\n", path );
      return -1;
    }
  return 0;
}

static int bufr_tableD_append_row ( struct bufr_tableD *td, const struct bufr_tableD_csv_row *row, buf_t *i )
{
  struct bufr_descriptor desc;
  char *c;
  uint32_t ix;
  buf_t idx;

  if ( *i >= BUFR_MAXLINES_TABLED )
    {
      return 1;
    }

  idx = *i;
  memcpy ( td->item[idx].key, row->key, sizeof ( td->item[idx].key ) );
  memcpy ( td->item[idx].key2, row->key2, sizeof ( td->item[idx].key2 ) );
  memcpy ( td->item[idx].description, row->description, sizeof ( td->item[idx].description ) );
  memcpy ( td->item[idx].description2, row->description2, sizeof ( td->item[idx].description2 ) );

  ix = strtoul ( row->key, &c, 10 );
  uint32_t_to_descriptor ( &desc, ix );
  if ( td->num[desc.x] == 0 )
    {
      td->x_start[desc.x] = idx;
    }
  ( td->num[desc.x] ) ++;
  ( *i ) ++;
  return 0;
}

static void bufr_tableD_build_ecmwf_lines ( struct bufr_tableD *td )
{
  buf_t i;
  buf_t j0 = 0;
  int nj = 0;
  char oldkey[8] = {0};
  char aux[32];

  for ( i = 0; i < td->nlines; i++ )
    {
      if ( strcmp ( oldkey, td->item[i].key ) )
        {
          if ( oldkey[0] )
            {
              snprintf ( aux, sizeof ( aux ), "%s%3d", oldkey, nj );
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

      memcpy ( oldkey, td->item[i].key, sizeof ( oldkey ) );
      snprintf ( td->l[i], sizeof ( td->l[i] ), "           %s", td->item[i].key2 );
    }

  if ( oldkey[0] )
    {
      snprintf ( aux, sizeof ( aux ), "%s%3d", oldkey, nj );
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
}

/*!
  \fn int bufr_read_tableD ( struct bufrdeco *b )
  \brief Reads a file with table D content according with WMO csv format
  \param [in,out] b Pointer to a target struct \ref bufrdeco
  \return If succeeded return 0, otherwise 1
*/
int bufr_read_tableD ( struct bufrdeco *b )
{
  FILE *t;
  FILE *t_local;
  buf_t i = 0;
  int have_master;
  int have_local;
  char key_active[8];
  char caux[256];
  char caux_local[BUFRDECO_PATH_LENGTH];
  char laux[CSV_MAXL];
  struct bufr_tableD *td;
  struct bufr_tableD_csv_row row_master;
  struct bufr_tableD_csv_row row_local;
  const struct bufr_tableD_csv_row *row;

  bufrdeco_assert (b != NULL);

  td = & ( b->tables->d );
  if ( td->path[0] == 0 )
    {
      return 1;
    }

  // Check if we've already readed this table.
  if ( strcmp ( td->path, td->old_path ) == 0 && strcmp ( td->local_path, td->local_path_old ) == 0 )
    {
#ifdef __DEBUG
      printf ( "# Reused table %s\n", td->path );
#endif
      return 0; // all done
    }

  memcpy ( caux, td->path, sizeof ( caux ) );
  memcpy ( caux_local, td->local_path, sizeof ( td->local_path ) );
  memset ( td, 0, sizeof ( struct bufr_tableD ) );
  memcpy ( td->path, caux, sizeof ( td->path ) );
  memcpy ( td->local_path, caux_local, sizeof ( td->local_path ) );
  t_local = NULL;
  if ( ( t = fopen ( td->path, "r" ) ) == NULL )
    {
      snprintf ( b->error, sizeof ( b->error ),"Unable to open table D file '%s'\n", td->path );
      return 1;
    }

  if ( td->local_path[0] )
    {
      t_local = fopen ( td->local_path, "r" );
    }

  // read first line, it is ignored
  if ( fgets ( laux, CSV_MAXL, t ) == NULL && ferror ( t ) )
    {
      snprintf ( b->error, sizeof ( b->error ),"Error reading from table D file '%s'\n", td->path );
      fclose ( t );
      if ( t_local != NULL )
        {
          fclose ( t_local );
        }
      return 1;
    }

  if ( t_local != NULL )
    {
      if ( fgets ( laux, CSV_MAXL, t_local ) == NULL && ferror ( t_local ) )
        {
          snprintf ( b->error, sizeof ( b->error ),"Error reading from table D file '%s'\n", td->local_path );
          fclose ( t );
          fclose ( t_local );
          return 1;
        }
    }

  have_master = bufr_tableD_read_next_csv_row ( t, td->path, &row_master, b );
  if ( have_master < 0 )
    {
      fclose ( t );
      if ( t_local != NULL )
        {
          fclose ( t_local );
        }
      return 1;
    }

  have_local = 0;
  if ( t_local != NULL )
    {
      have_local = bufr_tableD_read_next_csv_row ( t_local, td->local_path, &row_local, b );
      if ( have_local < 0 )
        {
          fclose ( t );
          fclose ( t_local );
          return 1;
        }
    }

  while ( ( have_master || have_local ) && i < BUFR_MAXLINES_TABLED )
    {
      if ( have_local && ( ! have_master || strcmp ( row_local.key, row_master.key ) <= 0 ) )
        {
          memcpy ( key_active, row_local.key, sizeof ( key_active ) );

          while ( have_local && strcmp ( row_local.key, key_active ) == 0 && i < BUFR_MAXLINES_TABLED )
            {
              row = &row_local;
              if ( bufr_tableD_append_row ( td, row, &i ) )
                {
                  break;
                }

              have_local = bufr_tableD_read_next_csv_row ( t_local, td->local_path, &row_local, b );
              if ( have_local < 0 )
                {
                  fclose ( t );
                  fclose ( t_local );
                  return 1;
                }
            }

          while ( have_master && strcmp ( row_master.key, key_active ) == 0 )
            {
              have_master = bufr_tableD_read_next_csv_row ( t, td->path, &row_master, b );
              if ( have_master < 0 )
                {
                  fclose ( t );
                  fclose ( t_local );
                  return 1;
                }
            }
        }
      else
        {
          memcpy ( key_active, row_master.key, sizeof ( key_active ) );

          while ( have_master && strcmp ( row_master.key, key_active ) == 0 && i < BUFR_MAXLINES_TABLED )
            {
              row = &row_master;
              if ( bufr_tableD_append_row ( td, row, &i ) )
                {
                  break;
                }

              have_master = bufr_tableD_read_next_csv_row ( t, td->path, &row_master, b );
              if ( have_master < 0 )
                {
                  fclose ( t );
                  if ( t_local != NULL )
                    {
                      fclose ( t_local );
                    }
                  return 1;
                }
            }
        }
    }

  fclose ( t );
  if ( t_local != NULL )
    {
      fclose ( t_local );
    }

  td->nlines = i;
  bufr_tableD_build_ecmwf_lines ( td );
  td->wmo_table = 1;
  memcpy ( td->old_path, td->path, sizeof ( td->old_path ) ); // store latest path
  memcpy ( td->local_path_old, td->local_path, sizeof ( td->local_path_old ) ); // store latest local path
  return 0;
}

/*!
 \fn  int bufr_find_tableD_index ( size_t *index, struct bufr_tableD *td, const char *key )
 \brief Find the index of a line in table D for a given key of a descriptor
 \param [out] index Pointer where to set the result
 \param [in] td Pointer to a struct \ref bufr_tableD where all table data is stored
 \param [in] key String in the form FXXYYY which is the key of descriptor we want to find out
 \return If the descriptor has been found with success then returns 0, otherwise returns 1
*/
int bufr_find_tableD_index ( buf_t *index, struct bufr_tableD *td, const char *key )
{
  buf_t i, i0;
  buf_t ix = 0;
  char *c, aux[8];

  bufrdeco_assert ( td != NULL && key != NULL && index != NULL);

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
 \param [out] s Target struct \ref bufr_sequence
 \param [in,out] b Pointer to the basic container struct \ref bufrdeco
 \param [in] key String in the form FXXYYY which is the key of descriptor we want to find out, F = 3
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
    memcpy ( s->name, td->item[i].description, sizeof ( s->name ) );
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
