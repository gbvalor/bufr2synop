/***************************************************************************
 *   Copyright (C) 2013-2018 by Guillermo Ballester Valor                  *
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
  \def BUFR_LEN
  \brief Max length of a BUFR file
*/
#define BUFR_LEN 512000

/*!
   \def BUFR_OBS_DATA_MASK
   \brief Bit mask for Observed data
*/
#define BUFR_OBS_DATA_MASK 128

/*!
   \def BUFR_COMPRESSED_DATA_MASK
   \brief Bit mask for compressed data
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
  \def BUFR_MAXLINES_TABLEB
  \brief The maximum expected lines in a Table B file
*/
#define BUFR_MAXLINES_TABLEB (2048)

/*!
  \def BUFR_MAXLINES_TABLEC
  \brief The maximum expected lines in a Table C file
*/
#define BUFR_MAXLINES_TABLEC (8192)


/*!
  \def BUFR_MAXLINES_TABLED
  \brief The maximum expected lines in a Table D file
*/
#define BUFR_MAXLINES_TABLED (8192)


/*!
  \def MAXLINES_TABLEC
  \brief The maximum expected lines in a Table C file
*/
#define MAXLINES_TABLEC BUFR_MAXLINES_TABLEC

/*!
  \def BUFR_NMAXSEQ
  \brief Maximum expected descriptors in a expanded sequence for a single subset
*/
#define BUFR_NMAXSEQ (2 * 16384)
// #define NMAXSEQ (16384)

/*!
  \def BUFR_EXPLAINED_LENGTH
  \brief Maximum length for a explained descriptor
*/
#define BUFR_EXPLAINED_LENGTH (256)

/*!
  \def CSV_MAXL
  \brief Maximum length in a string to be parsed as csv
*/
#define CSV_MAXL (2048)

/*!
   \def NMAXSEQ_DESCRIPTORS
   \brief Maximum nuber of unexpanded descriptors in a struct \ref bufr_sequence
*/
#define NMAXSEQ_DESCRIPTORS (256)

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
 \brief Bit mask for a replicator descriptor in a struct \ref bufr_atom_data
*/
#define DESCRIPTOR_IS_A_REPLICATOR (128)

/*!
 \def DESCRIPTOR_IS_AN_OPERATOR
 \brief Bit mask for an operator data descriptor in a struct \ref bufr_atom_data
*/
#define DESCRIPTOR_IS_AN_OPERATOR (256)

/*!
 \def DESCRIPTOR_IS_LOCAL
 \brief Bit mask for a flag table string in a struct \ref bufr_atom_data
*/
#define DESCRIPTOR_IS_LOCAL (512)

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
  \brief Max amount of unexpanded descriptors in a SEC3
*/
#define BUFR_LEN_UNEXPANDED_DESCRIPTOR (512)


/*!
  \def BUFR_MAX_EXPANDED_SEQUENCES
  \brief Max amount of unexpanded layers in a struct \ref bufrdeco_expanded_tree
*/
#define BUFR_MAX_EXPANDED_SEQUENCES (128)

/*!
  \def BUFR_MAX_QUALITY_DATA
  \brief Max amount of quality data which is maped by a struct \ref bufrdeco_bitmap_element
*/
#define BUFR_MAX_QUALITY_DATA (8)

/*!
  \def BUFR_MAX_BITMAP_PRESENT_DATA
  \brief Max number of data present in a bitmap definition 
*/
#define BUFR_MAX_BITMAP_PRESENT_DATA (4096)

/*!
  \def BUFR_MAX_BITMAPS
  \brief Max number of structs \ref bufrdeco_bitmap that can be allocated  in a strcut \ref bufrdeco_bitmap_array
*/
#define BUFR_MAX_BITMAPS (8)

/*!
  \def BUFR_LEN_SEC3
  \brief Max length in bytes for a sec3
*/
#define BUFR_LEN_SEC3 (8 + 2 * BUFR_LEN_UNEXPANDED_DESCRIPTOR)

/*!
  \def BUFRDECO_USE_ECMWF_TABLES
  \brief bit mask to force the use of ECMWF
*/
#define BUFRDECO_USE_ECMWF_TABLES (1)

/*!
  \def BUFRDECO_OUTPUT_HTML
  \brief bit mask to format output as html
*/
#define BUFRDECO_OUTPUT_HTML (2)

