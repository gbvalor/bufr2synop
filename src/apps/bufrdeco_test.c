/***************************************************************************
 *   Copyright (C) 2013-2022 by Guillermo Ballester Valor                 *
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
/*! \file bufrdeco_test.c
    \brief This file includes the code to debug and test bufrdeco library
*/
#include "bufrdeco.h"

struct bufrdeco BUFR;
int EXTRACT;
char ENTRADA[256];
int PRINT_JSON_SEC0; /*!< If != 0 Prints Sec 0 information in json format */
int PRINT_JSON_SEC1; /*!< If != 0 Prints Sec 1 information in json format */
int PRINT_JSON_SEC2; /*!< If != 0 Prints Sec 2 information in json format */
int PRINT_JSON_SEC3; /*!< If != 0 Prints Sec 3 information in json format */
int PRINT_JSON_SEC4; /*!< If != 0 Prints Sec 4 data in json format */
int PRINT_JSON_EXPANDED_TREE; /*!< If != 0 Prints expanded tree in json format */
int FIRST_SUBSET; /*!< First subset to parse */
int LAST_SUBSET; /*!< Last subset to parse */

void print_usage ( void )
{
  printf ( "Usage: \n" );
  printf ( "bufrdeco_test -i input_file [-h][more optional args...]\n" );
  printf ( "   -h Print this help\n" );
  printf ( "   -i Input file. Complete input path file for bufr file\n" );
  printf ( "   -J. Information, tree and data in json format. Equivalent to option -E01234\n" );
  printf ( "   -S first..last . Print only results for subsets in range first..last (First subset available is 0). Default is all subsets\n" );
  printf ( "   -T. Print expanded tree in json format\n" );
  printf ( "   -X. Extract first BUFR buffer found in input file (from first 'BUFR' item to next '7777')\n" );
  printf ( "   -0. Prints BUFR Sec 0 information in json format\n" );
  printf ( "   -1. Prints BUFR Sec 1 information in json format\n" );
  printf ( "   -2. Prints BUFR Sec 2 information in json format\n" );
  printf ( "   -3. Prints BUFR Sec 3 information in json format\n" );
  printf ( "   -4. Prints BUFR data in json format\n" );

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
  char *c, aux[128];

  // Default values
  EXTRACT = 0;
  PRINT_JSON_SEC0 = 0;
  PRINT_JSON_SEC1 = 0;
  PRINT_JSON_SEC2 = 0;
  PRINT_JSON_SEC3 = 0;
  PRINT_JSON_EXPANDED_TREE = 0;

  /*
     Read input options
  */
  while ( ( iopt = getopt ( _argc, _argv, "hi:JXT01234" ) ) !=-1 )
    switch ( iopt )
      {
      case 'X':
        EXTRACT = 1;
        break;

      case '0':
        PRINT_JSON_SEC0 = 1;
        break;

      case '1':
        PRINT_JSON_SEC1 = 1;
        break;

      case '2':
        PRINT_JSON_SEC2 = 1;
        break;

      case '3':
        PRINT_JSON_SEC3 = 1;
        break;

      case '4':
        PRINT_JSON_SEC4 = 1;
        break;

      case 'T':
        PRINT_JSON_EXPANDED_TREE = 1;
        break;

      case 'J':
        PRINT_JSON_SEC0 = 1;
        PRINT_JSON_SEC1 = 1;
        PRINT_JSON_SEC2 = 1;
        PRINT_JSON_SEC3 = 1;
        PRINT_JSON_SEC4 = 1;
        PRINT_JSON_EXPANDED_TREE = 1;
        break;

      case 'S':
        // Process subset sequence to show
        if ( strlen ( optarg ) < 128 &&
             strspn ( optarg, "0123456789." ) == strlen ( optarg ) )
          {
            if ( strstr ( optarg, ".." ) != NULL )
              {
                strcpy ( aux, optarg );
                c = strstr ( aux, ".." );
                if ( strlen ( c ) > 2 )
                  {
                    LAST_SUBSET = atoi ( c + 2 );
                  }
                *c = '\0';
                if ( aux[0] )
                  {
                    FIRST_SUBSET = atoi ( aux );
                  }
              }
            else
              {
                FIRST_SUBSET = atoi ( optarg );
                LAST_SUBSET = FIRST_SUBSET;
              }
          }
        break;
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

// set the bit mask according with input arguments
int set_bufrdeco_mask ( struct bufrdeco *b )
{
  if ( PRINT_JSON_SEC0 )
    b->mask |= BUFRDECO_OUTPUT_JSON_SEC0;

  if ( PRINT_JSON_SEC1 )
    b->mask |= BUFRDECO_OUTPUT_JSON_SEC1;

  if ( PRINT_JSON_SEC2 )
    b->mask |= BUFRDECO_OUTPUT_JSON_SEC2;

  if ( PRINT_JSON_SEC3 )
    b->mask |= BUFRDECO_OUTPUT_JSON_SEC3;

  if ( PRINT_JSON_SEC4 )
    b->mask |= BUFRDECO_OUTPUT_JSON_SUBSET_DATA;

  if ( PRINT_JSON_EXPANDED_TREE )
    b->mask |= BUFRDECO_OUTPUT_JSON_EXPANDED_TREE;

  return 0;
}

int main ( int argc, char *argv[] )
{
  size_t subset, first_subset, last_subset;
  struct bufrdeco_subset_sequence_data *seq;

  if ( read_args ( argc, argv ) < 0 )
    exit ( EXIT_FAILURE );

  bufrdeco_init ( &BUFR );

  // sets the bit mask according to readed args
  set_bufrdeco_mask ( &BUFR );

  // Check read file
  if ( EXTRACT )
    {
      if ( bufrdeco_extract_bufr ( &BUFR, ENTRADA ) )
        {
          printf ( "%s", BUFR.error );
          exit ( EXIT_FAILURE );
        }
    }
  else if ( bufrdeco_read_bufr ( &BUFR, ENTRADA ) )
    {
      printf ( "%s", BUFR.error );
      exit ( EXIT_FAILURE );
    }

  if ( bufrdeco_parse_tree ( &BUFR ) )
    {
      printf ( "# %s", BUFR.error );
      bufrdeco_close ( &BUFR );
      exit ( EXIT_FAILURE );
    }

  if ( PRINT_JSON_EXPANDED_TREE )
    bufrdeco_print_json_tree ( &BUFR );

  first_subset = FIRST_SUBSET;
  last_subset = LAST_SUBSET;

  // Fix first and last subset
  if ( first_subset >= BUFR.sec3.subsets )
    {
      fprintf ( BUFR.err, "Error, first subset to parse is greater than subset number in BUFR\n" );
      exit ( EXIT_FAILURE );
    }

  if ( last_subset < first_subset )
    last_subset = BUFR.sec3.subsets - 1;


  for ( subset = first_subset; subset <= last_subset ; subset++ )
    {
      if ( ( seq = bufrdeco_get_target_subset_sequence_data ( subset, &BUFR ) ) == NULL )
        {
          printf ( "# %s", BUFR.error );
          bufrdeco_close ( &BUFR );
          exit ( EXIT_FAILURE );
        }
    }

  //printf ( "So far so good !!\n" );
  bufrdeco_close ( &BUFR );
  exit ( EXIT_SUCCESS );
}

