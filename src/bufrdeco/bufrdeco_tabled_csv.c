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
 \file bufrdeco_tabled_csv.c
 \brief file with the code to read table D (Descriptor sequences)
 */
#include "bufrdeco.h"

/*!
  \fn int bufr_read_tabled_csv ( struct bufr_tabled *td, char *error )
  \brief Reads a file with table D content according with WMO csv format
  \param td pointer to a target struct \ref bufr_tabled
  \param error string where to set error if any

  If succeded return 0, otherwise 1
*/
int bufr_read_tabled_csv ( struct bufr_tabled *td, char *error )
{
  char aux[8], *c;
  char *tk[16];
  int nt;
  FILE *t;
  size_t ix = 0;
  size_t i = 0;
  int j0, nj = 0;
  char oldkey[8];
  char caux[256];
  char laux[256];
  struct bufr_descriptor desc;

  if ( td->path[0] == 0 )
    {
      return 1;
    }

  // Check if we've already readed this table.
  if ( strcmp ( td->path, td->old_path ) == 0 )
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

  // read first line, it is ignored
  fgets ( laux, 256, t );

  oldkey[0] = 0;
  j0 = 0;

  while ( fgets ( laux, 256, t ) != NULL && i < BUFR_MAXLINES_TABLED )
    {
      // Parse line
      if ( parse_csv_line ( &nt, tk, laux ) < 0 )
        {
          sprintf ( error,"Error parsing csv line from table D file '%s'\n", td->path );
          return 1;
        }

      if ( strcmp ( oldkey, tk[0] ) )
        {
          if (oldkey[0])
          {  // write number of descriptors in emulated ECMWF line
             sprintf(aux, "%2d", nj);
             td->l[j0][8] = aux[0];
             td->l[j0][9] = aux[1];
          }
          j0 = i;
          nj = 1;
        }
      else
        {
          strcpy ( oldkey, tk[0] );
          nj++;
        }

      ix = strtoul ( tk[0], &c, 10 );
      uint32_t_to_descriptor ( &desc, ix );

      if ( td->num[desc.x] == 0 )
        {
          td->x_start[desc.x] = i;  // marc the start
        }
      ( td->num[desc.x] ) ++;
      // Now emule ECMWF format
      sprintf ( td->l[i], " %s    %s", tk[0], tk[3] );
      i++;
    }
  fclose ( t );
  td->nlines = i;
  strcpy ( td->old_path, td->path ); // store latest path
  return 0;
}
