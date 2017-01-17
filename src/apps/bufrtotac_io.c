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
 \file bufrtotac_io.c
 \brief file with the i/o code for binary bufrtotac
 */

#include "bufrtotac.h"

void print_usage ( void )
{
  printf ( "%s %s\n", SELF, PACKAGE_VERSION );
  printf ( "Usage: \n" );
  printf ( "%s -i input_file [-i input] [-I list_of_files] [-t bufrtable_dir] [-o output] [-s] [-v][-j][-x][-c][-h]\n" , SELF );
  printf ( "       -c. The output is in csv format\n" );
  printf ( "       -D. Print some debug info\n" );
  printf ( "       -E. Use ECMWF package tables. Default is WMO csv tables\n" );
  printf ( "       -h Print this help\n" );
  printf ( "       -i Input file. Complete input path file for bufr file\n" );
  printf ( "       -I list_of_files. Pathname of a file with the list of files to parse, one filename per line\n" );
  printf ( "       -j. The output is in json format\n" );
  printf ( "       -n. Do not try to decode to TAC, just parse BUFR report\n" );
  printf ( "       -o output. Pathname of output file. Default is standar output\n" );
  printf ( "       -s prints a long output with explained sequence of descriptors\n" );
  printf ( "       -t bufrtable_dir. Pathname of bufr tables directory. Ended with '/'\n" );
  printf ( "       -V. Verbose output\n" );
  printf ( "       -v. Print version\n" );
  printf ( "       -x. The output is in xml format\n" );
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

  // Initial and default values
  INPUTFILE[0] = '\0';
  OUTPUTFILE[0] = '\0';
  LISTOFFILES[0] = '\0';
  BUFRTABLES_DIR[0] = '\0';
  VERBOSE = 0;
  SHOW_SEQUENCE = 0;
  DEBUG = 0;
  NFILES = 0;
  XML = 0;
  JSON = 0;
  CSV= 0;
  ECMWF = 0;
  HTML = 0;
  NOTAC = 0;

  /*
     Read input options
  */
  while ( ( iopt = getopt ( _argc, _argv, "cDEhi:jHI:no:st:vVx" ) ) !=-1 )
    switch ( iopt )
      {
      case 'i':
        if ( strlen ( optarg ) < 256 )
          strcpy ( INPUTFILE, optarg );
        break;
      case 'j':
        JSON = 1;
        break;
      case 'I':
        if ( strlen ( optarg ) < 256 )
          strcpy ( LISTOFFILES, optarg );
        break;
      case 'n':
        NOTAC = 1;
        break;
      case 'o':
        if ( strlen ( optarg ) < 256 )
          strcpy ( OUTPUTFILE, optarg );
        break;
      case 't':
        if ( strlen ( optarg ) < 256 )
          {
            strcpy ( BUFRTABLES_DIR, optarg );
          }
        break;
      case 'D':
        DEBUG = 1;
        break;
      case 'E':
        ECMWF = 1;
        break;
      case 'H':
        HTML = 1;
        break;
      case 'V':
        VERBOSE = 1;
        break;
      case 'v':
        printf ( "%s %s\n", SELF, PACKAGE_VERSION );
        exit ( EXIT_SUCCESS );
        break;
      case 's':
        SHOW_SEQUENCE = 1;
        break;
      case 'x':
        XML = 1;
        break;
      case 'c':
        CSV = 1;
        break;
      case 'h':
      default:
        print_usage();
        exit ( EXIT_SUCCESS );
      }

  if ( INPUTFILE[0] == 0 && LISTOFFILES[0] == 0 )
    {
      printf ( "read_args(): It is needed an input file. Use -i or -I option\n" );
      print_usage();
      return -1;
    }
  return 1;
}

/* this is an interface to use bufr2tac */
int bufrdeco_parse_subset_sequence ( struct metreport *m, struct bufr2tac_subset_state *st,
                                     struct bufrdeco *b, char *err )
{
  size_t i;
  int ksec1[40], res;
  int *kdtlst;
  size_t nlst = ( size_t ) b->sec3.ndesc;

  // memory for kdtlst
  if ( ( kdtlst = ( int * ) calloc ( 1, nlst * sizeof ( int ) ) ) == NULL )
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
  ksec1[6] = b->sec1.subcategory_local;

  // Finaly we call to bufr2tac library
  memset ( m, 0, sizeof ( struct metreport ) );
  m->h = &b->header;
  res = parse_subset_sequence ( m, &b->seq, st, kdtlst, nlst, ksec1, err );
  free ( ( void * ) kdtlst );
  return res;
}

/*!
  \fn char * get_bufrfile_path( char *filename, char *err)
  \brief Get bufr file names to parse
  \param filename string where the file is on output
  \param err string where to set the error if any

  If success, it returns a pointer to filename. If there is no more files
  to parse, or error returns NULL.

  This routine is also opens the file with file list and close it when no more
  file available in list

  The content format in LISTOFFILES is compatible with 'ls -1'. Example assume we
  want to parse all *.bufr files in current directory, we can do

  \code
  ls -1 *.bufr > list_file && bufrtotac -I list_file
  \endcode
*/
char * get_bufrfile_path ( char *filename, char *err )
{
  char aux[256], *c;
  if ( LISTOFFILES[0] == 0 )
    {
      if ( NFILES == 0 )
        {
          if ( filename != INPUTFILE )
            strcpy ( filename, INPUTFILE );
          return filename;
        }
      else
        return 0;
    }
  else if ( NFILES == 0 )
    {
      if ( ( FL = fopen ( LISTOFFILES,"r" ) ) == NULL )
        {
          sprintf ( err,"Cannot open '%s'", LISTOFFILES );
          return NULL;
        }
    }
  if ( fgets ( aux, 256, FL ) )
    {
      c =  strrchr ( aux,'\n' );
      if ( c )
        *c = 0;
      strcpy ( filename, aux );
      return filename;
    }
  else
    {
      fclose ( FL );
      return NULL;
    }
}
