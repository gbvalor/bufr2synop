/***************************************************************************
 *   Copyright (C) 2013-2026 by Guillermo Ballester Valor                  *
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

/*! \def REPORT_LENGTH
 *  \brief Maximum length for a meteorological report in alphanumeric format (16KB)
 */
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

/*!
  \fn char *bufr2tac_get_version(char *version, size_t dversion, char *build, size_t dbuild, char *builder, size_t dbuilder, int *version_major, int *version_minor, int *version_patch)
  \brief Get version information of bufr2tac library
  \param [out] version String where to write version string
  \param [in] dversion Size of version buffer
  \param [out] build String where to write build date
  \param [in] dbuild Size of build buffer
  \param [out] builder String where to write builder information
  \param [in] dbuilder Size of builder buffer
  \param [out] version_major Pointer to integer for major version number
  \param [out] version_minor Pointer to integer for minor version number
  \param [out] version_patch Pointer to integer for patch version number
  \return Pointer to version string on success, NULL on error
*/
char *bufr2tac_get_version(char *version, size_t dversion, char *build, size_t dbuild, char *builder, size_t dbuilder, 
                           int *version_major, int *version_minor, int *version_patch);


/* Error/debug functions */

/*!
  \fn int bufr2tac_push_error(struct bufr2tac_error_stack *e, int severity, const char *description)
  \brief Push an error or warning into the error stack
  \param [in,out] e Pointer to error stack
  \param [in] severity Level of severity (1=warning, 2=error)
  \param [in] description String with error description
  \return 0 on success, 1 if stack is full
*/
int bufr2tac_push_error ( struct bufr2tac_error_stack *e, int severity, const char *description );

/*!
  \fn int bufr2tac_clean_error_stack(struct bufr2tac_error_stack *e)
  \brief Clean/reset an error stack
  \param [in,out] e Pointer to error stack to clean
  \return 0 on success
*/
int bufr2tac_clean_error_stack ( struct bufr2tac_error_stack *e );

/*!
  \fn int bufr2tac_set_error(struct bufr2tac_subset_state *s, int severity, const char *origin, const char *explanation)
  \brief Set an error in the subset state error stack
  \param [in,out] s Pointer to subset state
  \param [in] severity Level of severity (1=warning, 2=error)
  \param [in] origin String indicating where the error occurred
  \param [in] explanation String explaining the error
  \return 0 on success, 1 on error
*/
int bufr2tac_set_error ( struct bufr2tac_subset_state *s, int severity, const char *origin, const char *explanation);

/*!
  \fn int bufr2tac_print_error(const struct bufr2tac_error_stack *e)
  \brief Print all errors/warnings in error stack
  \param [in] e Pointer to error stack to print
  \return 0 on success
*/
int bufr2tac_print_error ( const struct bufr2tac_error_stack *e );

/*!
  \fn int bufr2tac_set_debug_level(int level)
  \brief Set debug level for library
  \param [in] level Debug level (0=no debug, higher=more verbose)
  \return Previous debug level
*/
int bufr2tac_set_debug_level(int level);



/*!
  \fn void bufr2tac_clean_buoy_chunks(struct buoy_chunks *b)
  \brief Clean/reset all data in a buoy_chunks structure
  \param [in,out] b Pointer to buoy_chunks structure to clean
*/
void bufr2tac_clean_buoy_chunks ( struct buoy_chunks *b );

/*!
  \fn void bufr2tac_clean_synop_chunks(struct synop_chunks *s)
  \brief Clean/reset all data in a synop_chunks structure
  \param [in,out] s Pointer to synop_chunks structure to clean
*/
void bufr2tac_clean_synop_chunks ( struct synop_chunks *s );

/*!
  \fn void bufr2tac_clean_temp_chunks(struct temp_chunks *t)
  \brief Clean/reset all data in a temp_chunks structure
  \param [in,out] t Pointer to temp_chunks structure to clean
*/
void bufr2tac_clean_temp_chunks ( struct temp_chunks *t );

/*!
  \fn void bufr2tac_clean_climat_chunks(struct climat_chunks *c)
  \brief Clean/reset all data in a climat_chunks structure
  \param [in,out] c Pointer to climat_chunks structure to clean
*/
void bufr2tac_clean_climat_chunks ( struct climat_chunks *c );

/*!
  \fn void bufr2tac_clean_metreport(struct metreport *m)
  \brief Clean/reset all data in a metreport structure
  \param [in,out] m Pointer to metreport structure to clean
*/
void bufr2tac_clean_metreport (struct metreport *m);



/*!
  \fn int set_environment(char *default_bufrtables, char *bufrtables_dir)
  \brief Set up the environment for BUFR tables directory
  \param [in] default_bufrtables Default BUFR tables path
  \param [out] bufrtables_dir String where to write the configured directory
  \return 0 on success, 1 on error
*/
int set_environment ( char *default_bufrtables, char *bufrtables_dir );

/*!
  \fn int integer_to_descriptor(struct bufr_descriptor *d, int id)
  \brief Convert an integer to a BUFR descriptor structure
  \param [out] d Pointer to descriptor structure to fill
  \param [in] id Integer representation of descriptor
  \return 0 on success, 1 on error
*/
int integer_to_descriptor ( struct bufr_descriptor *d, int id );

/*!
  \fn int descriptor_to_integer(int *id, const struct bufr_descriptor *d)
  \brief Convert a BUFR descriptor structure to an integer
  \param [out] id Pointer to integer where to write result
  \param [in] d Pointer to descriptor structure
  \return 0 on success, 1 on error
*/
int descriptor_to_integer ( int *id, const struct bufr_descriptor *d );

/*!
  \fn unsigned int three_bytes_to_uint(const unsigned char *source)
  \brief Convert three bytes to an unsigned integer
  \param [in] source Pointer to three bytes array
  \return Unsigned integer value
*/
unsigned int three_bytes_to_uint ( const unsigned char *source );

/*!
  \fn char * charray_to_string(char *s, const unsigned char *buf, size_t size)
  \brief Convert a character array to a null-terminated string
  \param [out] s Pointer to output string buffer
  \param [in] buf Pointer to input character array
  \param [in] size Size of input array
  \return Pointer to output string
*/
char * charray_to_string ( char *s, const unsigned char *buf, size_t size );

/*!
  \fn char * adjust_string(char *s)
  \brief Adjust a string removing trailing spaces and control characters
  \param [in,out] s String to adjust
  \return Pointer to adjusted string
*/
char * adjust_string ( char *s );

/*!
  \fn char * get_explained_table_val(char *expl, size_t dim, const char tablec[MAXLINES_TABLEC][92], size_t nlines_tablec, const struct bufr_descriptor *d, int ival)
  \brief Get explanation from table C for a given descriptor value
  \param [out] expl Buffer where to write explanation
  \param [in] dim Size of explanation buffer
  \param [in] tablec Table C data
  \param [in] nlines_tablec Number of lines in table C
  \param [in] d Pointer to descriptor
  \param [in] ival Integer value to explain
  \return Pointer to explanation string
*/
char * get_explained_table_val ( char *expl, size_t dim, const char tablec[MAXLINES_TABLEC][92], size_t nlines_tablec,
                                 const struct bufr_descriptor *d, int ival );

/*!
  \fn char * get_explained_flag_val(char *expl, size_t dim, const char tablec[MAXLINES_TABLEC][92], size_t nlines_tablec, const struct bufr_descriptor *d, unsigned long ival)
  \brief Get explanation from table C for a given flag descriptor value
  \param [out] expl Buffer where to write explanation
  \param [in] dim Size of explanation buffer
  \param [in] tablec Table C data
  \param [in] nlines_tablec Number of lines in table C
  \param [in] d Pointer to descriptor
  \param [in] ival Unsigned long value to explain
  \return Pointer to explanation string
*/
char * get_explained_flag_val ( char *expl, size_t dim, const char tablec[MAXLINES_TABLEC][92], size_t nlines_tablec,
                                const struct bufr_descriptor *d, unsigned long ival );

/*!
  \fn char * get_ecmwf_tablename(char *target, char type, const char *bufrtables_dir, const int ksec1[40])
  \brief Get ECMWF BUFR table filename
  \param [out] target Buffer where to write table filename
  \param [in] type Type of table ('B', 'C', or 'D')
  \param [in] bufrtables_dir Directory where tables are located
  \param [in] ksec1 BUFR section 1 data array
  \return Pointer to table filename string
*/
char * get_ecmwf_tablename ( char *target, char type, const char *bufrtables_dir, const int ksec1[40] );

/*!
  \fn int parse_subset_as_buoy(struct metreport *m, struct bufr2tac_subset_state *s, struct bufr_subset_sequence_data *sq, char *err)
  \brief Parse a BUFR subset as a buoy report
  \param [out] m Pointer to metreport structure to fill
  \param [in,out] s Pointer to subset state
  \param [in] sq Pointer to subset sequence data
  \param [out] err Buffer for error messages
  \return 0 on success, 1 on error
*/
int parse_subset_as_buoy ( struct metreport *m, struct bufr2tac_subset_state *s, struct bufr_subset_sequence_data *sq,
                           char *err );

/*!
  \fn int parse_subset_as_synop(struct metreport *m, struct bufr2tac_subset_state *s, struct bufr_subset_sequence_data *sq, char *err)
  \brief Parse a BUFR subset as a synop report
  \param [out] m Pointer to metreport structure to fill
  \param [in,out] s Pointer to subset state
  \param [in] sq Pointer to subset sequence data
  \param [out] err Buffer for error messages
  \return 0 on success, 1 on error
*/
int parse_subset_as_synop ( struct metreport *m, struct bufr2tac_subset_state *s, struct bufr_subset_sequence_data *sq,
                            char *err );

/*!
  \fn int parse_subset_as_temp(struct metreport *m, struct bufr2tac_subset_state *s, struct bufr_subset_sequence_data *sq, char *err)
  \brief Parse a BUFR subset as a temp report
  \param [out] m Pointer to metreport structure to fill
  \param [in,out] s Pointer to subset state
  \param [in] sq Pointer to subset sequence data
  \param [out] err Buffer for error messages
  \return 0 on success, 1 on error
*/
int parse_subset_as_temp ( struct metreport *m, struct bufr2tac_subset_state *s, struct bufr_subset_sequence_data *sq,
                           char *err );

