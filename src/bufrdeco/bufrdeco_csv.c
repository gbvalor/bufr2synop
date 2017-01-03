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
 \file bufrdeco_csv.c
 \brief This file has the code of useful routines for read or write csv formatted lines
*/
#include "bufrdeco.h"

#define CSV_WAIT_ITEM 1
#define CSV_FINISHED_ITEM 2
#define CSV_IS_ITEM 4
#define CSV_IS_SEPARATOR 8
#define CSV_IS_CITED 16
#define CSV_WAIT_SEPARATOR 32
#define CSV_IS_DOUBLE_QUOTED 64

const char CSV_SPACE[] = " \t\r";
const char CSV_SEPARATOR = ',';
const char CSV_CITE = '\"';
const char CSV_FINAL = '\n';


/*!
  \fn char * csv_quoted_string( char *out, char *in)
  \brief Transform a string to a quoted string to be inserted in a csv file
  \param out resulting string
  \param in input string
  
  If problem returns NULL
*/
char * csv_quoted_string( char *out, char *in)
{
   size_t i = 0, j = 0;
   
   if ( in[0] == 0)
     return NULL;
   
   out[j++] = '\"';
   while (in[i] && j < (CSV_MAXL - 1)  && i < CSV_MAXL)
   {
       if (in[i] == '\"')
       {
           out[j++] = '\"';
           out[j++] = '\"';
           i++;
       }
       else
         out[j++] = in[i++];
   }
   out[j++] = '\"';
   out[j] = 0; // end of string
   return out;
}


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
  size_t i, j, k = 0, l, latest_char = 0;
  int flag;
  char c, caux[CSV_MAXL];

  l = strlen ( lin );
  if ( l > CSV_MAXL || l == 0 )
    return -1;

  // copy original string
  strcpy ( caux, lin );

  flag = CSV_WAIT_ITEM;

  for ( i = 0, j = 0, k = 0; i < l; i++ )
    {
      c = caux[i]; // original char

      if ( ( flag & CSV_IS_CITED ) == 0 )
        {
          if ( flag & CSV_WAIT_ITEM )
            {
              if ( strchr ( CSV_SPACE, c ) != NULL )
                {
                  lin[j++] = c; // copy to target
                  continue;
                }
              else if ( c == CSV_CITE )
                {
                  lin[j++] = '\0'; // copy to target
                  flag |= CSV_IS_CITED;
                  continue;
                }
              else if ( c == CSV_SEPARATOR )
                {
                  /* item vacio */
                  tk[k++] = lin + j;
                  lin[j++] = '\0';
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
                  tk[k++] = lin + j;
                  lin[j] = '\0';
                  *nt = k;
                  return 0;
                }
              else
                {
                  latest_char = j;
                  tk[k] = lin + j;
                  lin[j++] = c;
                  flag = CSV_IS_ITEM;
                  continue;
                }
            }
          else if ( flag & CSV_IS_ITEM )
            {
              if ( strchr ( CSV_SPACE, c ) != NULL )
                {
                  lin[j++] = c;
                  continue;
                }
              else if ( c == CSV_SEPARATOR )
                {
                  lin[latest_char + 1] = '\0';
                  j = latest_char + 2;
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
                  latest_char = j++;
                }
            }
          else if ( flag & CSV_WAIT_SEPARATOR )
            {
              if ( c == CSV_SEPARATOR )
                {
                  lin[j++] = '\0';
                  k++;
                  flag = CSV_WAIT_ITEM;
                }
              else if ( c == CSV_FINAL )
                {
                  lin[j++] = '\0';
                  *nt = k + 1;
                  return 0;
                }
              continue;
            }
        }
      else /* CITED */
        {
          if ( flag & CSV_WAIT_ITEM )
            {
              tk[k] = lin + j;
              flag &= ( ~CSV_WAIT_ITEM ); // Clean wait item bit
            }
          if ( c == CSV_CITE )
            {
              if ( caux[i + 1] == CSV_CITE )
                {
                  if ( flag & CSV_IS_DOUBLE_QUOTED )
                    {
                      flag &= ( ~CSV_IS_DOUBLE_QUOTED ) ; // Clean double quoted bit
                      lin[j++] = '"';
                      i++;
                    }
                  else
                    {
                      flag |= CSV_IS_DOUBLE_QUOTED;
                      lin[j++] = '"';
                      i++;
                    }
                    continue;
                }
              else
                {
                  lin[j++] = '\0';
                  flag = CSV_WAIT_SEPARATOR;
                }
            }
          else if ( c == CSV_FINAL )
            {
              lin[j++] = '\0';
              *nt = k + 1;
              return 0;
            }
          else
            {
              lin[j++] = c;
            }
          continue;
        }
    }
  return -1;
}
