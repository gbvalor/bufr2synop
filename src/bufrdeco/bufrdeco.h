/***************************************************************************
 *   Copyright (C) 2013-2022 by Guillermo Ballester Valor                  *
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

// If is used by c++ code
#ifdef __cplusplus
extern "C" {
#endif

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

// Uncomment following line to get timimgs in debug phase
//#define DEBUG_TIME

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <getopt.h>
#include <time.h>
#ifdef DEBUG_TIME
#include <sys/times.h>
#include <unistd.h>
#endif
#include <math.h>
#include <sys/stat.h>

// Uncomment following line to debug
//#define __DEBUG

/*!
 *  \def BUFRDECO
 * \brief Name of this library
 */
#define BUFRDECO "bufrdeco"

/*!
  \def BUFR_LEN
  \brief Max length of a BUFR file
*/
#define BUFR_LEN (512000U)

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
  \brief The maximum expected lines in a Table C file, legacy def used from ECMWF
*/
#define MAXLINES_TABLEC BUFR_MAXLINES_TABLEC

/*!
  \def BUFR_NMAXSEQ
  \brief Maximum expected descriptors in a expanded sequence for a single subset
*/
#define BUFR_NMAXSEQ (16384)

/*!
  \def BUFR_EXPLAINED_LENGTH
  \brief Maximum length for a explained descriptor string
*/
#define BUFR_EXPLAINED_LENGTH (512)

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
#define BUFR_LEN_SEC1 (8192U)

/*!
  \def BUFR_LEN_SEC2
  \brief Max length in bytes for a sec2
*/
#define BUFR_LEN_SEC2 (8192U)


/*!
  \def BUFR_LEN_UNEXPANDED_DESCRIPTOR
  \brief Max amount of unexpanded descriptors in a SEC3
*/
#define BUFR_LEN_UNEXPANDED_DESCRIPTOR (512U)


/*!
  \def BUFR_MAX_EXPANDED_SEQUENCES
  \brief Max amount of unexpanded sequences in a struct \ref bufrdeco_expanded_tree
*/
#define BUFR_MAX_EXPANDED_SEQUENCES (128U)

/*!
  \def BUFR_MAX_QUALITY_DATA
  \brief Max amount of quality data which is maped by a struct \ref bufrdeco_bitmap element
*/
#define BUFR_MAX_QUALITY_DATA (32U)

/*!
  \def BUFR_MAX_BITMAP_PRESENT_DATA
  \brief Max number of data present in a bitmap definition
*/
#define BUFR_MAX_BITMAP_PRESENT_DATA (4096)

/*!
  \def BUFR_MAX_BITMAPS
  \brief Max number of structs \ref bufrdeco_bitmap that can be allocated  in a struct \ref bufrdeco_bitmap_array
*/
#define BUFR_MAX_BITMAPS (8)

/*!
 * \def BUFR_MAX_SUBSETS
 * \brief Max number of subsets in the array off bitoffset subset marks the bufrdeco library can manage
 */
#define BUFR_MAX_SUBSETS (2048U)

/*!
 * \def BUFRDECO_MAX_ASSOCIATED_FIELD_STACK
 * \brief Max number of associated_fields_defined in a struct \ref bufrdeco_associated_field_stack 
 */
#define BUFRDECO_MAX_ASSOCIATED_FIELD_STACK (4U)


/*!
  \def BUFR_LEN_SEC3
  \brief Max length in bytes for a sec3
*/
#define BUFR_LEN_SEC3 (8U + 2U * BUFR_LEN_UNEXPANDED_DESCRIPTOR)

/*!
  \def BUFRDECO_OUTPUT_HTML
  \brief Bit mask to the member mask for struct \ref bufrdeco to format output as html for SECs 0 to 3
*/
#define BUFRDECO_OUTPUT_HTML (1)

/*!
  \def BUFRDECO_OUTPUT_XML
  \brief Bit mask to the member mask for struct \ref bufrdeco to format output as xml for SECs 0 to 3
*/
#define BUFRDECO_OUTPUT_XML (2)

/*!
 * \def BUFRDECO_USE_TABLES_CACHE
 * \brief Bit mask to the member mask of struct \ref bufrdeco to mark the use of \ref bufr_tables cache
 */
#define BUFRDECO_USE_TABLES_CACHE (4)


/*!
 *  \def BUFRDECO_OUTPUT_JSON_SEC0
 *  \brief Bit mask to the member mask of struct \ref bufrdeco to print info for SEC 0 in json format
 */
#define BUFRDECO_OUTPUT_JSON_SEC0 (8)

/*!
 *  \def BUFRDECO_OUTPUT_JSON_SEC1
 *  \brief Bit mask to the member mask of struct \ref bufrdeco to print info for SEC 1 in json format
 */
#define BUFRDECO_OUTPUT_JSON_SEC1 (16)

/*!
 *  \def BUFRDECO_OUTPUT_JSON_SEC2
 *  \brief Bit mask to the member mask of struct \ref bufrdeco to print bufr info for SEC 2 in json format
 */
#define BUFRDECO_OUTPUT_JSON_SEC2 (32)

/*!
 *  \def BUFRDECO_OUTPUT_JSON_SEC3
 *  \brief Bit mask to the member mask of struct \ref bufrdeco to print bufr info for SEC 3 in json format
 */
#define BUFRDECO_OUTPUT_JSON_SEC3 (64)

/*!
 *  \def BUFRDECO_OUTPUT_JSON_SUBSET_DATA
 *  \brief Bit mask to the member mask of struct \ref bufrdeco to print bufr subset data in json format
 */
#define BUFRDECO_OUTPUT_JSON_SUBSET_DATA (128)

/*!
 *  \def BUFRDECO_OUTPUT_JSON_EXPANDED_TREE
 *  \brief Bit mask to the member mask of struct \ref bufrdeco to print bufr expanded tree of descriptors
 */
#define BUFRDECO_OUTPUT_JSON_EXPANDED_TREE (256)

/*!
  \def BUFRDECO_OUTPUT_JSON
  \brief Bit mask to the member mask for struct \ref bufrdeco to format output as json for SECs 0 to 3
*/
#define BUFRDECO_OUTPUT_JSON (BUFRDECO_OUTPUT_JSON_SEC0 | BUFRDECO_OUTPUT_JSON_SEC1 | BUFRDECO_OUTPUT_JSON_SEC2 | BUFRDECO_OUTPUT_JSON_SEC3 )

/*!
  \def BUFR_TABLEB_NAME_LENGTH
  \brief Max length (in chars) reserved for a name of variable in table B
*/
#define BUFR_TABLEB_NAME_LENGTH (128U)

/*!
  \def BUFR_TABLEB_NAME_LENGTH
  \brief Max length (in chars) reserved for the unit string in table B
*/
#define BUFR_TABLEB_UNIT_LENGTH (64U)

/*!
 * \def BUFR_CVAL_LENGTH
 * \brief Max length (in chars) of a cval in a \ref bufr_atom_data
 */
#define BUFR_CVAL_LENGTH (128U)

/*!
 *  \def BUFRDECO_PATH_LENGTH
 *  \brief Length for files/directory path strings
 */
