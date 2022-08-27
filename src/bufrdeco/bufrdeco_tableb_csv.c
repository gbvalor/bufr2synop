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
 \file bufrdeco_tableb_csv.c
 \brief file with the code to read table B data in csv format (code and flag tables from WMO files)
 */
#include "bufrdeco.h"

/*!
  \fn int bufr_read_tableb_csv ( struct bufrdeco b )
  \brief Read a Table B file from a WMO csv formatted file and set the result in a struct \ref bufr_tableb
  \param b pointer to the struct \ref bufrdeco where to set the results

  Note that this function assumes that file is csv formatted as WMO table B
  Return 0 if success, 1 otherwise
*/
int bufr_read_tableb_csv ( struct bufrdeco *b )
{
  char *c;
  FILE *t;
  size_t i = 0;
  int nt;
  uint32_t ix;
  char l[CSV_MAXL];
  char caux[512];
  char *tk[16];
  struct bufr_tableb *tb;
  struct bufr_descriptor desc;

  bufrdeco_assert ( b != NULL );
  
  tb = &(b->tables->b);
  if ( tb->path[0] == 0 )
    {
      return 1;
    }

  // If we've already readed this table. We just regenerate the table with original values
  if ( strcmp ( tb->path, tb->old_path ) == 0 )
    {
#ifdef __DEBUG      
      printf ("# Reused table %s\n", tb->path);
#endif      
      for ( i = 0; i < tb->nlines ; i++ )
        {
          tb->item[i].scale = tb->item[i].scale_ori;
          tb->item[i].reference = tb->item[i].reference_ori;
          tb->item[i].nbits = tb->item[i].nbits_ori;
          tb->item[i].changed = 0;
        }
      return 0; // all done
    }

  strcpy_safe ( caux, tb->path );
  memset ( tb, 0, sizeof ( struct bufr_tableb ) );
  strcpy_safe ( tb->path, caux );
  if ( ( t = fopen ( tb->path, "r" ) ) == NULL )
    {
      snprintf ( b->error, sizeof (b->error),"Unable to open table B file '%s'\n", tb->path );
      return 1;
    }

  // read first line, it is ignored
  fgets ( l, CSV_MAXL, t );

  while ( fgets ( l, CSV_MAXL, t ) != NULL && i < BUFR_MAXLINES_TABLEB )
    {
      // Parse line
      if ( parse_csv_line ( &nt, tk, l ) < 0 || nt != 7 )
        {
          snprintf ( b->error, sizeof (b->error),"Error parsing csv line from table B file '%s'\n", tb->path );
          return 1;
        }

      // First we build the descriptor
      ix = strtoul ( tk[0], &c, 10 );
      uint32_t_to_descriptor ( &desc, ix );
      tb->item[i].changed = 0; // Original from table B
      tb->item[i].x = desc.x; // x
      tb->item[i].y = desc.y; // y
      strcpy_safe ( tb->item[i].key, desc.c ); // key
      if ( tb->num[desc.x] == 0 )
        {
          tb->x_start[desc.x] = i;  // marc the start
        }
      tb->y_ref[desc.x][desc.y] = i - tb->x_start[desc.x]; // marc the position from start of first x
      ( tb->num[desc.x] )++;

      // detailed name
      if ( tk[2][0] )
        {
          snprintf ( caux, sizeof (caux), "%s %s", tk[1], tk[2] );
        }
      else
        {
          strcpy_safe ( caux, tk[1] );
        }

      // and finally copy
      strncpy_safe ( tb->item[i].name, caux, BUFR_TABLEB_NAME_LENGTH );

      // unit
      strcpy_safe ( caux, tk[3] );
      strncpy_safe ( tb->item[i].unit, caux, BUFR_TABLEB_UNIT_LENGTH );

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
  tb->wmo_table = 1;
  strcpy_safe ( tb->old_path, tb->path ); // store latest path
  return 0;
}