/*!
  \def BUFRDECO_OUTPUT_JSON
  \brief bit mask to format output as json
*/
#define BUFRDECO_OUTPUT_JSON (4)

/*!
  \def BUFRDECO_OUTPUT_XML
  \brief bit mask to format output as xml
*/
#define BUFRDECO_OUTPUT_XML (8)

/*!
  \def BUFR_TABLEB_NAME_LENGTH
  \brief Max length (in chars) reserved for a name of variable in table B
*/
#define BUFR_TABLEB_NAME_LENGTH (128)

/*!
  \def BUFR_TABLEB_NAME_LENGTH
  \brief Max length (in chars) reserved for the unit string in table B
*/
#define BUFR_TABLEB_UNIT_LENGTH (32)



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
  \struct bufr_replicator
  \brief store the information when parsing related to replicators
*/
struct bufr_replicator
{
  struct bufr_sequence *s; /*!< struct to which this replicator struct belongs */
  size_t ixrep; /*!< index in a sequence of replicator descriptor */
  size_t ixdel; /*!< index in a sequence of a delayed replicator, if not then is the same that ixrep */
  size_t ndesc; /*!< amount of descritptors in sequence affected by replicator */
  size_t nloops; /*!< amount of loops to do */
};

/*!
  \struct bufr_atom_data
  \brief Contains all the information for a single data related with a descriptor in a expanded squence
*/
struct bufr_atom_data
{
  struct bufr_descriptor desc; /*!< struct \ref bufr_descriptor */
  uint32_t mask; /*!< Mask with for the type */
  char name[BUFR_TABLEB_NAME_LENGTH]; /*!< String with the name of descriptor */
  char unit[BUFR_TABLEB_UNIT_LENGTH]; /*!< String with the name of units */
  double val; /*!< Final value for the bufr descriptor data */
  int32_t escale; /*!< Scale applied to get the data */ 
  uint32_t associated; /*!< value for associated field, if any */
  char cval[128]; /*!< String value for the bufr descriptor */
  char ctable[BUFR_EXPLAINED_LENGTH]; /*!< Explained meaning for a code table */
  struct bufr_sequence *seq; /*!< Pointer to the struct bufr_sequence to which this descriptor belongs to */
  size_t ns; /*!< Element in bufr_sequence to which this descriptor belongs to */
  uint32_t is_bitmaped_by; /*!< Index of element in a struct \ref bufrdeco_subset_sequence_data which bitmap this one */ 
  uint32_t bitmap_to; /*!< Index of element in a struct \ref bufrdeco_subset_sequence_data which this one is mapping to */
};

/*!
  \struct bufrdeco_subset_sequence_data
  \brief Contains all the information for a subset in a expanded squence
  This is a verion to use with bufrdeco parse library
*/
struct bufrdeco_subset_sequence_data
{
  size_t dim; /*!< Amount of bufr_atom_data currently allocated */
  size_t nd; /*!< number of current amount of data used in sequence */
  struct bufr_atom_data *sequence; /*!< the array of data associated to a expanded sequence */
};

/*!
   \struct bufrdeco_bitmap_element
   \brief Stores data for a bitmap  
*/
struct bufrdeco_bitmap_element 
{
  uint32_t bitmap_to; /*!< Pointer to struct bufr_atom_data which this one is mapping to */
  uint32_t quality_given_by[BUFR_MAX_QUALITY_DATA]; /*!< array of pointers to struct bufr_atom_data which gives quality data to the one is mapping to */
};

/*!
  \struct bufrdeco_bitmap
  \brief Stores all structs \ref bufrdeco_bitmap_element for a bufr bitmap
*/
struct bufrdeco_bitmap
{
    size_t nb; /*!< Amount of elements used (data present) in the bitmap */
    struct bufrdeco_bitmap_element element[BUFR_MAX_BITMAP_PRESENT_DATA];
};

/*!
  \struct bufrdeco_bitmap_array
  \brief Stores all structs \ref bufrdeco_bitmap for a bufr bitmap
*/
struct bufrdeco_bitmap_array
{
    size_t nba; /*!< Amount of bitmaps used */
    struct bufrdeco_bitmap *bmap[BUFR_MAX_BITMAPS]; /*!< array of pointers to struct bufrdeco */
};

