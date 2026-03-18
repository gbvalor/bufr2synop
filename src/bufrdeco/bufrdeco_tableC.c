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
 \file bufrdeco_tableC.c
 \brief file with the code to read table C data (code and flag tables)
 */
#include "bufrdeco.h"

struct bufr_tableC_csv_row {
    char key[8];
    uint8_t x;
    uint8_t y;
    uint32_t ival;
    char description[BUFR_EXPLAINED_LENGTH];
};

static int bufr_tableC_read_next_csv_row ( FILE *f, const char *path, struct bufr_tableC_csv_row *row, struct bufrdeco *b )
{
  char *tk[16];
  char l[CSV_MAXL];
  int nt;
  uint32_t ix;
  char *c;
  struct bufr_descriptor desc;

  while ( fgets ( l, CSV_MAXL, f ) != NULL )
    {
      if ( parse_csv_line ( &nt, tk, l ) < 0 || nt != 6 )
        {
          snprintf ( b->error, sizeof ( b->error ),"Error parsing csv line from table C file '%s'\n", path );
          return -1;
        }

      if ( tk[1][0] == 0 || strchr ( tk[1], '-' ) != NULL || strstr ( tk[1], "All" ) != NULL )
        {
          continue;
        }

      ix = strtoul ( tk[0], &c, 10 );
      uint32_t_to_descriptor ( &desc, ix );
      strncpy_safe ( row->key, tk[0], sizeof ( row->key ) );
      row->x = desc.x;
      row->y = desc.y;
      row->ival = strtoul ( tk[1], &c, 10 );
      snprintf ( row->description, sizeof ( row->description ), "%s %s %s %s", tk[2], tk[3], tk[4], tk[5] );
      return 1;
    }

  if ( ferror ( f ) )
    {
      snprintf ( b->error, sizeof ( b->error ),"Error reading from table C file '%s'\n", path );
      return -1;
    }
  return 0;
}

static int bufr_tableC_append_row ( struct bufr_tableC *tc, const struct bufr_tableC_csv_row *row, buf_t *i )
{
  buf_t idx;

  if ( *i >= BUFR_MAXLINES_TABLEC )
    {
      return 1;
    }

  idx = *i;
  memcpy ( tc->item[idx].key, row->key, sizeof ( tc->item[idx].key ) );
  tc->item[idx].x = row->x;
  tc->item[idx].y = row->y;
  tc->item[idx].ival = row->ival;
  memcpy ( tc->item[idx].description, row->description, sizeof ( tc->item[idx].description ) );

  if ( tc->num[row->x] == 0 )
    {
      tc->x_start[row->x] = idx;
    }
  if ( tc->y_ref[row->x][row->y] == 0 )
    {
      tc->y_ref[row->x][row->y] = idx - tc->x_start[row->x];
    }
  ( tc->num[row->x] ) ++;
  ( *i ) ++;
  return 0;
}


