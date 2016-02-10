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
/*!
 \file bufr2synop.h
 \brief Include header file for binary bufr2synop
*/
#ifndef BUFR2SYN_H
#define BUFR2SYN_H

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

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
#include "mettemp.h"
#include "metclimat.h"
//#define DEBUG

/*!
  \def KELEM
  \brief max dimension of elements for a single report
*/
#define KELEM (8192)

/*!
  \def KVALS
  \brief dimension of arrays of doubles. It must store all the data for all subset
*/
#define KVALS (32768 * 512)

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
#define MAXLINES_TABLEC (8192)

/*!
  \def NMAXSEQ
  \brief Maximum expected descriptor in a expanded sequence for a single subset
*/
#define NMAXSEQ (16384)

/*!
  \def DESCRIPTOR_VALUE_MISSING
  \brief Bit mask for a missing value in a struct \ref bufr_atom_data
*/
#define DESCRIPTOR_VALUE_MISSING (1)

/*!
 \def DESCRIPTOR_HAVE_REAL_VALUE
 \brief Bit mask for a real value in a struct \ref bufr_atom_data
*/
#define DESCRIPTOR_HAVE_REAL_VALUE (2)

/*!
 \def DESCRIPTOR_HAVE_STRING_VALUE
 \brief Bit mask for a string in a struct \ref bufr_atom_data
*/
#define DESCRIPTOR_HAVE_STRING_VALUE (4)

/*!
 \def DESCRIPTOR_IS_CODE_TABLE
 \brief Bit mask for a code table in a struct \ref bufr_atom_data
*/
#define DESCRIPTOR_IS_CODE_TABLE (8)

/*!
 \def DESCRIPTOR_HAVE_CODE_TABLE_STRING
 \brief Bit mask for a code table string in a struct \ref bufr_atom_data
*/
#define DESCRIPTOR_HAVE_CODE_TABLE_STRING (16)

/*!
 \def DESCRIPTOR_HAVE_FLAG_TABLE_STRING
 \brief Bit mask for a flag table string in a struct \ref bufr_atom_data
*/
#define DESCRIPTOR_HAVE_FLAG_TABLE_STRING (32)

/*!
 \def DESCRIPTOR_IS_FLAG_TABLE
 \brief Bit mask for a flag table in a struct \ref bufr_atom_data
*/
#define DESCRIPTOR_IS_FLAG_TABLE (64)

/*!
 \def DESCRIPTOR_IS_A_REPLICATOR
 \brief Bit mask for a flag table string in a struct \ref bufr_atom_data
*/
#define DESCRIPTOR_IS_A_REPLICATOR (128)


/*!
 \def SUBSET_MASK_LATITUDE_SOUTH
 \brief Bit mask to mark a struct \ref bufr_subset_sequence_data with south latitude
*/
#define SUBSET_MASK_LATITUDE_SOUTH (1)

/*!
 \def SUBSET_MASK_LONGITUDE_WEST
 \brief Bit mask to mark a struct \ref bufr_subset_sequence_data with west longitude
*/
#define SUBSET_MASK_LONGITUDE_WEST (2)

/*!
 \def SUBSET_MASK_HAVE_TYPE_STATION
 \brief Bit mask to mark a struct \ref bufr_subset_sequence_data having type station information
*/
#define SUBSET_MASK_HAVE_TYPE_STATION (4)

/*!
 \def SUBSET_MASK_HAVE_NO_SIGNIFICANT_WW
 \brief Bit mask to mark a struct \ref bufr_subset_sequence_data without WW information
*/
#define SUBSET_MASK_HAVE_NO_SIGNIFICANT_WW (8)

/*!
 \def SUBSET_MASK_HAVE_NO_SIGNIFICANT_W1
 \brief Bit mask to mark a struct \ref bufr_subset_sequence_data without W1 information
*/
#define SUBSET_MASK_HAVE_NO_SIGNIFICANT_W1 (16)