/*!
   \struct bufrdeco_bitmap_compressed_element
   \brief Stores data for a bitmap_compressed  
*/
struct bufrdeco_bitmap_compressed_element 
{
  uint32_t bitmap_to; /*!< Index in a struct \ref bufr_compressed_data_references which  is mapping to */
  uint32_t quality_given_by[BUFR_MAX_QUALITY_DATA]; /*!< array of indexes in a struct \ref bufr_compressed_data_references which gives quality data to the one is mapping to */
};

/*!
  \struct bufrdeco_bitmap_compressed
  \brief Stores all structs \ref bufrdeco_bitmap_compressed_element for a bufr bitmap compressed
*/
struct bufrdeco_bitmap_compressed
{
    struct bufrdeco_compressed_data_references *rf;
    size_t nb; /*!< Amount of elements used (data present) in the bitmap_compressed */
    struct bufrdeco_bitmap_compressed_element element[BUFR_MAX_BITMAP_PRESENT_DATA];
};

/*!
  \struct bufrdeco_bitmap_compressed_array
  \brief Stores all structs \ref bufrdeco_bitmap_compressed for a bufr bitmap_compressed
*/
struct bufrdeco_bitmap_compressed_array
{
    size_t nba; /*!< Amount of bitmaps used */
    struct bufrdeco_bitmap_compressed *bmap[BUFR_MAX_BITMAPS]; /*!< array of pointers to struct bufrdeco */
};

/*!
  \struct bufrdeco_decoding_data_state
  \brief stores the state when expanding a sequence.

*/
struct bufrdeco_decoding_data_state
{
  size_t subset; /*!< Subset sequence index being parsed */
  size_t bit_offset; /*!< first data bit offset of current since the begining of data in byte 4 in SEC 4 */
  int8_t added_bit_length; /*!< Current aditional bit_length that can be changed by descriptor 2 01 YYY */
  int8_t added_scale; /*!< Current aditional scale factor that can be changed by descriptor 2 02 YYY */
  int32_t added_reference; /*!< Current aditional reference that can be changed bu descriptor 2 03 YYY */
  int32_t factor_reference; /*!< Factor to multiply current reference if > 1 */
  uint8_t assoc_bits; /*!< number of associated bits */
  uint8_t changing_reference; /*!< Changing reference as descriptor 2 03 YYY */
  uint8_t fixed_ccitt; /*!< Length in octests for a CCITT var. Changed with descriptor 2 08 YYY . default 0 (or 1)*/
  uint8_t local_bit_reserved; /*!< bits reserved for the inmediately local descriptor */
  uint8_t quality_active; /*!< If != 0 then all 33 class descriptros are refered to a defined bitmap */
  int32_t bitmaping; /*!< If != 0 then is the backard count reference defined by replicator descriptor after 2 36 000 operator */
  struct bufrdeco_bitmap *bitmap; /*!< Pointer to an active bitmap. If not bitmap defined then is NULL */ 
  struct bufrdeco_bitmap_compressed *bitmap_compressed; /*!< Pointer to an active bitmap. If not bitmap defined then is NULL */ 
};

/*!
  \struct bufr_sequence
  \brief Stores an unexpanded sequence of descriptors

  A sequence layer is needed when parsing expanded descriptor sec3 and sec4

  First bufr_sequence is the sequence of descriptors in sec3 after
  byte 8. This is a bufr_sequence in level 0.

  When a sequence descriptor is found in a layer, the sequence entries found in table D
  form this descriptor is a son bufr_sequence. This son has then a father
  and also can have one or more sons. The index level is incremented by one every step it
  go into decendents.

*/
struct bufr_sequence
{
  char key[8]; /*!< the key */
  size_t level; /*!< The recursion level. descriptors in sec3 are level 0 */
  struct bufr_sequence *father; /*!< Pointer to the father struct. It should be NULL if level = 0 */
  size_t ndesc; /*!< Number of unexpanded descriptor of a layer */
  struct bufr_descriptor lseq[NMAXSEQ_DESCRIPTORS]; /*!< Array of unexpanded descriptors */
  struct bufr_sequence *sons[NMAXSEQ_DESCRIPTORS]; /*!< Array of pointers to sons. It must be NULL
   except for sequence descriptors.  */
  int iseq; /*!< number of sequence when parsing a tree. for recursion level 0 in sec3 is asigned 0. 
                Is the index in member seq in struct bufereco_expanded_tree */
  char name[BUFR_EXPLAINED_LENGTH]; /*!< Name of sequence if any */
};

