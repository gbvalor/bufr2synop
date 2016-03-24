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
    \brief This file includes the code to debug and test bufrdeco library
*/
#include "bufrdeco.h"

struct bufr BUFR;
struct bufrdeco_subset_sequence_data SEQ;
struct bufrdeco_compressed_data_references REF;

char ENTRADA[256];

void print_usage ( void )
{
  printf ( "Usage: \n" );
  printf ( "bufrdeco_test -i input_file [-h]\n" );
  printf ( "   -h Print this help\n" );
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

  /*
     Read input options
  */
  while ( ( iopt = getopt ( _argc, _argv, "hi:" ) ) !=-1 )
    switch ( iopt )
      {
      case 'i':
        if ( strlen ( optarg ) < 256 )
          strcpy ( ENTRADA, optarg );
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

  print_sec0_info ( &BUFR );
  print_sec1_info ( &BUFR );
  print_sec3_info ( &BUFR );
  print_sec4_info ( &BUFR );

  if ( bufrdeco_parse_tree ( &BUFR ) )
    {
      printf ( "%s", BUFR.error );
      close_bufr ( &BUFR );
      exit ( EXIT_FAILURE );
    }

  bufrdeco_print_tree ( &BUFR );

  for ( subset = 0; subset < BUFR.sec3.subsets ; subset++ )
    {
      if ( bufrdeco_decode_data_subset ( &SEQ, &REF, &BUFR ) )
        {
          printf ( "%s", BUFR.error );
          close_bufr ( &BUFR );
          exit ( EXIT_FAILURE );
        }
      bufrdeco_print_subset_sequence_data(&SEQ);
    }

  printf ( "So far so good !!\n" );
  close_bufr ( &BUFR );
  bufrdeco_free_subset_sequence_data(&SEQ);
  exit ( EXIT_SUCCESS );
}