/*!
  \fn int parse_subset_as_climat(struct metreport *m, struct bufr2tac_subset_state *s, struct bufr_subset_sequence_data *sq, char *err)
  \brief Parse a BUFR subset as a climat report
  \param [out] m Pointer to metreport structure to fill
  \param [in,out] s Pointer to subset state
  \param [in] sq Pointer to subset sequence data
  \param [out] err Buffer for error messages
  \return 0 on success, 1 on error
*/
int parse_subset_as_climat ( struct metreport *m, struct bufr2tac_subset_state *s, struct bufr_subset_sequence_data *sq,
                             char *err );

/*!
  \fn int YYYYMMDDHHmm_to_met_datetime(struct met_datetime *t, const char *source)
  \brief Convert a YYYYMMDDHHmm string to met_datetime structure
  \param [out] t Pointer to met_datetime structure to fill
  \param [in] source String in YYYYMMDDHHmm format
  \return 0 on success, 1 on error
*/
int YYYYMMDDHHmm_to_met_datetime ( struct met_datetime *t, const char *source );

/*!
  \fn int round_met_datetime_to_hour(struct met_datetime *target, const struct met_datetime *source)
  \brief Round a datetime to the nearest hour
  \param [out] target Pointer to target datetime structure
  \param [in] source Pointer to source datetime structure
  \return 0 on success, 1 on error
*/
int round_met_datetime_to_hour ( struct met_datetime *target, const struct met_datetime *source );

/*!
  \fn int synop_YYYYMMDDHHmm_to_YYGG(struct synop_chunks *syn)
  \brief Convert datetime in synop structure to YYGG format
  \param [in,out] syn Pointer to synop_chunks structure
  \return 0 on success, 1 on error
*/
int synop_YYYYMMDDHHmm_to_YYGG ( struct synop_chunks *syn );

/*!
  \fn char *met_datetime_to_YYGG(char *target, size_t lmax, const struct met_datetime *t)
  \brief Convert met_datetime to YYGG string format
  \param [out] target Buffer where to write YYGG string
  \param [in] lmax Maximum size of target buffer
  \param [in] t Pointer to met_datetime structure
  \return Pointer to target string
*/
char *met_datetime_to_YYGG ( char *target, size_t lmax, const struct met_datetime *t );

/*!
  \fn int buoy_YYYYMMDDHHmm_to_JMMYYGGgg(struct buoy_chunks *b)
  \brief Convert datetime in buoy structure to JMMYYGGgg format
  \param [in,out] b Pointer to buoy_chunks structure
  \return 0 on success, 1 on error
*/
int buoy_YYYYMMDDHHmm_to_JMMYYGGgg ( struct buoy_chunks *b );

/*!
  \fn int check_date_from_future(const struct metreport *m)
  \brief Check if a report date is in the future
  \param [in] m Pointer to metreport structure
  \return 0 if date is valid, 1 if from future
*/
int check_date_from_future ( const struct metreport *m );

/*!
  \fn char *guess_WMO_region(char *A1, char *Reg, const char *II, const char *iii)
  \brief Guess WMO region from station indices
  \param [out] A1 Buffer for area indicator (A1)
  \param [out] Reg Buffer for region name
  \param [in] II Station block number
  \param [in] iii Station number
  \return Pointer to A1 string
*/
char *guess_WMO_region ( char *A1, char *Reg, const char *II, const char *iii );

/*!
  \fn char * guess_WMO_region_synop(struct synop_chunks *syn)
  \brief Guess WMO region for a synop report
  \param [in,out] syn Pointer to synop_chunks structure
  \return Pointer to A1 string in synop structure
*/
char * guess_WMO_region_synop ( struct synop_chunks *syn );

/*!
  \fn char *guess_WMO_region_temp(struct temp_chunks *temp)
  \brief Guess WMO region for a temp report
  \param [in,out] temp Pointer to temp_chunks structure
  \return Pointer to A1 string in temp structure
*/
char *guess_WMO_region_temp ( struct temp_chunks *temp );

/*!
  \fn int read_table_c(char tablec[MAXLINES_TABLEC][92], size_t *nlines_tablec, const char *bufrtables_dir, int ksec1[40])
  \brief Read BUFR table C
  \param [out] tablec Array where to store table C data
  \param [out] nlines_tablec Pointer to store number of lines read
  \param [in] bufrtables_dir Directory where tables are located
  \param [in] ksec1 BUFR section 1 data array
  \return 0 on success, 1 on error
*/
int read_table_c ( char tablec[MAXLINES_TABLEC][92], size_t *nlines_tablec, const char *bufrtables_dir, int ksec1[40] );

/*!
  \fn int parse_subset_sequence(struct metreport *m, struct bufr_subset_sequence_data *sq, struct bufr2tac_subset_state *st, const int *kdtlst, size_t nlst, const int *ksec1, char *err)
  \brief Parse a BUFR subset sequence
  \param [out] m Pointer to metreport structure to fill
  \param [in] sq Pointer to subset sequence data
  \param [in,out] st Pointer to subset state
  \param [in] kdtlst Array of descriptor list
  \param [in] nlst Number of descriptors in list
  \param [in] ksec1 BUFR section 1 data array
  \param [out] err Buffer for error messages
  \return 0 on success, 1 on error
*/
int parse_subset_sequence ( struct metreport *m, struct bufr_subset_sequence_data *sq, struct bufr2tac_subset_state *st,
                            const int *kdtlst, size_t nlst, const int *ksec1, char *err );

/*!
  \fn int find_descriptor(const int *haystack, size_t nlst, int needle)
  \brief Find a descriptor in a descriptor list
  \param [in] haystack Array of descriptors to search in
  \param [in] nlst Number of descriptors in array
  \param [in] needle Descriptor to find
  \return Index of descriptor if found, -1 otherwise
*/
int find_descriptor ( const int *haystack, size_t nlst, int needle );

/*!
  \fn int find_descriptor_interval(const int *haystack, size_t nlst, int needlemin, int needlemax)
  \brief Find a descriptor in a range within a descriptor list
  \param [in] haystack Array of descriptors to search in
  \param [in] nlst Number of descriptors in array
  \param [in] needlemin Minimum descriptor value in range
  \param [in] needlemax Maximum descriptor value in range
  \return Index of first descriptor in range if found, -1 otherwise
*/
int find_descriptor_interval ( const int *haystack, size_t nlst, int needlemin, int needlemax );

/*!
  \fn int bufr_set_environment(char *default_bufrtables, char *bufrtables_dir)
  \brief Set up BUFR environment and tables directory
  \param [in] default_bufrtables Default BUFR tables path
  \param [out] bufrtables_dir String where to write the configured directory
  \return 0 on success, 1 on error
*/
int bufr_set_environment ( char *default_bufrtables, char *bufrtables_dir );

/*!
  \fn int guess_gts_header(struct gts_header *h, const char *f)
  \brief Guess GTS header from filename
  \param [out] h Pointer to gts_header structure to fill
  \param [in] f Filename string
  \return 0 on success, 1 on error
*/
int guess_gts_header ( struct gts_header *h, const char *f );

/*!
  \fn int read_bufr(unsigned char *bufr, const char *filename, int *length)
  \brief Read a BUFR file into memory
  \param [out] bufr Buffer where to store BUFR data
  \param [in] filename Name of file to read
  \param [out] length Pointer to store length of data read
  \return 0 on success, 1 on error
*/
int read_bufr ( unsigned char *bufr, const char *filename, int *length );

/*!
  \fn int time_period_duration(const struct bufr2tac_subset_state *s)
  \brief Get time period duration from subset state
  \param [in] s Pointer to subset state
  \return Duration in seconds, -1 on error
*/
int time_period_duration ( const struct bufr2tac_subset_state *s );

/*!
  \fn int hour_rounded(const struct synop_chunks *syn)
  \brief Check if synop hour is rounded
  \param [in] syn Pointer to synop_chunks structure
  \return 1 if rounded, 0 otherwise
*/
int hour_rounded ( const struct synop_chunks *syn );

/*!
  \fn void direction_to_0877(char *dd, size_t lmax, uint32_t ival)
  \brief Convert direction integer value to 0877 format string
  \param [out] dd Buffer where to write direction string
  \param [in] lmax Maximum size of dd buffer
  \param [in] ival Integer direction value
*/
void direction_to_0877 ( char *dd, size_t lmax, uint32_t ival );



// Unit conversion functions

/*!
  \fn char * latlon_to_MMM(char *target, size_t lmax, double lat, double lon)
  \brief Convert latitude and longitude to MMM format (Marsden square)
  \param [out] target Buffer where to write MMM string
  \param [in] lmax Maximum size of target buffer
  \param [in] lat Latitude in degrees
  \param [in] lon Longitude in degrees
  \return Pointer to target string
*/
char * latlon_to_MMM ( char *target, size_t lmax, double lat, double lon );

/*!
  \fn char * kelvin_to_TTTT(char *target, size_t lmax, double T)
  \brief Convert temperature from Kelvin to TTTT format (tenths of degree)
  \param [out] target Buffer where to write TTTT string
  \param [in] lmax Maximum size of target buffer
  \param [in] T Temperature in Kelvin
  \return Pointer to target string
*/
char * kelvin_to_TTTT ( char *target, size_t lmax, double T );

/*!
  \fn char * kelvin_to_snTTT(char *target, size_t lmax, double T)
  \brief Convert temperature from Kelvin to snTTT format with sign
  \param [out] target Buffer where to write snTTT string
  \param [in] lmax Maximum size of target buffer
  \param [in] T Temperature in Kelvin
  \return Pointer to target string
*/
char * kelvin_to_snTTT ( char *target, size_t lmax, double T );

/*!
  \fn char * kelvin_to_snTT(char *target, size_t lmax, double T)
  \brief Convert temperature from Kelvin to snTT format with sign
  \param [out] target Buffer where to write snTT string
  \param [in] lmax Maximum size of target buffer
  \param [in] T Temperature in Kelvin
  \return Pointer to target string
*/
char * kelvin_to_snTT ( char *target, size_t lmax, double T );

/*!
  \fn char * kelvin_to_TT(char *target, size_t lmax, double T)
  \brief Convert temperature from Kelvin to TT format (whole degrees)
  \param [out] target Buffer where to write TT string
  \param [in] lmax Maximum size of target buffer
  \param [in] T Temperature in Kelvin
  \return Pointer to target string
*/
char * kelvin_to_TT ( char *target, size_t lmax, double T );