/*!
 \struct bufrdeco_expanded_tree
 \brief Array of structs \ref bufr_sequence
*/
struct bufrdeco_expanded_tree
{
  size_t nseq; /*!< current number of structs */
  struct bufr_sequence seq[BUFR_MAX_EXPANDED_SEQUENCES]; /*!< array of structs */
};

/*!
 \struct bufrdeco_compressed_ref
 \brief Struct to hold the needed reference bit offsets in a compressed BUFR
*/
struct bufrdeco_compressed_ref
{
  uint8_t is_associated ; /*!< 0 if is not associated data */
  uint8_t has_data ; /*!< 1 if has any subset with valid data. 0 if missing in all subsets */
  uint8_t bits; /*!< bits for data or associated in table b */
  int32_t ref; /*!< reference for a expanded data in table b */
  size_t bit0; /*!< first bit offset, i.e, most significant bit for ref0 */
  uint32_t ref0; /*!< local reference for a expanded data in subsets */
  char cref0[256]; /*!< Local reference in case of string */
  uint8_t inc_bits; /*!< number of inc bits for every subset  */
  int32_t escale; /*!< escale for a expanded data in subset */
  char name[BUFR_TABLEB_NAME_LENGTH]; /*!< String with the name of descriptor */
  char unit[BUFR_TABLEB_UNIT_LENGTH]; /*!< String with the name of units */
  struct bufr_descriptor desc; /*!< associated descriptor */
  uint32_t is_bitmaped_by; /*!< Index element in a struct \ref bufr_compressed_data_references which bitmap this one */ 
  uint32_t bitmap_to; /*!< Index element in a struct \ref bufr_compressed_data_references which this one is mapping to */
};

/*!
  \struct bufrdeco_compressed_data_references
  \brief Manage an array of structs \ref bufrdeco_compressed_ref
*/
struct bufrdeco_compressed_data_references
{
  size_t dim; /*!< dimension of array of compressed refs */
  size_t nd; /*!< current amount of data used */
  struct bufrdeco_compressed_ref *refs; /*!< pointer to allocated array */
};

/*!
  \struct bufr_sec0
  \brief Store de parsed section 0
*/
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


/*!
  \struct bufr_sec1
  \brief Store de parsed section 1 of a bufr file

  <pre>
  This is for version 4 BUFR
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

  And this is for version 3
  1–3        Length of section
  4          BUFR master table (zero if standard WMO FM 94 BUFR tables are used – see Note 2)
  5          Originating/generating sub-centre: Code table 0 01 034 (defined, if necessary, by associated
             originating/generating centre – see Note 3 of Common Code table C–1 in Part C/c.)
  6          Originating/generating centre: Code table 0 01 033 (Common Code table C–1 in Part C/c.)
  7          Update sequence number (zero for original BUFR messages; incremented for updates)
  8          Bit 1 = 0 No optional section
                   = 1 Optional section follows
             Bits 2–8 Set to zero (reserved)
  9          Data category (Table A)
  10         Data sub-category (defined by local automatic data processing (ADP) centres)
  11         Version number of master table used – see Notes 2 and 4
  12         Version number of local tables used to augment the master table in use – see Note 2
  13         Year of century
  14         Month
  15         Day Most typical for the BUFR message contents
  16         Hour
  17         Minute
  18-        If not required by ADP centres for local use, octet 18 only shall be included and set to zero with
             reference to Regulation 94.1.3
  </pre>
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

/*!
  \struct bufr_sec2
  \brief Store a parsed sec2 from a bufr file
*/
struct bufr_sec2
{
  uint32_t length;
  uint8_t raw[BUFR_LEN_SEC2]; /*!< Raw data for sec2 as is in original BUFR file */
};

/*!
  \struct bufr_sec3
  \brief Store a parsed sec3 from a bufr file
*/
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

/*!
  \struct bufr_sec4
  \brief Store a parsed sec4 from a bufr file

  Note that member \a raw  must be initialized to allocate needed memory
*/
struct bufr_sec4
{
  uint32_t length; /*!< length of sec4 in bytes */
  size_t bit_offset; /*!< Offset to current first bit in raw data sec4 to parse */
  uint8_t raw[BUFR_LEN]; /*!< Pointer to a raw data for sec4 as in original BUFR file */
};

