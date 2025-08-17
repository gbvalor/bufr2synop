/***************************************************************************
 *   Copyright (C) 2013-2025 by Guillermo Ballester Valor                  *
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
 \file bufr2tac.h
 \brief Include header file for binary bufr2tac
*/
#ifndef BUFR2TAC_H
#define BUFR2TAC_H

// If is used by c++ code
#ifdef __cplusplus
extern "C" {
#endif

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
#include "bufrdeco.h"
#include "metsynop.h"
#include "metbuoy.h"
#include "mettemp.h"
#include "metclimat.h"
//#define DEBUG

//#define USE_BUFRDC

#ifdef USE_BUFRDC
/*!
  \def KELEM
  \brief max dimension of elements for a single report
*/
#define KELEM (8192 * 1)

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

#endif // USE_BUFRDC

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
 \def SUBSET_MASK_HAVE_GUST10
 \brief Bit mask to mark a struct \ref bufr_subset_sequence_data having wind gust observation other than 10 minutes
*/
#define SUBSET_MASK_HAVE_GUST10 ( 2 * 131072)

/*!
 \def SUBSET_MASK_HAVE_WIGOS_ID
 \brief Bit mask to mark if a subset has a WIGOS ID
*/
#define SUBSET_MASK_HAVE_WIGOS_ID ( 4 * 131072)


/*! \def BUFR2TAC_ERROR_STACK_DIM
 *  \brief set de dimension of a struct \ref bufr2tac_error_stack 
 */
#define BUFR2TAC_ERROR_STACK_DIM 16

/*! \def BUFR2TAC_ERROR_DESCRIPTION_LENGTH
 *  \brief set de dimension of member \a description of a struct \ref bufr2tac_error 
 */
#define BUFR2TAC_ERROR_DESCRIPTION_LENGTH 1024


#define REPORT_LENGTH (16384)

/*!
 *  \def PRINT_BITMASK_WIGOS
 *  \brief Bit mask to member \a print_mask in struct \ref metreport to print WIGOS Identifier
 */
#define PRINT_BITMASK_WIGOS (1)

/*!
 *  \def PRINT_BITMASK_GEO
 *  \brief Bit mask to member \a print_mask in struct \ref metreport to print geographic position
 */
#define PRINT_BITMASK_GEO (2)

/*!
 * \def bufr_subset_sequence_data
 * \brief To use bufrdeco library with legacy old code using ECMWF library which is not used currently
 */
# define bufr_subset_sequence_data bufrdeco_subset_sequence_data

/*!
 * \struct bufr2tac_error
 * \brief Store an error/warning/info and its severity
 */
struct bufr2tac_error {
  int severity; /*!< Level of severity. if = 1 then is a warning. if = 2 is an error */
  char description[BUFR2TAC_ERROR_DESCRIPTION_LENGTH]; /*!< string with error description */
};

/*!
 * \struct bufr2tac_error_stack
 * \brief A stack of structs \ref bufr2tac_error
 */ 
struct bufr2tac_error_stack {
  size_t ne; /*!< Current warning/error active. If = 0 then no errors */
  int full; /*!< if 1 then one or more warnings cannot be pushed because of full stack. If = 2 an error cannot be pushed */
  struct bufr2tac_error err[BUFR2TAC_ERROR_STACK_DIM] ; /*!< Dimension of stack array */  
};

/*!
  \struct bufr2tac_subset_state
  \brief stores information needed to parse a sequential list of expanded descriptors for a subset
*/
struct bufr2tac_subset_state
{
  char type_report[16]; /*!< The type of report to decode (MMMM) */
  struct bufr2tac_error_stack e; /*!< Pointer to a struct \ref bufr2tac_error_stack */
  struct bufr_atom_data *a; /*!< the current struct \ref bufr_atom_data being parsed */
  struct bufr_atom_data *a1; /*!< the prior struct \ref bufr_atom_data being parsed */
  size_t i; /*!< current index in array element */
  int rep;  /*!< Latest delayed replicator */
  size_t k_rep;  /*!< Index of latest delayed replicator */
  int ival; /*!< the integer value in the descriptor */
  double val; /*!< the float value in the descriptor */
  int itval; /*!< Latest parsed time displacement in seconds */
  int itmask; /*!< Latest time displacement descriptor mask */
  size_t k_itval; /*!< index in array of latest time displacemet descriptor */
  int jtval; /*!< Prior to latest parsed time displacement in seconds */
  int jtmask; /*!< Prior to latest time displacement descriptor mask */
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
  double hsensor; /*!< Sensor height over station ground (m) */
  double hwsensor; /*!< Sensor height over water surface (m) */
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
  int SnSn; /*!< Latest int value of Synop suplementary information */
  int tw1w2; /*!< Period for synop w1w2 (seconds) */
  struct temp_raw_data *r; /*!< pointer to a struct where to set the data from a temp profile being parsed */
  struct temp_raw_wind_shear_data *w; /*!< pointer to a struct where to set the data from a temp profile being parsed */
};

/*!
   \struct met_geo
   \brief Geographic meta information
*/
struct met_geo
{
  struct wigos_id wid; /*!< The WIGOS Id if known or '0-0-0-MISSING' */
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
  int subset; /*!< Subset index in bufr report */
  int print_mask; /*!< Mode of print 0=legacy, 1= with WIGOS id, 2=with lat/lon/alt */
  struct gts_header *h; /*!< A pointer to a GTS Header Bulletin */
  struct met_datetime t; /*!< The date/time information for report */
  struct met_geo g; /*!< The geographical info */
  struct synop_chunks synop; /*!< The possible parsed synop */
  struct buoy_chunks buoy; /*!< The possible parsed buoy */
  struct temp_chunks temp; /*!< The possible parsed temp */
  struct climat_chunks climat; /*!< The pssible parsed climat */
  char type[8]; /*!< The type of report as MiMiMjMj */
  char alphanum[REPORT_LENGTH]; /*!< The alphanumeric report */
  char type2[8]; /*!< The type of report of part 2 as MiMiMjMj */
  char alphanum2[REPORT_LENGTH]; /*!< The alphanumeric report, part 2 */
  char type3[8]; /*!< The type of report of part 3 as MiMiMjMj */
  char alphanum3[REPORT_LENGTH]; /*!< The alphanumeric report, part 3 */
  char type4[8]; /*!< The type of report of part 4 as MiMiMjMj */
  char alphanum4[REPORT_LENGTH]; /*!< The alphanumeric report, part 4 */
};

/* Functions definitions */
char *bufr2tac_get_version(char *version, size_t dversion, char *build, size_t dbuild, char *builder, size_t dbuilder, 
                           int *version_major, int *version_minor, int *version_patch);

/* Error/debug functions */
int bufr2tac_push_error ( struct bufr2tac_error_stack *e, int severity, const char *description );
int bufr2tac_clean_error_stack ( struct bufr2tac_error_stack *e );
int bufr2tac_set_error ( struct bufr2tac_subset_state *s, int severity, const char *origin, const char *explanation);
int bufr2tac_print_error ( const struct bufr2tac_error_stack *e );
int bufr2tac_set_debug_level(int level);

void bufr2tac_clean_buoy_chunks ( struct buoy_chunks *b );
void bufr2tac_clean_synop_chunks ( struct synop_chunks *s );
void bufr2tac_clean_temp_chunks ( struct temp_chunks *t );
void bufr2tac_clean_climat_chunks ( struct climat_chunks *c );
void bufr2tac_clean_metreport (struct metreport *m);

int set_environment ( char *default_bufrtables, char *bufrtables_dir );
int integer_to_descriptor ( struct bufr_descriptor *d, int id );
int descriptor_to_integer ( int *id, const struct bufr_descriptor *d );
unsigned int three_bytes_to_uint ( const unsigned char *source );
char * charray_to_string ( char *s, const unsigned char *buf, size_t size );
char * adjust_string ( char *s );
char * get_explained_table_val ( char *expl, size_t dim, const char tablec[MAXLINES_TABLEC][92], size_t nlines_tablec,
                                 const struct bufr_descriptor *d, int ival );
char * get_explained_flag_val ( char *expl, size_t dim, const char tablec[MAXLINES_TABLEC][92], size_t nlines_tablec,
                                const struct bufr_descriptor *d, unsigned long ival );
char * get_ecmwf_tablename ( char *target, char type, const char *bufrtables_dir, const int ksec1[40] );
int parse_subset_as_buoy ( struct metreport *m, struct bufr2tac_subset_state *s, struct bufr_subset_sequence_data *sq,
                           char *err );
int parse_subset_as_synop ( struct metreport *m, struct bufr2tac_subset_state *s, struct bufr_subset_sequence_data *sq,
                            char *err );
int parse_subset_as_temp ( struct metreport *m, struct bufr2tac_subset_state *s, struct bufr_subset_sequence_data *sq,
                           char *err );
int parse_subset_as_climat ( struct metreport *m, struct bufr2tac_subset_state *s, struct bufr_subset_sequence_data *sq,
                             char *err );
int YYYYMMDDHHmm_to_met_datetime ( struct met_datetime *t, const char *source );
int round_met_datetime_to_hour ( struct met_datetime *target, const struct met_datetime *source );
int synop_YYYYMMDDHHmm_to_YYGG ( struct synop_chunks *syn );
char *met_datetime_to_YYGG ( char *target, const struct met_datetime *t );
int buoy_YYYYMMDDHHmm_to_JMMYYGGgg ( struct buoy_chunks *b );
int check_date_from_future ( const struct metreport *m );
char *guess_WMO_region ( char *A1, char *Reg, const char *II, const char *iii );
char * guess_WMO_region_synop ( struct synop_chunks *syn );
char *guess_WMO_region_temp ( struct temp_chunks *temp );
int read_table_c ( char tablec[MAXLINES_TABLEC][92], size_t *nlines_tablec, const char *bufrtables_dir, int ksec1[40] );
int parse_subset_sequence ( struct metreport *m, struct bufr_subset_sequence_data *sq, struct bufr2tac_subset_state *st,
                            const int *kdtlst, size_t nlst, const int *ksec1, char *err );
int find_descriptor ( const int *haystack, size_t nlst, int needle );
int find_descriptor_interval ( const int *haystack, size_t nlst, int needlemin, int needlemax );
int bufr_set_environment ( char *default_bufrtables, char *bufrtables_dir );
int guess_gts_header ( struct gts_header *h, const char *f );
int read_bufr ( unsigned char *bufr, const char *filename, int *length );
int time_period_duration ( const struct bufr2tac_subset_state *s );
int hour_rounded ( const struct synop_chunks *syn );
void direction_to_0877 ( char *dd, uint32_t ival );

char * latlon_to_MMM ( char *target, double lat, double lon );
char * kelvin_to_TTTT ( char *target, double T );
char * kelvin_to_snTTT ( char *target, double T );
char * kelvin_to_snTT ( char *target, double T );
char * kelvin_to_TT ( char *target, double T );
char * kelvin_to_TTTa ( char *target, double T );
char * dewpoint_depression_to_DnDn ( char * target, double T, double Td );
char * m_to_h ( char *target, double h );
char * m_to_hh ( char *target, double h );
char * m_to_9h ( char *target, double h );
char * m_to_RR ( char *target, double m );
char * pascal_to_ppp ( char *target, double P );
char * pascal_to_pnpnpn ( char *target, double P );
char * pascal_to_PPPP ( char *target, double P );
char * percent_to_okta ( char *target, double perc );
char * prec_to_RRR ( char *target, double r );
char * prec_to_RRRR24 ( char *target, double r );
char * secs_to_tt ( char *tt, int secs );
char * vism_to_VV ( char *target, double V );
char * recent_snow_to_ss ( char *target, double r );
char * total_snow_depth_to_sss ( char *target, double r );
char * wind_to_dndnfnfnfn ( char *target, double dd, double ff );
char * grad_to_D ( char *D, double grad );
char * grad_to_ec ( char *target, double grad );
int check_kj_m2 ( double val );
int check_j_cm2 ( double val );

size_t print_geo ( char **geo,  size_t lmax, const struct metreport *m );
size_t print_wigos_id ( char **wid,  size_t lmax, const struct metreport *m );

int print_synop_report ( struct metreport *m);
size_t print_synop_sec0 ( char **sec0, size_t lmax, const struct synop_chunks *syn );
size_t print_synop_sec1 ( char **sec1, size_t lmax, struct synop_chunks *syn );
size_t print_synop_sec2 ( char **sec2, size_t lmax, const struct synop_chunks *syn );
size_t print_synop_sec3 ( char **sec3, size_t lmax, struct synop_chunks *syn );
size_t print_synop_wigos_id ( char **wid,  size_t lmax, struct synop_chunks *syn );

int buoy_YYYYMMDDHHmm_to_JMMYYGGgg ( struct buoy_chunks *b );


int print_buoy_report ( struct metreport *m);
size_t print_buoy_sec0 ( char **sec0, size_t lmax, const struct buoy_chunks *b );
size_t print_buoy_sec1 ( char **sec1, size_t lmax, struct buoy_chunks *b );
size_t print_buoy_sec2 ( char **sec2, size_t lmax, const struct buoy_chunks *b );
size_t print_buoy_sec3 ( char **sec3, size_t lmax, const struct buoy_chunks *b );
size_t print_buoy_wigos_id ( char **wid,  size_t lmax, const struct buoy_chunks *b);

int print_climat_report ( struct metreport *m);
size_t print_climat_sec0 ( char **sec0, size_t lmax, const struct climat_chunks *cl );
size_t print_climat_sec1 ( char **sec1, size_t lmax, struct climat_chunks *cl );
size_t print_climat_sec2 ( char **sec2, size_t lmax, struct climat_chunks *cl );
size_t print_climat_sec3 ( char **sec3, size_t lmax, struct climat_chunks *cl );
size_t print_climat_sec4 ( char **sec4, size_t lmax, struct climat_chunks *cl );
size_t print_climat_wigos_id ( char **wid,  size_t lmax, struct climat_chunks *cl);

int print_temp_report ( struct metreport *m );
int print_temp_a ( struct metreport *m );
size_t print_temp_a_sec1 ( char **sec1, size_t lmax, const struct temp_chunks *t );
size_t print_temp_a_sec2 ( char **sec2, size_t lmax, const struct temp_chunks *t );
size_t print_temp_a_sec3 ( char **sec3, size_t lmax, const struct temp_chunks *t );
size_t print_temp_a_sec4 ( char **sec4, size_t lmax, const struct temp_chunks *t );
size_t print_temp_a_sec7 ( char **sec7, size_t lmax, const struct temp_chunks *t );
int print_temp_b ( struct metreport *m );
size_t print_temp_b_sec1 ( char **sec1, size_t lmax, const struct temp_chunks *t );
size_t print_temp_b_sec5 ( char **sec5, size_t lmax, const struct temp_chunks *t );
size_t print_temp_b_sec6 ( char **sec6, size_t lmax, const struct temp_chunks *t );
size_t print_temp_b_sec7 ( char **sec7, size_t lmax, const struct temp_chunks *t );
size_t print_temp_b_sec8 ( char **sec8, size_t lmax, const struct temp_chunks *t );
int print_temp_c ( struct metreport *m );
size_t print_temp_c_sec1 ( char **sec1, size_t lmax, const struct temp_chunks *t );
size_t print_temp_c_sec2 ( char **sec2, size_t lmax, const struct temp_chunks *t );
size_t print_temp_c_sec3 ( char **sec3, size_t lmax, const struct temp_chunks *t );
size_t print_temp_c_sec4 ( char **sec4, size_t lmax, const struct temp_chunks *t );
size_t print_temp_c_sec7 ( char **sec7, size_t lmax, const struct temp_chunks *t );
int print_temp_d ( struct metreport *m );
size_t print_temp_d_sec1 ( char **sec1, size_t lmax, const struct temp_chunks *t );
size_t print_temp_d_sec5 ( char **sec5, size_t lmax, const struct temp_chunks *t );
size_t print_temp_d_sec6 ( char **sec6, size_t lmax, const struct temp_chunks *t );
size_t print_temp_d_sec7 ( char **sec7, size_t lmax, const struct temp_chunks *t );
size_t print_temp_d_sec8 ( char **sec8, size_t lmax, const struct temp_chunks *t );
size_t print_temp_wigos_id ( char **wid,  size_t lmax, struct temp_chunks *t );

int parse_temp_raw_data ( struct temp_chunks *t, struct temp_raw_data *r );
int parse_temp_raw_wind_shear_data ( struct temp_chunks *t, struct temp_raw_wind_shear_data *w );
int print_temp_raw_data ( const struct temp_raw_data *r );
int print_temp_raw_wind_shear_data ( const struct temp_raw_wind_shear_data *w );

int print_csv ( FILE *f, const struct metreport *m );
int print_json ( FILE *f, const struct metreport *m );
int print_xml ( FILE *f, const struct metreport *m );
int print_plain ( FILE *f, const struct metreport *m );
int print_html ( FILE *f, const struct metreport *m );

int syn_parse_x01 ( struct synop_chunks *syn, struct bufr2tac_subset_state *s );
int syn_parse_x02 ( struct synop_chunks *syn, struct bufr2tac_subset_state *s );
int syn_parse_x04 ( struct synop_chunks *syn, struct bufr2tac_subset_state *s );
int syn_parse_x05 ( struct synop_chunks *syn, struct bufr2tac_subset_state *s );
int syn_parse_x06 ( struct synop_chunks *syn, struct bufr2tac_subset_state *s );
int syn_parse_x07 ( struct synop_chunks *syn, struct bufr2tac_subset_state *s );
int syn_parse_x08 ( const struct synop_chunks *syn, struct bufr2tac_subset_state *s );
int syn_parse_x10 ( struct synop_chunks *syn, struct bufr2tac_subset_state *s );
int syn_parse_x11 ( struct synop_chunks *syn, struct bufr2tac_subset_state *s );
int syn_parse_x12 ( struct synop_chunks *syn, struct bufr2tac_subset_state *s );
int syn_parse_x13 ( struct synop_chunks *syn, struct bufr2tac_subset_state *s );
int syn_parse_x14 ( struct synop_chunks *syn, struct bufr2tac_subset_state *s );
int syn_parse_x20 ( struct synop_chunks *syn, struct bufr2tac_subset_state *s );
int syn_parse_x22 ( struct synop_chunks *syn, struct bufr2tac_subset_state *s );
int syn_parse_x31 ( const struct synop_chunks *syn, struct bufr2tac_subset_state *s );

int buoy_parse_x01 ( struct buoy_chunks *b, struct bufr2tac_subset_state *s );
int buoy_parse_x02 ( struct buoy_chunks *b, struct bufr2tac_subset_state *s );
int buoy_parse_x04 ( struct buoy_chunks *b, struct bufr2tac_subset_state *s );
int buoy_parse_x05 ( struct buoy_chunks *b, struct bufr2tac_subset_state *s );
int buoy_parse_x06 ( struct buoy_chunks *b, struct bufr2tac_subset_state *s );
int buoy_parse_x07 ( const struct buoy_chunks *b, struct bufr2tac_subset_state *s );
int buoy_parse_x08 ( const struct buoy_chunks *b, struct bufr2tac_subset_state *s );
int buoy_parse_x10 ( struct buoy_chunks *b, struct bufr2tac_subset_state *s );
int buoy_parse_x11 ( struct buoy_chunks *b, struct bufr2tac_subset_state *s );
int buoy_parse_x12 ( struct buoy_chunks *b, struct bufr2tac_subset_state *s );
int buoy_parse_x13 ( const struct buoy_chunks *b, struct bufr2tac_subset_state *s );
int buoy_parse_x14 ( const struct buoy_chunks *b, struct bufr2tac_subset_state *s );
int buoy_parse_x20 ( const struct buoy_chunks *b, struct bufr2tac_subset_state *s );
int buoy_parse_x22 ( struct buoy_chunks *b, struct bufr2tac_subset_state *s );
int buoy_parse_x31 ( const struct buoy_chunks *b, struct bufr2tac_subset_state *s );
int buoy_parse_x33 ( struct buoy_chunks *b, struct bufr2tac_subset_state *s );

int climat_parse_x01 ( struct climat_chunks *c, struct bufr2tac_subset_state *s );
int climat_parse_x02 ( struct climat_chunks *c, struct bufr2tac_subset_state *s );
int climat_parse_x04 ( struct climat_chunks *c, struct bufr2tac_subset_state *s );
int climat_parse_x05 ( const struct climat_chunks *c, struct bufr2tac_subset_state *s );
int climat_parse_x06 ( const struct climat_chunks *c, struct bufr2tac_subset_state *s );
int climat_parse_x07 ( const struct climat_chunks *c, struct bufr2tac_subset_state *s );
int climat_parse_x08 ( struct climat_chunks *c, struct bufr2tac_subset_state *s );
int climat_parse_x10 ( struct climat_chunks *c, struct bufr2tac_subset_state *s );
int climat_parse_x11 ( struct climat_chunks *c, struct bufr2tac_subset_state *s );
int climat_parse_x12 ( struct climat_chunks *c, struct bufr2tac_subset_state *s );
int climat_parse_x13 ( struct climat_chunks *c, struct bufr2tac_subset_state *s );
int climat_parse_x14 ( struct climat_chunks *c, struct bufr2tac_subset_state *s );
int climat_parse_x20 ( struct climat_chunks *c, struct bufr2tac_subset_state *s );
int climat_parse_x22 ( struct climat_chunks *c, struct bufr2tac_subset_state *s );
int climat_parse_x31 ( struct climat_chunks *c, struct bufr2tac_subset_state *s );
int climat_parse_x33 ( struct climat_chunks *c, struct bufr2tac_subset_state *s );

int temp_parse_x01 ( struct temp_chunks *t, struct bufr2tac_subset_state *s );
int temp_parse_x02 ( struct temp_chunks *t, struct bufr2tac_subset_state *s );
int temp_parse_x04 ( struct temp_chunks *t, struct bufr2tac_subset_state *s );
int temp_parse_x05 ( struct temp_chunks *t, struct bufr2tac_subset_state *s );
int temp_parse_x06 ( struct temp_chunks *t, struct bufr2tac_subset_state *s );
int temp_parse_x07 ( struct temp_chunks *t, struct bufr2tac_subset_state *s );
int temp_parse_x08 ( const struct temp_chunks *t, struct bufr2tac_subset_state *s );
int temp_parse_x10 ( const struct temp_chunks *t, struct bufr2tac_subset_state *s );
int temp_parse_x11 ( const struct temp_chunks *t, struct bufr2tac_subset_state *s );
int temp_parse_x12 ( const struct temp_chunks *t, struct bufr2tac_subset_state *s );
int temp_parse_x20 ( struct temp_chunks *t, struct bufr2tac_subset_state *s );
int temp_parse_x22 ( struct temp_chunks *t, struct bufr2tac_subset_state *s );
int temp_parse_x31 ( const struct temp_chunks *t, struct bufr2tac_subset_state *s );
int temp_parse_x33 ( struct temp_chunks *t, struct bufr2tac_subset_state *s );

// These are prototypes for used ecmwf bufr library functions
int bus012_ ( int *, unsigned int *, int *, int *, int *, int *, int * );
int buprs0_ ( int * );
int buprs1_ ( int * );
int buprs3_ ( int *, int *, int *, int *, int *, int *, char ** );
int bufrex_ ( int *, int *, int *, int *, int *, int *, int *, int *, int *, char **, char **, int *, double *, char **, int * );
int busel_ ( int *, int *, int *, int *, int * );
int busel2_ ( int *, int *, int *,  char **, int *, char **, char **, char **, int * );
int buukey_ ( int *, int *, int *, int *, int * );
int buprt_ ( int *, int *, int *, int *, char **, char **, char **, int *, double *, int *, int *, int * );

// Global static vars
extern int BUFR2TAC_DEBUG_LEVEL;

#ifndef PATH_MAX
#define PATH_MAX 1024
#endif

extern char BUFR_TABLES_DIRENV[PATH_MAX];


#ifdef __cplusplus
} /* closing brace for extern "C" */
#endif

#endif  // from ifndef BUFR2TAC_H