/*!
  \fn char * kelvin_to_TTTa(char *target, size_t lmax, double T)
  \brief Convert temperature from Kelvin to TTTa format (air temperature)
  \param [out] target Buffer where to write TTTa string
  \param [in] lmax Maximum size of target buffer
  \param [in] T Temperature in Kelvin
  \return Pointer to target string
*/
char * kelvin_to_TTTa ( char *target, size_t lmax, double T );

/*!
  \fn char * dewpoint_depression_to_DnDn(char * target, size_t lmax, double T, double Td)
  \brief Convert temperature and dewpoint to dewpoint depression DnDn format
  \param [out] target Buffer where to write DnDn string
  \param [in] lmax Maximum size of target buffer
  \param [in] T Temperature in Kelvin
  \param [in] Td Dewpoint temperature in Kelvin
  \return Pointer to target string
*/
char * dewpoint_depression_to_DnDn ( char * target, size_t lmax, double T, double Td );

/*!
  \fn char * m_to_h(char *target, size_t lmax, double h)
  \brief Convert height from meters to h format
  \param [out] target Buffer where to write h string
  \param [in] lmax Maximum size of target buffer
  \param [in] h Height in meters
  \return Pointer to target string
*/
char * m_to_h ( char *target, size_t lmax, double h );

/*!
  \fn char * m_to_hh(char *target, size_t lmax, double h)
  \brief Convert height from meters to hh format
  \param [out] target Buffer where to write hh string
  \param [in] lmax Maximum size of target buffer
  \param [in] h Height in meters
  \return Pointer to target string
*/
char * m_to_hh ( char *target, size_t lmax, double h );

/*!
  \fn char * m_to_9h(char *target, size_t lmax, double h)
  \brief Convert height from meters to 9h format (cloud height)
  \param [out] target Buffer where to write 9h string
  \param [in] lmax Maximum size of target buffer
  \param [in] h Height in meters
  \return Pointer to target string
*/
char * m_to_9h ( char *target, size_t lmax, double h );

/*!
  \fn char * m_to_RR(char *target, size_t lmax, double m)
  \brief Convert precipitation from meters to RR format (mm)
  \param [out] target Buffer where to write RR string
  \param [in] lmax Maximum size of target buffer
  \param [in] m Precipitation in meters
  \return Pointer to target string
*/
char * m_to_RR ( char *target, size_t lmax, double m );

/*!
  \fn char * pascal_to_ppp(char *target, size_t lmax, double P)
  \brief Convert pressure from Pascals to ppp format (tenths of hPa)
  \param [out] target Buffer where to write ppp string
  \param [in] lmax Maximum size of target buffer
  \param [in] P Pressure in Pascals
  \return Pointer to target string
*/
char * pascal_to_ppp ( char *target, size_t lmax, double P );

/*!
  \fn char * pascal_to_pnpnpn(char *target, size_t lmax, double P)
  \brief Convert pressure from Pascals to pnpnpn format (pressure change)
  \param [out] target Buffer where to write pnpnpn string
  \param [in] lmax Maximum size of target buffer
  \param [in] P Pressure in Pascals
  \return Pointer to target string
*/
char * pascal_to_pnpnpn ( char *target, size_t lmax, double P );

/*!
  \fn char * pascal_to_PPPP(char *target, size_t lmax, double P)
  \brief Convert pressure from Pascals to PPPP format (hPa)
  \param [out] target Buffer where to write PPPP string
  \param [in] lmax Maximum size of target buffer
  \param [in] P Pressure in Pascals
  \return Pointer to target string
*/
char * pascal_to_PPPP ( char *target, size_t lmax, double P );

/*!
  \fn char * percent_to_okta(char *target, size_t lmax, double perc)
  \brief Convert cloud cover from percentage to okta format
  \param [out] target Buffer where to write okta string
  \param [in] lmax Maximum size of target buffer
  \param [in] perc Cloud cover in percentage
  \return Pointer to target string
*/
char * percent_to_okta ( char *target, size_t lmax, double perc );

/*!
  \fn char * prec_to_RRR(char *target, size_t lmax, double r)
  \brief Convert precipitation to RRR format (mm)
  \param [out] target Buffer where to write RRR string
  \param [in] lmax Maximum size of target buffer
  \param [in] r Precipitation in kg/m2 or mm
  \return Pointer to target string
*/
char * prec_to_RRR ( char *target, size_t lmax, double r );

/*!
  \fn char * prec_to_RRRR24(char *target, size_t lmax, double r)
  \brief Convert 24-hour precipitation to RRRR24 format (mm)
  \param [out] target Buffer where to write RRRR24 string
  \param [in] lmax Maximum size of target buffer
  \param [in] r Precipitation in kg/m2 or mm
  \return Pointer to target string
*/
char * prec_to_RRRR24 ( char *target, size_t lmax, double r );

/*!
  \fn char * secs_to_tt(char *tt, size_t lmax, int secs)
  \brief Convert time period from seconds to tt format
  \param [out] tt Buffer where to write tt string
  \param [in] lmax Maximum size of tt buffer
  \param [in] secs Time period in seconds
  \return Pointer to tt string
*/
char * secs_to_tt ( char *tt, size_t lmax, int secs );

/*!
  \fn char * vism_to_VV(char *target, size_t lmax, double V)
  \brief Convert visibility from meters to VV format (km)
  \param [out] target Buffer where to write VV string
  \param [in] lmax Maximum size of target buffer
  \param [in] V Visibility in meters
  \return Pointer to target string
*/
char * vism_to_VV ( char *target, size_t lmax, double V );

/*!
  \fn char * recent_snow_to_ss(char *target, size_t lmax, double r)
  \brief Convert recent snow depth to ss format (cm)
  \param [out] target Buffer where to write ss string
  \param [in] lmax Maximum size of target buffer
  \param [in] r Snow depth in meters
  \return Pointer to target string
*/
char * recent_snow_to_ss ( char *target, size_t lmax, double r );

/*!
  \fn char * total_snow_depth_to_sss(char *target, size_t lmax, double r)
  \brief Convert total snow depth to sss format (cm)
  \param [out] target Buffer where to write sss string
  \param [in] lmax Maximum size of target buffer
  \param [in] r Snow depth in meters
  \return Pointer to target string
*/
char * total_snow_depth_to_sss ( char *target, size_t lmax, double r );

/*!
  \fn char * wind_to_dndnfnfnfn(char *target, size_t lmax, double dd, double ff)
  \brief Convert wind direction and speed to dndnfnfnfn format
  \param [out] target Buffer where to write wind string
  \param [in] lmax Maximum size of target buffer
  \param [in] dd Wind direction in degrees
  \param [in] ff Wind speed in m/s
  \return Pointer to target string
*/
char * wind_to_dndnfnfnfn ( char *target, size_t lmax, double dd, double ff );

/*!
  \fn char * grad_to_D(char *D, size_t lmax, double grad)
  \brief Convert angle from gradians to D format
  \param [out] D Buffer where to write D string
  \param [in] lmax Maximum size of D buffer
  \param [in] grad Angle in gradians
  \return Pointer to D string
*/
char * grad_to_D ( char *D, size_t lmax, double grad );

/*!
  \fn char * grad_to_ec(char *target, size_t lmax, double grad)
  \brief Convert angle from gradians to ec format (elevation correction)
  \param [out] target Buffer where to write ec string
  \param [in] lmax Maximum size of target buffer
  \param [in] grad Angle in gradians
  \return Pointer to target string
*/
char * grad_to_ec ( char *target, size_t lmax, double grad );

/*!
  \fn int check_kj_m2(double val)
  \brief Check if value is valid in kJ/m2 units
  \param [in] val Value to check
  \return 1 if valid, 0 otherwise
*/
int check_kj_m2 ( double val );

/*!
  \fn int check_j_cm2(double val)
  \brief Check if value is valid in J/cm2 units
  \param [in] val Value to check
  \return 1 if valid, 0 otherwise
*/
int check_j_cm2 ( double val );



// Geographic and WIGOS ID print functions

/*!
  \fn size_t print_geo(char **geo, size_t lmax, const struct metreport *m)
  \brief Print geographic information (lat/lon/alt) from metreport
  \param [out] geo Pointer to string pointer where to write geographic info
  \param [in] lmax Maximum size available
  \param [in] m Pointer to metreport structure
  \return Number of bytes written
*/
size_t print_geo ( char **geo,  size_t lmax, const struct metreport *m );

/*!
  \fn size_t print_wigos_id(char **wid, size_t lmax, const struct metreport *m)
  \brief Print WIGOS identifier from metreport
  \param [out] wid Pointer to string pointer where to write WIGOS ID
  \param [in] lmax Maximum size available
  \param [in] m Pointer to metreport structure
  \return Number of bytes written
*/
size_t print_wigos_id ( char **wid,  size_t lmax, const struct metreport *m );

// SYNOP print functions

/*!
  \fn int print_synop_report(struct metreport *m)
  \brief Print a complete SYNOP report from metreport structure
  \param [in,out] m Pointer to metreport structure containing SYNOP data
  \return 0 on success, 1 on error
*/
int print_synop_report ( struct metreport *m);

/*!
  \fn size_t print_synop_sec0(char **sec0, size_t lmax, const struct synop_chunks *syn)
  \brief Print SYNOP section 0 (header)
  \param [out] sec0 Pointer to string pointer where to write section 0
  \param [in] lmax Maximum size available
  \param [in] syn Pointer to synop_chunks structure
  \return Number of bytes written
*/
size_t print_synop_sec0 ( char **sec0, size_t lmax, const struct synop_chunks *syn );

/*!
  \fn size_t print_synop_sec1(char **sec1, size_t lmax, struct synop_chunks *syn)
  \brief Print SYNOP section 1 (surface observations)
  \param [out] sec1 Pointer to string pointer where to write section 1
  \param [in] lmax Maximum size available
  \param [in,out] syn Pointer to synop_chunks structure
  \return Number of bytes written
*/
size_t print_synop_sec1 ( char **sec1, size_t lmax, struct synop_chunks *syn );

/*!
  \fn size_t print_synop_sec2(char **sec2, size_t lmax, const struct synop_chunks *syn)
  \brief Print SYNOP section 2 (ship/mobile station data)
  \param [out] sec2 Pointer to string pointer where to write section 2
  \param [in] lmax Maximum size available
  \param [in] syn Pointer to synop_chunks structure
  \return Number of bytes written
*/
size_t print_synop_sec2 ( char **sec2, size_t lmax, const struct synop_chunks *syn );

