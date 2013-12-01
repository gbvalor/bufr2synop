/***************************************************************************
 *   Copyright (C) 2012 by Guillermo Ballester Valor   *
 *   gbv@ogimet.com   *
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
  \file bufr2syn_utils.c
  \brief file with the utility routines for binary bufr2synop 
*/
#include "bufr2synop.h"


/*!
  \fn three_bytes_to_uint(const unsigned char *source)
  \brief returns the integer value from an array of three bytes, most significant first
*/
unsigned int three_bytes_to_uint(const unsigned char *source)
{
   return  (source[2] + source[1] * 256 + source[0] * 65536);
}

/*!
  \fn int integer_to_descriptor(struct bufr_descriptor *d, int id)
  \brief parse an integer with a descriptor fom bufr ECWMF libary
  \param d pointer to a struct \ref bufr_descriptor where to set the result on output
  \param id integer with the descriptor from ewcwf
*/
int integer_to_descriptor(struct bufr_descriptor *d, int id)
{
   if (d == NULL)
     return 1;
   d->f = id / 100000;
   d->x = (id % 100000) / 1000;
   d->y = id % 1000;
   sprintf(d->c, "%06d", id);
   return 0;
}

/*!
  \fn char * charray_to_string(char *s, unsigned char *buf, size_t size)
  \brief get a null termitated c-string from an array of unsignde chars
  \param s resulting string
  \param buf pointer to first element in array
  \param size number of chars in array
*/
char * charray_to_string(char *s, unsigned char *buf, size_t size)
{
   // copy
   memcpy(s , buf, size);
   // add final string mark
   s[size] = '\0';
   return s;
}

/*!
  \fn char * adjust_string(char *s)
  \brief Supress trailing blanks of a string
  \param s string to process
*/
char * adjust_string(char *s)
{
   size_t l;

   while (l && s[--l] == ' ') 
     s[l] = '\0';
   return s;
}
