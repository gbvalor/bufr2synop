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
 \file bufrdeco.c
 \brief This file has the code of user level routines for library bufrdeco
*/
#include "bufrdeco.h"

int bufrdeco_read_bufr(struct bufr *b,  char *filename, char *error)
{
  int aux;
  uint8_t *bufrx;
  size_t n = 0;
  FILE *fp;
  struct stat st;

  /* Open input file */
  if ( stat ( filename, &st) < 0)
  {
      sprintf ( error, "bufrdeco_read_bufr(): cannot stat file '%s'\n", filename );
      return 1;
  }
  
  /* Alloc nedeed memory for bufr */
  if (S_ISREG(st.st_mode) || S_ISLNK(st.st_mode))
  {
    if ( (bufrx = (uint8_t *) calloc( 1, st.st_size)) == NULL)
    {
      sprintf ( error, "bufrdeco_read_bufr(): cannot alloc memory for file '%s'\n", filename );
      return 1;
    }
  }
  else
  {
    sprintf( error, "bufrdeco_read_bufr(): '%s' is not a regular file nor symbolic link\n", filename );
  }

  /* Inits bufr struct */
  init_bufr(b, (size_t) st.st_size);

  
  // Open and read the file
  if ( ( fp = fopen ( filename, "r" ) ) == NULL )
    {
      sprintf ( error, "bufrdeco_read_bufr(): cannot open file '%s'\n", filename );
      free( (void *) bufrx);
      return 1;
    }

  while ( ( aux = fgetc ( fp ) ) != EOF && ( int ) n < st.st_size )
    bufrx[n++] = (uint8_t) aux;

  // close the file
  fclose ( fp );

  // Some fast checks
  if ( n < 8 )
  {
    sprintf(error, "bufrdeco_read_bufr(): Too few bytes for a bufr\n");
    return 1;
  }
  
  // check if begins with BUFR
  if ( bufrx[0] != 'B' || bufrx[1] != 'U' || bufrx[2] != 'F' || bufrx[3] != 'R' )
  {
    sprintf(error, "bufrdeco_read_bufr(): file '%s' does not begin with 'BUFR' chars\n", filename);
    return 1;
  }
  
  // check if end with '7777'
  if ( bufrx[n - 4] != '7' || bufrx[n - 3] != '7' || bufrx[n - 2] != '7' || bufrx[n - 1] != '7' )
  {
    sprintf(error, "bufrdeco_read_bufr(): file '%s' does not end with '7777' chars\n", filename);
    return 1;
  }
  
  
  return 0;
}