/*!
  \fn size_t print_synop_sec3(char **sec3, size_t lmax, struct synop_chunks *syn)
  \brief Print SYNOP section 3 (regional/national data)
  \param [out] sec3 Pointer to string pointer where to write section 3
  \param [in] lmax Maximum size available
  \param [in,out] syn Pointer to synop_chunks structure
  \return Number of bytes written
*/
size_t print_synop_sec3 ( char **sec3, size_t lmax, struct synop_chunks *syn );

/*!
  \fn size_t print_synop_wigos_id(char **wid, size_t lmax, struct synop_chunks *syn)
  \brief Print WIGOS identifier for SYNOP report
  \param [out] wid Pointer to string pointer where to write WIGOS ID
  \param [in] lmax Maximum size available
  \param [in,out] syn Pointer to synop_chunks structure
  \return Number of bytes written
*/
size_t print_synop_wigos_id ( char **wid,  size_t lmax, struct synop_chunks *syn );

int buoy_YYYYMMDDHHmm_to_JMMYYGGgg ( struct buoy_chunks *b );

// BUOY print functions

/*!
  \fn int print_buoy_report(struct metreport *m)
  \brief Print a complete BUOY report from metreport structure
  \param [in,out] m Pointer to metreport structure containing BUOY data
  \return 0 on success, 1 on error
*/
int print_buoy_report ( struct metreport *m);

/*!
  \fn size_t print_buoy_sec0(char **sec0, size_t lmax, const struct buoy_chunks *b)
  \brief Print BUOY section 0 (header)
  \param [out] sec0 Pointer to string pointer where to write section 0
  \param [in] lmax Maximum size available
  \param [in] b Pointer to buoy_chunks structure
  \return Number of bytes written
*/
size_t print_buoy_sec0 ( char **sec0, size_t lmax, const struct buoy_chunks *b );

/*!
  \fn size_t print_buoy_sec1(char **sec1, size_t lmax, struct buoy_chunks *b)
  \brief Print BUOY section 1 (identification and position)
  \param [out] sec1 Pointer to string pointer where to write section 1
  \param [in] lmax Maximum size available
  \param [in,out] b Pointer to buoy_chunks structure
  \return Number of bytes written
*/
size_t print_buoy_sec1 ( char **sec1, size_t lmax, struct buoy_chunks *b );

/*!
  \fn size_t print_buoy_sec2(char **sec2, size_t lmax, const struct buoy_chunks *b)
  \brief Print BUOY section 2 (meteorological data)
  \param [out] sec2 Pointer to string pointer where to write section 2
  \param [in] lmax Maximum size available
  \param [in] b Pointer to buoy_chunks structure
  \return Number of bytes written
*/
size_t print_buoy_sec2 ( char **sec2, size_t lmax, const struct buoy_chunks *b );

/*!
  \fn size_t print_buoy_sec3(char **sec3, size_t lmax, const struct buoy_chunks *b)
  \brief Print BUOY section 3 (regional/national data)
  \param [out] sec3 Pointer to string pointer where to write section 3
  \param [in] lmax Maximum size available
  \param [in] b Pointer to buoy_chunks structure
  \return Number of bytes written
*/
size_t print_buoy_sec3 ( char **sec3, size_t lmax, const struct buoy_chunks *b );

/*!
  \fn size_t print_buoy_wigos_id(char **wid, size_t lmax, const struct buoy_chunks *b)
  \brief Print WIGOS identifier for BUOY report
  \param [out] wid Pointer to string pointer where to write WIGOS ID
  \param [in] lmax Maximum size available
  \param [in] b Pointer to buoy_chunks structure
  \return Number of bytes written
*/
size_t print_buoy_wigos_id ( char **wid,  size_t lmax, const struct buoy_chunks *b);

// CLIMAT print functions

/*!
  \fn int print_climat_report(struct metreport *m)
  \brief Print a complete CLIMAT report from metreport structure
  \param [in,out] m Pointer to metreport structure containing CLIMAT data
  \return 0 on success, 1 on error
*/
int print_climat_report ( struct metreport *m);

/*!
  \fn size_t print_climat_sec0(char **sec0, size_t lmax, const struct climat_chunks *cl)
  \brief Print CLIMAT section 0 (header)
  \param [out] sec0 Pointer to string pointer where to write section 0
  \param [in] lmax Maximum size available
  \param [in] cl Pointer to climat_chunks structure
  \return Number of bytes written
*/
size_t print_climat_sec0 ( char **sec0, size_t lmax, const struct climat_chunks *cl );

/*!
  \fn size_t print_climat_sec1(char **sec1, size_t lmax, struct climat_chunks *cl)
  \brief Print CLIMAT section 1 (monthly means)
  \param [out] sec1 Pointer to string pointer where to write section 1
  \param [in] lmax Maximum size available
  \param [in,out] cl Pointer to climat_chunks structure
  \return Number of bytes written
*/
size_t print_climat_sec1 ( char **sec1, size_t lmax, struct climat_chunks *cl );

/*!
  \fn size_t print_climat_sec2(char **sec2, size_t lmax, struct climat_chunks *cl)
  \brief Print CLIMAT section 2 (monthly extremes)
  \param [out] sec2 Pointer to string pointer where to write section 2
  \param [in] lmax Maximum size available
  \param [in,out] cl Pointer to climat_chunks structure
  \return Number of bytes written
*/
size_t print_climat_sec2 ( char **sec2, size_t lmax, struct climat_chunks *cl );

/*!
  \fn size_t print_climat_sec3(char **sec3, size_t lmax, struct climat_chunks *cl)
  \brief Print CLIMAT section 3 (monthly totals)
  \param [out] sec3 Pointer to string pointer where to write section 3
  \param [in] lmax Maximum size available
  \param [in,out] cl Pointer to climat_chunks structure
  \return Number of bytes written
*/
size_t print_climat_sec3 ( char **sec3, size_t lmax, struct climat_chunks *cl );

/*!
  \fn size_t print_climat_sec4(char **sec4, size_t lmax, struct climat_chunks *cl)
  \brief Print CLIMAT section 4 (regional data)
  \param [out] sec4 Pointer to string pointer where to write section 4
  \param [in] lmax Maximum size available
  \param [in,out] cl Pointer to climat_chunks structure
  \return Number of bytes written
*/
size_t print_climat_sec4 ( char **sec4, size_t lmax, struct climat_chunks *cl );

/*!
  \fn size_t print_climat_wigos_id(char **wid, size_t lmax, struct climat_chunks *cl)
  \brief Print WIGOS identifier for CLIMAT report
  \param [out] wid Pointer to string pointer where to write WIGOS ID
  \param [in] lmax Maximum size available
  \param [in,out] cl Pointer to climat_chunks structure
  \return Number of bytes written
*/
size_t print_climat_wigos_id ( char **wid,  size_t lmax, struct climat_chunks *cl);

// TEMP print functions

/*!
  \fn int print_temp_report(struct metreport *m)
  \brief Print a complete TEMP report from metreport structure
  \param [in,out] m Pointer to metreport structure containing TEMP data
  \return 0 on success, 1 on error
*/
int print_temp_report ( struct metreport *m );

/*!
  \fn int print_temp_a(struct metreport *m)
  \brief Print TEMP part A (mandatory levels)
  \param [in,out] m Pointer to metreport structure
  \return 0 on success, 1 on error
*/
int print_temp_a ( struct metreport *m );

/*!
  \fn size_t print_temp_a_sec1(char **sec1, size_t lmax, const struct temp_chunks *t)
  \brief Print TEMP part A section 1
  \param [out] sec1 Pointer to string pointer where to write section 1
  \param [in] lmax Maximum size available
  \param [in] t Pointer to temp_chunks structure
  \return Number of bytes written
*/
size_t print_temp_a_sec1 ( char **sec1, size_t lmax, const struct temp_chunks *t );

/*!
  \fn size_t print_temp_a_sec2(char **sec2, size_t lmax, const struct temp_chunks *t)
  \brief Print TEMP part A section 2
  \param [out] sec2 Pointer to string pointer where to write section 2
  \param [in] lmax Maximum size available
  \param [in] t Pointer to temp_chunks structure
  \return Number of bytes written
*/
size_t print_temp_a_sec2 ( char **sec2, size_t lmax, const struct temp_chunks *t );

/*!
  \fn size_t print_temp_a_sec3(char **sec3, size_t lmax, const struct temp_chunks *t)
  \brief Print TEMP part A section 3
  \param [out] sec3 Pointer to string pointer where to write section 3
  \param [in] lmax Maximum size available
  \param [in] t Pointer to temp_chunks structure
  \return Number of bytes written
*/
size_t print_temp_a_sec3 ( char **sec3, size_t lmax, const struct temp_chunks *t );

/*!
  \fn size_t print_temp_a_sec4(char **sec4, size_t lmax, const struct temp_chunks *t)
  \brief Print TEMP part A section 4
  \param [out] sec4 Pointer to string pointer where to write section 4
  \param [in] lmax Maximum size available
  \param [in] t Pointer to temp_chunks structure
  \return Number of bytes written
*/
size_t print_temp_a_sec4 ( char **sec4, size_t lmax, const struct temp_chunks *t );

/*!
  \fn size_t print_temp_a_sec7(char **sec7, size_t lmax, const struct temp_chunks *t)
  \brief Print TEMP part A section 7
  \param [out] sec7 Pointer to string pointer where to write section 7
  \param [in] lmax Maximum size available
  \param [in] t Pointer to temp_chunks structure
  \return Number of bytes written
*/
size_t print_temp_a_sec7 ( char **sec7, size_t lmax, const struct temp_chunks *t );

/*!
  \fn int print_temp_b(struct metreport *m)
  \brief Print TEMP part B (wind at mandatory levels)
  \param [in,out] m Pointer to metreport structure
  \return 0 on success, 1 on error
*/
int print_temp_b ( struct metreport *m );

/*!
  \fn size_t print_temp_b_sec1(char **sec1, size_t lmax, const struct temp_chunks *t)
  \brief Print TEMP part B section 1
  \param [out] sec1 Pointer to string pointer where to write section 1
  \param [in] lmax Maximum size available
  \param [in] t Pointer to temp_chunks structure
  \return Number of bytes written
*/
size_t print_temp_b_sec1 ( char **sec1, size_t lmax, const struct temp_chunks *t );

