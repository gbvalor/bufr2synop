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
/*! \file bufrtotac.c
    \brief This file includes the code to test bufr to TAC libraries using bufrdeco library to decode bufr

  \mainpage Bufr to traditional alphanumeric code

  \section Introduction
  This is a package to make the transition to bufr reports from alphanumeric text easiest as possible.

  A lot of software is coded assuming that the primary source of meteorological reports is in alphanumeric format. Decode libraries are expecting this. But time is changing, meteorological services are migrating to bufr and other binary formats. Most decode sofware have to be changed.

  This is a software to get meteorological reports in old alphanumeric format from bufr files. At the moment includes the following reports:

  - FM 12-XIV SYNOP
  - FM 13-XIV SHIP
  - FM 14-XIV SYNOP MOBIL
  - FM 18-XII BUOY
  - FM 35-XI  TEMP
  - FM 36-XI  TEMP SHIP
  - FM 38-XI  TEMP MOBIL
  - FM 71-XII CLIMAT

  The software is based in bufrdc library from ECMWF. Before version 0.7 this package should to be
  installed. Since version 0.7, a library for decode a wide subset of bufr reports has been writen
  from scratch. This is a fast and light bufr decoder. Anyway it still uses the bufr tables installed
  by ECMWF packages so it still need to be installed. You can grab ECMWF library from

  https://software.ecmwf.int/wiki/display/BUFR/Releases

  Note that the results from this library is not intended to match at %100 level to original alphanumeric reports. It cannot. Some variables in alphanumeric code rules can be coded in several ways, and there is not a regional even national decision about them. As example, the 'hshs' item (table code 1617) for synop FM 12 can be coded using 00-80 range or 90-99 one. A numeric value for heigh of base clouds can be coded in two ways. And there some few more examples.

  \section Install

  Remember you have to download and install bufrdc library from ECMWF

  https://software.ecmwf.int/wiki/display/BUFR/Releases

  You also will need a Fortran and C compilers in your system and the usual development packages. GNU gcc and gfortran compilers are ok.

  Then, to install bufr2synop, download the tarball and uncompress it

  \code
  tar -xvzf bufr2synop-X.Y.tar.gz
  \endcode

  Where X.Y is the current version. This will create the directory \a bufr2synop-X.Y

  You then have to configure

  \code
  cd bufr2synop-X.Y
  ./configure
  \endcode

  after a succesfully configuration you then can then compile the package

  \code
  make
  \endcode

  and finally install it.
  \code
  make install
  \endcode

  by default, it will install binaries and libraries under \a /usr/local tree, so you will need root privileges.
*/

#include "bufrtotac.h"

struct bufrdeco BUFR;
struct metreport REPORT; /*!< stuct to set the parsed report */
struct bufr2tac_subset_state STATE; /*!< Includes the info when parsing a subset sequence */

const char SELF[]= "bufrtotac"; /*! < the name of this binary */
char ERR[256]; /*!< string with an error */
char BUFRTABLES_DIR[256]; /*!< Directory for BUFR tables set by user */
char LISTOFFILES[256]; /*!< The pathname of a file which includes a list of bufr files to parse */
char INPUTFILE[256]; /*!< The pathname of input file */
char OUTPUTFILE[256]; /*!< The pathname of output file */
int VERBOSE; /*!< If != 0 the verbose output */
int SHOW_SEQUENCE; /*!< Output explained sequence */
int DEBUG; /*!< Show debug information */
int NFILES; /*!< The amount of files processed  */
int GTS_HEADER; /*!< If == 1 GTS header have been guessed from filename */
int XML; /*!< If == 1 then output is in xml format */
int JSON; /*!< If == 1 then output is in json format */
int CSV; /*!< If == 1 then output is in csv format */
int ECMWF; /*!< If == 1 then use tables from ECMWF package */
int HTML; /*!< If == 1 then output is in HTML format */
FILE *FL; /*!< Buffer to read the list of files */