/*!
 \def SUBSET_MASK_HAVE_NO_SIGNIFICANT_W2
 \brief Bit mask to mark a struct \ref bufr_subset_sequence_data without W1 information
*/
#define SUBSET_MASK_HAVE_NO_SIGNIFICANT_W2 (32)

/*!
 \def SUBSET_MASK_HAVE_LATITUDE
 \brief Bit mask to mark a struct \ref bufr_subset_sequence_data having latitude
*/
#define SUBSET_MASK_HAVE_LATITUDE (64)

/*!
 \def SUBSET_MASK_HAVE_LONGITUDE
 \brief Bit mask to mark a struct \ref bufr_subset_sequence_data having longitude
*/
#define SUBSET_MASK_HAVE_LONGITUDE (128)

/*!
 \def SUBSET_MASK_HAVE_ALTITUDE
 \brief Bit mask to mark a struct \ref bufr_subset_sequence_data having altitude
*/
#define SUBSET_MASK_HAVE_ALTITUDE (256)

/*!
 \def SUBSET_MASK_HAVE_NAME
 \brief Bit mask to mark a struct \ref bufr_subset_sequence_data having station name
*/
#define SUBSET_MASK_HAVE_NAME (512)

/*!
 \def SUBSET_MASK_HAVE_COUNTRY
 \brief Bit mask to mark a struct \ref bufr_subset_sequence_data having country name
*/
#define SUBSET_MASK_HAVE_COUNTRY (1024)

/*!
 \def SUBSET_MASK_HAVE_YEAR
 \brief Bit mask to mark a struct \ref bufr_subset_sequence_data having observation year
*/
#define SUBSET_MASK_HAVE_YEAR (2048)

/*!
 \def SUBSET_MASK_HAVE_MONTH
 \brief Bit mask to mark a struct \ref bufr_subset_sequence_data having observation month
*/
#define SUBSET_MASK_HAVE_MONTH (4096)

/*!
 \def SUBSET_MASK_HAVE_DAY
 \brief Bit mask to mark a struct \ref bufr_subset_sequence_data having observation day in a month
*/
#define SUBSET_MASK_HAVE_DAY (8192)

/*!
 \def SUBSET_MASK_HAVE_HOUR
 \brief Bit mask to mark a struct \ref bufr_subset_sequence_data having observation hour
*/
#define SUBSET_MASK_HAVE_HOUR (16384)

/*!
 \def SUBSET_MASK_HAVE_MINUTE
 \brief Bit mask to mark a struct \ref bufr_subset_sequence_data having observation minute
*/
#define SUBSET_MASK_HAVE_MINUTE (32768)

/*!
 \def SUBSET_MASK_HAVE_SECOND
 \brief Bit mask to mark a struct \ref bufr_subset_sequence_data having observation second
*/
#define SUBSET_MASK_HAVE_SECOND (65536)

/*!
 \def SUBSET_MASK_HAVE_GUST
 \brief Bit mask to mark a struct \ref bufr_subset_sequence_data having wind gust observation other than 10 minutes
*/
#define SUBSET_MASK_HAVE_GUST (131072)


/*!
  \struct bufr_descriptor
  \brief BUFR descriptor
*/
struct bufr_descriptor
{
  unsigned char f; /*!< F part of descriptor, 2 bits */
  unsigned char x; /*!< X part of descriptor, 6 bits */
  unsigned char y; /*!< Y part of descriptor, 8 bits */
  char c[8]; /*!< string with whole descriptor (6 decimal digits plus the 0x00 as the end of string */
};

