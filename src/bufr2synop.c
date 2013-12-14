/***************************************************************************
 *   Copyright (C) 2012 by Guillermo Ballester Valor                       *
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
 \file bufr2synop.c
 \brief decodes a bufr file using ECMWF bufr library and tries to pass the decoded reports to synop-format

 */
#include "bufr2synop.h"

/*!
   Note that big arrays are defined as global here to avoid segfaults
*/

unsigned char BUFR_MESSAGE[BUFR_LEN]; /*!< The array where the bufr file will be stored when readed */

char CNAMES[KELEM][64]; /*!< Array of strings with name of vars */

char CUNITS[KELEM][24]; /*!< Array of strings with name of used units */

int KSUP[9]; /*!< array containing supplementary information */

int KSEC0[3]; /*!< array (size 3) containing Bufr Section 0 information.
    [0] Length of section 0 in bytes
    [1] Total length of Bufr message in bytes
    [2] Bufr Edition number (currently 4) */

int KSEC1[40]; /*!< array of at least 40 words containing Bufr Section 1 information.
 When Section 1 contains data for local use, KSEC1 should be sized accordingly.
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
   [13] Bufr Master Table used (zero if standard WMO FM 94-IX BUFR tables are used)
   [14] Version number of Master table used
   [15] Originating sub-centre
   [16] International sub-category
   [17] Second
   [18-39] Local ADP centre information (byte by byte) */

int KSEC2[4096];/*!< array of 4096 words containing Bufr Section 2 information. ECMWF uses this
 section to store Report Data Base KEY.
   [0] Length of Section 2 in bytes
   [1-4095] Report Data Base KEY in packed form */

int KSEC3[4]; /*!< array of 4 words containing Bufr Section 3 information.
   [0] Length of Section 3 in bytes
   [1] Reserved
   [2] Number of subsets
   [3] Flag (data type, compression) */

int KSEC4[2]; /*!< array of 2 words containing Section 4 information.
   [0] Length of Section 4 in bytes
   [1] Reserved */

int KEY[46];

int KERR; /*!< An INTEGER containing an error code.*/

char ERR[256]; /*!< string with an error */

char CVALS[KVALS][80]; /*!< array of strings with value of data */

double VALUES[KVALS]; /*!< array of KVALS words containing element values.*/

int KTDLST[KELEM]; /*!< array with list of descriptor in sec3 */

int KTDEXP[KELEM]; /*!< array with expanded list of descriptors */

char SELF[] = "bufr2synop"; /*!< The name of this binary */
char BUFRTABLES_DIR[256]; /*!< Directory for BUFR tables set by user */
char INPUTFILE[256]; /*!< The pathname of input file */
char OUTPUTFILE[256]; /*!< The pathname of output file */
int VERBOSE; /*!< If != 0 the verbose output */
int SHOW_SEQUENCE; /*!< Output explained sequence */
int SHOW_ECMWF_OUTPUT; /*!< Print original output from ECMWF library */
int DEBUG; /*!< Show debug information */

size_t NLINES_TABLEC; /*!< current number of TABLE C file lines */
char TYPE[8]; /*! Type of report being parsed  (code MMMM) */
char TABLEC[MAXLINES_TABLEC][92]; /*!< Here is where store the lines from table C file*/
char REPORT[2048]; /*!< string to set the report */
char LINAUX[2048]; // output line


struct bufr_subset_sequence_data SUBSET; /*!< ALl data decoded for a subset*/

struct synop_chunks SYN; /*!< struct where to set chunks of synops taken from a bufr subset */

char DEFAULT_BUFRTABLES[] = "/usr/local/lib/bufrtables/"; /*!< Default bufr tables dir */

struct synop_chunks SYNOP;
struct buoy_chunks BUOY;

