/***************************************************************************
 *   Copyright (C) 2013-2022 by Guillermo Ballester Valor                  *
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
struct bufr2tac_error_stack ERRS; /*!< struct to store warnings and errors */

const char SELF[]= "bufrtotac"; /*! < the name of this binary */
char ERR[256]; /*!< string with an error */
char BUFRTABLES_DIR[256]; /*!< Directory for BUFR tables set by user */
char LISTOFFILES[256]; /*!< The pathname of a file which includes a list of bufr files to parse */
char INPUTFILE[256]; /*!< The pathname of input file */
char OFFSETFILE[BUFRDECO_PATH_LENGTH + 8]; /*< The path name of optional file with bit offsets for non-compressed BUFR. */
char OUTPUTFILE[256]; /*!< The pathname of output file */
int VERBOSE; /*!< If != 0 the verbose output */
int SHOW_SEQUENCE; /*!< Output explained sequence */
int DEBUG; /*!< Show debug information */
int NFILES; /*!< The amount of files processed  */
int GTS_HEADER; /*!< If == 1 GTS header have been guessed from filename */
int XML; /*!< If == 1 then output is in xml format */
int JSON; /*!< If == 1 then output is in json format */
int CSV; /*!< If == 1 then output is in csv format */
int EXTRACT; /*!< if != 0 then the decoder tries to extract an embebed bufr in a file seraching for a first '7777' after first 'BUFR' */
int ECMWF; /*!< If == 1 then use tables from ECMWF package */
int HTML; /*!< If == 1 then output is in HTML format */
int NOTAC; /*!< if == 1 then do not decode to TAC */
int FIRST_SUBSET; /*!< First subset index in output. First available is 0 */
int LAST_SUBSET; /*!< Last subset index in output. First available is 0 */
int PRINT_WIGOS_ID; /*!< if != 0 then print wigos id in output */
int PRINT_GEO; /*!< if != 0 then print latitude, longitude and altitude */
int READ_OFFSETS; /*!< if != then read bit offsets */
int WRITE_OFFSETS; /*!< if != 0 then write bit offsets */
int USE_CACHE; /*!< if != 0 then use cache of tables */
int SUBSET ; /*!< Index of subset in a BUFR being parsed */
int PRINT_JSON_DATA; /*!< If != 0 then the data subset is in json format */
int PRINT_JSON_SEC0;
int PRINT_JSON_SEC1;
int PRINT_JSON_SEC2;
int PRINT_JSON_SEC3;
int PRINT_JSON_EXPANDED_TREE;

FILE *FL; /*!< Buffer to read the list of files */
FILE *OUT; /*!< Buffer to write to OUTPUTFILE */
#ifdef DEBUG_TIME
  clock_t clk_start, clk_end;
#endif

