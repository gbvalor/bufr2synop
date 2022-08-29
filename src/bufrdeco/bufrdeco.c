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
 \file bufrdeco.c
 \brief This file has the code for library bufrdeco API functions
*/
#ifndef CONFIG_H
# include "config.h"
# define CONFIG_H
#endif

#include "bufrdeco.h"
 
#ifdef DEBUG_TIME
  clock_t bufrdeco_clock_start, bufrdeco_clock_end;
#endif

/*!
  \fn char *bufrdeco_get_version(char *version, char *build, char *builder, int *version_major, int *version_minor, int *version_patch)
  \brief Get strings with version information and build date and time
  \param version  pointer to string with version as result if not NULL
  \param dversion dimension of string \ref version
  \param build pointer to string with compiler and compilation date and time if not NULL
  \param dbuild dimension of string \ref build
  \param builder pointer to string with builder utility. 'cmake' or 'autotools' if not NULL
  \param dbuilder dimension of string \ref builder
  \param version_major pointer to string with version_major component if not NULL
  \param version_minor pointer to string with version_minor component if not NULL
  \param version_patch pointer to string with version_patch component if not NULL

  Retuns string pointer \ref version.
 */
char *bufrdeco_get_version ( char *version, size_t dversion, char *build, size_t dbuild, char *builder, size_t dbuilder,
                             int *version_major, int *version_minor, int *version_patch )
{
  int major = 0, minor = 0, patch = 0;
  size_t used;

  // Check argument
  if ( version == NULL )
    return NULL;

  snprintf ( version, dversion, "%s", VERSION );
  // default
  sscanf ( version, "%d.%d.%d", &major, &minor, &patch );

  if ( build )
    {
      used = 0;
#if defined(__INTEL_COMPILER)
      used += snprintf ( build + used, dbuild - used, "using INTEL C compiler icc %d.%d ", __INTEL_COMPILER, __INTEL_COMPILER_UPDATE );
#elif defined(__clang_version__)
      used += snprintf ( build + used, dbuild - used, "using clang C compiler ", __clang_version__ );
#elif defined(__GNUC__)
      used += snprintf ( build + used, dbuild - used, "using GNU C compiler gcc %d.%d.%d ", __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__ );
#elif defined(_MSC_VER)
      used += snprintf ( build + used, dbuild - used, "using MICROSOFT C compiler %d ", _MSC_VER );
#else
      used += snprintf ( build + used, dbuild - used, "using an unknown C compiler " );
#endif
      snprintf ( build + used, dbuild - used,"at %s %s",__DATE__,__TIME__ );
    }

  if ( builder )
#ifdef BUILD_USING_CMAKE
    strncpy_safe ( builder, "cmake", dbuilder );
#else
    strncpy_safe ( builder, "autotools", dbuilder );
#endif
  if ( version_major )
    *version_major = major;
  if ( version_minor )
    *version_minor = minor;
  if ( version_patch )
    *version_patch = patch;
  return version;
}

/*!
  \fn int bufrdeco_parse_tree ( struct bufrdeco *b )
  \brief Parse the tree of descriptors without expand the replicators
  \param b Pointer to the source struct \ref bufrdeco

  This is the user function to parse the descriptor structure of a BUFR report. This is the first task
  we need to perform after read the bufr report with the aid of \ref bufrdeco_read_bufr function.

  At the end we will get an array of structs \ref bufr_sequence defining the tree

  A sequence layer is needed when parsing expanded descriptor sec3 and sec4

  First bufr_sequence is the sequence of descriptors in sec3 after
  byte 8. This is a bufr_sequence in level 0.

  When a sequence descriptor is found in a layer, the sequence entries found in table D
  form this descriptor is a son bufr_sequence. This son has then a father
  and also can have one or more sons. The index level is incremented by one every step it
  go into decendents.

  And so we go in a recursive way up to the end.

  If success return 0, if something went wrong return 1
*/
int bufrdeco_parse_tree ( struct bufrdeco *b )
{
  bufrdeco_assert ( b != NULL );

  // here we start the parse
  return  bufrdeco_parse_tree_recursive ( b, NULL, NULL );
}

/*!
   \fn int bufrdeco_init(struct bufrdeco *b)
   \brief Inits and allocate memory for a struct \ref bufrdeco
   \param b pointer to the target struct

   This function only must be called once. When finished the function \ref bufrdeco_close must be called to
   free all needed memory

   If succeeded return 0, otherwise 1
*/
int bufrdeco_init ( struct bufrdeco *b )
{
  bufrdeco_assert ( b != NULL );

  // First clear all
  memset ( b, 0, sizeof ( struct bufrdeco ) );

  // Then allocate all needed memory when starting. Further we will need to allocate some more depending on
  // data

  // allocate memory for Tables
  if ( bufrdeco_init_tables ( &b->tables ) )
    {
      snprintf ( b->error, sizeof ( b->error ),"%s(): Cannot allocate space for tables\n", __func__ );
      return 1;
    }

  // allocate memory for expanded tree of descriptors
  if ( bufrdeco_init_expanded_tree ( &b->tree ) )
    {
      snprintf ( b->error, sizeof ( b->error ),"%s(): Cannot allocate space for expanded tree of descriptors\n", __func__ );
      return 1;
    }


  return 0;
}

