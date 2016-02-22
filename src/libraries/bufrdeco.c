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

const char DEFAULT_BUFRTABLES_DIR1[] = "/usr/local/lib/bufrtables/";
const char DEFAULT_BUFRTABLES_DIR2[] = "/usr/lib/bufrtables/";

/*!
  \fn int bufrdeco_read_bufr ( struct bufr *b,  char *filename, char *error )
  \brief Read bufr file and does preliminary and first decode pass
  \param b pointer to struct \ref bufr
  \param filename complete path of BUFR file
  \param error string where to explain error if nedeed

  This function does the folowing tasks:
  - Read the file and checks the marks at the begining and end to see wheter is a BUFR file
  - Init the structs and allocate the needed memory
  - Splits and parse the BUFR sections (without expanding descriptors nor parsing data)
  - Reads the needed Table files and store them in memory.

  Returns 0 if all is OK, 1 otherwise
 */
int bufrdeco_read_bufr ( struct bufr *b,  char *filename, char *error )
{
  int aux;
  uint8_t *bufrx = NULL, *c;
  size_t ix, ud, n = 0;
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
      return 1;
    }

  /* Inits bufr struct */
  if ( init_bufr ( b, ( size_t ) st.st_size ) )
    {
      sprintf ( error, "bufrdeco_read_bufr(): Cannot init bufr struct\n" );
      free ( ( void * ) bufrx );
      return 1;
    }


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
      free ( ( void * ) bufrx );
      clean_bufr ( b );
      return 1;
    }

  // check if begins with BUFR
  if ( bufrx[0] != 'B' || bufrx[1] != 'U' || bufrx[2] != 'F' || bufrx[3] != 'R' )
    {
      sprintf ( error, "bufrdeco_read_bufr(): file '%s' does not begin with 'BUFR' chars\n", filename );
      free ( ( void * ) bufrx );
      clean_bufr ( b );
      return 1;
    }

  // check if end with '7777'
  if ( bufrx[n - 4] != '7' || bufrx[n - 3] != '7' || bufrx[n - 2] != '7' || bufrx[n - 1] != '7' )
    {
      sprintf ( error, "bufrdeco_read_bufr(): file '%s' does not end with '7777' chars\n", filename );
      free ( ( void * ) bufrx );
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
      free ( ( void * ) bufrx );
      clean_bufr ( b );
      return 1;
    }

  // set bufr edition number
  b->sec0.edition = bufrx[7];

  // raw
  memcpy ( &b->sec0.raw[0], &bufrx[0], 8 );

  /******************* section 1 *****************************/
  c = &bufrx[8]; // pointer to begin of sec1

  b->sec1.length = three_bytes_to_uint32 ( c );
  b->sec1.master = c[3];
  b->sec1.centre = two_bytes_to_uint32 ( &c[4] );
  b->sec1.subcentre = two_bytes_to_uint32 ( &c[6] );
  b->sec1.update = c[8];
  b->sec1.options = c[9];
  b->sec1.category = c[10];
  b->sec1.subcategory = c[11];
  b->sec1.subcategory_local = c[12];
  b->sec1.master_version = c[13];
  b->sec1.master_local = c[14];
  b->sec1.year = two_bytes_to_uint32 ( &c[15] );
  b->sec1.month = c[17];
  b->sec1.day = c[18];
  b->sec1.hour = c[19];
  b->sec1.minute = c[20];
  b->sec1.second = c[21];
  memcpy ( b->sec1.raw, c, b->sec1.length ); // raw data
  c += b->sec1.length;

  /******************* section 2 (Optional) ******************/
  if ( b->sec1.options & 0x80 )
    {
      b->sec2.length = three_bytes_to_uint32 ( c );
      memcpy ( b->sec2.raw, c, b->sec2.length );
      c += b->sec2.length;
    }

  /******************* section 3 *****************************/
  b->sec3.length = three_bytes_to_uint32 ( c );
  b->sec3.subsets = two_bytes_to_uint32 ( &c[4] );
  if ( c[6] & 0x80 )
    b->sec3.observed = 1;
  if ( c[6] & 0x40 )
    b->sec3.compressed = 1;
  // loop of unexpanded descriptors
  for ( ix = 7, ud = 0; ix < b->sec3.length && ud < BUFR_LEN_UNEXPANDED_DESCRIPTOR; ix += 2, ud += 1 )
    {
      two_bytes_to_descriptor ( &b->sec3.unexpanded[ud], &c[ix] );
    }
  b->sec3.ndesc = ud;
  memcpy ( b->sec3.raw, c, b->sec3.length );
  c += b->sec3.length;

  /******************* section 4 *****************************/
  b->sec4.length = three_bytes_to_uint32 ( c );
  memcpy ( b->sec4.raw, c, b->sec4.length );
  b->sec4.bit_offset = 32; // the first bit in byte 4
  free ( ( void * ) bufrx );

  // get tablenames
  if ( get_ecmwf_tablenames ( b, NULL ) )
    {
      sprintf ( error, "bufrdeco_read_bufr(): Cannot find bufr tebles\n" );
      clean_bufr ( b );
      return 1;
    }

  // read tables
  if ( bufr_read_tableb ( & ( b->table->b ), error ) )
    {
      clean_bufr ( b );
      return 1;
    }
  if ( bufr_read_tablec ( & ( b->table->c ), error ) )
    {
      clean_bufr ( b );
      return 1;
    }
  if ( bufr_read_tabled ( & ( b->table->d ), error ) )
    {
      clean_bufr ( b );
      return 1;
    }

  return 0;
}

