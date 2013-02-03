/***************************************************************************
 *   Copyright (C) 2012 by Guillermo Ballester Valor   *
 *   gbv@ogimet.com   *
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
 \file bufr_decode.c
 \brief decodes a bufr file using ECMWF bufr library and tries to pass the decoded reports to synop-format

*/
#include "bufr2synop.h"


/*!
  Note that big arrays are defined as global here to avoid segfaults
*/

char BUFR_MESSAGE[BUFR_LEN]; /*!< The array where the bufr file will be stored when readed */

char CNAMES[KELEM][64]; /*!< Array of strings with name of vars */
char CUNITS[KELEM][24]; /*!< Array of strings with name of used units */

int KSUP[9];  /*!< array containing supplementary information */
int KSEC0[3]; /*!< array (size 3) containing Bufr Section 0 information.
                   [0] Length of section 0 in bytes
                   [1] Total length of Bufr message in bytes
                   [2] Bufr Edition number (currently 4) */
int KSEC1[40]; /*!< array of at least 40 words containing Bufr Section 1 information. When Section
                 1 contains data for local use, KSEC1 should be sized accordingly.
                   [0] Length of section 1 in bytes
                   [1] Bufr Edition number (currently 4)
                   [2] Originating centre
                   [3] Update sequence number
                   [4] Flag (presence of Section 2 in the message)
                   [5] Bufr message type ( Bufr Table A)
                   [6] Bufr message subtype (local use)
                   [7] Version number of local table used
                   [8] Year
                   [9] Month
                   [10] Day
                   [11] Hour
                   [12] Minute
                   [13] Bufr Master Table used
                   [14] Version number of Master table used
                   [15] Originating sub-centre
                   [16] International sub-category
                   [17] Second
                   [18-39] Local ADP centre information (byte by byte) */
int KSEC2[4096];/*!< array of 4096 words containing Bufr Section 2 information. ECMWF uses this
                 section to store Report Data Base KEY.
                   [0] Length of Section 2 in bytes
                   [1-4095] Report Data Base KEY in packed form */
int KSEC3[4];   /*!< array of 4 words containing Bufr Section 3 information.
                   [0] Length of Section 3 in bytes
                   [1] Reserved
                   [2] Number of subsets
                   [3] Flag (data type, compression) */
int KSEC4[2];   /*!< array of 2 words containing Section 4 information.
                   [0] Length of Section 4 in bytes
                   [1] Reserved */
int KEY[46];
int KERR; /*!< An INTEGER containing an error code.*/


char CVALS[KVALS][80]; /*!< array of strings with value of data */

double VALUES[KVALS]; /*!< array of KVALS words containing element values.*/
int KTDLST[KELEM]; 
int KTDEXP[KELEM];

char SELF[] = "bufr2synop"; /*!< The name of this binary */
char INPUTFILE[256]; /*!< The pathname of input file */
char OUTPUTFILE[256]; /*!< The pathname of output file */
int VERBOSE; /*!< If != 0 the verbose output */

struct synop_chunks SYN; /*!< struct where to set chunks of synops taken from a bufr subset */

