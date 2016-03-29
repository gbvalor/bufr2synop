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
/*! \file bufrdeco.c
    \brief This file includes the code to test bufr to TAC libraries using bufrdeco library to decode bufr
*/
#include "bufrdeco.h"
#include "bufr2tac.h"

struct bufr BUFR;
struct bufrdeco_subset_sequence_data SEQ;
struct bufrdeco_compressed_data_references REF;
struct metreport REPORT; /*!< stuct to set the parsed report */
struct bufr_subset_state STATE; /*!< Includes the info when parsing a subset sequence */


char ENTRADA[256];
int DEBUG;

void print_usage ( void )
{
  printf ( "Usage: \n" );
  printf ( "bufrdeco_test2 -i input_file [-h]\n" );
  printf ( "   -h Print this help\n" );
  printf ( "   -D debug output\n" );
  printf ( "   -i Input file. Complete input path file for bufr file\n" );
}

/*!
  \fn int read_args( int _argc, char * _argv[])
  \brief read the arguments from stdio
  \param _argc number of arguments passed
  \param _argv array of arguments

  Returns 1 if succcess, -1 othewise
*/
int read_args ( int _argc, char * _argv[] )
{
  int iopt;

  DEBUG = 0;
  /*
     Read input options
  */
  while ( ( iopt = getopt ( _argc, _argv, "hi:D" ) ) !=-1 )
    switch ( iopt )
      {
      case 'i':
        if ( strlen ( optarg ) < 256 )
          strcpy ( ENTRADA, optarg );
        break;
      case 'D':
        DEBUG = 1;
        break;
      case 'h':
      default:
        print_usage();
        exit ( EXIT_SUCCESS );
      }

  if ( ENTRADA[0] == '\0' )
    {
      printf ( "read_args(): It is needed an input file. Use -i option\n" );
      return -1;
    }
  return 1;
}

/* this is an interface to use bufr2syn */
int bufrdeco_parse_subset_sequence ( struct metreport *m, struct bufrdeco_subset_sequence_data *sq, struct bufr_subset_state *st,
                                     struct bufr *b, char *err )
{
  size_t i;
  int ksec1[40];
  int *kdtlst;
  size_t nlst = b->sec3.ndesc;

  // memory for kdtlst
  if ( ( kdtlst = ( int * ) malloc ( nlst * sizeof ( int ) ) ) == NULL )
    {
      sprintf ( err, "decobufr_parse_subset_sequence(): cannot alloc memory for kdtlst\n" );
      return 1;
    }

  // sets descriptor as integer according to ECMWF
  for ( i = 0; i < nlst ; i++ )
    {
      descriptor_to_integer ( &kdtlst[i], &b->sec3.unexpanded[i] );
    }

  // And now set only used ksec1 elements
  ksec1[5] = b->sec1.category;
  ksec1[6] = b->sec1.subcategory;

  // Finaly we call to bufr2syn library
  return parse_subset_sequence ( m, sq, st, kdtlst, nlst, ksec1, err );

}


int main ( int argc, char *argv[] )
{
  size_t subset;

  char error[256];
  if ( read_args ( argc, argv ) < 0 )
    exit ( EXIT_FAILURE );

  // Check read file
  if ( bufrdeco_read_bufr ( &BUFR, ENTRADA, error ) )
    {
      printf ( "%s", error );
      exit ( EXIT_FAILURE );
    }

  if ( DEBUG )
    {
      print_sec0_info ( &BUFR );
      print_sec1_info ( &BUFR );
      print_sec3_info ( &BUFR );
      print_sec4_info ( &BUFR );
    }

  if ( bufrdeco_parse_tree ( &BUFR ) )
    {
      printf ( "%s", BUFR.error );
      close_bufr ( &BUFR );
      exit ( EXIT_FAILURE );
    }

  if ( DEBUG )
    bufrdeco_print_tree ( &BUFR );

  for ( subset = 0; subset < BUFR.sec3.subsets ; subset++ )
    {
      if ( bufrdeco_decode_data_subset ( &SEQ, &REF, &BUFR ) )
        {
          printf ( "%s", BUFR.error );
          close_bufr ( &BUFR );
          exit ( EXIT_FAILURE );
        }
        
      if ( DEBUG )
      {
	if ((subset == 0) && BUFR.sec3.compressed)
	   print_bufrdeco_compressed_data_references(&REF);
        bufrdeco_print_subset_sequence_data ( &SEQ );
      }
      if ( bufrdeco_parse_subset_sequence ( &REPORT, &SEQ, &STATE, &BUFR, error ) )
        {
          if ( DEBUG )
            fprintf ( stderr, "#%s\n", error );
        }
      else
        {
          print_plain ( stdout, &REPORT );
        }

    }

  close_bufr ( &BUFR );
  bufrdeco_free_subset_sequence_data ( &SEQ );
  exit ( EXIT_SUCCESS );
}