/*!
  \fn void print_sec0_info(struct bufr *b)
  \brief Prints info from sec0
  \param b pointer to the source struct \ref bufr
*/
void print_sec0_info ( struct bufr *b )
{
  printf ( "#### SEC 0 INFO ###\n" );
  printf ( "Bufr length:           %5u\n", b->sec0.bufr_length );
  printf ( "Bufr edition:          %5u\n", b->sec0.edition );
}

/*!
  \fn void print_sec1_info(struct bufr *b)
  \brief Prints info from sec1
  \param b pointer to the source struct \ref bufr
*/
void print_sec1_info ( struct bufr *b )
{
  printf ( "\n#### SEC 1 INFO ###\n" );
  printf ( "Sec1 length:           %5u\n", b->sec1.length );
  printf ( "Bufr master table:     %5u\n", b->sec1.master );
  printf ( "Centre:                %5u\n", b->sec1.centre );
  printf ( "Sub-Centre:            %5u\n", b->sec1.subcentre );
  printf ( "Update sequence:       %5u\n", b->sec1.update );
  printf ( "Options:               %5x\n", b->sec1.options );
  printf ( "Category:              %5u\n", b->sec1.category );
  printf ( "Subcategory:           %5u\n", b->sec1.subcategory );
  printf ( "Sub-category local:    %5u\n", b->sec1.subcategory_local );
  printf ( "Master table version:  %5u\n", b->sec1.master_version );
  printf ( "Master table local:    %5u\n", b->sec1.master_local );
  printf ( "Year:                  %5u\n", b->sec1.year );
  printf ( "Month:                 %5u\n", b->sec1.month );
  printf ( "Day:                   %5u\n", b->sec1.day );
  printf ( "Hour:                  %5u\n", b->sec1.hour );
  printf ( "Minute:                %5u\n", b->sec1.minute );
  printf ( "Second:                %5u\n", b->sec1.second );
  printf ( "Aditional space:       %5u\n", b->sec1.length - 22 );
}

/*!
  \fn void print_sec3_info(struct bufr *b)
  \brief Prints info from sec3
  \param b pointer to the source struct \ref bufr
*/
void print_sec3_info ( struct bufr *b )
{
  size_t i;
  printf ( "\n#### SEC 3 INFO ###\n" );
  printf ( "Sec3 length:           %5u\n", b->sec3.length );
  printf ( "Subsets:               %5u\n", b->sec3.subsets );
  printf ( "Observed:              %5u\n", b->sec3.observed );
  printf ( "Compressed:            %5u\n", b->sec3.compressed );
  printf ( "Unexpanded descriptors %5u\n", b->sec3.ndesc );
  for ( i = 0; i < b->sec3.ndesc; i++ )
    {
      printf ( "  %3lu:                      %u %02u %03u\n", i, b->sec3.unexpanded[i].f,
               b->sec3.unexpanded[i].x, b->sec3.unexpanded[i].y );
    }
}

/*!
  \fn void print_sec4_info(struct bufr *b)
  \brief Prints info from sec3
  \param b pointer to the source struct \ref bufr
*/
void print_sec4_info ( struct bufr *b )
{
  printf ( "\n#### SEC 4 INFO ###\n" );
  printf ( "Sec4 length:           %5u\n", b->sec4.length );
}