int main ( int argc, char *argv[] )
{

  FILE *fp;
  int length = BUFR_LEN;
  int status = 0;

  unsigned int *kbuff;

  int i;
  int kelem = KELEM, kvals = KVALS;
  int icode = 0, ktdlen, ktdexl;
  int current_ss;

  if (read_arguments(argc, argv))
    {
      fprintf (stderr, "%s: error geting arguments\n", SELF);
      return (EXIT_FAILURE);
    }

  /*     Open input file       */
  if ( ( fp = fopen ( INPUTFILE,"r" ) ) == NULL )
    {
      fprintf (stderr, "cannot open file\n" );
      exit ( EXIT_FAILURE );
    }

  /*     Read in bufr messages */

  while ( status >= 0 )
    {
      status = readbufr ( fp,&BUFR_MESSAGE,&length );

      if ( status == -1 )
        fprintf ( stderr, "End of file.\n" );
      else if ( status == -2 )
        fprintf ( stderr, "Error in file handling\n" );
      else if ( status == -3 )
        fprintf ( stderr, "Too small input array.\n" );
      else if ( status == -4 )
        fprintf ( stderr, "Too small input array.\n" );
      else if (VERBOSE)
        {
          printf ( "It is OK.\n" );
          printf ( "message read " );
          printf ( "%d\n", length );
          printf ( "%s\n", &BUFR_MESSAGE[0] ); // print the string 'BUFR'
        }
      status=-1;

      /*! Fortran routines works with 4-bytes words, so the readed file byte-oriented
          is redefined as an array of unsigned int
      */
      kbuff = ( unsigned int * ) BUFR_MESSAGE;
      length /= 4; // Now length is sized in words

      if (VERBOSE)
        {

          bus012_ ( &length, kbuff , KSUP, KSEC0, KSEC1, KSEC2,  &KERR ) ;
          buprs0_ ( KSEC0 );
          buprs1_ ( KSEC1 );

          // KSUP[5] is the number of subsets
          if ( KSUP[5] > 1 )
            kelem = kvals/KSUP[5];
          else
            kelem = KELEM;

          if ( kelem > KELEM ) kelem = KELEM;
        }

      /*! Here we call to bufrex() rotine, doing all de decode bufr work

       ON INPUT:

         length - An INTEGER variable containing length of Bufr message in words.
         kbuff - An INTEGER array containing Bufr message.
         kelem - An INTEGER variable containing expected number of expanded elements
         kvals - An INTEGER variable containing expected number of data values.

       ON OUTPUT:

         KSEC0 - An INTEGER array (size 3) containing Bufr Section 0 information.
                [0] Length of section 0 in bytes
                [1] Total length of Bufr message in bytes
                [2] Bufr Edition number (currently 4)

         KSEC1 - An INTEGER array of at least 40 words containing Bufr Section 1 information. When Section
                 1 contains data for local use, KSEC1 should be sized accordingly.
                [0] Length of section 1 in bytes
                [1] Bufr Edition number (currently 4)
                [2] Originating centre
                [3] Update sequence number
                [4] Flag (presence of Section 2 in the message)
                [5] Bufr message type ( Bufr Table A)
                [6] Bufr message subtype (local use)
                [7] Version number of local table used
                [8] Year
                [9] Month
                [10] Day
                [11] Hour
                [12] Minute
                [13] Bufr Master Table used
                [14] Version number of Master table used
                [15] Originating sub-centre
                [16] International sub-category
                [17] Second
                [18-39] Local ADP centre information (byte by byte)

         KSEC2 - An INTEGER array of 4096 words containing Bufr Section 2 information. ECMWF uses this
                 section to store Report Data Base KEY.
                 [0] Length of Section 2 in bytes
                 [1-4095] Report Data Base KEY in packed form

         KSEC3 - An INTEGER array of 4 words containing Bufr Section 3 information.
                 [0] Length of Section 3 in bytes
                 [1] Reserved
                 [2] Number of subsets
                 [3] Flag (data type, compression)

         KSEC4 - An INTEGER array of 2 words containing Section 4 information.
                 [0] Length of Section 4 in bytes
                 [1] Reserved

         KSUP - An INTEGER array (size 9) containing supplementary information.
                 [0] Dimension of KSEC1 array
                 [0] Dimension of KSEC2 array
                 [0] Dimension of KSEC3 array
                 [0] Dimension of KSEC4 array
                 [0] Real number of expanded elements
                 [0] Number of subsets
                 [0] Real number of elements in CVALS array
                 [0] Total Bufr message length in bytes
                 [0] Dimension of KSEC0 array

         CNAMES - CHARACTER*64 array of KELEM words containing element names.
         CUNITS - CHARACTER*24 array of KELEM words containing element units.
         VALUES - REAL*8 array of KVALS words containing element values.
         CVALS - CHARACTER*80 array of KVALS containing CCITT IA No.5 element entries.
         KERR - An INTEGER containing an error code.

      */

      KERR = 0;
      bufrex_ ( &length, ( int * ) kbuff, KSUP, KSEC0, KSEC1, KSEC2, KSEC3, KSEC4,
                &kelem, ( char ** ) CNAMES, ( char ** ) CUNITS, &kvals,
                VALUES, ( char ** ) CVALS, &KERR );
      if ( KERR )
        {
          printf("KERR=%d\n", KERR);
          KERR = 0;
        }

      if (VERBOSE)
        {
          /*! Section 2 of the Bufr message is an optional section and every ADP centre can pack
              any information in this section. The Bufr software decodes this local information
              and stores it into KSEC2 array. ECMWF is storing RDB key in the Section 2 of the Bufr
              messages. To print content of the Section 2, subroutine BUUKEY must be called before
              the BUPRS2 routine. */
          printf ( "KSEC3[3]=%d\n", KSEC3[2] );
          buukey_ ( KSEC1, KSEC2, KEY, KSUP, &KERR );

          /*! Returns lists of unexpanded and expanded data descriptors from the Bufr message. 
              The lists contains Bufr Table D sequence numbers, and the Bufr Table B reference numbers. */

          busel_ ( &ktdlen, KTDLST, &ktdexl, KTDEXP, &KERR );

          /*! Prints section 3.
              Prior to calling the BUPRS3 routine, the BUSEL or BUSEL2 routine has to be called to get lists 
              of unexpanded and fully expanded Data descriptors. In the case of multi-subset uncompressed bufr 
              data the expanded list of descriptors might be different for different subsets. */
          buprs3_ ( KSEC3, &ktdlen, KTDLST, &ktdexl, KTDEXP, &kelem, ( char ** ) CNAMES );

          /*! Print data */
          icode = 0;
          current_ss = 1;
          buprt_ ( &icode, &current_ss, &KSEC3[2], &kelem, ( char ** ) CNAMES,
                   ( char ** ) CUNITS, ( char ** ) CVALS,
                   &kvals, VALUES, KSUP, KSEC1, &KERR );
        }
    }

  return KERR;

}