/*!
   \fn int bufrdeco_reset(struct bufrdeco *b)
   \brief Reset an struct \ref bufrdeco. This is needed when changing to another bufr.
   \param b pointer to the target struct to be resed with another bufrfile

   This function must be called when parsing another bufrfile without calling
   \ref bufrdeco_close and \ref bufrdeco_init. It 

   If succeeded return 0, otherwise 1
*/
int bufrdeco_reset ( struct bufrdeco *b )
{
  struct bufr_tables *tb;
  struct bufr_tables_cache ch;
  
  uint32_t mask;
  bufrdeco_assert ( b != NULL );

  // save the data we do not reset
  memcpy(&ch, &b->cache, sizeof (struct bufr_tables_cache));
  tb = b->tables;
  mask = b->mask;
  
  bufrdeco_free_subset_sequence_data ( & ( b->seq ) );
  bufrdeco_free_compressed_data_references ( & ( b->refs ) );
  bufrdeco_free_expanded_tree ( & ( b->tree ) );
  bufrdeco_free_bitmap_array ( & ( b->bitmap ) );

  memset ( b, 0, sizeof ( struct bufrdeco ) );
  
  // Restore data
  b->mask = mask;
  b->tables = tb;  
  memcpy(&b->cache, &ch, sizeof (struct bufr_tables_cache));

  // allocate memory for expanded tree of descriptors
  if ( bufrdeco_init_expanded_tree ( &b->tree ) )
    {
      snprintf ( b->error, sizeof ( b->error ),"%s(): Cannot allocate space for expanded tree of descriptors\n", __func__ );
      return 1;
    }

  return 0;
}

/*!
  \fn int bufrdeco_close ( struct bufrdeco *b )
  \brief Free all allocated memory. Needed when no more task to do with bufrdeco library
  \param b pointer to the target struct

  This function must be called at the end when no more calls to bufrdeco library is needed

  If succeeded return 0, otherwise 1
*/
int bufrdeco_close ( struct bufrdeco *b )
{
  bufrdeco_assert ( b != NULL );

  // first deallocate all memory
  bufrdeco_free_subset_sequence_data ( & ( b->seq ) );
  bufrdeco_free_compressed_data_references ( & ( b->refs ) );
  bufrdeco_free_expanded_tree ( & ( b->tree ) );
  if (b->mask & BUFRDECO_USE_TABLES_CACHE)
  {
    bufrdeco_free_cache_tables(&(b->cache));
    b->tables = 0;
  }
  else
  {
    bufrdeco_free_tables ( & ( b->tables ) );
  }
  bufrdeco_free_bitmap_array ( & ( b->bitmap ) );
  return 0;
}

/*!
 * \fn int bufrdeco_set_tables_dir( struct bufrdeco *b, char *tables_dir)
 * \brief Sets the directory path for BUFR tables. Needed if it is not any default directories.
 * \param b pointer to the target struct
 * \param tables_dir Source path of tables directory
 *
 * The default directories are '/usr/share/bufr2synop' and '/usr/local/share/bufr2synop'
 *
 * Retuns 1 if problems, 0 otherwise
 */
int bufrdeco_set_tables_dir ( struct bufrdeco *b, char *tables_dir )
{
  bufrdeco_assert_with_return_val ( b != NULL || tables_dir == NULL, 1 );

  strncpy_safe ( b->bufrtables_dir, tables_dir, BUFRDECO_PATH_LENGTH );
  return 0;
}

/*!
  \fn int bufrdeco_get_bufr ( struct bufrdeco *b,  char *filename )
  \brief Read file and try to find a bufr report inserted in. Once found do the same that \ref bufrdeco_read_bufr()
  \param b pointer to struct \ref bufrdeco
  \param filename complete path of BUFR file


  This function does the folowing tasks:
  - Try to find first buffer of bytes begining with 'BUFR' chars and ending with '7777'. This will be considered as a bufr file.
  - Read the file and checks the marks at the begining and end to see wheter is a BUFR file
  - Init the structs and allocate the needed memory for raw data if not done previously
  - Splits and parse the BUFR sections (without expanding descriptors nor parsing data)
  - Reads the needed Table files and store them in memory.

  Returns 0 if all is OK, 1 otherwise
 */
