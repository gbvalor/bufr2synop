/***************************************************************************
 *   Copyright (C) 2013-2014 by Guillermo Ballester Valor                  *
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
#ifndef BUFR2SYNOP_H
#define BUFR2SYNOP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <time.h>
#include <math.h>
#include <sys/stat.h>

// project includes
#include "metsynop.h"
#include "metbuoy.h"

//#define DEBUG

/*!
  \def KELEM
  \brief max dimension of elements for a single report
*/
#define KELEM 16384

/*!
  \def KVALS
  \brief dimension of arrays of doubles. It must store all the data for all subset
*/
#define KVALS 16384000

/*!
  \def KSUBS
  \brief Maximum number of subset this version can manage
*/
#define KSUBS (KVALS / KELEM)

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
#define NMAXSEQ 16384

#define DESCRIPTOR_VALUE_MISSING 1 
#define DESCRIPTOR_HAVE_REAL_VALUE 2
#define DESCRIPTOR_HAVE_STRING_VALUE 4
#define DESCRIPTOR_IS_CODE_TABLE 8
#define DESCRIPTOR_HAVE_CODE_TABLE_STRING 16
#define DESCRIPTOR_HAVE_FLAG_TABLE_STRING 32
#define DESCRIPTOR_IS_FLAG_TABLE 64
#define DESCRIPTIR_IS_A_REPLICATOR 128

#define SUBSET_MASK_LATITUDE_SOUTH 1
#define SUBSET_MASK_LONGITUDE_WEST 2
#define SUBSET_MASK_HAVE_TYPE_STATION 4
#define SUBSET_MASK_HAVE_NO_SIGNIFICANT_WW 8
#define SUBSET_MASK_HAVE_NO_SIGNIFICANT_W1 16
#define SUBSET_MASK_HAVE_NO_SIGNIFICANT_W2 32
#define SUBSET_MASK_HAVE_LATITUDE 64
#define SUBSET_MASK_HAVE_LONGITUDE 128
#define SUBSET_MASK_HAVE_ALTITUDE 256
#define SUBSET_MASK_HAVE_NAME 512
#define SUBSET_MASK_HAVE_COUNTRY 1024


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
   char name[92]; /*!< String with the name of descriptor */
   char unit[32]; /*!< String with the name of units */
   double val; /*!< Value for the bufr descriptor */
   char cval[128]; /*!< String value for the bufr descriptor */
   char ctable[256]; /*!< Explained meaning for a code table */
};

/*!
  \struct bufr_subset_sequence_data
  \brief Contains all the information for a subset in a expanded squence 
*/
struct bufr_subset_sequence_data {
   size_t nd; /*!< number of current amount of data in sequence */
   struct bufr_atom_data sequence[NMAXSEQ]; /*!< the array of data associated to a expanded sequence */
};

/*!
  \struct bufr_subset_state
  \brief stores information needed to parse a sequential list of expanded descriptors for a subset
*/
struct bufr_subset_state {
   struct bufr_atom_data *a; /*!< the current struct \ref bufr_atom_data being parsed */
   size_t i; /*!< current index in array element */
   int rep;  /*!< Latest delayed replicator */
   size_t k_rep;  /*!< Index of latest delayed replicator */
   int ival; /*!< the integer value in the descritor */
   double val; /*!< the float value in the descriptor */
   int itval; /*!< Latest parsed time displacement in seconds */
   size_t k_itval; /*!< index in array of latest time displacemet descriptor */
   int jtval; /*!< Prior to Latest parsed time displacement in seconds */
   size_t k_jtval; /*!< index in array of time prior to latest displacemet descriptor */
   int isq; /*!< if 1, the current atom data is in a Significant qualifier squence, and not computed */
   int type; /*!< type of station */
   int layer; /*!< Layer/level of data when parsing Buoy report*/
   int deep; /*!< Latest parsed deep in meters of a layer */ 
   double lat; /*!< Latitude of station */
   double lon; /*!< longitude of station */
   double alt; /*!< Altitude (m)*/
   char name[80]; /*!< Name of observatory, if any */
   char country[80]; /*!< Name of state/country if known */ 
   time_t tobs; /*!< Unix time of observation */
   int mask; /*!< mask which contain several information from the subset data taken at the moment */
};