/*!
  \fn int bufr_read_tableC ( struct bufrdeco *b )
  \brief Reads a file with table C content (Code table and bit flags) according with csv WMO format
  \param [in,out] b Pointer to a target struct \ref bufrdeco
  \return  0 if success, 1 otherwise
*/
int bufr_read_tableC ( struct bufrdeco *b )
{
  FILE *t;
  FILE *t_local;
  int have_master;
  int have_local;
  buf_t i = 0;
  struct bufr_tableC *tc;
  struct bufr_tableC_csv_row row_master;
  struct bufr_tableC_csv_row row_local;
  const struct bufr_tableC_csv_row *row;
  char key_active[8];
  char caux[256], l[CSV_MAXL];

  bufrdeco_assert ( b != NULL );
  
  tc = & ( b->tables->c );
  
  if ( tc->path[0] == 0 )
    {
      return 1;
    }

  // If we've already readed this table.
  if ( strcmp ( tc->path, tc->old_path ) == 0 && strcmp ( tc->local_path, tc->local_path_old ) == 0 )
    {
#ifdef __DEBUG      
      printf ("# Reused table %s\n", tc->path);
#endif      
      return 0; // all done
    }

  memcpy ( caux, tc->path, sizeof ( caux ) );
  memset ( tc, 0, sizeof ( struct bufr_tableC ) );
  memcpy ( tc->path, caux, sizeof ( tc->path ) );
  t_local = NULL;
  if ( ( t = fopen ( tc->path, "r" ) ) == NULL )
    {
      snprintf ( b->error, sizeof ( b->error ),"Unable to open table C file '%s'\n", tc->path );
      return 1;
    }

  if ( tc->local_path[0] )
    {
      t_local = fopen ( tc->local_path, "r" );
    }

  // read first line, it is ignored
  if ( fgets ( l, CSV_MAXL, t ) == NULL && ferror ( t ) )
    {
      snprintf ( b->error, sizeof ( b->error ),"Error reading from table C file '%s'\n", tc->path );
      fclose ( t );
      if ( t_local != NULL )
        {
          fclose ( t_local );
        }
      return 1;
    }

  if ( t_local != NULL )
    {
      if ( fgets ( l, CSV_MAXL, t_local ) == NULL && ferror ( t_local ) )
        {
          snprintf ( b->error, sizeof ( b->error ),"Error reading from table C file '%s'\n", tc->local_path );
          fclose ( t );
          fclose ( t_local );
          return 1;
        }
    }

  have_master = bufr_tableC_read_next_csv_row ( t, tc->path, &row_master, b );
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
      have_local = bufr_tableC_read_next_csv_row ( t_local, tc->local_path, &row_local, b );
      if ( have_local < 0 )
        {
          fclose ( t );
          fclose ( t_local );
          return 1;
        }
    }

  while ( ( have_master || have_local ) && i < BUFR_MAXLINES_TABLEC )
    {
      if ( have_local && ( ! have_master || strcmp ( row_local.key, row_master.key ) <= 0 ) )
        {
          memcpy ( key_active, row_local.key, sizeof ( key_active ) );

          while ( have_local && strcmp ( row_local.key, key_active ) == 0 && i < BUFR_MAXLINES_TABLEC )
            {
              row = &row_local;
              if ( bufr_tableC_append_row ( tc, row, &i ) )
                {
                  break;
                }
              have_local = bufr_tableC_read_next_csv_row ( t_local, tc->local_path, &row_local, b );
              if ( have_local < 0 )
                {
                  fclose ( t );
                  fclose ( t_local );
                  return 1;
                }
            }

          while ( have_master && strcmp ( row_master.key, key_active ) == 0 )
            {
              have_master = bufr_tableC_read_next_csv_row ( t, tc->path, &row_master, b );
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

          while ( have_master && strcmp ( row_master.key, key_active ) == 0 && i < BUFR_MAXLINES_TABLEC )
            {
              row = &row_master;
              if ( bufr_tableC_append_row ( tc, row, &i ) )
                {
                  break;
                }
              have_master = bufr_tableC_read_next_csv_row ( t, tc->path, &row_master, b );
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
  tc->nlines = i;
  tc->wmo_table = 1;
  memcpy ( tc->old_path, tc->path, sizeof ( tc->old_path ) ); // store latest path
  memcpy ( tc->local_path_old, tc->local_path, sizeof ( tc->local_path_old ) ); // store latest local path
  return 0;
}


/*!
  \fn int bufr_find_tableC_csv_index ( buf_t *index, struct bufr_tableC *tc, const char *key, uint32_t code )
  \brief found a descriptor index in a struct \ref bufr_tableC
  \param [out] index Pointer to a size_t where to set the result if success
  \param [in] tc Pointer to struct \ref bufr_tableC where are stored all table C data
  \param [in] key Descriptor string in format FXXYYY
  \param [in] code Value to search in this table/flag
  \return  0 if success, 1 otherwise
*/
int bufr_find_tableC_csv_index ( buf_t *index, struct bufr_tableC *tc, const char *key, uint32_t code )
{
  uint32_t ix;
  buf_t i, i0;
  char *c;
  struct bufr_descriptor desc;

  bufrdeco_assert ( tc != NULL && index != NULL );
  
  ix = strtoul ( key, &c, 10 );
  uint32_t_to_descriptor ( &desc, ix );
  i0 = tc->x_start[desc.x];
  for ( i = i0 ; i < i0 + tc->num[desc.x] ; i++ )
    {
      if ( tc->item[i].y == desc.y && tc->item[i].ival == code )
        {
          *index = i;
          return 0; // found
        }
    }
  return 1; // not found
}

/*!
  \fn char * bufrdeco_explained_table_csv_val (char *expl, size_t dim, struct bufr_tableC *tc, struct bufr_descriptor *d, int ival)
  \brief gets a string with the meaning of a value for a code table descriptor
  \param [out] expl String with resulting meaning
  \param [in] dim Número máximo de caracteres de la cadena resultante
  \param [in] tc Pointer to a \ref bufr_tableC struct
  \param [out] index Element to read if is not 0
  \param [in] d Pointer to the source descriptor
  \param [in] ival Integer value for the descriptor
  \return If something went wrong, it returns NULL . Otherwise it returns \a expl
*/
char * bufrdeco_explained_table_val ( char *expl, size_t dim, struct bufr_tableC *tc, uint32_t *index, const struct bufr_descriptor *d, uint32_t ival )
{
  buf_t i;

  bufrdeco_assert ( tc != NULL && expl != NULL && index != NULL && d != NULL);
  
  if ( bufr_find_tableC_csv_index ( &i, tc, d->c, ival ) )
    {
      return NULL; // descritor not found
    }

  // here the calling b item learn where to find first table C struct for a given x and y.
  *index = tc->x_start[d->x] + tc->y_ref[d->x][d->y];

  strncpy_safe ( expl, tc->item[i].description, dim );
  return expl;
}

/*!
  \fn char * bufrdeco_explained_flag_val(char *expl, size_t dim, struct bufr_descriptor *d, unsigned long ival, uint8_t nbits)
  \brief gets a string with the meaning of a value for a flag table descriptor
  \param [out] expl String with resulting meaning
  \param [in] dim Max length allowed for \a expl string
  \param [in] tc Pointer to table C struct
  \param [in] d Pointer to the source descriptor
  \param [in] ival Integer value for the descriptor
  \param [in] nbits uint8_t with the bit extension of descriptor
  \return If something went wrong, it returns NULL . Otherwise it returns \a expl

  Remember that in FLAG tables for bufr, bit 1 is most significant and N the less one. Bit N only is set to
  1 when all others are also set to one, i.e. in case of missing value.

*/
char * bufrdeco_explained_flag_val ( char *expl, size_t dim, struct bufr_tableC *tc, const struct bufr_descriptor *d,
    uint64_t ival, uint8_t nbits )
{
  size_t used = 0;
  uint64_t test0;
  uint64_t v;
  size_t i, j;

  bufrdeco_assert ( tc != NULL && expl != NULL && d != NULL);

  if ( tc->num[d->x] == 0 )
    {
      //printf("Descriptor %s No encontrado\n", d->c);
      return NULL;
    }

  // First line for descriptor
  i = tc->x_start[d->x] + tc->y_ref[d->x][d->y];

  // init description
  expl[0] = '\0';

  for ( j = 0, test0 = 1 ; j < nbits && ( tc->item[i].x == d->x ) && ( tc->item[i].y == d->y ) ; i++ )
    {
      v = tc->item[i].ival; // v is the bit number
      j++;

      // case 0 with meaning
      if ( v == 0 )
        {
          test0 = 1;
          if ( ival == 0 )
            {
              used += snprintf ( expl + used, dim - used, "|" );
              snprintf ( expl + used, dim - used, "%s", tc->item[i].description );
              return expl;
            }
        }

      uint64_t test = test0 << ( nbits - v );

      if ( v && ( test & ival ) != 0 )
        {
          used += snprintf ( expl + used, dim - used, "|" );
          used += snprintf ( expl + used, dim - used, "%s", tc->item[i].description );
        }
      else
        {
          continue;
        }
      //printf ( "%s\n", expl );
    }
  // if match then we have finished the search
  return expl;
}