int main ( int argc, char *argv[] )
{
  size_t subset;
  struct bufrdeco_subset_sequence_data *seq;

  if ( read_args ( argc, argv ) < 0 )
    exit ( EXIT_FAILURE );

  // init bufr struct
  if ( bufrdeco_init ( &BUFR ) )
    {
      printf ( "%s(): Cannot init bufr struct\n", SELF );
      return 1;
    }

  // If needed mark to use ECMWF tables
  if ( ECMWF )
      BUFR.mask |= BUFRDECO_USE_ECMWF_TABLES;

  if ( HTML )
      BUFR.mask |= BUFRDECO_OUTPUT_HTML;

  /**** Big loop. a cycle per file ****/
  while ( get_bufrfile_path ( INPUTFILE, ERR ) )
    {
      //printf ( "%s\n", INPUTFILE );
      if ( DEBUG )
        printf ( "# %s\n", INPUTFILE );

      // The following call to bufrdeco_read_bufr() does the folowing tasks:
      // - Read the file and checks the marks at the begining and end to see wheter is a BUFR file
      // - Init the structs and allocate the needed memory if not done previously
      // - Splits and parse the BUFR sections (without expanding descriptors nor parsing data)
      // - Reads the needed Table files and store them in memory.
      if ( bufrdeco_read_bufr ( &BUFR, INPUTFILE ) )
        {
          if ( DEBUG )
            printf ( "# %s\n", BUFR.error );
          NFILES++;
          bufrdeco_reset ( &BUFR );
          continue;
        }

      /* Try to guess a GTS header from filename*/
      GTS_HEADER = guess_gts_header ( &BUFR.header , INPUTFILE );  // GTS_HEADER = 1 if succeeded
      if ( GTS_HEADER && DEBUG )
        printf ( "#%s %s %s %s %s\n", BUFR.header.timestamp, BUFR.header.bname, BUFR.header.center,
                 BUFR.header.dtrel, BUFR.header.order );

      /* Prints sections if verbose */
      if ( VERBOSE )
        {
          print_sec0_info ( &BUFR );
          print_sec1_info ( &BUFR );
          print_sec3_info ( &BUFR );
          print_sec4_info ( &BUFR );
        }

      if ( bufrdeco_parse_tree ( &BUFR ) )
        {
          if ( DEBUG )
            printf ( "# %s", BUFR.error );
          NFILES++;
          bufrdeco_reset ( &BUFR );
          continue;
        }
      if ( VERBOSE )
        bufrdeco_print_tree ( &BUFR );

      for ( subset = 0; subset < BUFR.sec3.subsets ; subset++ )
        {
          if ( ( seq = bufrdeco_get_subset_sequence_data ( &BUFR ) ) == NULL )
            {
              if ( DEBUG )
                printf ( "# %s", BUFR.error );
              goto fin;
            }

          if ( VERBOSE )
            {
              if ( ( subset == 0 ) && BUFR.sec3.compressed )
                print_bufrdeco_compressed_data_references ( & ( BUFR.refs ) );
              bufrdeco_print_subset_sequence_data ( seq );
            }

          if ( BUFR.sec3.ndesc &&  bufrdeco_parse_subset_sequence ( &REPORT, &STATE, &BUFR, ERR ) )
            {
              if ( DEBUG )
                fprintf ( stderr, "# %s\n", ERR );
            }
          if ( XML )
            {
              if ( subset == 0 )
                fprintf ( stdout, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" );
              print_xml ( stdout, &REPORT );
            }
          else if ( JSON )
            {
              print_json ( stdout, &REPORT );
            }
          else if ( CSV )
            {
              if ( subset == 0 )
                fprintf ( stdout, "TYPE,FILE,DATETIME,INDEX,NAME,COUNTRY,LATITUDE,LONGITUDE,ALTITUDE,REPORT\n" );
              print_csv ( stdout, &REPORT );
            }
          else
            {
              print_plain ( stdout, &REPORT );
            }
        }
    fin:
      ;
      bufrdeco_reset ( &BUFR );
      NFILES ++;
    } // End of big loop parsing files

  bufrdeco_close ( &BUFR );
  exit ( EXIT_SUCCESS );
}



