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
   \file bufrnoaa_io.c
   \brief File with I/O code for bufrnoaa binary
*/
#include "bufrnoaa.h"

void print_usage ( void )
{
  printf ( "Usage: \n" );
  printf ( "bufrnoaa -i input_file [-h][-f][-l][-F prefix][-T T2_selection][-O selo][-S sels][-U selu]\n" );
  printf ( "   -h Print this help\n" );
  printf ( "   -i Input file. Complete input path file for NOAA *.bin bufr archive file\n" );
  printf ( "   -l list the names of reports in input file\n" );
  printf ( "   -f Extract selected reports and write them in files, one per bufr message, as \n" );
  printf ( "      example '20110601213442_ISIE06_SBBR_012100_RRB.bufr'. First field in name is input file timestamp \n" );
  printf ( "      Other fields are from header\n" );
  printf ( "   -F prefix. Builds an archive file with the same format as NOAA one but just with selected messages\n" );
  printf ( "      witgh option  -T. Resulting name is 'prefix_input_filename'\n" );
  printf ( "      If no -F option no archive bin file is created.\n" );
  printf ( "      If no message is selected, the a void file is created.\n" );
  printf ( "      File timestamp is the same than input file\n" );
  printf ( "   -T T2_selection. A string with selection. A character per type (T2 code)\n" );
  printf ( "      'S' = Surface . 'O'= Oceanographic. 'U'= upper air\n" );
  printf ( "      If no -T argument then nothing is selected\n" );
  printf ( "   -S sels. String with selection for A1 when T2='S'\n" );
  printf ( "      By default all A1 are selected\n" );
  printf ( "   -O selo. String with selection for A1 when T2='O'\n" );
  printf ( "      By default all A1 are selected\n" );
  printf ( "   -U sels. String with selection for A1 when T2='U'\n" );
  printf ( "      By default all A1 are selected\n" );
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

  ENTRADA[0] = '\0';
  SEL[0] = '\0';
  SELS[0] = '\0';
  SELO[0] = '\0';
  SELU[0] = '\0';
  PREFIX[0] =  '\0';
  SELECT = 0;
  INDIVIDUAL = 0;
  COLECT = 0;
  LISTF = 0;
  VERBOSE = 1;

  /*
     Read input options
  */
  while ( ( iopt = getopt ( _argc, _argv, "hi:flF:O:qS:T:U:" ) ) !=-1 )
    switch ( iopt )
      {
      case 'i':
        if ( strlen ( optarg ) < 256 )
          strcpy ( ENTRADA, optarg );
        break;
      case 'f':
        INDIVIDUAL = 1;
        break;
      case 'q':
        VERBOSE = 0;
        break;
      case 'l':
        LISTF = 1;
        break;
      case 'F':
        if ( strlen ( optarg ) && strlen ( optarg ) < 64 )
          {
            strcpy ( PREFIX, optarg );
            COLECT = 1;
          }
        break;
      case 'T':
        if ( strlen ( optarg ) < 64 )
          {
            strcpy ( SEL, optarg );
            SELECT = 1;
          }
        break;
      case 'S':
        if ( strlen ( optarg ) < 64 )
          {
            strcpy ( SELS, optarg );
          }
        break;
      case 'O':
        if ( strlen ( optarg ) < 64 )
          {
            strcpy ( SELO, optarg );
          }
        break;
      case 'U':
        if ( strlen ( optarg ) < 64 )
          {
            strcpy ( SELU, optarg );
          }
        break;
      case 'h':
      default:
        print_usage();
        exit ( EXIT_SUCCESS );
      }

  if ( ENTRADA[0] == '\0' )
    {
      printf ( "%s: It is needed an input file. Use -i option\n", OWN );
      return -1;
    }
  return 1;
}