int main ( int argc, char *argv[] )
{
  int first_subset, last_subset;
  char subset_id[32];
  struct bufrdeco_subset_sequence_data *seq;


  if ( bufrtotac_read_args ( argc, argv ) < 0 )
    exit ( EXIT_FAILURE );

  // init bufr struct
#ifdef DEBUG_TIME      
      clk_start = clock ();
#endif
  if ( bufrdeco_init ( &BUFR ) )
    {
      printf ( "%s(): Cannot init bufr struct\n", SELF );
      return 1;
    }
    
#ifdef DEBUG_TIME        
      clk_end = clock();  
      print_timing (clk_start,clk_end,bufrdeco_init());
#endif      

  /**** set bitmask according with args readed from shell ****/    
  bufrtotac_set_bufrdeco_bitmask (&BUFR);
  
  /**** Set bufr tables dir ****/
  strcpy ( BUFR.bufrtables_dir, BUFRTABLES_DIR );

  /**** Big loop. a cycle per file. Get input filenames from LISTOFFILES[] ****/
  while ( get_bufrfile_path ( INPUTFILE, OFFSETFILE, ERR ) )
    {
#ifdef __DEBUG      
      printf ( "####### %s ######\n", INPUTFILE );
#endif      
      if ( DEBUG )
        printf ( "# %s\n", INPUTFILE );

      // The following call to bufrdeco_read_bufr() does the folowing tasks:
      // - Read the file and checks the marks at the begining and end to see wheter is a BUFR file
      // - Init the structs and allocate the needed memory if not done previously
      // - Splits and parse the BUFR sections (without expanding descriptors nor parsing data)
      // - Reads the needed Table files and store them in memory.
      //
      // If EXTRACT != 0 then the function bufrdeco_extract_bufr() is used instead of bufrdeco_read_bufr()
      // This act in the same way, but search and extract the first BUFR embebed in a file.
#ifdef DEBUG_TIME      
      clk_start = clock ();
#endif
      
      if ( ( EXTRACT && bufrdeco_extract_bufr ( &BUFR, INPUTFILE ) ) ||
           ( EXTRACT == 0 && bufrdeco_read_bufr ( &BUFR, INPUTFILE ) ) )
        {
          if ( DEBUG )
            printf ( "# %s\n", BUFR.error );
          NFILES++;
          bufrdeco_reset ( &BUFR );
          continue;
        }
#ifdef DEBUG_TIME        
      clk_end = clock();  
      print_timing (clk_start,clk_end,bufrdeco_extract_bufr());
#endif      

      // Check if have to read bit offsets file
      if ( READ_OFFSETS &&
           BUFR.sec3.compressed == 0 &&
           BUFR.sec3.subsets > 1 )
        {
          #ifdef DEBUG_TIME      
             clk_start = clock ();
          #endif
          bufrdeco_read_subset_offset_bits ( &BUFR, OFFSETFILE );
#ifdef DEBUG_TIME        
          clk_end = clock();  
          print_timing (clk_start,clk_end,bufrdeco_read_subset_offset_bits());
#endif      
        }

      /* Try to guess a GTS header from filename*/
      GTS_HEADER = guess_gts_header ( &BUFR.header, INPUTFILE );   // GTS_HEADER = 1 if succeeded
      if ( GTS_HEADER && DEBUG )
        printf ( "# Guessed GTS Header: %s %s %s %s %s\n", BUFR.header.timestamp, BUFR.header.bname, BUFR.header.center,
                 BUFR.header.dtrel, BUFR.header.order );

      /* Prints sections if verbose */
      if ( VERBOSE )
        {
          print_sec0_info ( &BUFR );
          print_sec1_info ( &BUFR );
          print_sec3_info ( &BUFR );
          print_sec4_info ( &BUFR );
        }

      // To get any data from any subset  we need to parse the tree
#ifdef DEBUG_TIME      
      clk_start = clock ();
#endif
      if ( bufrdeco_parse_tree ( &BUFR ) )
        {
          if ( DEBUG )
            printf ( "# %s", BUFR.error );
          NFILES++;
          bufrdeco_reset ( &BUFR );
          continue;
        }
#ifdef DEBUG_TIME        
      clk_end = clock();  
      print_timing (clk_start,clk_end,bufrdeco_parse_tree());
#endif      
      if (PRINT_JSON_EXPANDED_TREE)
        bufrdeco_print_json_tree( &BUFR);
      
      if ( VERBOSE )
        bufrdeco_print_tree ( &BUFR );

      first_subset = FIRST_SUBSET;
      last_subset = LAST_SUBSET;
      
      // Fix first and last subset
      if ( first_subset >= ( int ) BUFR.sec3.subsets )
        goto fin;

      if ( last_subset < first_subset)
        last_subset = BUFR.sec3.subsets - 1;

      for ( SUBSET = first_subset; SUBSET <= last_subset ; SUBSET++ )
        {
#ifdef DEBUG_TIME      
          clk_start = clock ();
#endif          
          if ( ( seq = bufrdeco_get_target_subset_sequence_data ( SUBSET, &BUFR ) ) == NULL )
            {
              if ( DEBUG )
                printf ( "# %s", BUFR.error );
              goto fin;
            }
#ifdef DEBUG_TIME        
          clk_end = clock();  
          print_timing (clk_start, clk_end,bufrdeco_get_target_subset_sequence_data());
#endif      

          if ( VERBOSE )
            {
              if ( ( SUBSET == first_subset ) && BUFR.sec3.compressed )
                print_bufrdeco_compressed_data_references ( & ( BUFR.refs ) );
              if ( BUFR.mask & BUFRDECO_OUTPUT_HTML )
                {
                  sprintf ( subset_id, "subset_%d", SUBSET );
                  bufrdeco_print_subset_sequence_data_tagged_html ( seq, subset_id );
                }
              else
                bufrdeco_print_subset_sequence_data ( seq );
            }

          if ( ! NOTAC )
            {
              // Here we perform the decode to TAC
#ifdef DEBUG_TIME      
              clk_start = clock ();
#endif          
              if ( BUFR.sec3.ndesc &&  bufrtotac_parse_subset_sequence ( &REPORT, &STATE, &BUFR, ERR ) )
                {
                  if ( DEBUG )
                    fprintf ( stderr, "# %s\n", ERR );
                }
#ifdef DEBUG_TIME        
              clk_end = clock();  
              print_timing (clk_start, clk_end, bufrtotac_parse_subset_sequence());
#endif      

              // And here print the results
              if ( XML )
                {
                  if ( SUBSET == 0 )
                    fprintf ( OUT, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" );
                  print_xml ( stdout, &REPORT );
                }
              else if ( JSON )
                {
                  print_json ( OUT, &REPORT );
                }
              else if ( CSV )
                {
                  if ( SUBSET == 0 )
                    fprintf ( OUT, "TYPE,FILE,DATETIME,INDEX,NAME,COUNTRY,LATITUDE,LONGITUDE,ALTITUDE,REPORT\n" );
                  print_csv ( OUT, &REPORT );
                }
              else if ( HTML )
                {
                  print_html ( OUT, &REPORT );
                }
              else
                {
                  print_plain ( OUT, &REPORT );
                }
            }
        }
    fin:
      ;

      // check if has to write bit offsets file
      if ( BUFR.sec3.compressed == 0 &&
           BUFR.sec3.subsets > 1 &&
           WRITE_OFFSETS )
        {
          bufrdeco_write_subset_offset_bits ( &BUFR, OFFSETFILE );
        }

#ifdef DEBUG_TIME      
      clk_start = clock ();
#endif          
      bufrdeco_reset ( &BUFR );
#ifdef DEBUG_TIME        
      clk_end = clock();  
      print_timing (clk_start, clk_end, bufrdeco_reset());
#endif      
      NFILES ++;
    } // End of big loop parsing files

  bufrdeco_close ( &BUFR );
  
  // Close the file if needed
  if (OUTPUTFILE[0])
    fclose (OUT);
  
  exit ( EXIT_SUCCESS );
}
