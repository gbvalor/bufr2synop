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
 \file bufrdeco_tablec_csv.c
 \brief file with the code to read table C (Code and flag tables from WMO files)
 */
#include "bufrdeco.h"

/*!
  \fn int bufr_read_tablec_csv ( struct bufrdeco *b )
  \brief Reads a file with table C content (Code table and bit flags) according with csv WMO format
  \param b pointer to a target struct \ref bufrdeco

  If succeded return 0, otherwise 1
*/
int bufr_read_tablec_csv ( struct bufrdeco *b )
{
  char *c;
  //size_t  used = 0;
  FILE *t;
  int nt;
  uint32_t ix;
  buf_t i = 0;
  struct bufr_descriptor desc;
  struct bufr_tablec *tc;
  char *tk[16];
  char caux[256], l[CSV_MAXL];

  //bufrdeco_assert ( b != NULL );
  
  tc = & ( b->tables->c );
  
  if ( tc->path[0] == 0 )
    {
      return 1;
    }

  // If we've already readed this table.
  if ( strcmp ( tc->path, tc->old_path ) == 0 )
    {
#ifdef __DEBUG      
      printf ("# Reused table %s\n", tc->path);
#endif      
      return 0; // all done
    }

  strcpy ( caux, tc->path );
  memset ( tc, 0, sizeof ( struct bufr_tablec ) );
  strcpy ( tc->path, caux );
  if ( ( t = fopen ( tc->path, "r" ) ) == NULL )
    {
      snprintf ( b->error, sizeof ( b->error ),"Unable to open table C file '%s'\n", tc->path );
      return 1;
    }

  // read first line, it is ignored
  fgets ( l, CSV_MAXL, t );

  while ( fgets ( l, CSV_MAXL, t ) != NULL && i < BUFR_MAXLINES_TABLEC )
    {

      // Parse line
      if ( parse_csv_line ( &nt, tk, l ) < 0 || nt != 6 )
        {
          snprintf ( b->error, sizeof ( b->error ),"Error parsing csv line from table C file '%s'. Found %d fields in line %u \n", tc->path, nt, i );
          return 1;
        }

      // Check if code contains other than non-numeric i.e. 'All' or '-'
      // In this case, the line is ignored
      if ( tk[1][0] == 0 || strchr ( tk[1], '-' ) != NULL || strstr ( tk[1], "All" ) != NULL )
        continue;

      // Key
      // First we build the descriptor
      ix = strtoul ( tk[0], &c, 10 );
      uint32_t_to_descriptor ( &desc, ix );
      strcpy ( tc->item[i].key, tk[0] );
      tc->item[i].x = desc.x;
      tc->item[i].y = desc.y;

      // Integer value
      tc->item[i].ival = strtoul ( tk[1], &c, 10 );

      // Description
      c = & tc->item[i].description[0];
      snprintf ( c, BUFR_EXPLAINED_LENGTH, "%s %s %s %s", tk[2], tk[3], tk[4], tk[5] );

      if ( tc->num[desc.x] == 0 )
        {
          tc->x_start[desc.x] = i;  // marc the start
        }
      if ( tc->y_ref[desc.x][desc.y] == 0 )
        {
          tc->y_ref[desc.x][desc.y] = i - tc->x_start[desc.x]; // marc the position from start of first x
        }
      ( tc->num[desc.x] ) ++;
      i++;
    }
  fclose ( t );
  tc->nlines = i;
  tc->wmo_table = 1;
  strcpy ( tc->old_path, tc->path ); // store latest path
  return 0;
}


/*!
  \fn int bufr_find_tablec_csv_index ( buf_t *index, struct bufr_tableb *tb, const char *key, uint32_t code )
  \brief found a descriptor index in a struct \ref bufr_tablec
  \param index pointer  to a size_t where to set the result if success
  \param tb pointer to struct \ref bufr_tableb where are stored all table B data
  \param key descriptor string in format FXXYYY
  \param code value to search in this table/flag

  Return 0 if success, 1 otherwise
*/
int bufr_find_tablec_csv_index ( buf_t *index, struct bufr_tablec *tc, const char *key, uint32_t code )
{
  uint32_t ix;
  buf_t i, i0;
  char *c;
  struct bufr_descriptor desc;

  //bufrdeco_assert ( tc != NULL && index != NULL );
  
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
  \fn char * bufrdeco_explained_table_csv_val (char *expl, size_t dim, struct bufr_tablec *tc, struct bufr_descriptor *d, int ival)
  \brief gets a string with the meaning of a value for a code table descriptor
  \param expl string with resulting meaning
  \param dim numero máximo de caracteres de la cadena resultante
  \param tc pointer to a \ref bufr_tablec struct
  \param index element to read if is not 0
  \param d pointer to the source descriptor
  \param ival integer value for the descriptor

  If something went wrong, it returns NULL . Otherwise it returns \a expl
*/
char * bufrdeco_explained_table_csv_val ( char *expl, size_t dim, struct bufr_tablec *tc, uint32_t *index, struct bufr_descriptor *d, uint32_t ival )
{
  buf_t i;

  bufrdeco_assert ( tc != NULL && expl != NULL && index != NULL && d != NULL);
  
  if ( bufr_find_tablec_csv_index ( &i, tc, d->c, ival ) )
    {
      return NULL; // descritor not found
    }

  // here the calling b item learn where to find first table C struct for a given x and y.
  *index = tc->x_start[d->x] + tc->y_ref[d->x][d->y];

  strncpy_safe ( expl, tc->item[i].description, dim );
  return expl;
}

/*!
  \fn char * bufrdeco_explained_flag_csv_val(char *expl, size_t dim, struct bufr_descriptor *d, unsigned long ival, uint8_t nbits)
  \brief gets a string with the meaning of a value for a flag table descriptor
  \param expl string with resulting meaning
  \param dim max length alowed for \a expl string
  \param d pointer to the source descriptor
  \param ival integer value for the descriptos
  \param nbits uint8_t with the bit extensión of descriptor

  Remember that in FLAG tables for bufr, bit 1 is most significant and N the less one. Bit N only is set to
  1 when all others are also set to one, i.e. in case of missing value.

  If something went wrong, it returns NULL . Otherwise it returns \a expl
*/
char * bufrdeco_explained_flag_csv_val ( char *expl, size_t dim, struct bufr_tablec *tc, struct bufr_descriptor *d,
    uint64_t ival, uint8_t nbits )
{
  size_t used = 0;
  uint64_t test, test0;
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
              used += snprintf ( expl + used, dim - used, "%s", tc->item[i].description );
              return expl;
            }
        }

      test = test0 << ( nbits - v );

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
