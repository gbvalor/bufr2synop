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
char * csv_quoted_string ( char *out, char *in )
{
  size_t i = 0, j = 0;

  if ( in == NULL || out == NULL)
      return NULL;

  if ( in[0] == 0 )
    return in;

  out[j++] = '\"';
  while ( in[i] && j < ( CSV_MAXL - 1 )  && i < CSV_MAXL )
    {
      if ( in[i] == '\"' )
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
  \fn int parse_csv_line2(int *nt, char *tk[], char *lin)
  \brief Parse a csv line
  \param nt pointer to a integer. On success is the number of items found
  \param tk array of pointers. Every pointer is a item on success
  \param lin input line which is modified in this routine to be splitted into items

  NOTE that input line is modified
  On success return 0, otherwise -1
*/
int parse_csv_line2 ( int *nt, char *tk[], char *lin )
{
  size_t i, j, k = 0, l, latest_char = 0;
  int flag;
  char c, caux[CSV_MAXL];

  bufrdeco_assert (lin != NULL && tk != NULL && nt != NULL);
  
  l = strlen ( lin );
  if ( l >= CSV_MAXL || l == 0 )
    return -1;

  // copy original string. In this case the copy is safe
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

/*!
  \fn int parse_csv_line(int *nt, char *tk[], char *lin)
  \brief Parse a csv line
  \param nt pointer to a integer. On success is the number of items found
  \param tk array of pointers. Every pointer is a item on success
  \param lin input line which is modified in this routine to be splitted into items

  This is an optimization of routine parse_csv_line2(). Here we suppose:
    -All no void items are closed between "" and separed by comas ','
    -No " in items.
    -No blank spaces at the end nor begin of items
  
  NOTE that input line is modified
  On success return 0, otherwise -1
*/
int parse_csv_line ( int *nt, char *tk[], char *lin )
{
  char *cq[CSV_MAXL / 4], *cc[CSV_MAXL / 4], *c0;
  buf_t nc, nq, i, j;

  //bufrdeco_assert (lin != NULL && tk != NULL && nt != NULL);
  
  *nt = 0;

  // clean final new line)
  if ((c0 = strchr(lin, CSV_FINAL)) != NULL)
    *c0 = '\0';

  c0 = lin;
  nq = 0;
  while (c0 != NULL && *c0  && nq < (CSV_MAXL / 4))
  {
      if ((c0 = strchr(c0, CSV_CITE)) != NULL)
     {
       cq[nq++] = c0++;
     }
  }

  if (nq % 2)
    return -1; // Problem, number of quotes are non paired

  c0 = lin;  
  nc = 0;
  while (c0 != NULL && *c0 && nc < (CSV_MAXL / 4))
  {
     if ((c0 = strchr(c0, CSV_SEPARATOR)) != NULL)
     {
        j = 0;
        if (nq)
        {
          for (; j < nq ; j+=2)
            if ((cq[j] < c0) && (c0 < cq[j + 1]))
            {
              j = nq;// is a comma between two cites. Ignored
              c0++;
            }
        }
        if (j == nq)
        {
           // is a comma between two items.
           cc[nc++] = c0++;
        }
     }
  }

  // Now go to tokens, in this pass, still CSV_CITE are in items
  c0 = lin;
  if (nc == 0)
    tk[(*nt)++] = c0;
  else
  {
    for (i = 0; i < nc; i++)
    {
      if (cc[i] == c0)
      {
        tk[(*nt)++] = c0;
        *(c0) = '\0';
        c0++;
      }
      else
      {
        tk[(*nt)++] = c0;
        *(cc[i]) = '\0';
        c0 = cc[i] + 1;
      }
    }
  } 
  tk[(*nt)++] = c0;
  
  // fix the tokens supresing the first CSV_CITE if any
  for (i = 0; i < (buf_t)(*nt); i++)
  {
    if( *(tk[i]) == CSV_CITE)
      (tk[i])++;
  }
  
  // And then supress all CSV_CITE (also in the end of every item)
  for (i = 0; i < nq ; i++)
    *(cq[i]) = '\0';
  
  return 0;
}
