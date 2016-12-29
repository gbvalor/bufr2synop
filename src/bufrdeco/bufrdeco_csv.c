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
 \file bufrdeco_csv.c
 \brief This file has the code of useful routines for read or write csv formatted lines
*/
#include "bufrdeco.h"

#define CSV_WAIT_ITEM 1
#define CSV_FINISHED_ITEM 2
#define CSV_IS_ITEM 4
#define CSV_IS_SEPARATOR 8
#define CSV_IS_CITED 16
#define CSV_MAXL 256

const char CSV_SPACE[] = " \t\r";
const char CSV_SEPARATOR = ',';
const char CSV_CITE = '\"';
const char CSV_FINAL = '\n';

/*!
  \fn int parse_csv_line(int *nt, char *tk[], char *lin)
  \brief Parse a csv line 
  \param nt pointer to a integer. On success is the number of items found
  \param tk array of pointers. Every pointer is a item on success
  \param lin input line which is modified in this routine to be splitted into items 
  
  NOTE that input line is modified
  On success return 0, otherwise -1
*/
int parse_csv_line ( int *nt, char *tk[], char *lin )
{
  size_t i, k = 0, l, latest_char = 0;
  int flag;
  char c;

  l = strlen ( lin );
  if ( l > CSV_MAXL || l == 0 )
    return -1;

  flag = CSV_WAIT_ITEM;

  for ( i = 0, k = 0; i < l; i++ )
    {
      c = lin[i];

      if ( ( flag & CSV_IS_CITED ) == 0 )
        {
          if ( flag & CSV_WAIT_ITEM )
            {
              if ( strchr ( CSV_SPACE, c ) != NULL )
                continue;
              else if ( c == CSV_CITE )
                {
                  lin[i] = '\0';
                  flag |= CSV_IS_CITED;
                  continue;
                }
              else if ( c == CSV_SEPARATOR )
                {
                  /* item vacio */
                  lin[i] = '\0';
                  tk[k] = lin + i;
                  k++;
                  flag = CSV_WAIT_ITEM;
                  continue;
                }
              else if ( c == CSV_FINAL )
                {
                  if ( flag & CSV_FINISHED_ITEM )
                    {
                      *nt = k;
                      return 0;
                    }
                  /* al menos un token, vacio */
                  lin[i] = '\0';
                  tk[k] = lin + i;
                  *nt = k + 1;
                  return 0;
                }
              else
                {
                  latest_char = i;
                  tk[k] = lin + i;
                  flag = CSV_IS_ITEM;
                  continue;
                }
            }
          else if ( flag & CSV_IS_ITEM )
            {
              if ( strchr ( CSV_SPACE, c ) != NULL )
                continue;
              else if ( c == CSV_SEPARATOR )
                {
                  lin[latest_char + 1] = '\0';
                  k++;
                  flag = CSV_WAIT_ITEM | CSV_FINISHED_ITEM;
                  continue;
                }
              else if ( c == CSV_FINAL )
                {
                  lin[latest_char + 1] = '\0';
                  k++;
                  *nt = k;
                  return 0;
                }
              else
                {
                  latest_char = i;
                }
            }
        }
      else /* CITED */
        {
          if ( flag & CSV_WAIT_ITEM )
            {
              tk[k] = lin + i;
              flag &= ( ~CSV_WAIT_ITEM ); // Clean wait item bit
            }
          if ( c == CSV_CITE )
            {
              lin[i] = '\0';
              flag = CSV_WAIT_ITEM;
              k++;
            }
          else if ( c == CSV_FINAL )
            {
              lin[i] = '\0';
              *nt = k + 1;
              return 0;
            }
          continue;
        }
    }
  return -1;
}