/*!
  \struct bufr_atom_data
  \brief Contains all the information for a single descriptor in a expanded squence
*/
struct bufr_atom_data
{
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
struct bufr_subset_sequence_data
{
  size_t nd; /*!< number of current amount of data in sequence */
  struct bufr_atom_data sequence[NMAXSEQ]; /*!< the array of data associated to a expanded sequence */
};

/*!
  \struct bufr_subset_state
  \brief stores information needed to parse a sequential list of expanded descriptors for a subset
*/
struct bufr_subset_state
{
  char type_report[16]; /*!< The type of report to decode (MMMM) */
  struct bufr_atom_data *a; /*!< the current struct \ref bufr_atom_data being parsed */
  struct bufr_atom_data *a1; /*!< the prior struct \ref bufr_atom_data being parsed */
  size_t i; /*!< current index in array element */
  int rep;  /*!< Latest delayed replicator */
  size_t k_rep;  /*!< Index of latest delayed replicator */
  int ival; /*!< the integer value in the descriptor */
  double val; /*!< the float value in the descriptor */
  int itval; /*!< Latest parsed time displacement in seconds */
  size_t k_itval; /*!< index in array of latest time displacemet descriptor */
  int jtval; /*!< Prior to Latest parsed time displacement in seconds */
  size_t k_jtval; /*!< index in array of time prior to latest displacemet descriptor */
  int isq; /*!< if 1, the current atom data is in a Significant qualifier squence, and not computed */
  int isq_val; /*!< Value of a significant qualifier */
  int type; /*!< type of station */
  int clayer; /*!< cloud layer being parsed */
  int layer; /*!< Layer/level of data when parsing Buoy report*/
  int deep; /*!< Latest parsed deep in meters of a layer */
  double lat; /*!< Latitude of station */
  double lon; /*!< longitude of station */
  double alt; /*!< Altitude (m)*/
  char name[80]; /*!< Name of observatory, if any */
  char country[80]; /*!< Name of state/country if known */
  time_t tobs; /*!< Unix time reference*/
  int dift; /*!< UTC - LST , in hours */
  int day; /*!< Day of ephemerides in some reports */
  int more_days; /*!< If = 1 there are more than a day in ephemeride */
  int nday; /*!< period (days) in some reports */
  int month; /*!< month for some normal values */
  int is_normal; /*!< if 1, the values are normal of a defined period */
  int mask; /*!< mask which contain several information from the subset data taken at the moment */
  struct temp_raw_data *r; /*!< pointer to a struct where to set the data from a temp profile being parsed */
  struct temp_raw_wind_shear_data *w; /*!< pointer to a struct where to set the data from a temp profile being parsed */
};

/*!
  \struct gts_header
  \brief stores WMO GTS header info
*/
struct gts_header
{
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
struct met_datetime
{
  time_t t; /*!< Unix instant for report date/time reference */
  struct tm tim;  /*!<  struct tm with report date/time info (UTC) */
  char datime[16]; /*!< date/time reference (UTC) as string with YYYYMMDDHHmm[ss] format */
};

/*!
   \struct met_geo
   \brief Geographic meta information
*/
struct met_geo
{
  char index[16]; /*!< The index indetifier of place, if any */
  char name[80]; /*!< The common name of place */
  char country[80]; /*!< The country name, if known */
  double lat; /*!< Latitude in degrees. North positive */
  double lon; /*!< Longitude in degrees. East positive */
  double alt; /*!< Altitude in metres */
};

/*!
   \struct metreport
   \brief all the information for a meteorological report in WMO text format from a BUFR file
*/
struct metreport
{
  char source[256];/*!< The bufr source filename */
  struct gts_header *h; /*!< A pointer to a GTS Header Bulletin */
  struct met_datetime t; /*!< The date/time information for report */
  struct met_geo g; /*!< The geographical info */
  struct synop_chunks synop; /*!< The possible parsed synop */
  struct buoy_chunks buoy; /*!< The possible parsed buoy */
  struct temp_chunks temp; /*!< The possible parsed temp */
  struct climat_chunks climat; /*!< The pssible parsed climat */
  char type[8]; /*!< The type of report as MiMiMjMj */
  char alphanum[2048]; /*!< The alphanumeric report */
  char type2[8]; /*!< The type of report of part 2 as MiMiMjMj */
  char alphanum2[2048]; /*!< The alphanumeric report, part 2 */
  char type3[8]; /*!< The type of report of part 3 as MiMiMjMj */
  char alphanum3[2048]; /*!< The alphanumeric report, part 3 */
  char type4[8]; /*!< The type of report of part 4 as MiMiMjMj */
  char alphanum4[2048]; /*!< The alphanumeric report, part 4 */
};

/* Functions definitions */

void clean_buoy_chunks ( struct buoy_chunks *b );
void clean_synop_chunks ( struct synop_chunks *s );
void clean_temp_chunks ( struct temp_chunks *t );
void clean_climat_chunks ( struct climat_chunks *c );

int set_environment ( char *default_bufrtables, char *bufrtables_dir );
int integer_to_descriptor ( struct bufr_descriptor *d, int id );
unsigned int three_bytes_to_uint ( const unsigned char *source );
char * charray_to_string ( char *s, unsigned char *buf, size_t size );
char * adjust_string ( char *s );
char * get_explained_table_val ( char *expl, size_t dim, char tablec[MAXLINES_TABLEC][92], size_t nlines_tablec,
                                 struct bufr_descriptor *d, int ival );
char * get_explained_flag_val ( char *expl, size_t dim, char tablec[MAXLINES_TABLEC][92], size_t nlines_tablec,
                                struct bufr_descriptor *d, unsigned long ival );
char * get_ecmwf_tablename ( char *target, char type, char *bufrtables_dir, int ksec1[40] );
char * get_bufrfile_path ( char *filename, char *err );
int parse_subset_as_buoy ( struct metreport *m, struct bufr_subset_state *s, struct bufr_subset_sequence_data *sq,
                           char *err );
int parse_subset_as_synop ( struct metreport *m, struct bufr_subset_state *s, struct bufr_subset_sequence_data *sq,
                            char *err );
int parse_subset_as_temp ( struct metreport *m, struct bufr_subset_state *s, struct bufr_subset_sequence_data *sq,
                           char *err );
int parse_subset_as_climat ( struct metreport *m, struct bufr_subset_state *s, struct bufr_subset_sequence_data *sq,
                           char *err );
int YYYYMMDDHHmm_to_met_datetime ( struct met_datetime *t, const char *source );
int synop_YYYYMMDDHHmm_to_YYGG ( struct synop_chunks *syn );
int buoy_YYYYMMDDHHmm_to_JMMYYGGgg ( struct buoy_chunks *b );
int check_date_from_future(struct metreport *m);
char *guess_WMO_region ( char *A1, char *Reg, const char *II, const char *iii );
char * guess_WMO_region_synop ( struct synop_chunks *syn );
char *guess_WMO_region_temp ( struct temp_chunks *temp );
int read_table_c ( char tablec[MAXLINES_TABLEC][92], size_t *nlines_tablec, char *bufrtables_dir, int ksec1[40] );
int parse_subset_sequence ( struct metreport *m, struct bufr_subset_sequence_data *sq, struct bufr_subset_state *st,
                            int *kdtlst, size_t nlst, int *ksec1, char *err );
int find_descriptor ( int *haystack, size_t nlst, int needle );
int find_descriptor_interval ( int *haystack, size_t nlst, int needlemin, int needlemax );
int bufr_set_environment ( char *default_bufrtables, char *bufrtables_dir );
int guess_gts_header ( struct gts_header *h, const char *f );
int read_bufr ( unsigned char *bufr, char *filename, int *length );

char * latlon_to_MMM ( char *target, double lat, double lon );
char * kelvin_to_TTTT ( char *target, double T );
char * kelvin_to_snTTT ( char *target, double T );
char * kelvin_to_snTT ( char *target, double T );
char * kelvin_to_TT ( char *target, double T );
char * m_to_h ( char *target, double h );
char * m_to_hh ( char *target, double h );
char * pascal_to_ppp ( char *target, double P );
char * pascal_to_PPPP ( char *target, double P );
char * percent_to_okta ( char *target, double perc );
char * prec_to_RRR ( char *target, double r );
char * prec_to_RRRR24 ( char *target, double r );
char * secs_to_tt ( char *tt, int secs );
char * vism_to_VV ( char *target, double V );
char * recent_snow_to_ss ( char *target, double r );

int print_synop ( char *report, size_t lmax, struct synop_chunks *syn );
char * print_synop_sec0 ( char **sec0, size_t lmax, struct synop_chunks *syn );
char * print_synop_sec1 ( char **sec1, size_t lmax, struct synop_chunks *syn );
char * print_synop_sec2 ( char **sec2, size_t lmax, struct synop_chunks *syn );
char * print_synop_sec3 ( char **sec3, size_t lmax, struct synop_chunks *syn );

int buoy_YYYYMMDDHHmm_to_JMMYYGGgg ( struct buoy_chunks *b );

int print_buoy ( char *report, size_t lmax, struct buoy_chunks *syn );
char * print_buoy_sec0 ( char **sec0, size_t lmax, struct buoy_chunks *syn );
char * print_buoy_sec1 ( char **sec1, size_t lmax, struct buoy_chunks *syn );
char * print_buoy_sec2 ( char **sec2, size_t lmax, struct buoy_chunks *syn );
char * print_buoy_sec3 ( char **sec3, size_t lmax, struct buoy_chunks *syn );

int print_climat ( char *report, size_t lmax, struct climat_chunks *cl );
char * print_climat_sec0 ( char **sec0, size_t lmax, struct climat_chunks *cl );
char * print_climat_sec1 ( char **sec1, size_t lmax, struct climat_chunks *cl );
char * print_climat_sec2 ( char **sec2, size_t lmax, struct climat_chunks *cl );
char * print_climat_sec3 ( char **sec3, size_t lmax, struct climat_chunks *cl );
char * print_climat_sec4 ( char **sec4, size_t lmax, struct climat_chunks *cl );


int print_csv ( FILE *f, struct metreport *m );
int print_json ( FILE *f, struct metreport *m );
int print_xml ( FILE *f, struct metreport *m );

int syn_parse_x01 ( struct synop_chunks *syn, struct bufr_subset_state *s );
int syn_parse_x02 ( struct synop_chunks *syn, struct bufr_subset_state *s );
int syn_parse_x04 ( struct synop_chunks *syn, struct bufr_subset_state *s );
int syn_parse_x05 ( struct synop_chunks *syn, struct bufr_subset_state *s );
int syn_parse_x06 ( struct synop_chunks *syn, struct bufr_subset_state *s );
int syn_parse_x07 ( struct synop_chunks *syn, struct bufr_subset_state *s );
int syn_parse_x08 ( struct synop_chunks *syn, struct bufr_subset_state *s );
int syn_parse_x10 ( struct synop_chunks *syn, struct bufr_subset_state *s );
int syn_parse_x11 ( struct synop_chunks *syn, struct bufr_subset_state *s );
int syn_parse_x12 ( struct synop_chunks *syn, struct bufr_subset_state *s );
int syn_parse_x13 ( struct synop_chunks *syn, struct bufr_subset_state *s );
int syn_parse_x14 ( struct synop_chunks *syn, struct bufr_subset_state *s );
int syn_parse_x20 ( struct synop_chunks *syn, struct bufr_subset_state *s );
int syn_parse_x22 ( struct synop_chunks *syn, struct bufr_subset_state *s );
int syn_parse_x31 ( struct synop_chunks *syn, struct bufr_subset_state *s );

int buoy_parse_x01 ( struct buoy_chunks *b, struct bufr_subset_state *s );
int buoy_parse_x02 ( struct buoy_chunks *b, struct bufr_subset_state *s );
int buoy_parse_x04 ( struct buoy_chunks *b, struct bufr_subset_state *s );
int buoy_parse_x05 ( struct buoy_chunks *b, struct bufr_subset_state *s );
int buoy_parse_x06 ( struct buoy_chunks *b, struct bufr_subset_state *s );
int buoy_parse_x07 ( struct buoy_chunks *b, struct bufr_subset_state *s );
int buoy_parse_x08 ( struct buoy_chunks *b, struct bufr_subset_state *s );
int buoy_parse_x10 ( struct buoy_chunks *b, struct bufr_subset_state *s );
int buoy_parse_x11 ( struct buoy_chunks *b, struct bufr_subset_state *s );
int buoy_parse_x12 ( struct buoy_chunks *b, struct bufr_subset_state *s );
int buoy_parse_x13 ( struct buoy_chunks *b, struct bufr_subset_state *s );
int buoy_parse_x14 ( struct buoy_chunks *b, struct bufr_subset_state *s );
int buoy_parse_x20 ( struct buoy_chunks *b, struct bufr_subset_state *s );
int buoy_parse_x22 ( struct buoy_chunks *b, struct bufr_subset_state *s );
int buoy_parse_x31 ( struct buoy_chunks *b, struct bufr_subset_state *s );
int buoy_parse_x33 ( struct buoy_chunks *b, struct bufr_subset_state *s );

int climat_parse_x01 ( struct climat_chunks *c, struct bufr_subset_state *s );
int climat_parse_x02 ( struct climat_chunks *c, struct bufr_subset_state *s );
int climat_parse_x04 ( struct climat_chunks *c, struct bufr_subset_state *s );
int climat_parse_x05 ( struct climat_chunks *c, struct bufr_subset_state *s );
int climat_parse_x06 ( struct climat_chunks *c, struct bufr_subset_state *s );
int climat_parse_x07 ( struct climat_chunks *c, struct bufr_subset_state *s );
int climat_parse_x08 ( struct climat_chunks *c, struct bufr_subset_state *s );
int climat_parse_x10 ( struct climat_chunks *c, struct bufr_subset_state *s );
int climat_parse_x11 ( struct climat_chunks *c, struct bufr_subset_state *s );
int climat_parse_x12 ( struct climat_chunks *c, struct bufr_subset_state *s );
int climat_parse_x13 ( struct climat_chunks *c, struct bufr_subset_state *s );
int climat_parse_x14 ( struct climat_chunks *c, struct bufr_subset_state *s );
int climat_parse_x20 ( struct climat_chunks *c, struct bufr_subset_state *s );
int climat_parse_x22 ( struct climat_chunks *c, struct bufr_subset_state *s );
int climat_parse_x31 ( struct climat_chunks *c, struct bufr_subset_state *s );
int climat_parse_x33 ( struct climat_chunks *c, struct bufr_subset_state *s );

int temp_parse_x01 ( struct temp_chunks *t, struct bufr_subset_state *s );
int temp_parse_x02 ( struct temp_chunks *t, struct bufr_subset_state *s );
int temp_parse_x04 ( struct temp_chunks *t, struct bufr_subset_state *s );
int temp_parse_x07 ( struct temp_chunks *t, struct bufr_subset_state *s );
int temp_parse_x08 ( struct temp_chunks *t, struct bufr_subset_state *s );
int temp_parse_x20 ( struct temp_chunks *t, struct bufr_subset_state *s );
int temp_parse_x22 ( struct temp_chunks *t, struct bufr_subset_state *s );
int temp_parse_x33 ( struct temp_chunks *t, struct bufr_subset_state *s );

// These are prototypes for used ecmwf bufr library functions
int bus012_ ( int *, unsigned int *, int *, int *, int *, int *, int * );
int buprs0_ ( int * );
int buprs1_ ( int * );
int buprs3_ ( int *, int *, int *, int *, int *, int *, char ** );
int bufrex_ ( int *, int *, int *, int *, int *, int *, int *, int *, int *, char **, char **, int *, double *, char **,                int * );
int busel_ ( int *, int *, int *, int *, int * );
int busel2_ ( int *, int *, int *,  char **, int *, char **, char **, char **, int * );
int buukey_ ( int *, int *, int *, int *, int * );
int buprt_ ( int *, int *, int *, int *, char **, char **, char **, int *, double *, int *, int *, int * );



#endif  // from ifndef BUFR2SYN_H