/*!
  \fn size_t print_temp_b_sec5(char **sec5, size_t lmax, const struct temp_chunks *t)
  \brief Print TEMP part B section 5
  \param [out] sec5 Pointer to string pointer where to write section 5
  \param [in] lmax Maximum size available
  \param [in] t Pointer to temp_chunks structure
  \return Number of bytes written
*/
size_t print_temp_b_sec5 ( char **sec5, size_t lmax, const struct temp_chunks *t );

 /*!
  \fn size_t print_temp_b_sec6(char **sec6, size_t lmax, const struct temp_chunks *t)
  \brief Print TEMP part B section 6
  \param [out] sec6 Pointer to string pointer where to write section 6
  \param [in] lmax Maximum size available
  \param [in] t Pointer to temp_chunks structure
  \return Number of bytes written
*/
size_t print_temp_b_sec6 ( char **sec6, size_t lmax, const struct temp_chunks *t );

/*!
  \fn size_t print_temp_b_sec7(char **sec7, size_t lmax, const struct temp_chunks *t)
  \brief Print TEMP part B section 7
  \param [out] sec7 Pointer to string pointer where to write section 7
  \param [in] lmax Maximum size available
  \param [in] t Pointer to temp_chunks structure
  \return Number of bytes written
*/
size_t print_temp_b_sec7 ( char **sec7, size_t lmax, const struct temp_chunks *t );

/*!
  \fn size_t print_temp_b_sec8(char **sec8, size_t lmax, const struct temp_chunks *t)
  \brief Print TEMP part B section 8
  \param [out] sec8 Pointer to string pointer where to write section 8
  \param [in] lmax Maximum size available
  \param [in] t Pointer to temp_chunks structure
  \return Number of bytes written
*/
size_t print_temp_b_sec8 ( char **sec8, size_t lmax, const struct temp_chunks *t );

/*!
  \fn int print_temp_c(struct metreport *m)
  \brief Print TEMP part C (significant levels)
  \param [in,out] m Pointer to metreport structure
  \return 0 on success, 1 on error
*/
int print_temp_c ( struct metreport *m );

/*!
  \fn size_t print_temp_c_sec1(char **sec1, size_t lmax, const struct temp_chunks *t)
  \brief Print TEMP part C section 1
  \param [out] sec1 Pointer to string pointer where to write section 1
  \param [in] lmax Maximum size available
  \param [in] t Pointer to temp_chunks structure
  \return Number of bytes written
*/
size_t print_temp_c_sec1 ( char **sec1, size_t lmax, const struct temp_chunks *t );

/*!
  \fn size_t print_temp_c_sec2(char **sec2, size_t lmax, const struct temp_chunks *t)
  \brief Print TEMP part C section 2
  \param [out] sec2 Pointer to string pointer where to write section 2
  \param [in] lmax Maximum size available
  \param [in] t Pointer to temp_chunks structure
  \return Number of bytes written
*/
size_t print_temp_c_sec2 ( char **sec2, size_t lmax, const struct temp_chunks *t );

/*!
  \fn size_t print_temp_c_sec3(char **sec3, size_t lmax, const struct temp_chunks *t)
  \brief Print TEMP part C section 3
  \param [out] sec3 Pointer to string pointer where to write section 3
  \param [in] lmax Maximum size available
  \param [in] t Pointer to temp_chunks structure
  \return Number of bytes written
*/
size_t print_temp_c_sec3 ( char **sec3, size_t lmax, const struct temp_chunks *t );

/*!
  \fn size_t print_temp_c_sec4(char **sec4, size_t lmax, const struct temp_chunks *t)
  \brief Print TEMP part C section 4
  \param [out] sec4 Pointer to string pointer where to write section 4
  \param [in] lmax Maximum size available
  \param [in] t Pointer to temp_chunks structure
  \return Number of bytes written
*/
size_t print_temp_c_sec4 ( char **sec4, size_t lmax, const struct temp_chunks *t );

/*!
  \fn size_t print_temp_c_sec7(char **sec7, size_t lmax, const struct temp_chunks *t)
  \brief Print TEMP part C section 7
  \param [out] sec7 Pointer to string pointer where to write section 7
  \param [in] lmax Maximum size available
  \param [in] t Pointer to temp_chunks structure
  \return Number of bytes written
*/
size_t print_temp_c_sec7 ( char **sec7, size_t lmax, const struct temp_chunks *t );

/*!
  \fn int print_temp_d(struct metreport *m)
  \brief Print TEMP part D (wind at significant levels)
  \param [in,out] m Pointer to metreport structure
  \return 0 on success, 1 on error
*/
int print_temp_d ( struct metreport *m );

/*!
  \fn size_t print_temp_d_sec1(char **sec1, size_t lmax, const struct temp_chunks *t)
  \brief Print TEMP part D section 1
  \param [out] sec1 Pointer to string pointer where to write section 1
  \param [in] lmax Maximum size available
  \param [in] t Pointer to temp_chunks structure
  \return Number of bytes written
*/
size_t print_temp_d_sec1 ( char **sec1, size_t lmax, const struct temp_chunks *t );

/*!
  \fn size_t print_temp_d_sec5(char **sec5, size_t lmax, const struct temp_chunks *t)
  \brief Print TEMP part D section 5
  \param [out] sec5 Pointer to string pointer where to write section 5
  \param [in] lmax Maximum size available
  \param [in] t Pointer to temp_chunks structure
  \return Number of bytes written
*/
size_t print_temp_d_sec5 ( char **sec5, size_t lmax, const struct temp_chunks *t );

/*!
  \fn size_t print_temp_d_sec6(char **sec6, size_t lmax, const struct temp_chunks *t)
  \brief Print TEMP part D section 6
  \param [out] sec6 Pointer to string pointer where to write section 6
  \param [in] lmax Maximum size available
  \param [in] t Pointer to temp_chunks structure
  \return Number of bytes written
*/
size_t print_temp_d_sec6 ( char **sec6, size_t lmax, const struct temp_chunks *t );

/*!
  \fn size_t print_temp_d_sec7(char **sec7, size_t lmax, const struct temp_chunks *t)
  \brief Print TEMP part D section 7
  \param [out] sec7 Pointer to string pointer where to write section 7
  \param [in] lmax Maximum size available
  \param [in] t Pointer to temp_chunks structure
  \return Number of bytes written
*/
size_t print_temp_d_sec7 ( char **sec7, size_t lmax, const struct temp_chunks *t );

/*!
  \fn size_t print_temp_d_sec8(char **sec8, size_t lmax, const struct temp_chunks *t)
  \brief Print TEMP part D section 8
  \param [out] sec8 Pointer to string pointer where to write section 8
  \param [in] lmax Maximum size available
  \param [in] t Pointer to temp_chunks structure
  \return Number of bytes written
*/
size_t print_temp_d_sec8 ( char **sec8, size_t lmax, const struct temp_chunks *t );

/*!
  \fn size_t print_temp_wigos_id(char **wid, size_t lmax, struct temp_chunks *t)
  \brief Print WIGOS identifier for TEMP report
  \param [out] wid Pointer to string pointer where to write WIGOS ID
  \param [in] lmax Maximum size available
  \param [in,out] t Pointer to temp_chunks structure
  \return Number of bytes written
*/
size_t print_temp_wigos_id ( char **wid,  size_t lmax, struct temp_chunks *t );

// TEMP raw data parsing functions

/*!
  \fn int parse_temp_raw_data(struct temp_chunks *t, struct temp_raw_data *r)
  \brief Parse raw temperature/humidity data from TEMP
  \param [out] t Pointer to temp_chunks structure to fill
  \param [in] r Pointer to temp_raw_data structure with raw data
  \return 0 on success, 1 on error
*/
int parse_temp_raw_data ( struct temp_chunks *t, struct temp_raw_data *r );

/*!
  \fn int parse_temp_raw_wind_shear_data(struct temp_chunks *t, struct temp_raw_wind_shear_data *w)
  \brief Parse raw wind shear data from TEMP
  \param [out] t Pointer to temp_chunks structure to fill
  \param [in] w Pointer to temp_raw_wind_shear_data structure with raw data
  \return 0 on success, 1 on error
*/
int parse_temp_raw_wind_shear_data ( struct temp_chunks *t, struct temp_raw_wind_shear_data *w );

/*!
  \fn int print_temp_raw_data(const struct temp_raw_data *r)
  \brief Print raw temperature/humidity data for debugging
  \param [in] r Pointer to temp_raw_data structure
  \return 0 on success, 1 on error
*/
int print_temp_raw_data ( const struct temp_raw_data *r );

/*!
  \fn int print_temp_raw_wind_shear_data(const struct temp_raw_wind_shear_data *w)
  \brief Print raw wind shear data for debugging
  \param [in] w Pointer to temp_raw_wind_shear_data structure
  \return 0 on success, 1 on error
*/
int print_temp_raw_wind_shear_data ( const struct temp_raw_wind_shear_data *w );

// Output format functions

/*!
  \fn int print_csv(FILE *f, const struct metreport *m)
  \brief Print metreport in CSV format
  \param [in] f File pointer where to write output
  \param [in] m Pointer to metreport structure
  \return 0 on success, 1 on error
*/
int print_csv ( FILE *f, const struct metreport *m );

/*!
  \fn int print_json(FILE *f, const struct metreport *m)
  \brief Print metreport in JSON format
  \param [in] f File pointer where to write output
  \param [in] m Pointer to metreport structure
  \return 0 on success, 1 on error
*/
int print_json ( FILE *f, const struct metreport *m );

/*!
  \fn int print_xml(FILE *f, const struct metreport *m)
  \brief Print metreport in XML format
  \param [in] f File pointer where to write output
  \param [in] m Pointer to metreport structure
  \return 0 on success, 1 on error
*/
int print_xml ( FILE *f, const struct metreport *m );

/*!
  \fn int print_plain(FILE *f, const struct metreport *m)
  \brief Print metreport in plain text format
  \param [in] f File pointer where to write output
  \param [in] m Pointer to metreport structure
  \return 0 on success, 1 on error
*/
int print_plain ( FILE *f, const struct metreport *m );

/*!
  \fn int print_html(FILE *f, const struct metreport *m)
  \brief Print metreport in HTML format
  \param [in] f File pointer where to write output
  \param [in] m Pointer to metreport structure
  \return 0 on success, 1 on error
*/
int print_html ( FILE *f, const struct metreport *m );

// SYNOP descriptor parsing functions (class X)
// These functions parse BUFR descriptors from class X (0-3) for SYNOP reports
// Each function handles specific descriptor categories