/*!
  \struct gts_header
  \brief stores WMO GTS header info
*/
struct gts_header {
   char bname[16]; /*!< Name of bulletin, as 'SNAU48' */
   char center[8]; /*!< Release center name, as 'EGRR' */
   char dtrel[16]; /*!< Date and time of release (format DDHHmm) */
   char order[8]; /*!< sequence, as 'BBB' , 'RRA' 'CCA' ... */
   char filename[256]; /*!< filename of Bufr file */
   char timestamp[16]; /*!< String with timestamp (UTC) of file in GTS. Format YYYYMMDDHHmmss */ 
};

/*!
   \struct met_datetime 
   \brief stores date and time reference of a report, commonly the observation time
*/
struct met_datetime {
   time_t t; /*!< Unix instant for report date/time reference */
   struct tm tim;  /*!<  struct tm with report date/time info (UTC) */ 
   char datime[16]; /*!< date/time reference (UTC) as string with YYYYMMDDHHmm format */ 
};

/*!
   \struct met_geo
   \brief Geographic meta information 
*/
struct met_geo {
   char index[16]; /*!< The index indetifier of place, if any */
   char name[80]; /*!< The common name of place */
   char country[80]; /*!< The country name, if known */
   double lat; /*!< Latitude in degrees. North positive */
   double lon; /*!< Longitude in degrees. East positive */
   double alt; /*!< Altitude in metres */
};

