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
 \file bufrdeco_tablec_csv.c
 \brief file with the code to read table C (Code and flag tables from WMO files)
 */
#include "bufrdeco.h"

#define CSV_MAXL 1024

/*!
  \fn int bufr_read_tablec_csv ( struct bufr_tablec *tc, char *error )
  \brief Reads a file with table C content (Code table and bit flags) according with csv WMO format
  \param tc pointer to a target struct \ref bufr_tablec
  \param error string where to set error if any

  If succeded return 0, otherwise 1
*/
int bufr_read_tablec_csv ( struct bufr_tablec *tc, char *error )
{
  char *c;
  FILE *t;
  int nt;
  uint32_t ix;
  size_t i = 0;
  struct bufr_descriptor desc;
  char *tk[16];
  char caux[256], l[CSV_MAXL];

  if ( tc->path[0] == 0 )
    {
      return 1;
    }

  // If we've already readed this table.
  if ( strcmp ( tc->path, tc->old_path ) == 0 )
    {
      return 0; // all done
    }

  strcpy ( caux, tc->path );
  memset ( tc, 0, sizeof ( struct bufr_tablec ) );
  strcpy ( tc->path,caux );
  if ( ( t = fopen ( tc->path, "r" ) ) == NULL )
    {
      sprintf ( error,"Unable to open table C file '%s'\n", tc->path );
      return 1;
    }

  // read first line, it is ignored
  fgets ( l, 512, t );
 
  while ( fgets ( l, CSV_MAXL, t ) != NULL && i < BUFR_MAXLINES_TABLEC )
    {

      // Parse line
      if ( parse_csv_line ( &nt, tk, l ) < 0 )
        {
          sprintf ( error,"Error parsing csv line from table C file '%s'\n", tc->path );
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
      if ( strlen ( tk[2] ) < BUFR_EXPLAINED_LENGTH )
        {
          c += sprintf ( c,"%s", tk[2] );
          if ( tk[3][0] && ( strlen ( tk[3] ) + strlen ( tc->item[i].description ) ) < BUFR_EXPLAINED_LENGTH )
            {
              c += sprintf ( c," %s", tk[3] );
              if ( tk[4][0] && ( strlen ( tk[4] ) + strlen ( tc->item[i].description ) ) < BUFR_EXPLAINED_LENGTH )
                c += sprintf ( c, " %s", tk[4] );
            }
        }
      else
        {
          tk[2][BUFR_EXPLAINED_LENGTH - 1] = '\0';
          c += sprintf ( c,"%s", tk[2] );
        }

      if ( tc->num[desc.x] == 0 )
        {
          tc->x_start[desc.x] = i;  // marc the start
        }
      tc->y_ref[desc.x][desc.y] = i - tc->x_start[desc.x]; // marc the position from start of first x
      ( tc->num[desc.x] ) ++;
      i++;
    }
  fclose ( t );
  tc->nlines = i;
  strcpy ( tc->old_path, tc->path ); // store latest path
  return 0;
}


/*!
  \fn int bufr_find_tablec_csv_index ( size_t *index, struct bufr_tableb *tb, const char *key, uint32_t code )
  \brief found a descriptor index in a struct \ref bufr_tablec
  \param index pointer  to a size_t where to set the result if success
  \param tb pointer to struct \ref bufr_tableb where are stored all table B data
  \param key descriptor string in format FXXYYY
  \param code value to search in this table/flag

  Return 0 if success, 1 otherwise
*/
int bufr_find_tablec_csv_index ( size_t *index, struct bufr_tablec *tc, const char *key, uint32_t code )
{
  uint32_t ix;
  size_t i, i0;
  char *c;
  struct bufr_descriptor desc;

  ix = strtoul ( key, &c, 10 );
  uint32_t_to_descriptor ( &desc, ix );
  i0 = tc->x_start[desc.x];
  for ( i = i0 ; i < i0 + tc->num[desc.x] ; i++ )
    {
      if ( tc->item[i].x != desc.x )
        {
          return 1; // descriptor not found
        }
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
char * bufrdeco_explained_table_csv_val ( char *expl, size_t dim, struct bufr_tablec *tc, size_t *index, struct bufr_descriptor *d, uint32_t ival )
{
  size_t  i;

  if ( bufr_find_tablec_csv_index ( &i, tc, d->c, ival ) )
    {
      return NULL; // descritor not found
    }

  // here the calling b item learn where to find first table C struct for a given x and y.
  *index = tc->x_start[d->x] + tc->y_ref[d->x][d->y];

  if ( strlen ( tc->item[i].description ) < dim )
    {
      strcpy ( expl, tc->item[i].description );
      return expl;
    }
  else
    {
      return NULL;
    }
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
  char *s;
  uint64_t test, test0;
  uint64_t v;
  size_t i, j;

  if ( tc->num[d->x] == 0 )
    {
      //printf("Descriptor %s No encontrado\n", d->c);
      return NULL;
    }

  // First line for descriptor
  i = tc->x_start[d->x] + tc->y_ref[d->x][d->y];

  // init description
  s = expl;
  s[0] = '\0';

  for ( j = 0, test0 = 1; j < nbits && tc->item[i].x == d->x && tc->item[i].y == d->y ; i++ )
    {
      v = tc->item[i].ival; // v is the bit number
      j++;

      // case 0 with meaning
      if ( v == 0 )
        {
          test0 = 1;
          if ( ival == 0 )
            {
              if ( ( strlen ( expl ) && ( strlen ( expl ) + 1 ) ) < dim )
                {
                  s += sprintf ( s, "|" );
                }
              if ( strlen ( expl ) + strlen ( tc->item[i].description ) < dim )
                {
                  s += sprintf ( s,"%s", tc->item[i].description );
                }
              return expl;
            }
        }

      test = test0 << ( nbits - v );

      if ( v && ( test & ival ) != 0 )
        {
          if ( ( strlen ( expl ) && ( strlen ( expl ) + 1 ) ) < dim )
            {
              s += sprintf ( s, "|" );
            }
          if ( strlen ( expl ) + strlen ( tc->item[i].description ) < dim )
            {
              s += sprintf ( s,"%s", tc->item[i].description );
            }
        }
      else
        {
          continue;
        }
    }
  // if match then we have finished the search
  return expl;
}