/*!
  \fn int syn_parse_x01(struct synop_chunks *syn, struct bufr2tac_subset_state *s)
  \brief Parse BUFR class 01 descriptors (identification) for SYNOP
  \param [in,out] syn Pointer to synop_chunks structure to fill
  \param [in,out] s Pointer to subset state
  \return 0 on success, 1 on error
*/
int syn_parse_x01 ( struct synop_chunks *syn, struct bufr2tac_subset_state *s );

/*!
  \fn int syn_parse_x02(struct synop_chunks *syn, struct bufr2tac_subset_state *s)
  \brief Parse BUFR class 02 descriptors (instrumentation) for SYNOP
  \param [in,out] syn Pointer to synop_chunks structure to fill
  \param [in,out] s Pointer to subset state
  \return 0 on success, 1 on error
*/
int syn_parse_x02 ( struct synop_chunks *syn, struct bufr2tac_subset_state *s );

/*!
  \fn int syn_parse_x04(struct synop_chunks *syn, struct bufr2tac_subset_state *s)
  \brief Parse BUFR class 04 descriptors (location) for SYNOP
  \param [in,out] syn Pointer to synop_chunks structure to fill
  \param [in,out] s Pointer to subset state
  \return 0 on success, 1 on error
*/
int syn_parse_x04 ( struct synop_chunks *syn, struct bufr2tac_subset_state *s );

/*!
  \fn int syn_parse_x05(struct synop_chunks *syn, struct bufr2tac_subset_state *s)
  \brief Parse BUFR class 05 descriptors (horizontal coordinates) for SYNOP
  \param [in,out] syn Pointer to synop_chunks structure to fill
  \param [in,out] s Pointer to subset state
  \return 0 on success, 1 on error
*/
int syn_parse_x05 ( struct synop_chunks *syn, struct bufr2tac_subset_state *s );

/*!
  \fn int syn_parse_x06(struct synop_chunks *syn, struct bufr2tac_subset_state *s)
  \brief Parse BUFR class 06 descriptors (vertical coordinates) for SYNOP
  \param [in,out] syn Pointer to synop_chunks structure to fill
  \param [in,out] s Pointer to subset state
  \return 0 on success, 1 on error
*/
int syn_parse_x06 ( struct synop_chunks *syn, struct bufr2tac_subset_state *s );

/*!
  \fn int syn_parse_x07(struct synop_chunks *syn, struct bufr2tac_subset_state *s)
  \brief Parse BUFR class 07 descriptors (height, altitude, pressure) for SYNOP
  \param [in,out] syn Pointer to synop_chunks structure to fill
  \param [in,out] s Pointer to subset state
  \return 0 on success, 1 on error
*/
int syn_parse_x07 ( struct synop_chunks *syn, struct bufr2tac_subset_state *s );

/*!
  \fn int syn_parse_x08(const struct synop_chunks *syn, struct bufr2tac_subset_state *s)
  \brief Parse BUFR class 08 descriptors (significance qualifiers) for SYNOP
  \param [in] syn Pointer to synop_chunks structure
  \param [in,out] s Pointer to subset state
  \return 0 on success, 1 on error
*/
int syn_parse_x08 ( const struct synop_chunks *syn, struct bufr2tac_subset_state *s );

/*!
  \fn int syn_parse_x10(struct synop_chunks *syn, struct bufr2tac_subset_state *s)
  \brief Parse BUFR class 10 descriptors (pressure) for SYNOP
  \param [in,out] syn Pointer to synop_chunks structure to fill
  \param [in,out] s Pointer to subset state
  \return 0 on success, 1 on error
*/
int syn_parse_x10 ( struct synop_chunks *syn, struct bufr2tac_subset_state *s );

/*!
  \fn int syn_parse_x11(struct synop_chunks *syn, struct bufr2tac_subset_state *s)
  \brief Parse BUFR class 11 descriptors (wind data) for SYNOP
  \param [in,out] syn Pointer to synop_chunks structure to fill
  \param [in,out] s Pointer to subset state
  \return 0 on success, 1 on error
*/
int syn_parse_x11 ( struct synop_chunks *syn, struct bufr2tac_subset_state *s );

/*!
  \fn int syn_parse_x12(struct synop_chunks *syn, struct bufr2tac_subset_state *s)
  \brief Parse BUFR class 12 descriptors (temperature and dew point) for SYNOP
  \param [in,out] syn Pointer to synop_chunks structure to fill
  \param [in,out] s Pointer to subset state
  \return 0 on success, 1 on error
*/
int syn_parse_x12 ( struct synop_chunks *syn, struct bufr2tac_subset_state *s );

/*!
  \fn int syn_parse_x13(struct synop_chunks *syn, struct bufr2tac_subset_state *s)
  \brief Parse BUFR class 13 descriptors (humidity and moisture) for SYNOP
  \param [in,out] syn Pointer to synop_chunks structure to fill
  \param [in,out] s Pointer to subset state
  \return 0 on success, 1 on error
*/
int syn_parse_x13 ( struct synop_chunks *syn, struct bufr2tac_subset_state *s );

/*!
  \fn int syn_parse_x14(struct synop_chunks *syn, struct bufr2tac_subset_state *s)
  \brief Parse BUFR class 14 descriptors (radiation and brightness) for SYNOP
  \param [in,out] syn Pointer to synop_chunks structure to fill
  \param [in,out] s Pointer to subset state
  \return 0 on success, 1 on error
*/
int syn_parse_x14 ( struct synop_chunks *syn, struct bufr2tac_subset_state *s );

/*!
  \fn int syn_parse_x20(struct synop_chunks *syn, struct bufr2tac_subset_state *s)
  \brief Parse BUFR class 20 descriptors (visibility, clouds, and weather phenomena) for SYNOP
  \param [in,out] syn Pointer to synop_chunks structure to fill
  \param [in,out] s Pointer to subset state
  \return 0 on success, 1 on error
*/
int syn_parse_x20 ( struct synop_chunks *syn, struct bufr2tac_subset_state *s );

/*!
  \fn int syn_parse_x22(struct synop_chunks *syn, struct bufr2tac_subset_state *s)
  \brief Parse BUFR class 22 descriptors (oceanographic and hydrographic data) for SYNOP
  \param [in,out] syn Pointer to synop_chunks structure to fill
  \param [in,out] s Pointer to subset state
  \return 0 on success, 1 on error
*/
int syn_parse_x22 ( struct synop_chunks *syn, struct bufr2tac_subset_state *s );

/*!
  \fn int syn_parse_x31(const struct synop_chunks *syn, struct bufr2tac_subset_state *s)
  \brief Parse BUFR class 31 descriptors (data description and delayed replication factors) for SYNOP
  \param [in] syn Pointer to synop_chunks structure
  \param [in,out] s Pointer to subset state
  \return 0 on success, 1 on error
*/
int syn_parse_x31 ( const struct synop_chunks *syn, struct bufr2tac_subset_state *s );

// BUOY descriptor parsing functions (class X)
// These functions parse BUFR descriptors from class X (0-3) for BUOY reports
// Each function handles specific descriptor categories for marine data

/*!
  \fn int buoy_parse_x01(struct buoy_chunks *b, struct bufr2tac_subset_state *s)
  \brief Parse BUFR class 01 descriptors (identification) for BUOY
  \param [in,out] b Pointer to buoy_chunks structure to fill
  \param [in,out] s Pointer to subset state
  \return 0 on success, 1 on error
*/
int buoy_parse_x01 ( struct buoy_chunks *b, struct bufr2tac_subset_state *s );

/*!
  \fn int buoy_parse_x02(struct buoy_chunks *b, struct bufr2tac_subset_state *s)
  \brief Parse BUFR class 02 descriptors (instrumentation) for BUOY
  \param [in,out] b Pointer to buoy_chunks structure to fill
  \param [in,out] s Pointer to subset state
  \return 0 on success, 1 on error
*/
int buoy_parse_x02 ( struct buoy_chunks *b, struct bufr2tac_subset_state *s );

/*!
  \fn int buoy_parse_x04(struct buoy_chunks *b, struct bufr2tac_subset_state *s)
  \brief Parse BUFR class 04 descriptors (location) for BUOY
  \param [in,out] b Pointer to buoy_chunks structure to fill
  \param [in,out] s Pointer to subset state
  \return 0 on success, 1 on error
*/
int buoy_parse_x04 ( struct buoy_chunks *b, struct bufr2tac_subset_state *s );

/*!
  \fn int buoy_parse_x05(struct buoy_chunks *b, struct bufr2tac_subset_state *s)
  \brief Parse BUFR class 05 descriptors (horizontal coordinates) for BUOY
  \param [in,out] b Pointer to buoy_chunks structure to fill
  \param [in,out] s Pointer to subset state
  \return 0 on success, 1 on error
*/
int buoy_parse_x05 ( struct buoy_chunks *b, struct bufr2tac_subset_state *s );

/*!
  \fn int buoy_parse_x06(struct buoy_chunks *b, struct bufr2tac_subset_state *s)
  \brief Parse BUFR class 06 descriptors (vertical coordinates) for BUOY
  \param [in,out] b Pointer to buoy_chunks structure to fill
  \param [in,out] s Pointer to subset state
  \return 0 on success, 1 on error
*/
int buoy_parse_x06 ( struct buoy_chunks *b, struct bufr2tac_subset_state *s );

/*!
  \fn int buoy_parse_x07(const struct buoy_chunks *b, struct bufr2tac_subset_state *s)
  \brief Parse BUFR class 07 descriptors (height, altitude, pressure) for BUOY
  \param [in] b Pointer to buoy_chunks structure
  \param [in,out] s Pointer to subset state
  \return 0 on success, 1 on error
*/
int buoy_parse_x07 ( const struct buoy_chunks *b, struct bufr2tac_subset_state *s );

/*!
  \fn int buoy_parse_x08(const struct buoy_chunks *b, struct bufr2tac_subset_state *s)
  \brief Parse BUFR class 08 descriptors (significance qualifiers) for BUOY
  \param [in] b Pointer to buoy_chunks structure
  \param [in,out] s Pointer to subset state
  \return 0 on success, 1 on error
*/
int buoy_parse_x08 ( const struct buoy_chunks *b, struct bufr2tac_subset_state *s );

