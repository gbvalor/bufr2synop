/***************************************************************************
 *   Copyright (C) 2013-2014 by Guillermo Ballester Valor                  *
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
   \file bufrnoaa_utils.c
   \brief File with some routines for bufrnoaa binary
*/
#include "bufrnoaa.h"

int is_bufr(unsigned char *b)
{
  if (b[0] == 'R' && b[1] == 'F' && b[2] == 'U' && b[3] == 'B')
    return 1;
  return 0;
}

int is_head(unsigned char *b)
{
  if (b[0] == '*' && b[1] == '*' && b[2] == '*' && b[3] == '*')
    return 1;
  return 0;
}

int is_endb(unsigned char *b)
{
  if (b[0] == '7' && b[1] == '7' && b[2] == '7' && b[3] == '7')
    return 1;
  return 0;
}

/* Subtract the `struct timeval' values X and Y,
   storing the result in RESULT.
   Return 1 if the difference is negative, otherwise 0.
*/

int timeval_substract (struct timeval *result, struct timeval *x,
                       struct timeval *y)
{
  /* Perform the carry for the later subtraction by updating Y. */
  if (x->tv_usec < y->tv_usec)
    {
      int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
      y->tv_usec -= 1000000 * nsec;
      y->tv_sec += nsec;
    }
  if (x->tv_usec - y->tv_usec > 1000000)
    {
      int nsec = (x->tv_usec - y->tv_usec) / 1000000;
      y->tv_usec += 1000000 * nsec;
      y->tv_sec -= nsec;
    }

  /* Compute the time remaining to wait.
     `tv_usec' is certainly positive. */
  result->tv_sec = x->tv_sec - y->tv_sec;
  result->tv_usec = x->tv_usec - y->tv_usec;

  /* Return 1 if result is negative. */
  return x->tv_sec < y->tv_sec;
}

/*!
  \brief returns 1 if selected message 0 otherwise
*/
int bufr_is_selected(char *name)
{
   if (strlen(SEL) == 0)
     return 0;
   if (strchr(SEL, name[1]) == NULL)
     return 0;
   if (name[1] == 'S')
   {
      if (strlen(SELS) == 0)
        return 1;
      else if (strchr(SELS, name[2]) == NULL)
        return 0;
   }

   if (name[1] == 'O')
   {
      if (strlen(SELO) == 0)
        return 1;
      else if (strchr(SELO, name[2]) == NULL)
        return 0;
   }

   if (name[1] == 'U')
   {
      if (strlen(SELU) == 0)
        return 1;
      else if (strchr(SELU, name[2]) == NULL)
        return 0;
   }

   return 1;
}

int date_mtime_from_stat(char *date, struct stat *st)
{
   struct tm tim;
   memset(&tim, 0, sizeof(tim));
   gmtime_r(&(st->st_mtime), &tim);
   strftime(date, 16, "%Y%m%d%H%M%S", &tim);
   return 1; 
}

int mtime_from_stat(char *filename, struct stat *st)
{
   struct utimbuf ut;
   ut.modtime = st->st_mtime;
   ut.actime = st->st_mtime;
   utime(filename, &ut);
   return 1;
}
