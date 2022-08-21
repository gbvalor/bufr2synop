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
 \file bufrdeco_tabled_csv.c
 \brief file with the code to read table D (Descriptor sequences)
 */
#include "bufrdeco.h"

/*!
  \fn int bufr_read_tabled_csv ( struct bufrdeco *b )
  \brief Reads a file with table D content according with WMO csv format
  \param b pointer to a target struct \ref bufrdeco
  \param error string where to set error if any

  If succeded return 0, otherwise 1
*/
int bufr_read_tabled_csv ( struct bufrdeco *b )
{
  char aux[32], *c;
  char *tk[16];
  int nt;
  FILE *t;
  size_t ix = 0;
  size_t i = 0;
  int j0, nj = 0;
  char oldkey[8];
  char caux[256];
  char laux[CSV_MAXL];
  struct bufr_descriptor desc;
  struct bufr_tabled *td = &(b->tables->d);

  if ( td->path[0] == 0 )
    {
      return 1;
    }

  // Check if we've already readed this table.
  if ( strcmp ( td->path, td->old_path ) == 0 )
    {
      return 0; // all done
    }

  strcpy_safe ( caux, td->path );
  memset ( td, 0, sizeof ( struct bufr_tabled ) );
  strcpy_safe ( td->path,caux );
  if ( ( t = fopen ( td->path, "r" ) ) == NULL )
    {
      snprintf ( b->error, sizeof (b->error),"Unable to open table D file '%s'\n", td->path );
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
      if ( parse_csv_line ( &nt, tk, laux ) < 0 || ( nt != 2 && nt != 4) )
        {
          snprintf ( b->error, sizeof (b->error),"Error parsing csv line from table D file '%s' found %d tokens in line %lu\n", td->path, nt, i );
          return 1;
        }

      // item fields
      strcpy(td->item[i].key, tk[0]);
      strcpy_safe(td->item[i].key2, tk[1]);

      if (nt == 4 && tk[2][0])
        strcpy_safe (td->item[i].description, tk[2])
      else
        td->item[i].description[0] = 0;

      if (nt == 4 && tk[3][0])
        strcpy_safe(td->item[i].description2, tk[3])
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
      strcpy_safe ( oldkey, tk[0] );

      ix = strtoul ( tk[0], &c, 10 );
      uint32_t_to_descriptor ( &desc, ix );

      if ( td->num[desc.x] == 0 )
        {
          td->x_start[desc.x] = i;  // marc the start
        }
      ( td->num[desc.x] ) ++;

      // Now emule ECMWF format
      snprintf ( td->l[i], sizeof (td->l[i]), "           %s", tk[1] );
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
  strcpy_safe ( td->old_path, td->path ); // store latest path
  return 0;
}
