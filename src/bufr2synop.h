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
 \file bufr2synop.h
 \brief Include header file for binary bufr2synop 
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <time.h>
#include <sys/stat.h>

// project includes
#include "metsynop.h"
#include "metbuoy.h"

//#define DEBUG

/*!
  \def KVALS
  \brief dimension of arrays of doubles
*/
#define KVALS 1024000

/*!
  \def KELEM
  \brief max dimension of elements for a single report
*/
#define KELEM 4096

/*!
  \def BUFR_LEN
  \brief max length of a bufrfile 
*/
#define BUFR_LEN 512000

/*!
   \def BUFR_OBS_DATA_MASK
   \brief bit mask for Observed data 
*/
#define BUFR_OBS_DATA_MASK 128

/*!
   \def BUFR_COMPRESSED_DATA_MASK
   \brief bit mask for Observed data  
*/
#define BUFR_COMPRESSED_DATA_MASK 64

/*!
  \def MISSING_REAL
  \brief The missing default value for real values
*/
#define MISSING_REAL (1.7e38)

/*!
  \def MISSING_INTEGER
  \brief The missing default value for integer values
*/
#define MISSING_INTEGER (2147483647)

/*!
  \def MAXLINES_TABLEC
  \brief The maximum expected lines in a Table C file
*/
#define MAXLINES_TABLEC 8192

/*!
  \def NMAXSEQ
  \brief Maximum expected descriptor in a expanded sequence for a single subset
*/
#define NMAXSEQ 8192

#define DESCRIPTOR_VALUE_MISSING 1 
#define DESCRIPTOR_HAVE_REAL_VALUE 2
#define DESCRIPTOR_HAVE_STRING_VALUE 4
#define DESCRIPTOR_IS_CODE_TABLE 8
#define DESCRIPTOR_HAVE_CODE_TABLE_STRING 16
#define DESCRIPTOR_HAVE_FLAG_TABLE_STRING 32
#define DESCRIPTOR_IS_FLAG_TABLE 64
#define DESCRIPTIR_IS_A_REPLICATOR 128

/*!
  \struct bufr_descriptor
  \brief BUFR descriptor
*/
struct bufr_descriptor {
   unsigned char f; /*!< F part of descriptor, 2 bits */
   unsigned char x; /*!< X part of descriptor, 6 bits */
   unsigned char y; /*!< Y part of descriptor, 8 bits */
   char c[8]; /*!< string with whole descriptor (6 decimal digits plus the 0x00 as the end of string */
};

/*!
  \struct bufr_atom_data
  \brief Contains all the information for a single descriptor in a expanded squence 
*/
struct bufr_atom_data {
   struct bufr_descriptor desc; /*!< struct \ref bufr_descriptor */
   int mask; /*!< Mask with for the type */
   char name[66]; /*!< String with the name of descriptir */
   char unit[26]; /*!< String with the name of units */
   double val; /*!< Value for the bufr descriptor */
   char cval[80]; /*!< String value for the bufr descriptor */
   char ctable[256]; /*!< Explained meaning for a code table */
};

struct bufr_subset_sequence_data {
   size_t nd; 
   struct bufr_atom_data sequence[NMAXSEQ];
};

extern unsigned char BUFR_MESSAGE[BUFR_LEN]; 
extern char CNAMES[KELEM][64]; 
extern char CUNITS[KELEM][24]; 

extern int KSUP[9]; /*!< array containing supplementary information */
extern int KSEC0[3];
extern int KSEC1[40];
extern int KSEC2[4096];
extern int KSEC3[4];
extern int KSEC4[2];
extern int KEY[46];
extern int KERR;


extern char CVALS[KVALS][80]; /*!< array of strings with value of data */

extern double VALUES[KVALS], VALS[KVALS];
extern int KTDLST[KELEM], KTDEXP[KELEM];

extern char INPUTFILE[256];
extern char OUTPUTFILE[256];
extern char BUFRTABLES_DIR[256];
extern char SELF[];
extern int VERBOSE;
extern int SHOW_SEQUENCE;
extern int SHOW_ECMWF_OUTPUT;
extern int DEBUG;
extern struct synop_chunks SYN;

extern size_t NLINES_TABLEC; 
extern char TABLEC[MAXLINES_TABLEC][92];
extern char DEFAULT_BUFRTABLES[];
extern char TYPE[8];
extern char REPORT[2048];

extern struct bufr_subset_sequence_data SUBSET;

int set_environment(void);
int integer_to_descriptor(struct bufr_descriptor *d, int id);
unsigned int three_bytes_to_uint(const unsigned char *source);
char * charray_to_string(char *s, unsigned char *buf, size_t size);
char * adjust_string(char *s);
char * get_explained_table_val(char *expl, size_t dim, struct bufr_descriptor *d, int ival);
char * get_explained_flag_val(char *expl, size_t dim, struct bufr_descriptor *d, unsigned long ival);
char * get_ecmwf_tablename(char *target, char TYPE);

int parse_subset_as_aaxx(struct synop_chunks *syn, struct bufr_subset_sequence_data *sq, int *kdtlst, size_t nlst, 
                          int *ksec1, char *err);
int synop_YYYYMMDDHHmm_to_YYGG(struct synop_chunks *syn);