/*!
  \fn int main(int argc, char *argv[])
  \brief the standard C main entry function
  \param argc Number of arguments entered in standard input, including the the name of this binary.
  \param argv the array of strings with the arguents

*/
int main(int argc, char *argv[])
{

  FILE *fp;
  int length = BUFR_LEN; // max length of a bufrfile in bytes
  int status = 0; // initial status
  struct bufr_descriptor d; // bufr descriptor
  unsigned int *kbuff;
  char aux[256]; // auxiliar string
  char *c; // auxiliar pointer for sprintf tasks
  int i, j, k,nsub, nsub1;
  int kelem = KELEM, kvals = KVALS;
  int icode = 0, ktdlen, ktdexl;
  int current_ss;
  char cunits[26], cnames[66],cvals[82];

  if (read_arguments(argc, argv))
    {
      fprintf(stderr, "%s: error geting arguments\n", SELF);
      return (EXIT_FAILURE);
    }

  // set needed enviroment before use bufr library
  set_environment();

  /* Read in bufr messages */
  //status = readbufr ( fp, &BUFR_MESSAGE, &length );
  if ((status = read_bufr(BUFR_MESSAGE, INPUTFILE, &length)))
    {
      if (status == -1)
        fprintf(stderr, "File '%s' do not begin with 'BUFR'.\n", INPUTFILE);
      else if (status == -2)
        fprintf(stderr, "FILE '%s' do not end with '7777'.\n", INPUTFILE);
      else if (status == -3)
        fprintf(stderr, "FILE '%s' has different size than expected.\n",
                INPUTFILE);
      else if (status == -4)
        fprintf(stderr, "Too small input FILE.\n");
      exit(EXIT_FAILURE);
    }
  else if (DEBUG)
    {
      printf("#It is OK.\n");
      printf("#message read ");
      printf("%d\n", length);
      printf("#%s\n", &BUFR_MESSAGE[0]); // print the string 'BUFR'
    }

  /* Fortran routines works with 4-bytes words, so the readed file byte-oriented
   is redefined as an array of unsigned int
   */
  kbuff = (unsigned int *) BUFR_MESSAGE;
  length /= 4; // Now length is sized in words

  if (SHOW_ECMWF_OUTPUT)
    {

      bus012_(&length, kbuff, KSUP, KSEC0, KSEC1, KSEC2, &KERR);
      buprs0_(KSEC0);
      buprs1_(KSEC1);

      // KSUP[5] is the number of subsets
      if (KSUP[5] > 1)
        kelem = kvals / KSUP[5];
      else
        kelem = KELEM;

      if (kelem > KELEM)
        kelem = KELEM;
    }

  /* Here we call to bufrex() rotine, doing all de decode bufr work

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
   [13] Bufr Master Table used (zero if standard WMO FM 94-IX BUFR tables are used)
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
   [1] Dimension of KSEC2 array
   [2] Dimension of KSEC3 array
   [3] Dimension of KSEC4 array
   [4] Real number of expanded elements
   [5] Number of subsets
   [6] Real number of elements in CVALS array
   [7] Total Bufr message length in bytes
   [8] Dimension of KSEC0 array

   CNAMES - CHARACTER*64 array of KELEM words containing element names.
   CUNITS - CHARACTER*24 array of KELEM words containing element units.
   VALUES - REAL*8 array of KVALS words containing element values.
   CVALS - CHARACTER*80 array of KVALS containing CCITT IA No.5 element entries.
   KERR - An INTEGER containing an error code.

   */

  KERR = 0;
  bufrex_(&length, (int *) kbuff, KSUP, KSEC0, KSEC1, KSEC2, KSEC3, KSEC4,
          &kelem, (char **) CNAMES, (char **) CUNITS, &kvals, VALUES,
          (char **) CVALS, &KERR);

  if (KERR)
    {
      printf("KERR=%d\n", KERR);
      KERR = 0;
    }

  // Check about the WMO master table
  if (KSEC1[13])
    {
      fprintf(stderr,"Sorry, we only accept WMO BUFR master Table\n");
      exit(EXIT_FAILURE);
    }

  if (KSEC1[5] != 0 && KSEC1[5] != 1 && KSEC1[5] != 2 && KSEC1[5] !=  7)
    {
      fprintf(stderr,"The data category %d is not parsed at the moment");
      exit(EXIT_FAILURE);
    }

  if (KSUP[5] > KSUBS)
    {
      fprintf(stderr,"%s: Warning, not all the %d subsets for this file can be managed, just %d\n", SELF, KSUP[5], KSUBS);
    }

  /*
   Expand the descriptors

   kdelen - A pointer to an INTEGER variable containing number of data descriptors in KTDLST array
   KTDLST - An INTEGER array containing the list of kdtlen data descriptors
   kdtexl - A pointer to an INTEGER variable containing number of expanded data descriptors
   KTDEXP - An INTEGER array containing the list of KTDEXL data descriptors
   KERR - An INTEGER containing error code.
  */
  busel_(&ktdlen, KTDLST, &ktdexl, KTDEXP, &KERR);

  /*
   read table C if VERBOSE
  */
  if (VERBOSE)
    {
      if (read_table_c() == 0)
        {
          fprintf(stderr,"Cannot read C Table  File\n");
          exit(EXIT_FAILURE);
        }
    }

  if (DEBUG)
    {
      for (i = 0; i < ktdlen; i++)
        {
          integer_to_descriptor(&d, KTDLST[i]);
          printf("#KTDLST[%03d]=%d %02d %03d\n", i, d.f, d.x, d.y);
        }
    }

  // loop for every subset
  for (nsub = 0; nsub < KSUP[5]; nsub++)
    {
      nsub1 = nsub + 1;

      memset(&SUBSET, 0, sizeof( struct bufr_subset_sequence_data));

      /*
         Expand the descriptors for a subset

         NOTE that not all subset have the same descriptor expanded list because of replication factor
         so is safer expand the subset descriptors individually

         Also NOTE that all integers must be called by reference
      */
      busel2_(&nsub1, &kelem, &ktdlen, (char **)KTDLST, &ktdexl, (char **)KTDEXP, (char **)CNAMES, (char **)CUNITS, &KERR);

      if (KERR)
        continue;

      for (j = 0 ; j < ktdexl && j < KSUBS ; j++)
        {
          i = nsub * KELEM + j;
          LINAUX[0] = '\0'; // clean the output line
          c = LINAUX;
          charray_to_string(SUBSET.sequence[j].name, CNAMES[j], 64);
          charray_to_string(SUBSET.sequence[j].unit, CUNITS[j], 24);
          integer_to_descriptor(&SUBSET.sequence[j].desc, KTDEXP[j]);

          c += sprintf(c, "KTDEXP[%03d]=%s |%03d |", j, SUBSET.sequence[j].desc.c, nsub);
          c += sprintf(c, "'%s'|", SUBSET.sequence[j].name);
          if (VALUES[i] != MISSING_REAL)
            {
              if (strstr(SUBSET.sequence[j].unit,"CCITTIA5") != NULL)
                {
                  SUBSET.sequence[j].mask |= DESCRIPTOR_HAVE_STRING_VALUE;
                  k = ((int)VALUES[i]) % 1000;
                  charray_to_string(SUBSET.sequence[j].cval, CVALS[(int)(VALUES[i]/1000.0) - 1], k);
                  c += sprintf(c, "'%s'", SUBSET.sequence[j].cval);
                }
              else if (strstr(SUBSET.sequence[j].unit,"CODE TABLE") == SUBSET.sequence[j].unit)
                {
                  SUBSET.sequence[j].mask |= DESCRIPTOR_IS_CODE_TABLE;
                  SUBSET.sequence[j].val = VALUES[i];
                  if (VERBOSE)
                    {
                      if (get_explained_table_val (SUBSET.sequence[j].ctable, 256, &SUBSET.sequence[j].desc, (int) VALUES[i]) != NULL)
                        {
                          SUBSET.sequence[j].mask |= DESCRIPTOR_HAVE_CODE_TABLE_STRING;
                          c += sprintf(c, "%9d : '%s'", (int)VALUES[i], SUBSET.sequence[j].ctable);
                        }
                      else
                        {
                          c += sprintf(c,"%9d : 'NOT FOUND'", (int)VALUES[i]);
                        }
                    }
                  else
                    {
                      c += sprintf(c, "%9d", (int)VALUES[i]);
                    }
                }
              else if (strstr(SUBSET.sequence[j].unit,"FLAG") == SUBSET.sequence[j].unit)
                {
                  SUBSET.sequence[j].mask |= DESCRIPTOR_IS_FLAG_TABLE;
                  SUBSET.sequence[j].val = VALUES[i];
                  if (VERBOSE)
                    {
                      if (get_explained_flag_val (SUBSET.sequence[j].ctable, 256, &SUBSET.sequence[j].desc, (unsigned long) VALUES[i]) != NULL)
                        {
                          SUBSET.sequence[j].mask |= DESCRIPTOR_HAVE_FLAG_TABLE_STRING;
                          c += sprintf(c, "%9d : '%s'", (int)VALUES[i], SUBSET.sequence[j].ctable);
                        }
                      else
                        {
                          c += sprintf(c, "%9d : 'NOT FOUND'", (int)VALUES[i]);
                        }
                    }
                  else
                    {
                      c += sprintf(c, "%9d", (int)VALUES[i]);
                    }
                }
              else
                {
                  SUBSET.sequence[j].mask |= DESCRIPTOR_HAVE_REAL_VALUE;
                  SUBSET.sequence[j].val = VALUES[i];
                  c += sprintf(c, "%14.4lf %s ", VALUES[i], SUBSET.sequence[j].unit);
                }
            }
          else
            {
              SUBSET.sequence[j].mask |= DESCRIPTOR_VALUE_MISSING;
              c += sprintf(c, "   MISSING     %s ", SUBSET.sequence[j].unit);
            }
          SUBSET.nd = j;
          c += sprintf(c, "\n");
          if (SHOW_SEQUENCE && strlen(LINAUX))
            printf("#%s", LINAUX);
        }

      /**** the call to parse the sequence and transform to solicited asciicode, if possible *****/
      if (parse_subset_sequence(&SUBSET, KTDLST, ktdlen, KSEC1, ERR) && DEBUG)
        {
          fprintf(stderr, "#%s\n", ERR);
        }
    }


  if (SHOW_ECMWF_OUTPUT)
    {
      /* Section 2 of the Bufr message is an optional section and every ADP centre can pack
       any information in this section. The Bufr software decodes this local information
       and stores it into KSEC2 array. ECMWF is storing RDB key in the Section 2 of the Bufr
       messages. To print content of the Section 2, subroutine BUUKEY must be called before
       the BUPRS2 routine. */
      if (KSEC1[4])
        buukey_(KSEC1, KSEC2, KEY, KSUP, &KERR);

      /* Returns lists of unexpanded and expanded data descriptors from the Bufr message.
       The lists contains Bufr Table D sequence numbers, and the Bufr Table B reference numbers. */

      busel_(&ktdlen, KTDLST, &ktdexl, KTDEXP, &KERR);

      /* Prints section 3.
       Prior to calling the BUPRS3 routine, the BUSEL or BUSEL2 routine has to be called to get lists
       of unexpanded and fully expanded Data descriptors. In the case of multi-subset uncompressed bufr
       data the expanded list of descriptors might be different for different subsets. */
      buprs3_(KSEC3, &ktdlen, KTDLST, &ktdexl, KTDEXP, &kelem,
              (char **) CNAMES);

      /* Print data */
      icode = 0;
      current_ss = 1;
      buprt_(&icode, &current_ss, &KSEC3[2], &kelem, (char **) CNAMES,
             (char **) CUNITS, (char **) CVALS, &kvals, VALUES, KSUP, KSEC1,
             &KERR);
    }

  if (DEBUG)
    {
      for (i = 0; i < 3 ; i++)
        printf("#KSEC0[%d] = %d\n", i,  KSEC0[i]);
      for (i = 0; i < 40 ; i++)
        printf("#KSEC1[%d] = %d\n", i, KSEC1[i]);
      if (KSEC1[4])
        {
          for (i = 0; i < 4096; i++)
            printf("#KSEC2[%d] = %d\n", i, KSEC2[i]);
        }
      for (i = 0; i < 4; i++)
        printf("#KSEC3[%d] = %d\n", i, KSEC3[i]);

      for (i = 0; i < 9; i++)
        printf("#KSUP[%d] = %d\n", i, KSUP[i]);
    }

  return KERR;

}