#define BUFRDECO_PATH_LENGTH (256U)

/*!
 * \def BUFRDECO_TABLES_CACHE_SIZE
 * \brief Max number of structs \ref bufr_tables in a \ref bufr_tables_cache
 */
#define BUFRDECO_TABLES_CACHE_SIZE (16U)

/*! \typedef buf_t
    \brief Type to set offsets and dimension of arrays or counters used in bufrdeco
*/
typedef uint32_t buf_t;

/*! \typedef ibuf_t
    \brief Type to set offsets and dimension of arrays or counters used in bufrdeco
*/
typedef int32_t ibuf_t;


/*!
 * \def strcpy_safe
 * \brief Macro to make safely a strcpy when we know in calling function the size of string _target_ with sizeof()
 */
#define strcpy_safe(_target_,_src_) {\
   strncpy(_target_,_src_,(sizeof(_target_) - 1)); \
   _target_[sizeof(_target_) - 1] = '\0';}

/*!
 * \def strncpy_safe
 * \brief Macro to make safely a strcpy when we know in calling function the size of string _target_ directly
 */
#define strncpy_safe(_target_,_src_,_dim_) {\
   strncpy(_target_, _src_, _dim_ - 1); \
   _target_[_dim_ - 1] = '\0';}

/*!
 *  \def bufrdeco_assert
 *  \brief Check a expression and exit if it fails
 */