/*!
  \fn int buoy_parse_x10(struct buoy_chunks *b, struct bufr2tac_subset_state *s)
  \brief Parse BUFR class 10 descriptors (pressure) for BUOY
  \param [in,out] b Pointer to buoy_chunks structure to fill
  \param [in,out] s Pointer to subset state
  \return 0 on success, 1 on error
*/
int buoy_parse_x10 ( struct buoy_chunks *b, struct bufr2tac_subset_state *s );

/*!
  \fn int buoy_parse_x11(struct buoy_chunks *b, struct bufr2tac_subset_state *s)
  \brief Parse BUFR class 11 descriptors (wind data) for BUOY
  \param [in,out] b Pointer to buoy_chunks structure to fill
  \param [in,out] s Pointer to subset state
  \return 0 on success, 1 on error
*/
int buoy_parse_x11 ( struct buoy_chunks *b, struct bufr2tac_subset_state *s );

/*!
  \fn int buoy_parse_x12(struct buoy_chunks *b, struct bufr2tac_subset_state *s)
  \brief Parse BUFR class 12 descriptors (temperature and dew point) for BUOY
  \param [in,out] b Pointer to buoy_chunks structure to fill
  \param [in,out] s Pointer to subset state
  \return 0 on success, 1 on error
*/
int buoy_parse_x12 ( struct buoy_chunks *b, struct bufr2tac_subset_state *s );

/*!
  \fn int buoy_parse_x13(const struct buoy_chunks *b, struct bufr2tac_subset_state *s)
  \brief Parse BUFR class 13 descriptors (humidity and moisture) for BUOY
  \param [in] b Pointer to buoy_chunks structure
  \param [in,out] s Pointer to subset state
  \return 0 on success, 1 on error
*/
int buoy_parse_x13 ( const struct buoy_chunks *b, struct bufr2tac_subset_state *s );

/*!
  \fn int buoy_parse_x14(const struct buoy_chunks *b, struct bufr2tac_subset_state *s)
  \brief Parse BUFR class 14 descriptors (radiation and brightness) for BUOY
  \param [in] b Pointer to buoy_chunks structure
  \param [in,out] s Pointer to subset state
  \return 0 on success, 1 on error
*/
int buoy_parse_x14 ( const struct buoy_chunks *b, struct bufr2tac_subset_state *s );

/*!
  \fn int buoy_parse_x20(const struct buoy_chunks *b, struct bufr2tac_subset_state *s)
  \brief Parse BUFR class 20 descriptors (visibility, clouds, and weather phenomena) for BUOY
  \param [in] b Pointer to buoy_chunks structure
  \param [in,out] s Pointer to subset state
  \return 0 on success, 1 on error
*/
int buoy_parse_x20 ( const struct buoy_chunks *b, struct bufr2tac_subset_state *s );

/*!
  \fn int buoy_parse_x22(struct buoy_chunks *b, struct bufr2tac_subset_state *s)
  \brief Parse BUFR class 22 descriptors (oceanographic and hydrographic data) for BUOY
  \param [in,out] b Pointer to buoy_chunks structure to fill
  \param [in,out] s Pointer to subset state
  \return 0 on success, 1 on error
*/
int buoy_parse_x22 ( struct buoy_chunks *b, struct bufr2tac_subset_state *s );

/*!
  \fn int buoy_parse_x31(const struct buoy_chunks *b, struct bufr2tac_subset_state *s)
  \brief Parse BUFR class 31 descriptors (data description and delayed replication factors) for BUOY
  \param [in] b Pointer to buoy_chunks structure
  \param [in,out] s Pointer to subset state
  \return 0 on success, 1 on error
*/
int buoy_parse_x31 ( const struct buoy_chunks *b, struct bufr2tac_subset_state *s );

/*!
  \fn int buoy_parse_x33(struct buoy_chunks *b, struct bufr2tac_subset_state *s)
  \brief Parse BUFR class 33 descriptors (data quality information) for BUOY
  \param [in,out] b Pointer to buoy_chunks structure to fill
  \param [in,out] s Pointer to subset state
  \return 0 on success, 1 on error
*/
int buoy_parse_x33 ( struct buoy_chunks *b, struct bufr2tac_subset_state *s );

// CLIMAT descriptor parsing functions (class X)
// These functions parse BUFR descriptors from class X (0-3) for CLIMAT reports
// Each function handles specific descriptor categories for climate data

/*!
  \fn int climat_parse_x01(struct climat_chunks *c, struct bufr2tac_subset_state *s)
  \brief Parse BUFR class 01 descriptors (identification) for CLIMAT
  \param [in,out] c Pointer to climat_chunks structure to fill
  \param [in,out] s Pointer to subset state
  \return 0 on success, 1 on error
*/
int climat_parse_x01 ( struct climat_chunks *c, struct bufr2tac_subset_state *s );

/*!
  \fn int climat_parse_x02(struct climat_chunks *c, struct bufr2tac_subset_state *s)
  \brief Parse BUFR class 02 descriptors (instrumentation) for CLIMAT
  \param [in,out] c Pointer to climat_chunks structure to fill
  \param [in,out] s Pointer to subset state
  \return 0 on success, 1 on error
*/
int climat_parse_x02 ( struct climat_chunks *c, struct bufr2tac_subset_state *s );

/*!
  \fn int climat_parse_x04(struct climat_chunks *c, struct bufr2tac_subset_state *s)
  \brief Parse BUFR class 04 descriptors (location) for CLIMAT
  \param [in,out] c Pointer to climat_chunks structure to fill
  \param [in,out] s Pointer to subset state
  \return 0 on success, 1 on error
*/
int climat_parse_x04 ( struct climat_chunks *c, struct bufr2tac_subset_state *s );

/*!
  \fn int climat_parse_x05(const struct climat_chunks *c, struct bufr2tac_subset_state *s)
  \brief Parse BUFR class 05 descriptors (horizontal coordinates) for CLIMAT
  \param [in] c Pointer to climat_chunks structure
  \param [in,out] s Pointer to subset state
  \return 0 on success, 1 on error
*/
int climat_parse_x05 ( const struct climat_chunks *c, struct bufr2tac_subset_state *s );

/*!
  \fn int climat_parse_x06(const struct climat_chunks *c, struct bufr2tac_subset_state *s)
  \brief Parse BUFR class 06 descriptors (vertical coordinates) for CLIMAT
  \param [in] c Pointer to climat_chunks structure
  \param [in,out] s Pointer to subset state
  \return 0 on success, 1 on error
*/
int climat_parse_x06 ( const struct climat_chunks *c, struct bufr2tac_subset_state *s );

/*!
  \fn int climat_parse_x07(const struct climat_chunks *c, struct bufr2tac_subset_state *s)
  \brief Parse BUFR class 07 descriptors (height, altitude, pressure) for CLIMAT
  \param [in] c Pointer to climat_chunks structure
  \param [in,out] s Pointer to subset state
  \return 0 on success, 1 on error
*/
int climat_parse_x07 ( const struct climat_chunks *c, struct bufr2tac_subset_state *s );

/*!
  \fn int climat_parse_x08(struct climat_chunks *c, struct bufr2tac_subset_state *s)
  \brief Parse BUFR class 08 descriptors (significance qualifiers) for CLIMAT
  \param [in,out] c Pointer to climat_chunks structure to fill
  \param [in,out] s Pointer to subset state
  \return 0 on success, 1 on error
*/
int climat_parse_x08 ( struct climat_chunks *c, struct bufr2tac_subset_state *s );

/*!
  \fn int climat_parse_x10(struct climat_chunks *c, struct bufr2tac_subset_state *s)
  \brief Parse BUFR class 10 descriptors (pressure) for CLIMAT
  \param [in,out] c Pointer to climat_chunks structure to fill
  \param [in,out] s Pointer to subset state
  \return 0 on success, 1 on error
*/
int climat_parse_x10 ( struct climat_chunks *c, struct bufr2tac_subset_state *s );

/*!
  \fn int climat_parse_x11(struct climat_chunks *c, struct bufr2tac_subset_state *s)
  \brief Parse BUFR class 11 descriptors (wind data) for CLIMAT
  \param [in,out] c Pointer to climat_chunks structure to fill
  \param [in,out] s Pointer to subset state
  \return 0 on success, 1 on error
*/
int climat_parse_x11 ( struct climat_chunks *c, struct bufr2tac_subset_state *s );

/*!
  \fn int climat_parse_x12(struct climat_chunks *c, struct bufr2tac_subset_state *s)
  \brief Parse BUFR class 12 descriptors (temperature and dew point) for CLIMAT
  \param [in,out] c Pointer to climat_chunks structure to fill
  \param [in,out] s Pointer to subset state
  \return 0 on success, 1 on error
*/
int climat_parse_x12 ( struct climat_chunks *c, struct bufr2tac_subset_state *s );

/*!
  \fn int climat_parse_x13(struct climat_chunks *c, struct bufr2tac_subset_state *s)
  \brief Parse BUFR class 13 descriptors (humidity and moisture) for CLIMAT
  \param [in,out] c Pointer to climat_chunks structure to fill
  \param [in,out] s Pointer to subset state
  \return 0 on success, 1 on error
*/
int climat_parse_x13 ( struct climat_chunks *c, struct bufr2tac_subset_state *s );

/*!
  \fn int climat_parse_x14(struct climat_chunks *c, struct bufr2tac_subset_state *s)
  \brief Parse BUFR class 14 descriptors (radiation and brightness) for CLIMAT
  \param [in,out] c Pointer to climat_chunks structure to fill
  \param [in,out] s Pointer to subset state
  \return 0 on success, 1 on error
*/
int climat_parse_x14 ( struct climat_chunks *c, struct bufr2tac_subset_state *s );

/*!
  \fn int climat_parse_x20(struct climat_chunks *c, struct bufr2tac_subset_state *s)
  \brief Parse BUFR class 20 descriptors (visibility, clouds, and weather phenomena) for CLIMAT
  \param [in,out] c Pointer to climat_chunks structure to fill
  \param [in,out] s Pointer to subset state
  \return 0 on success, 1 on error
*/
int climat_parse_x20 ( struct climat_chunks *c, struct bufr2tac_subset_state *s );

/*!
  \fn int climat_parse_x22(struct climat_chunks *c, struct bufr2tac_subset_state *s)
  \brief Parse BUFR class 22 descriptors (oceanographic and hydrographic data) for CLIMAT
  \param [in,out] c Pointer to climat_chunks structure to fill
  \param [in,out] s Pointer to subset state
  \return 0 on success, 1 on error
*/
int climat_parse_x22 ( struct climat_chunks *c, struct bufr2tac_subset_state *s );

