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
 \file bufrdeco.h
 \brief Include header file for bufrdeco library
*/
#ifndef BUFRDECO_H
#define BUFRDECO_H

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <getopt.h>
#include <time.h>
#include <math.h>
#include <sys/stat.h>

//#define DEBUG

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
  \def BUFR_LEN_SEC1
  \brief Max length in bytes for a sec1
*/
#define BUFR_LEN_SEC1 (8192)

/*!
  \def BUFR_LEN_SEC2
  \brief Max length in bytes for a sec2
*/
#define BUFR_LEN_SEC2 (8192)


/*!
  \def BUFR_LEN_UNEXPANDED_DESCRIPTOR
  \brief Max amount of unexpanded descriptor in a SEC3
*/
#define BUFR_LEN_UNEXPANDED_DESCRIPTOR (512)

/*!
  \def BUFR_LEN_SEC3
  \brief Max length in bytes for a sec3
*/
#define BUFR_LEN_SEC3 (8 + 2 * BUFR_LEN_UNEXPANDED_DESCRIPTOR)

#define BUFR_MAXLINES_TABLEB (2048)
#define BUFR_MAXLINES_TABLEC (8192)
#define BUFR_MAXLINES_TABLED (8192)

/*!
  \struct bufr_descriptor
  \brief BUFR descriptor
*/
struct bufr_descriptor
{
  uint8_t f; /*!< F part of descriptor, 2 bits */
  uint8_t x; /*!< X part of descriptor, 6 bits */
  uint8_t y; /*!< Y part of descriptor, 8 bits */
  char c[8]; /*!< string with whole descriptor (6 decimal digits plus the 0x00 as the end of string */
};

/*!
  \struct bufr_atom_data
  \brief Contains all the information for a single descriptor in a expanded squence
*/
struct bufr_atom_data
{
  struct bufr_descriptor desc; /*!< struct \ref bufr_descriptor */
  uint32_t mask; /*!< Mask with for the type */
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

struct bufr_sec0
{
  uint32_t bufr_length; /*!< Original Bufr file size in bytes */
  uint8_t edition; /*!< bufr edition nuber */
  uint8_t raw[8]; /*!< bytes as in the bufr message */
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


/*
  Section 1 – Identification section
  Octet No.  Contents
  1–3        Length of section
  4          BUFR master table (zero if standard WMO FM 94 BUFR tables are used – see Note 2)
  5–6        Identification of originating/generating centre (see Common Code table C-11)
  7–8        Identification of originating/generating sub-centre (allocated by originating/generating centre
             – see Common Code table C-12)
  9          Update sequence number (zero for original messages and for messages containing only
             delayed reports; incremented for the other updates)
  10         Bit 1 = 0 No optional section 
                   = 1 Optional section follows
             Bits 2-8 Set to zero (reserved)
  11         Data category (Table A)
  12         International data sub-category (see Common Code table C-13 and Note 3)
  13         Local data sub-category (defined locally by automatic data-processing (ADP) centres 
  14         Version number of master table 
  15         Version number of local tables used to augment master table in use 
  16–17      Year (4 digits)
  18         Month
  19         Day Most typical time for the BUFR message contents – see Note 4
  20         Hour
  21         Minute
  22         Second
  23–Optional – for local use by ADP centres

*/ 
struct bufr_sec1
{
  uint32_t length; /*!< Length of section 1 */
  uint8_t master; /*!< Bufr master table */
  uint32_t centre; /*!< Centre */
  uint32_t subcentre; /*!< sub-Centre */
  uint8_t update; /*!< update sequence number */
  uint8_t options; /*!< options */
  uint8_t category; /*!< Data category (table A) */
  uint8_t subcategory; /*!< subcategory */
  uint8_t subcategory_local;/*!< local data subcategory */
  uint8_t master_version; /*!< Version of master table */
  uint8_t master_local; /*!< Version number of local tables used to augment master table in use */
  uint32_t year; /*!< Year */
  uint8_t month; /*!< Month */
  uint8_t day; /*!< Day */
  uint8_t hour; /*!< hour */
  uint8_t minute; /*!< minute */
  uint8_t second; /*!< second */
  uint8_t raw[BUFR_LEN_SEC1]; /*!< Raw data for sec1 as is in original BUFR file */
};

struct bufr_sec2
{
  uint32_t length; 
  uint8_t raw[BUFR_LEN_SEC2]; /*!< Raw data for sec2 as is in original BUFR file */
};

struct bufr_sec3
{
  uint32_t length; /*!< length of sec3 in bytes */
  uint32_t subsets; /*!< Number of data subsets */
  uint8_t observed; /*!< 1 if observed */
  uint8_t compressed; /*!< 1 if compressed */
  uint32_t ndesc; /*!< Current number of unexpanded descriptors */
  struct bufr_descriptor unexpanded[BUFR_LEN_UNEXPANDED_DESCRIPTOR]; /*!< Array of unexpanded descriptors */
  uint8_t raw[BUFR_LEN_SEC3]; /*!< Raw data for sec3 as is in original BUFR file */
};

struct bufr_sec4
{
  uint32_t length; /*< length of sec4 in bytes */
  size_t allocated; /*!< Allocated size of raw */
  uint8_t *raw; /*!< Pointer to a raw data for sec4 as in original BUFR file */
};

struct bufr_tableb
{
  char path[256];
  size_t nlines;
  char l[BUFR_MAXLINES_TABLEB][180];
};

struct bufr_tablec
{
  char path[256];
  size_t nlines;
  char l[BUFR_MAXLINES_TABLEC][96];
};

struct bufr_tabled
{
  char path[256];
  size_t nlines;
  char l[BUFR_MAXLINES_TABLED][128];
};

struct bufr_tables
{
  struct bufr_tableb b;
  struct bufr_tablec c;
  struct bufr_tabled d;
};

struct bufr
{
  struct gts_header header;
  struct bufr_sec0 sec0;
  struct bufr_sec1 sec1;
  struct bufr_sec2 sec2;
  struct bufr_sec3 sec3;
  struct bufr_sec4 sec4;
  struct bufr_tables *table;
};

extern const char DEFAULT_BUFRTABLES_DIR1[];
extern const char DEFAULT_BUFRTABLES_DIR2[];


int init_bufr(struct bufr *b, size_t l);
int clean_bufr(struct bufr *b);
int bufrdeco_read_bufr(struct bufr *b,  char *filename, char *error);
void print_sec0_info(struct bufr *b);
void print_sec1_info(struct bufr *b);
void print_sec3_info(struct bufr *b);
void print_sec4_info(struct bufr *b);
int bufr_read_tableb(struct bufr_tableb *tb, char *error);
int bufr_read_tablec(struct bufr_tablec *tc, char *error);
int bufr_read_tabled(struct bufr_tabled *td, char *error);

// Utililies functions
uint32_t two_bytes_to_uint32 ( const uint8_t *source );
uint32_t three_bytes_to_uint32 ( const uint8_t *source );
size_t get_bits_as_uint32_t(uint32_t *target, uint8_t *source, size_t *bit0_offset, size_t bit_length);
int two_bytes_to_descriptor (struct bufr_descriptor *d, const uint8_t *source);
int uint32_t_to_descriptor ( struct bufr_descriptor *d, uint32_t id );
int get_ecmwf_tablenames ( struct bufr *b, const char *bufrtables_dir );
#endif  // from ifndef BUFRDECO_H