/*!
   \struct bufr_tableb_decoded_item
   \brief Store parameters for a descriptor in table b, i. e. with f = 0
*/
struct bufr_tableb_decoded_item
{
  uint8_t changed; /*!< flag. If 0 = not changed from table B. If 1 Changed */
  uint8_t x; /*!< x value of descriptor */
  uint8_t y; /*!< y value of descriptor */
  char key[8]; /*!< c value of descriptor */
  char name[BUFR_TABLEB_NAME_LENGTH]; /*!< name */
  char unit[BUFR_TABLEB_UNIT_LENGTH]; /*!< unit */
  int32_t scale; /*!< escale */
  int32_t scale_ori; /*!< escale as readed from table b */
  int32_t reference; /*!< reference */
  int32_t reference_ori; /*!< reference as readed from table b */
  size_t nbits; /*!< bits */
  size_t nbits_ori; /*!< bits as readed from table b */
  size_t tablec_ref; /*!< item to point table c, if any */
  size_t tabled_ref; /*!< item to point table d, if any */
};

/*!
  \struct bufr_tableb
  \brief Store a table B readed from a file formated and named as ECMWF bufrdc package
*/
struct bufr_tableb
{
  int wmo_table; /*!< 1 when table is from WMO csv file, 0 otherwise */
  char path[256]; /*!< Complete path of current file readed */
  char old_path[256]; /*!< Cmplete path of prior file readed, used to avoid read two consecutive times the same file */
  size_t nlines; /*!< Current lines readed from file, i. e. used in array item[] */
  size_t x_start[64]; /*!< Index in array \a item[] for first x. x_start[j] is index for first descriptor which x == j */
  size_t num[64]; /*!< Amount of items for x. num[i] is the amount of items in array where x = i */
  uint8_t y_ref[64][256]; /*!< index for y since first x. x_ref[i][j] is index since x_start[i] where y == j */
  struct bufr_tableb_decoded_item item[BUFR_MAXLINES_TABLEB]; /*!< Array with structs containing parsed lines readed from file */
};


/*!
   \struct bufr_tablec_decoded_item
   \brief Store parameters for a descriptor in table C, i. e. for Table code and flags
   Remember this is NOT the C WMO tables
*/
struct bufr_tablec_decoded_item
{
  char key[8]; /*!< c value of descriptor */
  uint8_t x; /*!< x value of descriptor */
  uint8_t y; /*!< y value of descriptor */
  uint32_t ival; /*!< code value */
  char description[BUFR_EXPLAINED_LENGTH]; /*!< Description string of code */
};


/*!
  \struct bufr_tablec
  \brief Store a table C readed from a file formated and named as ECMWF bufrdc package
*/
struct bufr_tablec
{
  int wmo_table; /*!< 1 when table is from WMO csv file, 0 otherwise */
  char path[256]; /*!< Complete path of current file readed */
  char old_path[256]; /*!< Cmplete path of prior file readed, used to avoid read two consecutive times the same file */
  size_t nlines; /*!< Current lines readed from file, i. e. used in array \a l[] */
  size_t x_start[64]; /*!< Index in array \a l[] for first x. x_start[j] is index for first descriptor which x == j */
  size_t num[64]; /*!< Amonut of lines for x. num[i] is the amount of items in array where x = i */
  size_t y_ref[64][256]; /*!< index for first y since first x. x_ref[i][j] is index since x_start[i] where y == j */
  char l[BUFR_MAXLINES_TABLEC][96]; /*!< Array with lines readed from file */
  struct bufr_tablec_decoded_item item[BUFR_MAXLINES_TABLEC]; /*!< Array of decoded lines */
};


/*!
  \struct bufr_tabled_decoded_item
  \brief Store parameters for a line in table D (sequence of descritors)
*/
struct bufr_tabled_decoded_item 
{
  char key[8]; /*!< c key value of sequence descriptor */
  char key2[8]; /*!< c key 2 of sequence element */
  char description[BUFR_EXPLAINED_LENGTH]; /*!< Explained name of sequence */
  char description2[BUFR_EXPLAINED_LENGTH]; /*!< Explained name of elemet of sequence */
};

