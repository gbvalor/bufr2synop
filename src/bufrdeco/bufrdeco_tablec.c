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
 \file bufrdeco_tablec.c
 \brief file with the code to read table C data (code and flag tables)
 */
#include "bufrdeco.h"

/*!
  \fn int bufr_read_tablec ( struct bufr_tablec *tc, char *error )
  \brief Reads a file with table C content according with ECMWF format
  \param tc pointer to a target struct \ref bufr_tablec
  \param error string where to set error if any

  If succeded return 0, otherwise 1
*/
int bufr_read_tablec ( struct bufr_tablec *tc, char *error )
{
  char aux[8], *c;
  size_t startx = 0;
  FILE *t;
  size_t i = 0;
  char caux[256];

  if ( tc->path[0] == '\0' )
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

  while ( fgets ( tc->l[i], 96, t ) != NULL && i < BUFR_MAXLINES_TABLEC )
    {
      // supress the newline
      if ( ( c = strrchr ( tc->l[i],'\n' ) ) != NULL )
        {
          *c = '\0';
        }
      if ( tc->l[i][1] != ' ' && tc->l[i][2] != ' ' )
        {
          aux[0] = tc->l[i][1];
          aux[1] = tc->l[i][2];
          aux[2] = '\0';
          startx = strtoul ( aux, &c, 10 );
          if ( tc->x_start[startx] == 0 )
            {
              tc->x_start[startx] = i;  // marc the start
            }
        }
      ( tc->num[startx] ) ++;
      i++;
    }
  fclose ( t );
  tc->nlines = i;
  tc->wmo_table = 0;
  strcpy ( tc->old_path, tc->path ); // store latest path
  return 0;
}

