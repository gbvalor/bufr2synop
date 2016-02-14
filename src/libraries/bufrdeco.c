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

int bufrdeco_read_bufr ( struct bufr *b,  char *filename, char *error )
{
  int aux;
  char caux[32];
  struct tm tmx;
  uint8_t *bufrx, *c;
  size_t n = 0;
  FILE *fp;
  struct stat st;

  /* Open input file */
  if ( stat ( filename, &st ) < 0 )
    {
      sprintf ( error, "bufrdeco_read_bufr(): cannot stat file '%s'\n", filename );
      return 1;
    }

  /* Alloc nedeed memory for bufr */
  if ( S_ISREG ( st.st_mode ) || S_ISLNK ( st.st_mode ) )
    {
      if ( ( bufrx = ( uint8_t * ) calloc ( 1, st.st_size ) ) == NULL )
        {
          sprintf ( error, "bufrdeco_read_bufr(): cannot alloc memory for file '%s'\n", filename );
          return 1;
        }
    }
  else
    {
      sprintf ( error, "bufrdeco_read_bufr(): '%s' is not a regular file nor symbolic link\n", filename );
    }

  /* Inits bufr struct */
  init_bufr ( b, ( size_t ) st.st_size );


  // Open and read the file
  if ( ( fp = fopen ( filename, "r" ) ) == NULL )
    {
      sprintf ( error, "bufrdeco_read_bufr(): cannot open file '%s'\n", filename );
      free ( ( void * ) bufrx );
      clean_bufr ( b );
      return 1;
    }

  while ( ( aux = fgetc ( fp ) ) != EOF && ( int ) n < st.st_size )
    bufrx[n++] = ( uint8_t ) aux;

  // close the file
  fclose ( fp );

  // Some fast checks
  if ( n < 8 )
    {
      sprintf ( error, "bufrdeco_read_bufr(): Too few bytes for a bufr\n" );
      clean_bufr ( b );
      return 1;
    }

  // check if begins with BUFR
  if ( bufrx[0] != 'B' || bufrx[1] != 'U' || bufrx[2] != 'F' || bufrx[3] != 'R' )
    {
      sprintf ( error, "bufrdeco_read_bufr(): file '%s' does not begin with 'BUFR' chars\n", filename );
      clean_bufr ( b );
      return 1;
    }

  // check if end with '7777'
  if ( bufrx[n - 4] != '7' || bufrx[n - 3] != '7' || bufrx[n - 2] != '7' || bufrx[n - 1] != '7' )
    {
      sprintf ( error, "bufrdeco_read_bufr(): file '%s' does not end with '7777' chars\n", filename );
      clean_bufr ( b );
      return 1;
    }

  /******************* section 0  *****************************/

  // we have already checked about begin and end

  // length of whole messge
  b->sec0.bufr_length = three_bytes_to_uint32 ( &bufrx[4] );
  // check if length is correct
  if ( b->sec0.bufr_length != ( uint32_t ) n )
    {
      sprintf ( error, "bufrdeco_read_bufr(): file '%s' have %u bytes and it says %u\n", filename,
                ( uint32_t ) n, b->sec0.bufr_length );
      clean_bufr ( b );
      return 1;
    } 
  
  // set bufr edition number
  b->sec0.edition = bufrx[7];
  
  // raw
  memcpy(&b->sec0.raw[0], &bufrx[0], 8);

  /******************* section 1 *****************************/
  c = &bufrx[8]; // pointer to begin of sec1
  
  b->sec1.length = three_bytes_to_uint32(c);
  b->sec1.master = c[3];
  b->sec1.centre = two_bytes_to_uint32(&c[4]);
  b->sec1.subcentre = two_bytes_to_uint32(&c[6]);
  b->sec1.update = c[8];
  b->sec1.options = c[9];
  b->sec1.category = c[10];
  b->sec1.subcategory = c[11];
  b->sec1.subcategory_local = c[12];
  b->sec1.master_version = c[13];
  b->sec1.master_local = c[14];
  sprintf(caux,"%04u%02u%02u%02u%02u%02u", two_bytes_to_uint32(&c[15]), c[17],c[18],c[19],c[20],c[21]);
  memset(&tmx, 0, sizeof(struct tm));
  strptime(caux, "%Y%m%d%%H%M%S",&tmx);
  b->sec1.datetime = timegm(&tmx);
  return 0;
}