/*!
  \struct bufr_tabled
  \brief Store a table D readed from a file formated and named as ECMWF bufrdc package
*/
struct bufr_tabled
{
  int wmo_table; /*!< 1 when table is from WMO csv file, 0 otherwise */
  char path[256]; /*!< Complete path of current file readed */
  char old_path[256]; /*!< Cmplete path of prior file readed, used to avoid read two consecutive times the same file */
  size_t nlines; /*!< Current lines readed from file, i. e. used in array \a l[] */
  size_t x_start[64]; /*!< Index in array \a l[] for first x. x_start[j] is index for first descriptor which x == j */
  size_t num[64]; /*!< Amonut of lines for x. num[i] is the amount of items in array where x = i */
  char l[BUFR_MAXLINES_TABLED][128]; /*!< Array with lines readed from file */
  struct bufr_tabled_decoded_item item[BUFR_MAXLINES_TABLED];
};

/*!
  \struct bufr_tables
  \brief Contains all tables needed to parse a bufr file

  All readed files need to be named an formed as ECMWF bufrdc package
*/
struct bufr_tables
{
  struct bufr_tableb b; /*!< Table B */
  struct bufr_tablec c; /*!< Table C */
  struct bufr_tabled d; /*!< Table D */
};

/*!
  \struct bufrdeco
  \brief This struct contains all needed data to parse and decode a BUFR file

  NOTE that must be initializad before use
*/
struct bufrdeco
{
  uint32_t mask; /*!< Array of bit mask */
  struct gts_header header; /*!< GTS data */
  struct bufr_sec0 sec0; /*!< Parsed sec0 */
  struct bufr_sec1 sec1; /*!< Parsed sec1 */
  struct bufr_sec2 sec2; /*!< Parsed sec2 */
  struct bufr_sec3 sec3; /*!< Parsed sec3 */
  struct bufr_sec4 sec4; /*!< Parsed sec4 */
  struct bufr_tables *tables; /*!< Pointer to a the struct containing all tables needed for a single bufr */
  struct bufrdeco_expanded_tree *tree; /*!< Pointer to a struct containing the parsed descriptor tree (without explansion) */
  struct bufrdeco_decoding_data_state state; /*!< Struct with data needed when parsing bufr */
  struct bufrdeco_compressed_data_references refs; /*!< struct with data references in case of compressed bufr */
  struct bufrdeco_subset_sequence_data seq; /*!< sequence with data subset after parse */
  struct bufrdeco_bitmap_array bitmap; /*!< Stores data for bit-maps */
  struct bufrdeco_bitmap_compressed_array bitmapc; /*!< Stores data for bit-maps in case od compressed bufr*/
  char bufrtables_dir[256]; /*!< string with the path of bufr table directories */
  char error[1024]; /*!< String with detected errors, if any */
};

extern const char DEFAULT_BUFRTABLES_ECMWF_DIR1[];
extern const char DEFAULT_BUFRTABLES_ECMWF_DIR2[];
extern const char DEFAULT_BUFRTABLES_WMO_CSV_DIR1[];
extern const char DEFAULT_BUFRTABLES_WMO_CSV_DIR2[];
extern const double pow10pos[8];
extern const double pow10neg[8];
extern const int32_t pow10pos_int[10];


// Memory funcions
int bufrdeco_init ( struct bufrdeco *b );
int bufrdeco_close ( struct bufrdeco *b );
int bufrdeco_reset ( struct bufrdeco *b );
int bufrdeco_init_tables ( struct bufr_tables **t );
int bufrdeco_free_tables ( struct bufr_tables **t );
int bufrdeco_substitute_tables ( struct bufr_tables **replaced, struct bufr_tables *source, struct bufrdeco *b );
int bufrdeco_init_subset_sequence_data ( struct bufrdeco_subset_sequence_data *ba );
int bufrdeco_clean_subset_sequence_data ( struct bufrdeco_subset_sequence_data *ba );
int bufrdeco_free_subset_sequence_data ( struct bufrdeco_subset_sequence_data *ba );
int bufrdeco_free_compressed_data_references ( struct bufrdeco_compressed_data_references *rf );
int bufrdeco_init_compressed_data_references ( struct bufrdeco_compressed_data_references *rf );
int bufrdeco_increase_data_array ( struct bufrdeco_subset_sequence_data *s );

// Read bufr functions
int bufrdeco_read_bufr ( struct bufrdeco *b,  char *filename );
int bufrdeco_read_buffer ( struct bufrdeco *b,  uint8_t *bufrx, size_t size );
int get_ecmwf_tablenames ( struct bufrdeco *b );
int bufr_read_tables_ecmwf ( struct bufrdeco *b );
int bufr_read_tableb ( struct bufr_tableb *tb, char *error );
int bufr_read_tablec ( struct bufr_tablec *tc, char *error );
int bufr_read_tabled ( struct bufr_tabled *td, char *error );

