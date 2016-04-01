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
/*! \file bufrtotac.c
    \brief This file includes the code to test bufr to TAC libraries using bufrdeco library to decode bufr
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

  /**** Big loop. a cycle per file ****/
  while ( get_bufrfile_path ( INPUTFILE, ERR ) )
    {
      //printf ( "%s\n", INPUTFILE );
      if ( bufrdeco_read_bufr ( &BUFR, INPUTFILE) )
        {
	  if (DEBUG)
	    printf("# %s\n", BUFR.error);
          NFILES++;
          bufrdeco_reset(&BUFR);
          continue;
        }

      /* Try to guess a GTS header from filename*/
      GTS_HEADER = guess_gts_header ( &BUFR.header , INPUTFILE );
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
	  if (DEBUG)
          printf ( "# %s", BUFR.error );
          NFILES++;
          bufrdeco_reset(&BUFR);
          continue;
        }
      if ( VERBOSE )
        bufrdeco_print_tree ( &BUFR );

      for ( subset = 0; subset < BUFR.sec3.subsets ; subset++ )
        {
          if ( (seq = bufrdeco_get_subset_sequence_data ( &BUFR )) == NULL)
            {
              if ( DEBUG )
                printf ( "# %s", BUFR.error );
              goto fin;
            }

          if ( VERBOSE )
            {
              if ( ( subset == 0 ) && BUFR.sec3.compressed )
                print_bufrdeco_compressed_data_references ( &(BUFR.refs) );
              bufrdeco_print_subset_sequence_data ( seq );
            }

          if (BUFR.sec3.ndesc &&  bufrdeco_parse_subset_sequence ( &REPORT, &STATE, &BUFR, ERR ) )
            {
              if ( DEBUG )
                fprintf ( stderr, "# %s\n", ERR );
            }
          else
            {
              print_plain ( stdout, &REPORT );
            }

        }
      fin:;
      bufrdeco_reset(&BUFR);
      NFILES ++;
    } // End of big loop parsing files

  bufrdeco_close(&BUFR);
  exit ( EXIT_SUCCESS );
}