#define bufrdeco_assert(__my_expr__) \
   if ( !(__my_expr__)) {\
     fprintf(stderr, "%s: %s():%d -> ***The expression (%s) is false***\n", \
     basename(__FILE__), __func__, __LINE__, #__my_expr__) ; \
     exit (EXIT_FAILURE);\
   }

/*!
 *  \def bufrdeco_assert_with_return_val
 *  \brief Check a expression and returns a given value if it fails
 */
#define bufrdeco_assert_with_return_val(__my_expr__, __returnval__) \
   if ( !(__my_expr__)) {\
     fprintf(stderr, "%s: %s():%d -> ***The expression (%s) is false***\n", \
     basename(__FILE__), __func__, __LINE__, #__my_expr__) ; \
     return (__returnval__);\
   }

#ifdef DEBUG_TIME
/*!
 * \def print_timing
 * \brief Macro to print the time interval between two clocks(). Useful in debug time.
 */
# define print_timing(__clk_start, __clk_end,__explanation) \
  printf ("# %.6lf s <- Elapsed time for %s\n", (double)(__clk_end - __clk_start)/(double)(CLOCKS_PER_SEC), #__explanation)

extern clock_t bufrdeco_clock_start, bufrdeco_clock_end;

#endif

/*!
  \struct bufr_descriptor
  \brief BUFR descriptor
*/
struct bufr_descriptor
{
  uint8_t f; /*!< F part of descriptor, 2 bits */
  uint8_t x; /*!< X part of descriptor, 6 bits */
  uint8_t y; /*!< Y part of descriptor, 8 bits */
  uint8_t foo; /*!< Not used , to align the struct and size it to 16 bytes */
  char c[12]; /*!< string with whole descriptor (6 decimal digits plus the 0x00 as the end of string */
};

/*!
  \struct bufr_replicator
  \brief store the information when parsing related to replicators
*/
struct bufr_replicator
{
  struct bufr_sequence *s; /*!< struct to which this replicator struct belongs */
  buf_t ixrep; /*!< index in a sequence of replicator descriptor */
  buf_t ixdel; /*!< index in a sequence of a delayed replicator, if not then is the same that ixrep */
  buf_t ndesc; /*!< amount of descritptors in sequence affected by replicator */
  buf_t nloops; /*!< amount of loops to do */
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
  uint32_t associated;
  buf_t associated_to; /*!< Index in an a struct \ref bufrdeco_subset_sequence_data which this is associated to */
  char cval[BUFR_CVAL_LENGTH]; /*!< String value for the bufr descriptor */
  char ctable[BUFR_EXPLAINED_LENGTH]; /*!< Explained meaning for a code table */
  struct bufr_sequence *seq; /*!< Pointer to the struct \ref bufr_sequence to which this descriptor belongs to */
  buf_t ns; /*!< Element in bufr_sequence to which this descriptor belongs to */
  buf_t me; /*!< index in a struct \ref bufrdeco_subset_sequence_data which this one belongs to*/
  buf_t bitac; /*!< index in bitacora related to this struct */
  buf_t is_bitmaped_by; /*!< Index of element in a struct \ref bufrdeco_subset_sequence_data which bitmap this one */
  buf_t bitmap_to; /*!< Index of element in a struct \ref bufrdeco_subset_sequence_data which this one is mapping to */
  buf_t related_to; /*!< Index of element in a struct \ref bufrdeco_subset_sequence_data which this one is related to */
};

/*!
  \struct bufrdeco_subset_sequence_data
  \brief Contains all the information for a subset in a expanded squence
  This is a version to use with bufrdeco parse library
*/
struct bufrdeco_subset_sequence_data
{
  buf_t dim; /*!< Amount of bufr_atom_data currently allocated */
  buf_t nd; /*!< number of current amount of data used in sequence */
  buf_t ss; /*!< Index of subset in the bufr report */
  struct bufr_atom_data *sequence; /*!< the array of data associated structs \ref bufr_atom_data of an expanded sequence */
};

/*!
  \struct bufrdeco_bitmap
  \brief Stores all needed data for a bufr bitmap
*/
struct bufrdeco_bitmap
{
  buf_t dim; /*!< Extension, in bits, for a bitmap. It will count both bits 0 and 1 */                
  buf_t nb0; /*!< Subset data index for first bitmap data (its value may be 0 or 1). 
                  For a compressed bufr is the data index of a struct \ref bufrdeco_compressed_data_references 
                  For non compressed data is the data index in a struct \ref bufrdeco_subset_sequence_data */
  buf_t nb; /*!< Amount of elements used (data present) in the bitmap. i.e. those with bit = 0 and 1*/
  buf_t bitmap_to[BUFR_MAX_BITMAP_PRESENT_DATA]; /*!< Array of indexes in a sequence which bitmaps to */
  buf_t me[BUFR_MAX_BITMAP_PRESENT_DATA]; /*!< Array of data indexes with bit = 0 in a bitmaps */
  buf_t nq; /*!< Amount of quality parameters used per bitmaped data */
  buf_t quality[BUFR_MAX_QUALITY_DATA]; /*!< array of data indexes of first quality value related to bitmap_to[0] */
  buf_t subs; /*!< index of substituted value related to bitmap_to[0] */
  buf_t retain; /*!< Index of retained value related tp bitmap_to[0] */
  buf_t ns1; /*!< amount of first order statistical parameters used per bitmaped data */
  buf_t stat1[BUFR_MAX_QUALITY_DATA]; /*!< Array of indexes of First-order statistical value related to bitmap_to[0] */
  buf_t stat1_desc[BUFR_MAX_QUALITY_DATA]; /*!< Array of indexes which describes the First-order statistical parameter */
  char stat1_expl[BUFR_MAX_QUALITY_DATA][BUFR_EXPLAINED_LENGTH]; /*!< Ctable of a code flag of define fist order statistics */
  buf_t nds; /*!< amount of difference statistical parameters used per bitmaped data */
  buf_t dstat[BUFR_MAX_QUALITY_DATA]; /*!< Array of indexes of Difference statistical value related tp bitmap_to[0] */
  buf_t dstat_desc[BUFR_MAX_QUALITY_DATA]; /*!< Array of indexes which describes the diffenece statistical parameter */
  char dstat_expl[BUFR_MAX_QUALITY_DATA][BUFR_EXPLAINED_LENGTH]; /*!< Ctable of a code flag  */
};


/*!
  \struct bufrdeco_bitmap_array
  \brief Stores all structs \ref bufrdeco_bitmap for a bufr bitmap
*/
struct bufrdeco_bitmap_array
{
  buf_t nba; /*!< Amount of bitmaps used */
  struct bufrdeco_bitmap *bmap[BUFR_MAX_BITMAPS]; /*!< array of pointers to struct \ref bufrdeco_bitmap */
};

/*!
  \struct bufrdeco_bitmap_related_vars
  \brief Contains data related to a target variable with the aid of a bitmap
*/
struct bufrdeco_bitmap_related_vars
{
  buf_t target; /*! Index in a subset sequence which related bitmaped vars are set in this struct */
  buf_t nba;  /*!< Index of struct \ref bufrdeco_bitmap in a \ref bufrdeco_bitmap_array which is used to get info for target */
  buf_t nb; /*!< Index in a \ref bufrdeco_bitmap which is related with target */
  buf_t bitmaped_by; /*!< Index in a subset sequence which bitmaps the target */
  buf_t nq; /*!< Amount of quality parameters used to bitmap the target */
  buf_t qualified_by[BUFR_MAX_QUALITY_DATA]; /*!< array of indexes quality values related to target */
  buf_t substituted; /*!< index of subsituted value related to target */
  buf_t retained; /*!< Index of retained value related to target */
  buf_t ns1; /*!< amount of first order statistical parameters used to bitmap the data */
  buf_t stat1[BUFR_MAX_QUALITY_DATA]; /*!< Array of indexes of First-order statistical value related to target */
  buf_t stat1_desc[BUFR_MAX_QUALITY_DATA]; /*!< Array of indexes which describes the First-order statistical parameter */
  buf_t nds; /*!< Amount of difference statistical parameters related to target */
  buf_t dstat[BUFR_MAX_QUALITY_DATA]; /*!< Attay of indexes of Difference statistical value related to target */
  buf_t dstat_desc[BUFR_MAX_QUALITY_DATA]; /*!< Array of indexes which describes the diffenece statistical parameter related to target*/
};

struct bufrdeco_associated_field
{
   buf_t index; /*!< Index in the array which this struct belongs */ 
   uint8_t assoc_bits;
   uint8_t val; /*!< val of descriptor 0 21 031, the code flag which set what kind of var */
   char cval[BUFR_EXPLAINED_LENGTH]; /*!< cval of code flag */
};

struct bufrdeco_associated_field_stack
{
   buf_t nd; /*!< Numbers of current associated fields */
   struct bufrdeco_associated_field afield[BUFRDECO_MAX_ASSOCIATED_FIELD_STACK]; /*!< Stack with indexes of the current associated fields */
};

struct bufrdeco_associated_field_array
{
   buf_t nd; /*!< Numbers of current associated fields */
   struct bufrdeco_associated_field afield[BUFRDECO_MAX_ASSOCIATED_FIELD_STACK * 256]; /*!< Array with the associated fields for a subset */
};

/*!
  \struct bufrdeco_decoding_data_state
  \brief stores the state when expanding a sequence.
*/
struct bufrdeco_decoding_data_state
{
  buf_t subset; /*!< Subset sequence index being parsed */
  buf_t bit_offset; /*!< First data bit offset of current since the begining of data in byte 4 in SEC 4 */
  int8_t added_bit_length; /*!< Current aditional bit_length that can be changed by descriptor 2 01 YYY */
  int8_t added_scale; /*!< Current aditional scale factor that can be changed by descriptor 2 02 YYY */
  int32_t added_reference; /*!< Current aditional reference that can be changed bu descriptor 2 03 YYY */
  int32_t factor_reference; /*!< Factor to multiply current reference if > 1 */
  uint8_t assoc_bits; /*!< number of associated bits */
  uint8_t changing_reference; /*!< Changing reference as descriptor 2 03 YYY */
  uint8_t fixed_ccitt; /*!< Length in octests for a CCITT var. Changed with descriptor 2 08 YYY . default 0 (or 1)*/
  uint8_t local_bit_reserved; /*!< bits reserved for the inmediately local descriptor */
  uint8_t quality_active; /*!< If != 0 then all 33 class descriptros are refered to a defined bitmap */
  uint8_t subs_active; /*!< If != 0 then Substituted operator values is active */
  uint8_t retained_active; /*!< If != 0 then Replaced/retained operator values is active */
  uint8_t stat1_active; /*!< If != 0 then firsr order statistical values follow */
  uint8_t dstat_active; /*!< If != 0 then difference statistical value follow */
  int32_t bitmaping; /*!< If != 0 then is the backard count reference defined by replicator descriptor after 2 36 000 operator */
  buf_t data_repetition_factor; /*!< for descriptors 0 31 011 and 0 31 012. Active if != 0 */
  struct bufrdeco_bitmap *bitmap; /*!< Pointer to an active bitmap. If not bitmap defined then is NULL */
  struct bufrdeco_associated_field_stack associated; /*!< Struct with associated field stack */
};

/*!
 *  \struct bufr_sequence_index_range
 *  \brief range of indexes in a struct \ref bufr_sequence where a condition must me chequed or marked, as example no data present or event
 */
struct bufr_sequence_index_range
{
  int active; /*!< if != 0 then the range of descriptors is active */
  buf_t first; /*!< First index for a descriptor in a struct \ref bufr_sequence to be chequed */
  buf_t last; /*!< Last index for a descriptor in a struct \ref bufr_sequence to be chequed */
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
  buf_t level; /*!< The recursion level. descriptors in sec3 are level 0 */
  buf_t ndesc; /*!< Number of unexpanded descriptor of a layer */
  struct bufr_sequence *father; /*!< Pointer to the father struct. It should be NULL if level = 0 */
  buf_t father_idesc ; /*!< Index of a descriptor in father to which this sequence is expanded */
  struct bufr_sequence_index_range no_data_present; /*!< struct \ref bufr_sequence_index_range with no data descriptor range */
  struct bufr_sequence_index_range event; /*!< struct \ref bufr_sequence_index_range with an event range */
  struct bufr_sequence_index_range cond_event; /*!< struct \ref bufr_sequence_index_range with a conditioning event range */
  struct bufr_sequence_index_range cat_forecast; /*!< struct \ref bufr_sequence_index_range with a categorical forecasts range */
  uint8_t replicated[NMAXSEQ_DESCRIPTORS]; /*! Array of recursion level of replication 0 = no replicated. N = level of rep */
  struct bufr_descriptor lseq[NMAXSEQ_DESCRIPTORS]; /*!< Array of unexpanded descriptors */
  struct bufr_sequence *sons[NMAXSEQ_DESCRIPTORS]; /*!< Array of pointers to sons. It must be NULL
   except for sequence descriptors.  */
  buf_t iseq; /*!< number of sequence when parsing a tree. for recursion level 0 in sec3 is asigned 0.
                Is the index in member seq in struct bufereco_expanded_tree */
  char name[BUFR_EXPLAINED_LENGTH]; /*!< Name of sequence if any */
};

/*!
 \struct bufrdeco_expanded_tree
 \brief Array of structs \ref bufr_sequence from which bufr expanded tree is made.
*/
struct bufrdeco_expanded_tree
{
  buf_t nseq; /*!< current number of structs used */
  struct bufr_sequence seq[BUFR_MAX_EXPANDED_SEQUENCES]; /*!< array of structs */
};

/*!
 * \def BUFRDECO_EVENT_SEQUENCE_INIT_BITMASK
 * \brief Bitmask for struct \ref bufrdeco_decode_subset_event which marks the init of a descriptor sequence 
 */
#define BUFRDECO_EVENT_SEQUENCE_INIT_BITMASK (1)

/*!
 * \def BUFRDECO_EVENT_DATA_DESCRIPTOR_BITMASK
 * \brief Bitmask for struct \ref bufrdeco_decode_subset_event which marks a data descriptor 
 */
#define BUFRDECO_EVENT_DATA_DESCRIPTOR_BITMASK (2)

/*!
 * \def BUFRDECO_EVENT_OPERATOR_DESCRIPTOR_BITMASK
 * \brief Bitmask for struct \ref bufrdeco_decode_subset_event which marks an operator descriptor 
 */
#define BUFRDECO_EVENT_OPERATOR_DESCRIPTOR_BITMASK (4)

/*!
 * \def BUFRDECO_EVENT_REPLICATOR_DESCRIPTOR_BITMASK
 * \brief Bitmask for struct \ref bufrdeco_decode_subset_event which marks a replicator descriptor 
 */
#define BUFRDECO_EVENT_REPLICATOR_DESCRIPTOR_BITMASK (8)

/*!
 * \def BUFRDECO_EVENT_SEQUENCE_FINAL_BITMASK_BITMASK
 * \brief Bitmask for struct \ref bufrdeco_decode_subset_event which marks the final of a descriptor sequence 
 */
#define BUFRDECO_EVENT_SEQUENCE_FINAL_BITMASK (16)

/*!
 * \def BUFRDECO_EVENT_DATA_BITMAP_BITMASK
 * \brief Bitmask for struct \ref bufrdeco_decode_subset_event which marks a bitmap bit 
 */
#define BUFRDECO_EVENT_DATA_BITMAP_BITMASK (32)

/*!
 * \def BUFRDECO_EVENT_DATA_QUALIFIYER_BITMASK
 * \brief Bitmask for struct \ref bufrdeco_decode_subset_event which marks a qualifier data related to another one 
 */
#define BUFRDECO_EVENT_DATA_QUALIFIYER_BITMASK (64)

/*!
 * \def BUFRDECO_EVENT_DATA_FIRST_ORDER_STAT_BITMASK 
 * \brief Bitmask for struct \ref bufrdeco_decode_subset_event which marks a first order statistics data related to another one 
 */
#define BUFRDECO_EVENT_DATA_FIRST_ORDER_STAT_BITMASK (128)

/*!
 * \def BUFRDECO_EVENT_DATA_DIFF_STAT_BITMASK 
 * \brief Bitmask for struct \ref bufrdeco_decode_subset_event which marks a first difference statistics data related to another one 
 */
#define BUFRDECO_EVENT_DATA_DIFF_STAT_BITMASK (256)

/*!
 * \def BUFRDECO_EVENT_DATA_SUBSTITUTE_BITMASK 
 * \brief Bitmask for struct \ref bufrdeco_decode_subset_event which marks a substituded data 
 */
#define BUFRDECO_EVENT_DATA_SUBSITUTE_BITMASK (512)

/*!
 * \def BUFRDECO_EVENT_DATA_REPLACED_BITMASK 
 * \brief Bitmask for struct \ref bufrdeco_decode_subset_event which marks a replaced data 
 */
#define BUFRDECO_EVENT_DATA_REPLACED_BITMASK (1024)

/*!
 * \def BUFRDECO_EVENT_DATA_ASSOCIATED_BITMASK 
 * \brief Bitmask for struct \ref bufrdeco_decode_subset_event which marks a asociated field 
 */
#define BUFRDECO_EVENT_DATA_ASSOCIATED_BITMASK (2048)

/*!
 \struct bufrdeco_compressed_ref
 \brief Struct to hold the needed reference bit offsets, descriptor tree and replications in a compressed BUFR

 In a compressed bufr it is supossed that all subsets are coded under the same expanded tree, included the delayed descriptors.
 So the length of data in every subset is the same. To access to a field data we would need these strutcs \ref bufrdeco_compressed_ref
*/
struct bufrdeco_compressed_ref
{
  int32_t bitac; /*!< Index in the bitacora related to this struct */
  uint8_t is_associated ; /*!< 0 if is not associated data */
  uint8_t has_data ; /*!< 1 if has any subset with valid data. 0 if missing in all subsets */
  uint8_t bits; /*!< bits for data or associated in table B */
  uint8_t inc_bits; /*!< number of inc bits for every subset  */
  int32_t ref; /*!< reference for a expanded data in table B */
  buf_t bit0; /*!< first bit offset, i.e, most significant bit for ref0 */
  buf_t ref0; /*!< local reference for a expanded data in subsets */
  char cref0[256]; /*!< Local reference in case of string */
  int32_t escale; /*!< escale for a expanded data in subset */
  char name[BUFR_TABLEB_NAME_LENGTH]; /*!< String with the name of descriptor */
  char unit[BUFR_TABLEB_UNIT_LENGTH]; /*!< String with the name of units */
  struct bufr_descriptor *desc; /*!< associated descriptor */
  struct bufr_sequence *seq; /*!< associated sequence */
  buf_t me; /*!< index of the array where this struct belongs */
  buf_t is_bitmaped_by; /*!< Index element in a struct \ref bufrdeco_compressed_data_references which bitmap this one */
  buf_t bitmap_to; /*!< Index element in a struct \ref bufrdeco_compressed_data_references which this one is mapping to */
  buf_t related_to; /*!< Index of element in struct \ref bufrdeco_compressed_data_references which this one is related to */
  buf_t qualified_by; /*!< Index of element in a struct \ref bufrdeco_compressed_data_references which qualifies this one */ 
  buf_t replicated_desc; /*!< Index of a replicated descriptor in a replication interval at current level, first 1 */
  buf_t replicated_loop; /*!< Index of loop of a replicated descriptor in replication interval at current level, first 1 */
  buf_t replicated_ndesc; /*!< Amount of descriptors in a replication interval at current level */
  buf_t replicated_nloop; /*!< Amount of loops in a replication interval at current level */
  buf_t associated_to; /*!< Index of element in a struct \ref bufrdeco_compressed_data_references which this is associated to */
};


/*!
 *  \struct bufrdeco_decode_subset_event
 *  \brief A struct with an event when decoding a subset data in a bufr report
 */
struct bufrdeco_decode_subset_event
{
  uint32_t mask; /*!< Mask with the type of event */
  int32_t ref_index; /*!< If >= 0 index to an array of data in a struct. 
                       In case of compressed data may be a \ref bufrdeco_compressed_data_references. 
                       For non compressed data \ref bufrdeco_subset_sequence_data */
  void *pointer; /*!< Pointer to a needed struct depending on event mask and type of data (compressed or not)
                      For init or end of sequence : The pointer to sequence struct
                      For events with data: The pointer to the descriptor with data 
                  */
  void *pointer2; /*!< Secondary pointer to a struct if needed depending on event mask and type of data (compressed or not)
                      For data associted with  bit mask (bitmask, qualifier, first stat and diff stat) the pointer to the bitmask struct  
                  */
  buf_t iaux[9]; /*!< Auxiliar array of indexes, used in replications.
                      In all cases:
                         [0] index of sequence in expanded tree. Is the member iseq in a struct \ref bufr_seq 
                         [1] index of var (qualifier, first stat, or diff stat) according with mask
                         [6] index in array of associated field plus one. (No associated = 0)
                         [7] Reserved
                         [8] Reserved
                      Non compressed bufr:
                         [2] ordinal of descriptor in a replication (No replication = 0, first desriptor is 1)
                         [3] number of descriptors in a replication (No replication = 0)
                         [4] ordinal of loop in a replication (No replication = 0, first loop is 1)
                         [5] number of loops in a replication (No replication = 0)
                  */
};

/*!
 *  \struct bufrdeco_decode_subset_bitacora
 *  \brief store all the history and events of a decoding subset
 */
struct bufrdeco_decode_subset_bitacora
{
  buf_t dim; /*!< Dimension of array of indexes */
  buf_t nd; /*!< Current number of elements in array used, i.e, the current compressed refences with data in SEC4 */
  struct bufrdeco_decode_subset_event *event; /*!< array of strutcs \ref bufrdeco_decode_subset_event where to log all the decoding events in chronological order */   
};

/*!
  \struct bufrdeco_compressed_data_references
  \brief Manage an array of structs \ref bufrdeco_compressed_ref
*/
struct bufrdeco_compressed_data_references
{
  buf_t dim; /*!< dimension of array of compressed refs */
  buf_t nd; /*!< current amount of data used */
  struct bufrdeco_compressed_ref *refs; /*!< pointer to allocated array */
};

/*!
 *  \struct bufrdeco_subset_bit_offsets
 *  \brief Array of offset in bits for every subset in a non-compressed bufr. Offset is counted in bits from the init of SEC4 data, usually bit 32.
 *
 *   To set the offset for non compressed bufr it is neccesary to parse the expanded tree and extact the data of the prior subsets.
 *   If we already know the bit offset of the data then we can access directly to the subset data without parsing the prior subsets.
 *
 *   The utility of these offset array is that it can be write to and read from a binary file and access quickly to the
 *   desired subset data.
 */
struct bufrdeco_subset_bit_offsets
{
  buf_t nr ; /*!< Current number of used subset bit offsets. */
  buf_t ofs[BUFR_MAX_SUBSETS]; /*! Array of subset bitoffset. offsets[n] is the bit offset from first data in sequence 'n'.
  Offset is counted from the begining of SEC4 data in bits, usually bit 32 */
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
  char filename[BUFRDECO_PATH_LENGTH]; /*!< filename of Bufr file */
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
  \brief Store a parsed sec2 from a bufr file including rawdata
*/
struct bufr_sec2
{
  uint32_t length;
  uint8_t raw[BUFR_LEN_SEC2]; /*!< Raw data for sec2 as is in original BUFR file */
};

/*!
  \struct bufr_sec3
  \brief Store a parsed sec3 from a bufr file including rawdata
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
  \brief Store a parsed sec4 from a bufr file including rawdata

  Note that member \a raw  do not need to be allocated
*/
struct bufr_sec4
{
  uint32_t length; /*!< length of sec4 in bytes */
  size_t bit_offset; /*!< Offset to current first bit in raw data sec4 to parse */
  uint8_t raw[BUFR_LEN]; /*!< Pointer to a raw data for sec4 as in original BUFR file */
};

/*!
   \struct bufr_tableB_decoded_item
   \brief Store parameters for a descriptor in table b, i. e. with f = 0
*/
struct bufr_tableB_decoded_item
{
  uint8_t changed; /*!< flag. If 0 = not changed from table B. If 1 Changed */
  uint8_t x; /*!< x value of descriptor */
  uint8_t y; /*!< y value of descriptor */
  uint8_t kk; /*!< not used. A trick to align */
  char key[8]; /*!< c value of descriptor */
  char name[BUFR_TABLEB_NAME_LENGTH]; /*!< name */
  char unit[BUFR_TABLEB_UNIT_LENGTH]; /*!< unit */
  int32_t scale; /*!< escale */
  int32_t scale_ori; /*!< escale as readed from table b */
  int32_t reference; /*!< reference */
  int32_t reference_ori; /*!< reference as readed from table b */
  buf_t nbits; /*!< bits */
  buf_t nbits_ori; /*!< bits as readed from table b */
  buf_t tableC_ref; /*!< item to point table c, if any */
  buf_t tableD_ref; /*!< item to point table d, if any */
};

/*!
  \struct bufr_tableB
  \brief Store a table B readed from a file formated and named as ECMWF bufrdc package
*/
struct bufr_tableB
{
  int wmo_table; /*!< 1 when table is from WMO csv file, 0 otherwise */
  char path[BUFRDECO_PATH_LENGTH]; /*!< Complete path of current file readed */
  char old_path[BUFRDECO_PATH_LENGTH]; /*!< Cmplete path of prior file readed, used to avoid read two consecutive times the same file */
  buf_t nlines; /*!< Current lines readed from file, i. e. used in array item[] */
  buf_t x_start[64]; /*!< Index in array \a item[] for first x. x_start[j] is index for first descriptor which x == j */
  buf_t num[64]; /*!< Amount of items for x. num[i] is the amount of items in array where x = i */
  uint8_t y_ref[64][256]; /*!< index for y since first x. x_ref[i][j] is index since x_start[i] where y == j */
  struct bufr_tableB_decoded_item item[BUFR_MAXLINES_TABLEB]; /*!< Array with structs containing parsed lines readed from file */
};


/*!
   \struct bufr_tableC_decoded_item
   \brief Store parameters for a descriptor in table C, i. e. for Table code and flags
   Remember this is NOT the C WMO tables
*/
struct bufr_tableC_decoded_item
{
  char key[8]; /*!< c value of descriptor */
  uint8_t x; /*!< x value of descriptor */
  uint8_t y; /*!< y value of descriptor */
  uint32_t ival; /*!< code value */
  char description[BUFR_EXPLAINED_LENGTH]; /*!< Description string of code */
};


/*!
  \struct bufr_tableC
  \brief Store a table C readed from a file formated and named as ECMWF bufrdc package
*/
struct bufr_tableC
{
  int wmo_table; /*!< 1 when table is from WMO csv file, 0 otherwise */
  char path[BUFRDECO_PATH_LENGTH]; /*!< Complete path of current file readed */
  char old_path[BUFRDECO_PATH_LENGTH]; /*!< Cmplete path of prior file readed, used to avoid read two consecutive times the same file */
  buf_t nlines; /*!< Current lines readed from file, i. e. used in array \a l[] */
  buf_t x_start[64]; /*!< Index in array \a l[] for first x. x_start[j] is index for first descriptor which x == j */
  buf_t num[64]; /*!< Amonut of lines for x. num[i] is the amount of items in array where x = i */
  buf_t y_ref[64][256]; /*!< index for first y since first x. x_ref[i][j] is index since x_start[i] where y == j */
  struct bufr_tableC_decoded_item item[BUFR_MAXLINES_TABLEC]; /*!< Array of decoded lines */
};


/*!
  \struct bufr_tableD_decoded_item
  \brief Store parameters for a line in table D (sequence of descritors)
*/
struct bufr_tableD_decoded_item
{
  char key[8]; /*!< c key value of sequence descriptor */
  char key2[8]; /*!< c key 2 of sequence element */
  char description[BUFR_EXPLAINED_LENGTH]; /*!< Explained name of sequence */
  char description2[BUFR_EXPLAINED_LENGTH]; /*!< Explained name of elemet of sequence */
};

/*!
  \struct bufr_tableD
  \brief Store a table D readed from a file formated and named as ECMWF bufrdc package
*/
struct bufr_tableD
{
  int wmo_table; /*!< 1 when table is from WMO csv file, 0 otherwise */
  char path[BUFRDECO_PATH_LENGTH]; /*!< Complete path of current file readed */
  char old_path[BUFRDECO_PATH_LENGTH]; /*!< Cmplete path of prior file readed, used to avoid read two consecutive times the same file */
  buf_t nlines; /*!< Current lines readed from file, i. e. used in array \a l[] */
  buf_t x_start[64]; /*!< Index in array \a l[] for first x. x_start[j] is index for first descriptor which x == j */
  buf_t num[64]; /*!< Amonut of lines for x. num[i] is the amount of items in array where x = i */
  char l[BUFR_MAXLINES_TABLED][24]; /*!< Array with info about descriptor sequences emulating ECMW tables D */
  struct bufr_tableD_decoded_item item[BUFR_MAXLINES_TABLED]; /*!< Array of decoded lines */
};

/*!
  \struct bufr_tables
  \brief Contains all tables needed to parse a bufr file

  All readed files need to be named an formed as ECMWF bufrdc package
*/
struct bufr_tables
{
  struct bufr_tableB b; /*!< Table B */
  struct bufr_tableC c; /*!< Table C */
  struct bufr_tableD d; /*!< Table D */
};

/*!
 * \struct bufr_tables_cache
 * \brief Struct to store the cache of structs \ref bufr_tables
 *
 * When used, the member \a tables in main struct \ref bufrdeco is pointing to one of the elements in member array \a tab
 */
struct bufr_tables_cache
{
  buf_t nt; /*!< Tables actually allocated in cache */
  buf_t next; /*< index of next element in array to add */
  int8_t ver[BUFRDECO_TABLES_CACHE_SIZE]; /*!< Table version for array elements */
  struct bufr_tables *tab[BUFRDECO_TABLES_CACHE_SIZE]; /*! Array of structs \ref bufr_tables allocated */
};

/*!
  \struct bufrdeco
  \brief This struct contains all needed data to parse and decode a BUFR file

  NOTE that must be initializad before use
*/
struct bufrdeco
{
  uint32_t mask; /*!< Array of bit mask which set some runtime features of bufrdeco library */
  struct gts_header header; /*!< GTS data */
  struct bufr_sec0 sec0; /*!< Parsed sec0 */
  struct bufr_sec1 sec1; /*!< Parsed sec1 */
  struct bufr_sec2 sec2; /*!< Parsed sec2 */
  struct bufr_sec3 sec3; /*!< Parsed sec3 */
  struct bufr_sec4 sec4; /*!< Parsed sec4 */
  struct bufr_tables *tables; /*!< Pointer to a the struct containing all tables needed for a single bufr */
  struct bufr_tables_cache cache; /*!< Struct \ref bufr_tables_cache  */
  struct bufrdeco_expanded_tree *tree; /*!< Pointer to a struct containing the parsed descriptor tree (with explansion) */
  struct bufrdeco_decoding_data_state state; /*!< Struct with data needed when parsing bufr */
  struct bufrdeco_subset_bit_offsets offsets; /*!< Struct \ref bufrdeco_subset_bit_offsets with bit offset of start point of every subset in non compressed bufr */
  struct bufrdeco_compressed_data_references refs; /*!< struct with data references in case of compressed bufr */
  struct bufrdeco_decode_subset_bitacora bitacora; /*!< struct with the events log when decoding a subset data in sec4 */
  struct bufrdeco_subset_sequence_data seq; /*!< sequence with data subset after parse */
  struct bufrdeco_bitmap_array bitmap; /*!< Stores data for bit-maps */
  struct bufrdeco_bitmap_related_vars brv; /*!< Stores data related with the aid of a bit-maps */
  struct bufrdeco_associated_field_array assoc; /*!< Array with associated fields info used in a subset */
  char bufrtables_dir[BUFRDECO_PATH_LENGTH]; /*!< string with the path of bufr table directories */
  char error[1024]; /*!< String with detected errors, if any */
  FILE *out ; /*!< Stream used for normal output. By default 'stdout' */
  FILE *err ; /*!< Stream used for error output. By default 'stderr' */
};

extern const char DEFAULT_BUFRTABLES_ECMWF_DIR1[];
extern const char DEFAULT_BUFRTABLES_ECMWF_DIR2[];
extern const char DEFAULT_BUFRTABLES_WMO_CSV_DIR1[];
extern const char DEFAULT_BUFRTABLES_WMO_CSV_DIR2[];
extern const double pow10pos[8];
extern const double pow10neg[8];
extern const int32_t pow10pos_int[10];
extern const struct bufr_descriptor DESCRIPTOR_ROOT;

/************ Main API functions **************/
char *bufrdeco_get_version ( char *version, size_t dversion, char *build, size_t dbuild, char *builder, size_t dbuilder,
                             int *version_major, int *version_minor, int *version_patch );
int bufrdeco_parse_tree ( struct bufrdeco *b );
int bufrdeco_init ( struct bufrdeco *b );
int bufrdeco_close ( struct bufrdeco *b );
int bufrdeco_reset ( struct bufrdeco *b );
int bufrdeco_set_tables_dir ( struct bufrdeco *b, const char *tables_dir );
int bufrdeco_set_out_stream (FILE *out, struct bufrdeco *b);
int bufrdeco_set_err_stream (FILE *err, struct bufrdeco *b);
int bufrdeco_get_bufr ( struct bufrdeco *b, char *filename );
struct bufrdeco_subset_sequence_data *bufrdeco_get_target_subset_sequence_data ( buf_t nset, struct bufrdeco *b );
int bufrdeco_read_subset_offset_bits ( struct bufrdeco *b, char *filename );
int bufrdeco_write_subset_offset_bits ( struct bufrdeco *b, const char *filename );


// Memory funcions
int bufrdeco_init_expanded_tree ( struct bufrdeco_expanded_tree **t );
int bufrdeco_free_expanded_tree ( struct bufrdeco_expanded_tree **t );
int bufrdeco_init_tables ( struct bufr_tables **t );
int bufrdeco_free_tables ( struct bufr_tables **t );
int bufrdeco_substitute_tables ( struct bufr_tables **replaced, struct bufr_tables *source, struct bufrdeco *b );
int bufrdeco_init_subset_sequence_data ( struct bufrdeco_subset_sequence_data *ba );
int bufrdeco_clean_subset_sequence_data ( struct bufrdeco_subset_sequence_data *ba );
int bufrdeco_free_subset_sequence_data ( struct bufrdeco_subset_sequence_data *ba );
int bufrdeco_increase_data_array ( struct bufrdeco_subset_sequence_data *s );
int bufrdeco_increase_subset_sequence_data_count ( struct bufrdeco_subset_sequence_data *d, struct bufrdeco *b );
int bufrdeco_free_compressed_data_references ( struct bufrdeco_compressed_data_references *rf );
int bufrdeco_init_compressed_data_references ( struct bufrdeco_compressed_data_references *rf );
int bufrdeco_increase_compressed_ref_array ( struct bufrdeco_compressed_data_references *r );
int bufrdeco_increase_data_array ( struct bufrdeco_subset_sequence_data *s );
int bufrdeco_store_tables ( struct bufr_tables **t, struct bufr_tables_cache *c, uint8_t ver );
int bufrdeco_cache_tables_search ( const struct bufr_tables_cache *c, uint8_t ver );
int bufrdeco_free_cache_tables ( struct bufr_tables_cache *c );
int bufrdeco_add_event_to_bitacora ( struct bufrdeco *b, struct bufrdeco_decode_subset_event *event);
int bufrdeco_init_subset_bitacora ( struct bufrdeco *b);
int bufrdeco_increase_decode_subset_bitacora_array ( struct bufrdeco_decode_subset_bitacora *dsb );
int bufrdeco_free_decode_subset_bitacora ( struct bufrdeco_decode_subset_bitacora *dsb );
int bufrdeco_pop_associated_field (struct bufrdeco_associated_field *popped, struct bufrdeco_associated_field_stack *afs );
int bufrdeco_push_associated_field (const struct bufrdeco_associated_field *pushed, struct bufrdeco_associated_field_stack *afs );
int bufrdeco_add_associated_field (const struct bufrdeco_associated_field *added, struct bufrdeco_associated_field_array *afa );


// Read bufr functions
int bufrdeco_read_bufr ( struct bufrdeco *b,  char *filename );
int bufrdeco_extract_bufr ( struct bufrdeco *b,  char *filename );
int bufrdeco_read_buffer ( struct bufrdeco *b,  uint8_t *bufrx, buf_t size );

// Read bufr WMO csv table files
int get_wmo_tablenames ( struct bufrdeco *b );
int bufr_read_tableB ( struct bufrdeco *b );
int bufr_read_tableC ( struct bufrdeco *b );
int bufr_read_tableD ( struct bufrdeco *b );
int bufr_read_tables ( struct bufrdeco *b );
char * csv_quoted_string ( char *out, char *in );
int parse_csv_line ( int *nt, char *tk[], char *lin );


// Print and output functions
int print_bufrdeco_compressed_ref ( struct bufrdeco_compressed_ref *r, buf_t index );
int fprint_bufrdeco_compressed_ref ( FILE *f, struct bufrdeco_compressed_ref *r, buf_t index );
int print_bufrdeco_compressed_data_references ( struct bufrdeco_compressed_data_references *r );
int fprint_bufrdeco_compressed_data_references ( FILE *f, struct bufrdeco_compressed_data_references *r );
int print_sec0_info ( struct bufrdeco *b );
int print_sec1_info ( struct bufrdeco *b );
int print_sec3_info ( struct bufrdeco *b );
int print_sec4_info ( struct bufrdeco *b );
int sprint_sec0_info ( char *target, size_t lmax, struct bufrdeco *b );
int sprint_sec1_info ( char *target, size_t lmax, struct bufrdeco *b );
int sprint_sec3_info ( char *target, size_t lmax, struct bufrdeco *b );
int sprint_sec4_info ( char *target, size_t lmax, struct bufrdeco *b );
int sprint_sec0_info_html ( char *target, size_t lmax, struct bufrdeco *b );
int sprint_sec1_info_html ( char *target, size_t lmax, struct bufrdeco *b );
int sprint_sec3_info_html ( char *target, size_t lmax, struct bufrdeco *b );
int sprint_sec4_info_html ( char *target, size_t lmax, struct bufrdeco *b );
int bufrdeco_print_tree ( struct bufrdeco *b );
int bufrdeco_fprint_tree ( FILE *f, struct bufrdeco *b );
int bufrdeco_print_atom_data_stdout ( struct bufr_atom_data *a );
int bufrdeco_print_atom_data_file ( FILE *f, struct bufr_atom_data *a );
int bufrdeco_print_subset_sequence_data ( struct bufrdeco_subset_sequence_data *s );
int bufrdeco_print_subset_sequence_data_html ( struct bufrdeco_subset_sequence_data *s );
int bufrdeco_print_subset_sequence_data_tagged_html ( struct bufrdeco_subset_sequence_data *s, char *id );
int bufrdeco_fprint_subset_sequence_data_html ( FILE *f, struct bufrdeco_subset_sequence_data *s );
int bufrdeco_fprint_subset_sequence_data_tagged_html ( FILE *f, struct bufrdeco_subset_sequence_data *s, const char *id );
int bufrdeco_fprint_subset_sequence_data ( FILE *f, struct bufrdeco_subset_sequence_data *s );
char * bufrdeco_print_atom_data ( char *target, size_t lmax, struct bufr_atom_data *a );
char * bufrdeco_print_atom_data_html ( char *target, size_t lmax, struct bufr_atom_data *a, uint32_t ss );
char * get_formatted_value_from_escale ( char *fmt, size_t dim, int32_t escale, double val );
char * get_formatted_value_from_escale2 ( char *fmt, size_t dim, int32_t escale, double val );
int bufrdeco_print_event( const struct bufrdeco_decode_subset_event *e, struct bufrdeco *b);

// To parse. General
int bufrdeco_parse_tree_recursive ( struct bufrdeco *b, struct bufr_sequence *father, buf_t father_idesc, const char *key );
int bufrdeco_decode_data_subset ( struct bufrdeco *b );
int bufrdeco_decode_subset_data_recursive ( struct bufrdeco_subset_sequence_data *d, struct bufr_sequence *l, struct bufrdeco *b );
int bufrdeco_decode_replicated_subsequence ( struct bufrdeco_subset_sequence_data *d, struct bufr_replicator *r, struct bufrdeco *b );
int bufrdeco_parse_f2_descriptor ( struct bufrdeco_subset_sequence_data *s, const struct bufr_descriptor *d, struct bufrdeco *b );

// To parse compressed bufr
int bufrdeco_parse_compressed ( struct bufrdeco_compressed_data_references *r, struct bufrdeco *b );
int bufrdeco_parse_compressed_recursive ( struct bufrdeco_compressed_data_references *r, struct bufr_sequence *l, struct bufrdeco *b );
int bufr_decode_subset_data_compressed ( struct bufrdeco_subset_sequence_data *s, struct bufrdeco_compressed_data_references *r,
    struct bufrdeco *b );
int bufrdeco_decode_replicated_subsequence_compressed ( struct bufrdeco_compressed_data_references *r, struct bufr_replicator *rep,
    struct bufrdeco *b );
int bufrdeco_parse_f2_compressed ( struct bufrdeco_compressed_data_references *r, struct bufr_descriptor *d, struct bufrdeco *b );
int bufrdeco_tableB_compressed ( struct bufrdeco_compressed_ref *r, struct bufrdeco *b, struct bufr_descriptor *d, int mode );
int bufrdeco_get_atom_data_from_compressed_data_ref ( struct bufr_atom_data *a, struct bufrdeco_compressed_ref *r,
    buf_t subset, struct bufrdeco *b );
int bufrdeco_increase_compressed_data_references_count ( struct bufrdeco_compressed_data_references *r, struct bufrdeco *b );

// To get parsed data
struct bufrdeco_subset_sequence_data * bufrdeco_get_subset_sequence_data ( struct bufrdeco *b );

// To parse and print data and tree in json format
buf_t bufrdeco_print_json_sec0 ( FILE *out, struct bufrdeco *b );
buf_t bufrdeco_print_json_sec1 ( FILE *out, struct bufrdeco *b );
buf_t bufrdeco_print_json_sec2 ( FILE *out, struct bufrdeco *b );
buf_t bufrdeco_print_json_sec3 ( FILE *out, struct bufrdeco *b );
buf_t bufrdeco_print_json_tree ( struct bufrdeco *b );
buf_t bufrdeco_print_json_tree_recursive ( FILE *out, struct bufrdeco *b, struct bufr_sequence *seq );
buf_t bufrdeco_print_json_subset_data_prologue (FILE *out,  struct bufrdeco *b );
buf_t bufrdeco_print_json_subset_data_epilogue ( FILE *out );
buf_t bufrdeco_print_json_object_atom_data (FILE *out, struct bufr_atom_data *a, buf_t index_data, struct bufrdeco *b, const char *aux );
buf_t bufrdeco_print_json_object_operator_descriptor (FILE *out,  struct bufr_descriptor *d, const char *aux );
buf_t bufrdeco_print_json_object_replicator_descriptor (FILE *out,  const struct bufr_descriptor *d, const char *aux );
buf_t bufrdeco_print_json_object_event_data ( FILE *out,  struct bufr_atom_data *a, const struct bufrdeco_decode_subset_event *event, struct bufrdeco *b, const char *add );
buf_t bufrdeco_print_json_sequence_descriptor_header (FILE *out, const struct bufr_sequence *seq );
buf_t bufrdeco_print_json_sequence_descriptor_final ( FILE *out );
buf_t bufrdeco_print_json_separator( FILE *out );
buf_t bufrdeco_print_json_scape_string_cvals( FILE *out, const char *source);
buf_t bufrdeco_print_json_subset_data_prologue ( FILE *out,  struct bufrdeco *b );
buf_t bufrdeco_print_json_subset_data_epilogue ( FILE *out );
buf_t bufrdeco_print_json_subset_data ( struct bufrdeco *b);


// Functions to get bits of data
uint32_t two_bytes_to_uint32 ( const uint8_t *source );
uint32_t three_bytes_to_uint32 ( const uint8_t *source );
uint32_t get_bits_as_uint32_t ( uint32_t *target, uint8_t *has_data, uint8_t *source, buf_t *bit0_offset,
                                buf_t bit_length );
uint32_t get_bits_as_char_array ( char *target, uint8_t *has_data, uint8_t *source, buf_t *bit0_offset,
                                  buf_t bit_length );

// Utilities for tables
char * bufrdeco_explained_table_val ( char *expl, size_t dim, struct bufr_tableC *tc, uint32_t *index,
                                      const struct bufr_descriptor *d, uint32_t ival );
char * bufrdeco_explained_flag_val ( char *expl, size_t dim, struct bufr_tableC *tc, const struct bufr_descriptor *d,
                                     uint64_t ival, uint8_t nbits );
char * bufrdeco_explained_table_csv_val ( char *expl, size_t dim, struct bufr_tableC *tc, uint32_t *index,
    struct bufr_descriptor *d, uint32_t ival );
char * bufrdeco_explained_flag_csv_val ( char *expl, size_t dim, struct bufr_tableC *tc, struct bufr_descriptor *d,
    uint64_t ival, uint8_t nbits );
int bufrdeco_tableD_get_descriptors_array ( struct bufr_sequence *s, struct bufrdeco *b,
    const char *key );
int bufrdeco_tableB_val ( struct bufr_atom_data *a, struct bufrdeco *b, const struct bufr_descriptor *d, buf_t mode );
int bufr_find_tableB_index ( buf_t *index, struct bufr_tableB *tb, const char *key );
int get_table_b_reference_from_uint32_t ( int32_t *target, uint8_t bits, uint32_t source );
int bufrdeco_tableD_get_descriptors_array ( struct bufr_sequence *s, struct bufrdeco *b, const char *key );
int bufr_find_tableC_csv_index ( buf_t *index, struct bufr_tableC *tc, const char *key, uint32_t code );
char *bufrdeco_get_f2_descriptor_explanation ( char *e, size_t dim, const struct bufr_descriptor *d );

// utilities for bitmaps
int bufrdeco_allocate_bitmap ( struct bufrdeco *b );
int bufrdeco_clean_bitmaps ( struct bufrdeco *b );
int bufrdeco_free_bitmap_array ( struct bufrdeco_bitmap_array *a );
int bufrdeco_add_to_bitmap ( struct bufrdeco_bitmap *bm, buf_t index_to, buf_t index_by );
int bufrdeco_get_bitmaped_info ( struct bufrdeco_bitmap_related_vars *brv, uint32_t target, struct bufrdeco *b );

// utilities for descriptors
int two_bytes_to_descriptor ( struct bufr_descriptor *d, const uint8_t *source );
int uint32_t_to_descriptor ( struct bufr_descriptor *d, uint32_t id );
int is_a_delayed_descriptor ( const struct bufr_descriptor *d );
int is_a_local_descriptor ( const struct bufr_descriptor *d );
int is_a_short_delayed_descriptor ( const struct bufr_descriptor *d );

// Utilities for strings
char * bufr_adjust_string ( char *s );
char * bufr_charray_to_string ( char *s, const char *buf, size_t size );

// I/O of bit offset for subsets
int bufr_read_subset_offset_bits ( FILE *f, struct bufrdeco_subset_bit_offsets *off );
int bufr_write_subset_offset_bits ( FILE *f, const struct bufrdeco_subset_bit_offsets *off );


#ifdef __cplusplus
} /* closing brace for extern "C" */
#endif

#endif  // from ifndef BUFRDECO_H