// Read bufr WMO csv
int get_wmo_tablenames ( struct bufrdeco *b );
int bufr_read_tableb_csv ( struct bufr_tableb *tb, char *error );
int bufr_read_tablec_csv ( struct bufr_tablec *tc, char *error );
int bufr_read_tabled_csv ( struct bufr_tabled *td, char *error );
int bufr_read_tables_wmo ( struct bufrdeco *b );
char * csv_quoted_string ( char *out, char *in );
int parse_csv_line ( int *nt, char *tk[], char *lin );


// Print and output functions
void print_bufrdeco_compressed_ref ( struct bufrdeco_compressed_ref *r );
void fprint_bufrdeco_compressed_ref ( FILE *f, struct bufrdeco_compressed_ref *r );
void print_bufrdeco_compressed_data_references ( struct bufrdeco_compressed_data_references *r );
void fprint_bufrdeco_compressed_data_references ( FILE *f, struct bufrdeco_compressed_data_references *r );
void print_sec0_info ( struct bufrdeco *b );
void print_sec1_info ( struct bufrdeco *b );
void print_sec3_info ( struct bufrdeco *b );
void print_sec4_info ( struct bufrdeco *b );
int sprint_sec0_info ( char *target, size_t lmax, struct bufrdeco *b);
int sprint_sec1_info ( char *target, size_t lmax, struct bufrdeco *b);
int sprint_sec3_info ( char *target, size_t lmax, struct bufrdeco *b);
int sprint_sec4_info ( char *target, size_t lmax, struct bufrdeco *b);
int sprint_sec0_info_html ( char *target, size_t lmax, struct bufrdeco *b);
int sprint_sec1_info_html ( char *target, size_t lmax, struct bufrdeco *b);
int sprint_sec3_info_html ( char *target, size_t lmax, struct bufrdeco *b);
int sprint_sec4_info_html ( char *target, size_t lmax, struct bufrdeco *b);
void bufrdeco_print_tree ( struct bufrdeco *b );
void bufrdeco_fprint_tree ( FILE *f, struct bufrdeco *b );
void bufrdeco_print_atom_data_stdout ( struct bufr_atom_data *a );
void bufrdeco_print_atom_data_file ( FILE *f, struct bufr_atom_data *a );
void bufrdeco_print_subset_sequence_data ( struct bufrdeco_subset_sequence_data *s );
void bufrdeco_print_subset_sequence_data_html ( struct bufrdeco_subset_sequence_data *s );
void bufrdeco_print_subset_sequence_data_tagged_html ( struct bufrdeco_subset_sequence_data *s, char *id );
void bufrdeco_fprint_subset_sequence_data_html ( FILE *f, struct bufrdeco_subset_sequence_data *s );
void bufrdeco_fprint_subset_sequence_data_tagged_html ( FILE *f, struct bufrdeco_subset_sequence_data *s, char *id);
void bufrdeco_fprint_subset_sequence_data ( FILE *f, struct bufrdeco_subset_sequence_data *s );
char * bufrdeco_print_atom_data ( char *target, struct bufr_atom_data *a );
char * bufrdeco_print_atom_data_html ( char *target, struct bufr_atom_data *a );
char * get_formatted_value_from_escale ( char *fmt, int32_t escale, double val );


// To parse. General
int bufrdeco_parse_tree ( struct bufrdeco *b );
int bufrdeco_decode_data_subset ( struct bufrdeco_subset_sequence_data *s, struct bufrdeco_compressed_data_references *r, struct bufrdeco *b );
int bufrdeco_decode_subset_data_recursive ( struct bufrdeco_subset_sequence_data *s, struct bufr_sequence *l, struct bufrdeco *b );
int bufrdeco_decode_replicated_subsequence ( struct bufrdeco_subset_sequence_data *s,
    struct bufr_replicator *r, struct bufrdeco *b );
int bufrdeco_parse_f2_descriptor ( struct bufrdeco_subset_sequence_data *s, struct bufr_descriptor *d, struct bufrdeco *b );

// To parse compressed bufr
int bufrdeco_parse_compressed ( struct bufrdeco_compressed_data_references *r, struct bufrdeco *b );
int bufrdeco_parse_compressed_recursive ( struct bufrdeco_compressed_data_references *r, struct bufr_sequence *l, struct bufrdeco *b );
int bufr_decode_subset_data_compressed ( struct bufrdeco_subset_sequence_data *s, struct bufrdeco_compressed_data_references *r,
    struct bufrdeco *b );