int bufrdeco_get_bufr ( struct bufrdeco *b, char *filename )
{
  bufrdeco_assert_with_return_val ( b != NULL || filename == NULL, 1 );

  return bufrdeco_extract_bufr ( b, filename );
}


/*!
 * \fn int bufrdeco_write_subset_offset_bits ( struct bufrdeco *b, char *filename )
 * \brief Write offset bit array for subsets in a non-compressed bufr
 * \param filename complete path of output file to open
 * \param b pointer to the struct \ref bufrdeco
 */
int bufrdeco_write_subset_offset_bits ( struct bufrdeco *b, char *filename )
{
  FILE *f;

  // assert nice args
  bufrdeco_assert_with_return_val ( b != NULL || filename == NULL, 1 );

  // open the file
  f = fopen ( filename, "w" );
  bufrdeco_assert_with_return_val ( f != NULL, 1 );

  // Read the offsets
  if ( bufr_write_subset_offset_bits ( f, & ( b->offsets ) ) )
    {
      fclose ( f );
      return 1;
    }
  fclose ( f );
  return 0;
}

/*!
 * \fn int bufrdeco_read_subset_offset_bits ( struct bufrdeco *b, char *filename )
 * \brief Write offset bit array for subsets in a non-compressed bufr
 * \param filename complete path of input file to open
 * \param b pointer to the struct \ref bufrdeco
 */
int bufrdeco_read_subset_offset_bits ( struct bufrdeco *b, char *filename )
{
  struct stat st;
  FILE *f;

  // assert nice args
  bufrdeco_assert_with_return_val ( b != NULL || filename == NULL, 1 );

  // silently return if cannot stat the file
  if ( stat ( filename, &st ) < 0 )
    {
      return 1;
    }

  // open the file
  f = fopen ( filename, "r" );
  bufrdeco_assert_with_return_val ( f != NULL, 1 );

  // Read the offsets
  if ( bufr_read_subset_offset_bits ( f, & ( b->offsets ) ) )
    {
      fclose ( f );
      return 1;
    }
  fclose ( f );
  return 0;
}


/*!
 *  \fn struct bufrdeco_subset_sequence_data *bufrdeco_get_target_subset_sequence_data (buf_t nset, struct bufrdeco *b)
 *  \brief Prepare the struct \def bufrdeco to get data from the solicited subset
 *  \param nset index of subset we want to parse and get data. First subset in a BUFR file has index 0.
 *  \param b pointer to the struct \ref bufrdeco
 *
 * If succeeded returns a pointer to the struct \ref bufrdeco_subset_sequence_data with the results for the
 * desired subset,  otherwise returns NULL
 */
struct bufrdeco_subset_sequence_data *bufrdeco_get_target_subset_sequence_data ( buf_t nset, struct bufrdeco *b )
{
  buf_t n = 0;
  bufrdeco_assert ( b != NULL );

  if ( b->sec3.subsets <= nset )
    {
      snprintf ( b->error, sizeof ( b->error ), "%s(): The index of target subset is over the bufr subsets (%d)\n", __func__, b->sec3.subsets );
      return NULL;
    }

  if ( b->sec3.compressed )
    {
#ifdef __DEBUG
      printf ("# Compressed\n");
#endif      
      // case of compressed, we just need the compressed references
      if ( b->refs.nd == 0 )
        {
          // case of compressed data and still not parsed the refs
          if ( bufrdeco_parse_compressed ( & ( b->refs ), b ) )
            {
              return NULL;
            }

        }
    }
  else if ( nset > 0 )
    {
      // In case of not compressed bufr, to parse a subset we need to know the bit offset of the subset data in sec4
      // There are only two ways to know that offset:
      //    1) Parse the data of all prior subsets from n = 0 to (nset - 1).
      //    2) Get the subset bit offset from the struct bufrdeco_subset_bit_offset already stored.
      //       This is only possible if we already parsed this subset in this session (among two bufrdeco_reset() calls)
      //       or if readed the struct from a file
      // In any case the subset bit offset array must be poluted for all n < (nset - 1)

      if ( b->offsets.nr == 0 || b->offsets.ofs[nset] == 0 )
        {
          for ( n = b->state.subset ; n < nset ; n++ )
            {
              // In every call to bufrdeco_decode_data_subset(), b->state.subset is incremented and b->offsets is updated if needed
#ifdef __DEBUG
              printf ("# Go to parse for subset %lu waiting %lu\n", b->state.subset, nset);
#endif              
              bufrdeco_decode_data_subset ( b );
            }
        }
    }
  // Once the previous task is made, we just adjust the subset
  b->state.subset = nset;

  // and now parse and get the desired subset data
#ifdef __DEBUG  
  printf ("# Finally going to target parse for subset %lu\n", b->state.subset);
#endif  
  return bufrdeco_get_subset_sequence_data ( b );
}