/*!
 \fn  int bufr_find_tablec_index ( size_t *index, struct bufr_tablec *tc, const char *key )
 \brief Find the index of a line in table C for a given key of a descriptor
 \param index pointer where to set the result
 \param tc pointer to a struct \ref bufr_tablec where all table data is stored
 \param key string in the form FXXYYY which is the key of descriptor we want to find out

 If the descriptor has been found with success then returns 0, othewise returns 1
*/
int bufr_find_tablec_index ( size_t *index, struct bufr_tablec *tc, const char *key )
{
  size_t i, i0;
  size_t x = 0;
  char *c, aux[8];

  aux[0] = key[1];
  aux[1] = key[2];
  aux[2] = '\0';
  x = strtoul ( aux, &c, 10 );
  i0 = tc->x_start[x];
  for ( i = i0 ; i < i0 + tc->num[x] ; i++ )
    {
      if ( tc->l[i][0] != key[0] ||
           tc->l[i][1] != key[1] ||
           tc->l[i][2] != key[2] ||
           tc->l[i][3] != key[3] ||
           tc->l[i][4] != key[4] ||
           tc->l[i][5] != key[5] )
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
  \fn char * bufrdeco_explained_table_val (char *expl, size_t dim, struct bufr_tablec *tc, struct bufr_descriptor *d, int ival)
  \brief gets a string with the meaning of a value for a code table descriptor
  \param expl string with resulting meaning
  \param dim numero máximo de caracteres de la cadena resultante
  \param tc pointer to a \ref bufr_tablec struct
  \param index element to read if is not 0
  \param d pointer to the source descriptor
  \param ival integer value for the descriptor

  If something went wrong, it returns NULL . Otherwise it returns \a expl
*/
char * bufrdeco_explained_table_val ( char *expl, size_t dim, struct bufr_tablec *tc, size_t *index, struct bufr_descriptor *d, uint32_t ival )
{
  char *c;
  uint32_t nv, v,  nl;
  size_t  i, j;

  if ( tc->wmo_table )
    {
      // WMO case
      return bufrdeco_explained_table_csv_val ( expl, dim, tc, index, d, ival );
    }

  if ( *index == 0 )
    {
      // here the calling b item learn where to find table C line
      if ( bufr_find_tablec_index ( index, tc, d->c ) )
        {
          //sprintf ( b->error, "bufrdeco_explained_table_val(): descriptor '%s' not found in table D\n", d->c );
          return NULL; // descritor not found
        }
    }

  i = *index;
  // reads the amount of possible values
  if ( tc->l[i][7] != ' ' )
    {
      nv = strtoul ( &tc->l[i][7], &c, 10 );
    }
  else
    {
      return NULL;
    }

  // read a value
  for ( j = 0; j < nv && i < tc->nlines ; i++ )
    {
      if ( tc->l[i][12] != ' ' )
        {
          v = strtoul ( &tc->l[i][12], &c, 10 );
          j++;
          if ( v != ival )
            {
              continue;
            }
          break;
        }
    }

  if ( j == nv || i == tc->nlines )
    {
      return NULL;  // Value not found
    }

  // read how many lines for the descriptors
  nl = strtoul ( &tc->l[i][21], &c, 10 );


  // if match then we have finished the search
  strcpy ( expl, &tc->l[i][24] );
  if ( nl > 1 )
    {
      for ( nv = 1 ; nv < nl; nv++ )
        if ( ( strlen ( expl ) + strlen ( &tc->l[i + nv][22] ) ) < dim )
          {
            strcat ( expl, &tc->l[i + nv][22] );
          }
    }

  return expl;
}

/*!
  \fn char * bufrdeco_explained_flag_val(char *expl, size_t dim, struct bufr_descriptor *d, unsigned long ival, uint8_t nbits)
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
char * bufrdeco_explained_flag_val ( char *expl, size_t dim, struct bufr_tablec *tc, struct bufr_descriptor *d,
                                     uint64_t ival, uint8_t nbits )
{
  char *c, *s;
  uint64_t test, test0;
  uint64_t nb, nx, v,  nl;
  size_t i, j;

  if ( tc->wmo_table )
    {
      // WMO case
      return bufrdeco_explained_flag_csv_val ( expl, dim, tc, d, ival, nbits );
    }

  // Find first line for descriptor
  for ( i = 0; i <  tc->nlines; i++ )
    {
      if ( tc->l[i][0] != d->c[0] ||
           tc->l[i][1] != d->c[1] ||
           tc->l[i][2] != d->c[2] ||
           tc->l[i][3] != d->c[3] ||
           tc->l[i][4] != d->c[4] ||
           tc->l[i][5] != d->c[5] )
        {
          continue;
        }
      else
        {
          break;
        }
    }

  if ( i == tc->nlines )
    {
      //printf("Descriptor %s No encontrado\n", d->c);
      return NULL;
    }
  //printf("Descriptor %s en linea %d\n", d->c, i);

  // reads the amount of possible bits
  if ( tc->l[i][7] != ' ' )
    {
      nb = strtoul ( &tc->l[i][7], &c, 10 );
    }
  else
    {
      return NULL;
    }

  // read a value
  s = expl;
  s[0] = '\0';

  for ( j = 0, test0 = 1; j < nb && i < tc->nlines ; i++ )
    {
      if ( tc->l[i][12] != ' ' )
        {
          v = strtol ( &tc->l[i][12], &c, 10 ); // v is the bit number
          j++;

          // case 0 with meaning
          if ( v == 0 )
            {
              test0 = 1;
              if ( ival == 0 )
                {
                  nl = strtoul ( &tc->l[i][21], &c, 10 );
                  if ( strlen ( expl ) && ( strlen ( expl ) + 1 ) < dim )
                    {
                      s += sprintf ( s, "|" );
                    }
                  s += sprintf ( s,"%s", &tc->l[i][24] );
                  if ( nl > 1 )
                    {
                      for ( nx = 1 ; nx < nl; nx++ )
                        if ( ( strlen ( expl ) + strlen ( &tc->l[i + nx][22] ) )  < dim )
                          {
                            s += sprintf ( s, "%s", &tc->l[i + nx][22] );
                          }
                    }
                  return expl;
                }
            }

          test = test0 << ( nbits - v );

          if ( v && ( test & ival ) != 0 )
            {
              // bit match
              // read how many lines for the descriptors
              nl = strtol ( &tc->l[i][21], &c, 10 );
              if ( strlen ( expl ) && ( strlen ( expl ) + 1 ) < dim )
                {
                  s += sprintf ( s, "|" );
                }
              s += sprintf ( s,"%s", &tc->l[i][24] );
              if ( nl > 1 )
                {
                  for ( nx = 1 ; nx < nl; nx++ )
                    if ( ( strlen ( expl ) + strlen ( &tc->l[i + nx][22] ) )  < dim )
                      {
                        s += sprintf ( s, "%s", &tc->l[i + nx][22] );
                      }
                }

            }
          else
            {
              continue;
            }
        }
    }

  // if match then we have finished the search
  return expl;
}