/*!
  \fn int climat_parse_x31(struct climat_chunks *c, struct bufr2tac_subset_state *s)
  \brief Parse BUFR class 31 descriptors (data description and delayed replication factors) for CLIMAT
  \param [in,out] c Pointer to climat_chunks structure to fill
  \param [in,out] s Pointer to subset state
  \return 0 on success, 1 on error
*/
int climat_parse_x31 ( struct climat_chunks *c, struct bufr2tac_subset_state *s );

/*!
  \fn int climat_parse_x33(struct climat_chunks *c, struct bufr2tac_subset_state *s)
  \brief Parse BUFR class 33 descriptors (data quality information) for CLIMAT
  \param [in,out] c Pointer to climat_chunks structure to fill
  \param [in,out] s Pointer to subset state
  \return 0 on success, 1 on error
*/
int climat_parse_x33 ( struct climat_chunks *c, struct bufr2tac_subset_state *s );

// TEMP descriptor parsing functions (class X)
// These functions parse BUFR descriptors from class X (0-3) for TEMP/PILOT reports
// Each function handles specific descriptor categories for upper-air soundings

/*!
  \fn int temp_parse_x01(struct temp_chunks *t, struct bufr2tac_subset_state *s)
  \brief Parse BUFR class 01 descriptors (identification) for TEMP
  \param [in,out] t Pointer to temp_chunks structure to fill
  \param [in,out] s Pointer to subset state
  \return 0 on success, 1 on error
*/
int temp_parse_x01 ( struct temp_chunks *t, struct bufr2tac_subset_state *s );

/*!
  \fn int temp_parse_x02(struct temp_chunks *t, struct bufr2tac_subset_state *s)
  \brief Parse BUFR class 02 descriptors (instrumentation) for TEMP
  \param [in,out] t Pointer to temp_chunks structure to fill
  \param [in,out] s Pointer to subset state
  \return 0 on success, 1 on error
*/
int temp_parse_x02 ( struct temp_chunks *t, struct bufr2tac_subset_state *s );

/*!
  \fn int temp_parse_x04(struct temp_chunks *t, struct bufr2tac_subset_state *s)
  \brief Parse BUFR class 04 descriptors (location) for TEMP
  \param [in,out] t Pointer to temp_chunks structure to fill
  \param [in,out] s Pointer to subset state
  \return 0 on success, 1 on error
*/
int temp_parse_x04 ( struct temp_chunks *t, struct bufr2tac_subset_state *s );

/*!
  \fn int temp_parse_x05(struct temp_chunks *t, struct bufr2tac_subset_state *s)
  \brief Parse BUFR class 05 descriptors (horizontal coordinates) for TEMP
  \param [in,out] t Pointer to temp_chunks structure to fill
  \param [in,out] s Pointer to subset state
  \return 0 on success, 1 on error
*/
int temp_parse_x05 ( struct temp_chunks *t, struct bufr2tac_subset_state *s );

/*!
  \fn int temp_parse_x06(struct temp_chunks *t, struct bufr2tac_subset_state *s)
  \brief Parse BUFR class 06 descriptors (vertical coordinates) for TEMP
  \param [in,out] t Pointer to temp_chunks structure to fill
  \param [in,out] s Pointer to subset state
  \return 0 on success, 1 on error
*/
int temp_parse_x06 ( struct temp_chunks *t, struct bufr2tac_subset_state *s );

/*!
  \fn int temp_parse_x07(struct temp_chunks *t, struct bufr2tac_subset_state *s)
  \brief Parse BUFR class 07 descriptors (height, altitude, pressure) for TEMP
  \param [in,out] t Pointer to temp_chunks structure to fill
  \param [in,out] s Pointer to subset state
  \return 0 on success, 1 on error
*/
int temp_parse_x07 ( struct temp_chunks *t, struct bufr2tac_subset_state *s );

/*!
  \fn int temp_parse_x08(const struct temp_chunks *t, struct bufr2tac_subset_state *s)
  \brief Parse BUFR class 08 descriptors (significance qualifiers) for TEMP
  \param [in] t Pointer to temp_chunks structure
  \param [in,out] s Pointer to subset state
  \return 0 on success, 1 on error
*/
int temp_parse_x08 ( const struct temp_chunks *t, struct bufr2tac_subset_state *s );

/*!
  \fn int temp_parse_x10(const struct temp_chunks *t, struct bufr2tac_subset_state *s)
  \brief Parse BUFR class 10 descriptors (pressure) for TEMP
  \param [in] t Pointer to temp_chunks structure
  \param [in,out] s Pointer to subset state
  \return 0 on success, 1 on error
*/
int temp_parse_x10 ( const struct temp_chunks *t, struct bufr2tac_subset_state *s );

/*!
  \fn int temp_parse_x11(const struct temp_chunks *t, struct bufr2tac_subset_state *s)
  \brief Parse BUFR class 11 descriptors (wind data) for TEMP
  \param [in] t Pointer to temp_chunks structure
  \param [in,out] s Pointer to subset state
  \return 0 on success, 1 on error
*/
int temp_parse_x11 ( const struct temp_chunks *t, struct bufr2tac_subset_state *s );

/*!
  \fn int temp_parse_x12(const struct temp_chunks *t, struct bufr2tac_subset_state *s)
  \brief Parse BUFR class 12 descriptors (temperature and dew point) for TEMP
  \param [in] t Pointer to temp_chunks structure
  \param [in,out] s Pointer to subset state
  \return 0 on success, 1 on error
*/
int temp_parse_x12 ( const struct temp_chunks *t, struct bufr2tac_subset_state *s );

/*!
  \fn int temp_parse_x20(struct temp_chunks *t, struct bufr2tac_subset_state *s)
  \brief Parse BUFR class 20 descriptors (visibility, clouds, and weather phenomena) for TEMP
  \param [in,out] t Pointer to temp_chunks structure to fill
  \param [in,out] s Pointer to subset state
  \return 0 on success, 1 on error
*/
int temp_parse_x20 ( struct temp_chunks *t, struct bufr2tac_subset_state *s );

/*!
  \fn int temp_parse_x22(struct temp_chunks *t, struct bufr2tac_subset_state *s)
  \brief Parse BUFR class 22 descriptors (oceanographic and hydrographic data) for TEMP
  \param [in,out] t Pointer to temp_chunks structure to fill
  \param [in,out] s Pointer to subset state
  \return 0 on success, 1 on error
*/
int temp_parse_x22 ( struct temp_chunks *t, struct bufr2tac_subset_state *s );

/*!
  \fn int temp_parse_x31(const struct temp_chunks *t, struct bufr2tac_subset_state *s)
  \brief Parse BUFR class 31 descriptors (data description and delayed replication factors) for TEMP
  \param [in] t Pointer to temp_chunks structure
  \param [in,out] s Pointer to subset state
  \return 0 on success, 1 on error
*/
int temp_parse_x31 ( const struct temp_chunks *t, struct bufr2tac_subset_state *s );

/*!
  \fn int temp_parse_x33(struct temp_chunks *t, struct bufr2tac_subset_state *s)
  \brief Parse BUFR class 33 descriptors (data quality information) for TEMP
  \param [in,out] t Pointer to temp_chunks structure to fill
  \param [in,out] s Pointer to subset state
  \return 0 on success, 1 on error
*/
int temp_parse_x33 ( struct temp_chunks *t, struct bufr2tac_subset_state *s );

// ECMWF BUFR library function prototypes
// These are wrapper functions for the ECMWF BUFR library (written in FORTRAN)
// The trailing underscore follows FORTRAN naming conventions for C interoperability

/*!
  \fn int bus012_(int *, unsigned int *, int *, int *, int *, int *, int *)
  \brief ECMWF BUFR library function - Set up BUFR tables
  \return Status code
*/
int bus012_ ( int *, unsigned int *, int *, int *, int *, int *, int * );

/*!
  \fn int buprs0_(int *)
  \brief ECMWF BUFR library function - Print section 0 of BUFR message
  \return Status code
*/
int buprs0_ ( int * );

/*!
  \fn int buprs1_(int *)
  \brief ECMWF BUFR library function - Print section 1 of BUFR message
  \return Status code
*/
int buprs1_ ( int * );

/*!
  \fn int buprs3_(int *, int *, int *, int *, int *, int *, char **)
  \brief ECMWF BUFR library function - Print section 3 of BUFR message
  \return Status code
*/
int buprs3_ ( int *, int *, int *, int *, int *, int *, char ** );

/*!
  \fn int bufrex_(int *, int *, int *, int *, int *, int *, int *, int *, int *, char **, char **, int *, double *, char **, int *)
  \brief ECMWF BUFR library function - Expand BUFR message
  \return Status code
*/
int bufrex_ ( int *, int *, int *, int *, int *, int *, int *, int *, int *, char **, char **, int *, double *, char **, int * );

/*!
  \fn int busel_(int *, int *, int *, int *, int *)
  \brief ECMWF BUFR library function - Select BUFR subset
  \return Status code
*/
int busel_ ( int *, int *, int *, int *, int * );

/*!
  \fn int busel2_(int *, int *, int *, char **, int *, char **, char **, char **, int *)
  \brief ECMWF BUFR library function - Enhanced BUFR subset selection
  \return Status code
*/
int busel2_ ( int *, int *, int *,  char **, int *, char **, char **, char **, int * );

/*!
  \fn int buukey_(int *, int *, int *, int *, int *)
  \brief ECMWF BUFR library function - Get key information
  \return Status code
*/
int buukey_ ( int *, int *, int *, int *, int * );

/*!
  \fn int buprt_(int *, int *, int *, int *, char **, char **, char **, int *, double *, int *, int *, int *)
  \brief ECMWF BUFR library function - Print expanded BUFR data
  \return Status code
*/
int buprt_ ( int *, int *, int *, int *, char **, char **, char **, int *, double *, int *, int *, int * );

// Global variables
/*! Global debug level variable - Controls verbosity of debug output (0=none, higher=more verbose) */
extern int BUFR2TAC_DEBUG_LEVEL;

#ifndef PATH_MAX
#define PATH_MAX 1024
#endif

/*! Path to BUFR tables directory - Can be set via environment variable */
extern char BUFR_TABLES_DIRENV[PATH_MAX];


#ifdef __cplusplus
} /* closing brace for extern "C" */
#endif

#endif  // from ifndef BUFR2TAC_H