/*!
   \struct metreport
   \brief all the information for a meteorological report in WMO ascii format  
*/
struct metreport {
    char source[256];/*!< The bufr source filename */
    struct gts_header *h; /*!< A pointer to a GTS Header Bulletin */
    struct met_datetime t; /*!< The date/time information for report */
    struct met_geo g; /*!< The geographical info */
    char type[8]; /*!< The type of report as MiMiMjMj */
    char alphanum[2048]; /*!< The alphanumeric report */
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
extern char LISTOFFILES[256];
extern char SELF[];
extern int VERBOSE;
extern int SHOW_SEQUENCE;
extern int SHOW_ECMWF_OUTPUT;
extern int DEBUG;
extern int NFILES;
extern int GTS_HEADER;
extern int XML;

extern struct synop_chunks SYNOP;
extern struct buoy_chunks BUOY;

extern size_t NLINES_TABLEC; 
extern char TABLEC[MAXLINES_TABLEC][92];
extern char DEFAULT_BUFRTABLES[];
extern char TYPE[8];
extern struct metreport REPORT;
extern struct gts_header HEADER;
extern FILE * FL;
extern struct bufr_subset_sequence_data SUBSET;

/* Functions definitions */

void clean_buoy_chunks( struct buoy_chunks *b);
void clean_synop_chunks( struct synop_chunks *s);

int set_environment(void);
int integer_to_descriptor(struct bufr_descriptor *d, int id);
unsigned int three_bytes_to_uint(const unsigned char *source);
char * charray_to_string(char *s, unsigned char *buf, size_t size);
char * adjust_string(char *s);
char * get_explained_table_val(char *expl, size_t dim, struct bufr_descriptor *d, int ival);
char * get_explained_flag_val(char *expl, size_t dim, struct bufr_descriptor *d, unsigned long ival);
char * get_ecmwf_tablename(char *target, char TYPE);
char * get_bufrfile_path( char *filename, char *err);
int parse_subset_as_buoy(struct metreport *m, struct buoy_chunks *b, struct bufr_subset_sequence_data *sq, int *kdtlst, size_t nlst, int *ksec1, char *err );
int parse_subset_as_synop (struct metreport *m, struct synop_chunks *syn, char *type, struct bufr_subset_sequence_data *sq, int *kdtlst, size_t nlst, int *ksec1, char *err );
int YYYYMMDDHHmm_to_met_datetime(struct met_datetime *t, const char *source);
int synop_YYYYMMDDHHmm_to_YYGG(struct synop_chunks *syn);
int buoy_YYYYMMDDHHmm_to_JMMYYGGgg(struct buoy_chunks *b);
char * guess_WMO_region(struct synop_chunks *syn);

int parse_subset_sequence(struct metreport *m, struct bufr_subset_sequence_data *sq, int *kdtlst, size_t nlst, int *ksec1, char *err);
int find_descriptor(int *haystack, size_t nlst, int needle);
int find_descriptor_interval(int *haystack, size_t nlst, int needlemin, int needlemax);

char * latlon_to_MMM(char *target, double lat, double lon);
char * kelvin_to_TTTT ( char *target, double T );
char * kelvin_to_snTTT ( char *target, double T );
char * kelvin_to_snTT ( char *target, double T );
char * kelvin_to_TT ( char *target, double T );
char * m_to_h ( char *target, double h );
char * pascal_to_ppp ( char *target, double P );
char * pascal_to_PPPP ( char *target, double P );
char * percent_to_okta ( char *target, double perc );
char * prec_to_RRR ( char *target, double r );
char * prec_to_RRRR24 ( char *target, double r );
char * vism_to_VV ( char *target, double V );

int print_synop ( char *report, size_t lmax, struct synop_chunks *syn );
char * print_synop_sec0 (char **sec0, size_t lmax, struct synop_chunks *syn);
char * print_synop_sec1 (char **sec1, size_t lmax, struct synop_chunks *syn);
char * print_synop_sec2 (char **sec2, size_t lmax, struct synop_chunks *syn);
char * print_synop_sec3 (char **sec3, size_t lmax, struct synop_chunks *syn);

int print_buoy ( char *report, size_t lmax, struct buoy_chunks *syn );
char * print_buoy_sec0 (char **sec0, size_t lmax, struct buoy_chunks *syn);
char * print_buoy_sec1 (char **sec1, size_t lmax, struct buoy_chunks *syn);
char * print_buoy_sec2 (char **sec2, size_t lmax, struct buoy_chunks *syn);
char * print_buoy_sec3 (char **sec3, size_t lmax, struct buoy_chunks *syn);


int syn_parse_x01 ( struct synop_chunks *syn, struct bufr_subset_state *s, char *err );
int syn_parse_x02 ( struct synop_chunks *syn, struct bufr_subset_state *s, char *err );
int syn_parse_x04 ( struct synop_chunks *syn, struct bufr_subset_state *s, char *err );
int syn_parse_x05 ( struct synop_chunks *syn, struct bufr_subset_state *s, char *err );
int syn_parse_x06 ( struct synop_chunks *syn, struct bufr_subset_state *s, char *err );
int syn_parse_x07 ( struct synop_chunks *syn, struct bufr_subset_state *s, char *err );
int syn_parse_x08 ( struct synop_chunks *syn, struct bufr_subset_state *s, char *err );
int syn_parse_x10 ( struct synop_chunks *syn, struct bufr_subset_state *s, char *err );
int syn_parse_x11 ( struct synop_chunks *syn, struct bufr_subset_state *s, char *err );
int syn_parse_x12 ( struct synop_chunks *syn, struct bufr_subset_state *s, char *err );
int syn_parse_x13 ( struct synop_chunks *syn, struct bufr_subset_state *s, char *err );
int syn_parse_x14 ( struct synop_chunks *syn, struct bufr_subset_state *s, char *err );
int syn_parse_x20 ( struct synop_chunks *syn, struct bufr_subset_state *s, char *err );
int syn_parse_x22 ( struct synop_chunks *syn, struct bufr_subset_state *s, char *err );

int buoy_parse_x01 ( struct buoy_chunks *b, struct bufr_subset_state *s, char *err );
int buoy_parse_x02 ( struct buoy_chunks *b, struct bufr_subset_state *s, char *err );
int buoy_parse_x04 ( struct buoy_chunks *b, struct bufr_subset_state *s, char *err );
int buoy_parse_x05 ( struct buoy_chunks *b, struct bufr_subset_state *s, char *err );
int buoy_parse_x06 ( struct buoy_chunks *b, struct bufr_subset_state *s, char *err );
int buoy_parse_x07 ( struct buoy_chunks *b, struct bufr_subset_state *s, char *err );
int buoy_parse_x08 ( struct buoy_chunks *b, struct bufr_subset_state *s, char *err );
int buoy_parse_x10 ( struct buoy_chunks *b, struct bufr_subset_state *s, char *err );
int buoy_parse_x11 ( struct buoy_chunks *b, struct bufr_subset_state *s, char *err );
int buoy_parse_x12 ( struct buoy_chunks *b, struct bufr_subset_state *s, char *err );
int buoy_parse_x13 ( struct buoy_chunks *b, struct bufr_subset_state *s, char *err );
int buoy_parse_x20 ( struct buoy_chunks *b, struct bufr_subset_state *s, char *err );
int buoy_parse_x22 ( struct buoy_chunks *b, struct bufr_subset_state *s, char *err );


#endif  // from ifndef BUFR2SYNOP_H