int bufrdeco_decode_replicated_subsequence_compressed ( struct bufrdeco_compressed_data_references *r, struct bufr_replicator *rep,
    struct bufrdeco *b );
int bufrdeco_parse_f2_compressed ( struct bufrdeco_compressed_data_references *r, struct bufr_descriptor *d, struct bufrdeco *b );
int bufrdeco_tableb_compressed ( struct bufrdeco_compressed_ref *r, struct bufrdeco *b, struct bufr_descriptor *d, int mode );
int bufrdeco_get_atom_data_from_compressed_data_ref ( struct bufr_atom_data *a, struct bufrdeco_compressed_ref *r,
    size_t subset, struct bufrdeco *b );

// To get parsed data
struct bufrdeco_subset_sequence_data * bufrdeco_get_subset_sequence_data ( struct bufrdeco *b );

// To get bits functions
uint32_t two_bytes_to_uint32 ( const uint8_t *source );
uint32_t three_bytes_to_uint32 ( const uint8_t *source );
size_t get_bits_as_uint32_t ( uint32_t *target, uint8_t *has_data, uint8_t *source, size_t *bit0_offset,
                              size_t bit_length );
size_t get_bits_as_char_array ( char *target, uint8_t *has_data, uint8_t *source, size_t *bit0_offset,
                                size_t bit_length );

// Utilities for tables
char * bufrdeco_explained_table_val ( char *expl, size_t dim, struct bufr_tablec *tc, size_t *index,
                                      struct bufr_descriptor *d, uint32_t ival );
char * bufrdeco_explained_flag_val ( char *expl, size_t dim, struct bufr_tablec *tc, struct bufr_descriptor *d,
                                     uint64_t ival, uint8_t nbits );
char * bufrdeco_explained_table_csv_val ( char *expl, size_t dim, struct bufr_tablec *tc, size_t *index,
    struct bufr_descriptor *d, uint32_t ival );
char * bufrdeco_explained_flag_csv_val ( char *expl, size_t dim, struct bufr_tablec *tc, struct bufr_descriptor *d,
    uint64_t ival, uint8_t nbits );
int bufrdeco_tabled_get_descriptors_array ( struct bufr_sequence *s, struct bufrdeco *b,
    const char *key );
int bufrdeco_tableb_val ( struct bufr_atom_data *a, struct bufrdeco *b, struct bufr_descriptor *d );
int bufr_find_tableb_index ( size_t *index, struct bufr_tableb *tb, const char *key );
int get_table_b_reference_from_uint32_t ( int32_t *target, uint8_t bits, uint32_t source );
int bufrdeco_tabled_get_descriptors_array ( struct bufr_sequence *s, struct bufrdeco *b, const char *key );
int bufr_find_tablec_csv_index ( size_t *index, struct bufr_tablec *tc, const char *key, uint32_t code );

// utilities for bitmaps
int bufrdeco_allocate_bitmap ( struct bufrdeco *b );
int bufrdeco_clean_bitmaps ( struct bufrdeco *b);
int bufrdeco_free_bitmap_array ( struct bufrdeco_bitmap_array *a);
int bufrdeco_add_to_bitmap( struct bufrdeco_bitmap *bm, uint32_t index);
int bufrdeco_allocate_bitmap_compressed ( struct bufrdeco *b );
int bufrdeco_clean_bitmaps_compressed ( struct bufrdeco *b);
int bufrdeco_free_bitmap_compressed_array ( struct bufrdeco_bitmap_compressed_array *a);
int bufrdeco_add_to_bitmap_compressed( struct bufrdeco_bitmap_compressed *bm, uint32_t index);

// utilities for descriptors
int two_bytes_to_descriptor ( struct bufr_descriptor *d, const uint8_t *source );
int uint32_t_to_descriptor ( struct bufr_descriptor *d, uint32_t id );
int is_a_delayed_descriptor ( struct bufr_descriptor *d );
int is_a_local_descriptor ( struct bufr_descriptor *d );
int is_a_short_delayed_descriptor ( struct bufr_descriptor *d );

// Utilities for strings
char * bufr_adjust_string ( char *s );
char * bufr_charray_to_string ( char *s, char *buf, size_t size );

#endif  // from ifndef BUFRDECO_H
