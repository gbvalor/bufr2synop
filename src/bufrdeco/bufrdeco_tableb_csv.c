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
 \file bufrdeco_tableb_csv.c
 \brief file with the code to read table B data in csv format (code and flag tables from WMO files)
 */
#include "bufrdeco.h"

/*!
  \fn int bufr_read_tableb_csv ( struct bufr_tableb *tb, char *error )
  \brief Read a Table B file from a WMO csv formatted file and set the result in a struct \ref bufr_tableb
  \param tb pointer to the struct \ref bufr_tableb where to set the results
  \param error string with the error explanation if any

  Note that this function assumes that file is csv formatted as WMO table B
  Return 0 if success, 1 otherwise
*/
int bufr_read_tableb_csv ( struct bufr_tableb *tb, char *error )
{
  char *c;
  FILE *t;
  size_t i = 0;
  int nt;
  uint32_t ix;
  char l[256];
  char caux[256];
  char *tk[16];
  struct bufr_descriptor desc;

  if ( tb->path[0] == 0 )
    {
      return 1;
    }

  // If we've already readed this table. We just regenerate the table with original values
  if ( strcmp ( tb->path, tb->old_path ) == 0 )
    {
      //printf ("Reutilizo tablas\n");
      for ( i = 0; i < tb->nlines ; i++ )
        {
          tb->item[i].scale = tb->item[i].scale_ori;
          tb->item[i].reference = tb->item[i].reference_ori;
          tb->item[i].nbits = tb->item[i].nbits_ori;
          tb->item[i].changed = 0;
        }
      return 0; // all done
    }

  strcpy ( caux, tb->path );
  memset ( tb, 0, sizeof ( struct bufr_tableb ) );
  strcpy ( tb->path, caux );
  if ( ( t = fopen ( tb->path, "r" ) ) == NULL )
    {
      sprintf ( error,"Unable to open table B file '%s'\n", tb->path );
      return 1;
    }

  // read first line, it is ignored
  fgets( l, 256, t);
  
  while ( fgets ( l, 256, t ) != NULL && i < BUFR_MAXLINES_TABLEB )
    {
      // Parse line
      if (parse_csv_line(&nt, tk, l) < 0)
      {
        sprintf ( error,"Error parsing csv line from table B file '%s'\n", tb->path );
        return 1;
      }
          
      // First we build the descriptor
      ix = strtoul ( tk[0], &c, 10 );
      uint32_t_to_descriptor ( &desc, ix );
      tb->item[i].changed = 0; // Original from table B
      tb->item[i].x = desc.x; // x
      tb->item[i].y = desc.y; // y
      strcpy ( tb->item[i].key, desc.c ); // key
      if ( tb->num[desc.x] == 0)
        {
          tb->x_start[desc.x] = i;  // marc the start
        }
      tb->y_ref[desc.x][desc.y] = i - tb->x_start[desc.x]; // marc the position from start of first x
      (tb->num[desc.x])++;
      
      // detailed name
      if (tk[2][0])
        sprintf(tb->item[i].name, "%s %s", tk[1], tk[2]);
      else
        strcpy(tb->item[i].name, tk[1]);
      
      // unit
      strcpy(tb->item[i].unit, tk[3]);

      // escale
      tb->item[i].scale_ori = strtol ( tk[4], &c, 10 );
      tb->item[i].scale = tb->item[i].scale_ori;

      // reference
      tb->item[i].reference_ori = strtol ( tk[5], &c, 10 );
      tb->item[i].reference = tb->item[i].reference_ori;

      // bits
      tb->item[i].nbits_ori = strtol ( tk[6], &c, 10 );
      tb->item[i].nbits = tb->item[i].nbits_ori;

      i++;
    }
  tb->x_start[0] = 0; // fix the start for x = 0
  fclose ( t );
  tb->nlines = i;
  strcpy ( tb->old_path, tb->path ); // store latest path
  return 0;
}

