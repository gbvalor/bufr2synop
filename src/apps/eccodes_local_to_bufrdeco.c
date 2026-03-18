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
   \file eccodes_local_to_bufrdeco.c
   \brief Converts local ecCodes BUFR tables to bufrdeco CSV format
*/

#include "bufrdeco.h"

#ifndef CONFIG_H
# include "config.h"
# define CONFIG_H
#endif

#include <dirent.h>
#include <errno.h>
#include <ctype.h>
#include <limits.h>
#include <sys/stat.h>
#include <unistd.h>

const char SELF[] = "eccodes_local_to_bufrdeco";

#define MAX_LINE 8192

/*!
 \srtuct strlist
 \brief Simple dynamic array of strings 
*/
struct strlist
{
  char **v; // Array of strings
  size_t n; // Number of strings currently stored
  size_t cap; // Allocated capacity of the array
};

/*!
 \struct tableb_item
 \brief Represents an item in Table B
*/
struct tableb_item
{
  uint32_t fxy;
  char name[BUFR_TABLEB_NAME_LENGTH]; // Descriptor name
  char unit[32]; // Descriptor unit
  int scale; // Descriptor scale
  int reference; // Descriptor reference value
  int width; // Descriptor width in bits
};

/*!
 \struct tableb_list
 \brief List of Table B items
*/
struct tableb_list
{
  struct tableb_item *v; // Array of Table B items
  size_t n; // Number of items currently stored
  size_t cap; // Allocated capacity of the array
};

struct text_kv
{
  char key[8];
  char val[BUFR_TABLEB_NAME_LENGTH];
};

struct text_map
{
  struct text_kv *v;
  size_t n;
  size_t cap;
};

/*!
 \brief Checks if the given path is a regular file
 \param path The path to check
 \return 1 if the path is a regular file, 0 otherwise
*/
static int is_regular_file ( const char *path )
{
  struct stat st;
  if ( stat ( path, &st ) )
    return 0;
  return S_ISREG ( st.st_mode ) ? 1 : 0;
}

/*!
 \brief Checks if the given path is a directory
 \param path The path to check
 \return 1 if the path is a directory, 0 otherwise
*/
static int is_directory ( const char *path )
{
  struct stat st;
  if ( stat ( path, &st ) )
    return 0;
  return S_ISDIR ( st.st_mode ) ? 1 : 0;
}

/*!
 \brief Trims leading and trailing whitespace from a string
 \param s The string to trim
 \return A pointer to the trimmed string
*/
static char *trim ( char *s )
{
  char *e;
  while ( *s && isspace ( ( unsigned char ) *s ) )
    s++;
  if ( *s == 0 )
    return s;
  e = s + strlen ( s ) - 1;
  while ( e > s && isspace ( ( unsigned char ) *e ) )
    *e-- = 0;
  return s;
}

/*!
 \brief Prints a string as a CSV field
 \param out The output file
 \param s The string to print
*/
static void csv_print_field ( FILE *out, const char *s )
{
  const unsigned char *p = ( const unsigned char * ) s;
  if ( s == NULL || s[0] == '\0' )
    return;
  fputc ( '"', out );
  while ( *p )
    {
      if ( *p == '"' )
        fputc ( '"', out );
      fputc ( *p, out );
      p++;
    }
  fputc ( '"', out );
}

/*!
 \brief Prints a CSV row
 \param out The output file
 \param fields The array of fields
 \param n The number of fields
*/
static void csv_print_row_n ( FILE *out, const char **fields, size_t n )
{
  size_t i;
  for ( i = 0; i < n; i++ )
    {
      if ( i )
        fputc ( ',', out );
      csv_print_field ( out, fields[i] );
    }
  fputc ( '\n', out );
}

/*!
 \brief Parses an unsigned integer from a string
 \param out Pointer to store the parsed value
 \param s The string to parse
 \return 0 on success, 1 on failure
*/
static int parse_u32 ( uint32_t *out, const char *s )
{
  char *end;
  unsigned long v;
  errno = 0;
  v = strtoul ( s, &end, 10 );
  if ( errno || end == s || *end )
    return 1;
  if ( v > 999999UL )
    return 1;
  *out = ( uint32_t ) v;
  return 0;
}

/*!
 \brief Parses an integer from a string
 \param out Pointer to store the parsed value
 \param s The string to parse
 \return 0 on success, 1 on failure
*/
static int parse_int_value ( int *out, const char *s )
{
  char *end;
  long v;
  errno = 0;
  v = strtol ( s, &end, 10 );
  if ( errno || end == s || *end )
    return 1;
  if ( v < INT_MIN || v > INT_MAX )
    return 1;
  *out = ( int ) v;
  return 0;
}

/*!
 \brief Splits a string by a given character
 \param line The string to split
 \param sep The separator character
 \param out Array to store the split parts
 \param max_out Maximum number of parts to store
 \return The number of parts found
*/
static int split_by_char ( char *line, char sep, char **out, int max_out )
{
  int n = 0;
  char *p = line;
  if ( max_out <= 0 )
    return 0;

  out[n++] = p;
  while ( *p && n < max_out )
    {
      if ( *p == sep )
        {
          *p = 0;
          out[n++] = p + 1;
        }
      p++;
    }
  return n;
}

/*!
 \brief Ensures that a strlist has enough capacity
 \param l The strlist
 \param need The required capacity
 \return 0 on success, 1 on failure
*/
static int ensure_strlist_cap ( struct strlist *l, size_t need )
{
  char **nv;
  size_t ncap = l->cap ? l->cap * 2 : 64;
  while ( ncap < need )
    ncap *= 2;
  nv = ( char ** ) realloc ( l->v, ncap * sizeof ( char * ) );
  if ( nv == NULL )
    return 1;
  l->v = nv;
  l->cap = ncap;
  return 0;
}

/*!
 \brief Pushes a copy of a string to a strlist
 \param l The strlist
 \param s The string to push
 \return 0 on success, 1 on failure
*/
static int strlist_push_copy ( struct strlist *l, const char *s )
{
  char *cp;
  if ( l->n + 1 > l->cap && ensure_strlist_cap ( l, l->n + 1 ) )
    return 1;
  cp = strdup ( s );
  if ( cp == NULL )
    return 1;
  l->v[l->n++] = cp;
  return 0;
}

/*!
 \brief Frees the memory used by a strlist
 \param l The strlist to free
*/
static void strlist_free ( struct strlist *l )
{
  size_t i;
  if ( l == NULL )
    return;
  for ( i = 0; i < l->n; i++ )
    free ( l->v[i] );
  free ( l->v );
  l->v = NULL;
  l->n = 0;
  l->cap = 0;
}

/*!
 \brief Ensures that a tableb_list has enough capacity
 \param l The tableb_list
 \param need The required capacity
 \return 0 on success, 1 on failure
*/
static int ensure_tableb_cap ( struct tableb_list *l, size_t need )
{
  struct tableb_item *nv;
  size_t ncap = l->cap ? l->cap * 2 : 256;
  while ( ncap < need )
    ncap *= 2;
  nv = ( struct tableb_item * ) realloc ( l->v, ncap * sizeof ( struct tableb_item ) );
  if ( nv == NULL )
    return 1;
  l->v = nv;
  l->cap = ncap;
  return 0;
}

/*!
 \brief Pushes a tableb_item to a tableb_list
 \param l The tableb_list
 \param it The tableb_item to push
 \return 0 on success, 1 on failure
*/
static int tableb_push ( struct tableb_list *l, const struct tableb_item *it )
{
  if ( l->n + 1 > l->cap && ensure_tableb_cap ( l, l->n + 1 ) )
    return 1;
  l->v[l->n++] = *it;
  return 0;
}

/*!
 \brief Compares two tableb_item structures
 \param a Pointer to the first tableb_item
 \param b Pointer to the second tableb_item
 \return -1 if a < b, 1 if a > b, 0 if equal
*/
static int cmp_tableb_item ( const void *a, const void *b )
{
  const struct tableb_item *ia = ( const struct tableb_item * ) a;
  const struct tableb_item *ib = ( const struct tableb_item * ) b;
  if ( ia->fxy < ib->fxy )
    return -1;
  if ( ia->fxy > ib->fxy )
    return 1;
  return 0;
}

/*!
 \brief Compares two strings
 \param a Pointer to the first string
 \param b Pointer to the second string
 \return -1 if a < b, 1 if a > b, 0 if equal
*/
static int cmp_strptr ( const void *a, const void *b )
{
  const char *sa = * ( const char * const * ) a;
  const char *sb = * ( const char * const * ) b;
  return strcmp ( sa, sb );
}

static int ensure_text_map_cap ( struct text_map *m, size_t need )
{
  struct text_kv *nv;
  size_t ncap = m->cap ? m->cap * 2 : 256;
  while ( ncap < need )
    ncap *= 2;
  nv = ( struct text_kv * ) realloc ( m->v, ncap * sizeof ( struct text_kv ) );
  if ( nv == NULL )
    return 1;
  m->v = nv;
  m->cap = ncap;
  return 0;
}

static const char *text_map_find ( const struct text_map *m, const char *key )
{
  size_t i;
  if ( m == NULL || key == NULL )
    return NULL;
  for ( i = 0; i < m->n; i++ )
    {
      if ( strcmp ( m->v[i].key, key ) == 0 )
        return m->v[i].val;
    }
  return NULL;
}

static int text_map_put_if_absent ( struct text_map *m, const char *key, const char *val )
{
  if ( key == NULL || val == NULL || key[0] == 0 || val[0] == 0 )
    return 0;
  if ( text_map_find ( m, key ) != NULL )
    return 0;
  if ( m->n + 1 > m->cap && ensure_text_map_cap ( m, m->n + 1 ) )
    return 1;
  strncpy_safe ( m->v[m->n].key, key, sizeof ( m->v[m->n].key ) );
  strncpy_safe ( m->v[m->n].val, val, sizeof ( m->v[m->n].val ) );
  m->n++;
  return 0;
}

static void text_map_free ( struct text_map *m )
{
  if ( m == NULL )
    return;
  free ( m->v );
  m->v = NULL;
  m->n = 0;
  m->cap = 0;
}

static const char *local_tableb_name_from_key ( const struct tableb_list *tableb, const char *key )
{
  uint32_t fxy;
  size_t i;
  if ( tableb == NULL || key == NULL )
    return NULL;
  if ( parse_u32 ( &fxy, key ) )
    return NULL;
  for ( i = 0; i < tableb->n; i++ )
    {
      if ( tableb->v[i].fxy == fxy )
        return tableb->v[i].name;
    }
  return NULL;
}

static void build_wmo_tableb_name ( char *target, size_t ntarget, const char *name, const char *note )
{
  if ( name == NULL )
    {
      target[0] = 0;
      return;
    }
  if ( note && note[0] )
    snprintf ( target, ntarget, "%s %s", name, note );
  else
    strncpy_safe ( target, name, ntarget );
}

static int load_wmo_tableb_names ( const char *path, struct text_map *desc_name_map )
{
  FILE *f;
  char l[CSV_MAXL], caux[BUFR_TABLEB_NAME_LENGTH];

  if ( ! is_regular_file ( path ) )
    return 0;

  if ( ( f = fopen ( path, "r" ) ) == NULL )
    return 1;

  if ( fgets ( l, sizeof ( l ), f ) == NULL )
    {
      fclose ( f );
      return 0;
    }

  while ( fgets ( l, sizeof ( l ), f ) )
    {
      char *tk[16];
      int nt;
      const char *key, *name, *note;
      if ( parse_csv_line ( &nt, tk, l ) < 0 || nt != 7 )
        continue;
      key = trim ( tk[0] );
      name = trim ( tk[1] );
      note = trim ( tk[2] );
      if ( key[0] == 0 || name[0] == 0 )
        continue;
      build_wmo_tableb_name ( caux, sizeof ( caux ), name, note );
      if ( text_map_put_if_absent ( desc_name_map, key, caux ) )
        {
          fclose ( f );
          return 1;
        }
    }

  fclose ( f );
  return 0;
}

static int load_wmo_tabled_info ( const char *path, struct text_map *seq_title_map, struct text_map *desc_name_map )
{
  FILE *f;
  char l[CSV_MAXL];

  if ( ! is_regular_file ( path ) )
    return 0;

  if ( ( f = fopen ( path, "r" ) ) == NULL )
    return 1;

  if ( fgets ( l, sizeof ( l ), f ) == NULL )
    {
      fclose ( f );
      return 0;
    }

  while ( fgets ( l, sizeof ( l ), f ) )
    {
      char *tk[16];
      int nt;
      const char *k1, *k2;
      const char *title, *ename;
      if ( parse_csv_line ( &nt, tk, l ) < 0 || ( nt != 4 && nt != 2 ) )
        continue;
      k1 = trim ( tk[0] );
      k2 = trim ( tk[1] );
      if ( nt == 4 )
        {
          title = trim ( tk[2] );
          ename = trim ( tk[3] );
        }
      else
        {
          title = "";
          ename = "";
        }

      if ( title[0] && text_map_put_if_absent ( seq_title_map, k1, title ) )
        {
          fclose ( f );
          return 1;
        }

      if ( ename[0] && text_map_put_if_absent ( desc_name_map, k2, ename ) )
        {
          fclose ( f );
          return 1;
        }
    }

  fclose ( f );
  return 0;
}

/*!
 \brief Returns 1 if token should remain uppercase as acronym
 \param token Uppercase token
 \return 1 if acronym, 0 otherwise
*/
static int is_known_acronym ( const char *token )
{
  static const char *acr[] = {
    "ICAO", "WMO", "BUFR", "CREX", "NOAA", "SYNOP", "SHIP", "TEMP", "CLIMAT",
    "GPS", "GNSS", "VHF", "UHF", "NWP", "GTS", "QNH", "QFE", "QFF", "IA5",
    "DRR", "SPECI", "METAR", "AMDAR", "ACARS", "RADAR", "LIDAR", "SAR",
    "ASCAT", "AIRS", "IASI", "AMSU", "AVHRR", "GOES", "CFOSAT", "ERS"
  };
  size_t i;

  for ( i = 0; i < sizeof ( acr ) / sizeof ( acr[0] ); i++ )
    {
      if ( strcmp ( token, acr[i] ) == 0 )
        return 1;
    }
  return 0;
}

/*!
 \brief Normalizes element names to title case while preserving acronyms
 \param src Source string
 \param dst Destination string
 \param ndst Destination max length
*/
static void normalize_element_name_style ( const char *src, char *dst, size_t ndst )
{
  size_t i = 0, o = 0;
  int first_word_written = 0;

  if ( ndst == 0 )
    return;
  dst[0] = 0;
  if ( src == NULL )
    return;

  while ( src[i] && o + 1 < ndst )
    {
      if ( isalnum ( ( unsigned char ) src[i] ) )
        {
          char token[64];
          char up[64];
          size_t t = 0, j;

          while ( src[i] && isalnum ( ( unsigned char ) src[i] ) )
            {
              if ( t + 1 < sizeof ( token ) )
                token[t++] = src[i];
              i++;
            }
          token[t] = 0;

          for ( j = 0; j <= t; j++ )
            up[j] = ( char ) toupper ( ( unsigned char ) token[j] );

          if ( strpbrk ( up, "0123456789" ) || is_known_acronym ( up ) )
            {
              for ( j = 0; up[j] && o + 1 < ndst; j++ )
                dst[o++] = up[j];
              first_word_written = 1;
            }
          else
            {
              if ( ! first_word_written )
                {
                  dst[o++] = ( char ) toupper ( ( unsigned char ) token[0] );
                  for ( j = 1; token[j] && o + 1 < ndst; j++ )
                    dst[o++] = ( char ) tolower ( ( unsigned char ) token[j] );
                }
              else
                {
                  for ( j = 0; token[j] && o + 1 < ndst; j++ )
                    dst[o++] = ( char ) tolower ( ( unsigned char ) token[j] );
                }
              first_word_written = 1;
            }
        }
      else
        {
          dst[o++] = src[i++];
        }
    }
  dst[o] = 0;
}

/*!
 \brief Derives local version, centre, and subcentre from the working directory path
 \param workdir The working directory path
 \param local_version Buffer to store the derived local version
 \param nlocal Size of the local_version buffer
 \param centre Buffer to store the derived centre
 \param ncentre Size of the centre buffer
 \param subcentre Buffer to store the derived subcentre
 \param nsub Size of the subcentre buffer
 \return 0 on success, 1 on failure
*/
static int derive_ids_from_workdir ( const char *workdir, char *local_version, size_t nlocal,
                                     char *centre, size_t ncentre, char *subcentre, size_t nsub )
{
  char path[PATH_MAX];
  char *p, *parts[3];
  int i;

  if ( strlen ( workdir ) >= sizeof ( path ) )
    return 1;
  strcpy ( path, workdir );
  p = path;
  while ( *p )
    {
      if ( *p == '\\' )
        *p = '/';
      p++;
    }

  i = 0;
  p = strrchr ( path, '/' );
  while ( p && i < 3 )
    {
      parts[i++] = p + 1;
      *p = 0;
      p = strrchr ( path, '/' );
    }

  if ( i < 3 )
    return 1;

  strncpy_safe ( subcentre, parts[0], nsub );
  strncpy_safe ( centre, parts[1], ncentre );
  strncpy_safe ( local_version, parts[2], nlocal );
  return 0;
}

/*!
 \brief Parses a table file and fills a tableb_list
 \param path The path to the table file
 \param tableb Pointer to the tableb_list to fill
 \return 0 on success, 1 on failure
*/
static int parse_element_table ( const char *path, struct tableb_list *tableb )
{
  FILE *f;
  char line[MAX_LINE];
  int line_no = 0;

  if ( ( f = fopen ( path, "r" ) ) == NULL )
    {
      fprintf ( stderr, "%s: Cannot open '%s'\n", SELF, path );
      return 1;
    }

  while ( fgets ( line, sizeof ( line ), f ) )
    {
      char *tk[16];
      int nt;
      struct tableb_item it;
      char *s;
      line_no++;

      s = trim ( line );
      if ( s[0] == 0 || s[0] == '#' )
        continue;

      nt = split_by_char ( s, '|', tk, 16 );
      if ( nt < 8 )
        continue;

      memset ( &it, 0, sizeof ( it ) );

      tk[0] = trim ( tk[0] );
      tk[3] = trim ( tk[3] );
      tk[4] = trim ( tk[4] );
      tk[5] = trim ( tk[5] );
      tk[6] = trim ( tk[6] );
      tk[7] = trim ( tk[7] );

      if ( parse_u32 ( &it.fxy, tk[0] ) )
        continue;

      normalize_element_name_style ( tk[3], it.name, sizeof ( it.name ) );
      if ( tk[4][0] )
        {
          strncpy_safe ( it.unit, tk[4], sizeof ( it.unit ) );
        }
      else
        {
          strcpy_safe ( it.unit, "Numeric" );
        }

      if ( parse_int_value ( &it.scale, tk[5] ) )
        it.scale = 0;
      if ( parse_int_value ( &it.reference, tk[6] ) )
        it.reference = 0;
      if ( parse_int_value ( &it.width, tk[7] ) )
        it.width = 0;

      if ( tableb_push ( tableb, &it ) )
        {
          fclose ( f );
          fprintf ( stderr, "%s: No memory pushing tableB item at line %d\n", SELF, line_no );
          return 1;
        }
    }

  fclose ( f );
  if ( tableb->n )
    qsort ( tableb->v, tableb->n, sizeof ( tableb->v[0] ), cmp_tableb_item );
  return 0;
}


/*!
 \brief Collects codetable files from a directory
 \param codetables_dir The directory containing codetable files
 \param files Pointer to the strlist to fill with file paths
 \return 0 on success, 1 on failure
*/
static int collect_codetable_files ( const char *codetables_dir, struct strlist *files )
{
  DIR *d;
  const struct dirent *de;

  if ( ( d = opendir ( codetables_dir ) ) == NULL )
    {
      fprintf ( stderr, "%s: Cannot open directory '%s'\n", SELF, codetables_dir );
        return 1;
    }

  while ( ( de = readdir ( d ) ) != NULL )
    {
      char path[PATH_MAX];
      size_t n;
      const char *name = de->d_name;

      n = strlen ( name );
      if ( n < 7 || strcmp ( name + n - 6, ".table" ) )
        continue;

      if ( snprintf ( path, sizeof ( path ), "%s/%s", codetables_dir, name ) >= ( int ) sizeof ( path ) )
        {
          closedir ( d );
          fprintf ( stderr, "%s: Path too long for codetable file\n", SELF );
          return 1;
        }

      if ( ! is_regular_file ( path ) )
        continue;

      if ( strlist_push_copy ( files, path ) )
        {
          closedir ( d );
          fprintf ( stderr, "%s: No memory collecting codetable files\n", SELF );
          return 1;
        }
    }

  closedir ( d );
  if ( files->n )
    qsort ( files->v, files->n, sizeof ( files->v[0] ), cmp_strptr );
  return 0;
}

/*!
 \brief Derives the descriptor from a codetable filename
 \param fxy Pointer to store the derived descriptor
 \param path The path to the codetable file
 \return 0 on success, 1 on failure
*/
static int descriptor_from_codetable_filename ( uint32_t *fxy, const char *path )
{
  const char *base = strrchr ( path, '/' );
  char name[32];
  size_t i, j;
  if ( base )
    base++;
  else
    base = path;

  for ( i = 0, j = 0; base[i] && base[i] != '.' && j < sizeof ( name ) - 1; i++ )
    {
      if ( ! isdigit ( ( unsigned char ) base[i] ) )
        return 1;
      name[j++] = base[i];
    }
  name[j] = 0;

  if ( j == 0 )
    return 1;
  if ( j > 6 )
    name[6] = 0;

  return parse_u32 ( fxy, name );
}

/*!
 \brief Writes a tableb_list to a CSV file
 \param[in] path The path to the CSV file
 \param[in] tableb Pointer to the tableb_list to write
 \return 0 on success, 1 on failure
*/
static int write_tableb_csv ( const char *path, const struct tableb_list *tableb )
{
  FILE *out;
  size_t i;
  const char *hdr[] = {
    "FXY", "ElementName_en", "Note_en", "BUFR_Unit", "BUFR_Scale", "BUFR_ReferenceValue", "BUFR_DataWidth_Bits"
  };

  if ( ( out = fopen ( path, "w" ) ) == NULL )
    {
      fprintf ( stderr, "%s: Cannot create '%s'\n", SELF, path );
      return 1;
    }

  csv_print_row_n ( out, hdr, 7 );
  for ( i = 0; i < tableb->n; i++ )
    {
      char fxy[8], scale[32], ref[32], width[32];
      const char *row[7];
      snprintf ( fxy, sizeof ( fxy ), "%06u", tableb->v[i].fxy );
      snprintf ( scale, sizeof ( scale ), "%d", tableb->v[i].scale );
      snprintf ( ref, sizeof ( ref ), "%d", tableb->v[i].reference );
      snprintf ( width, sizeof ( width ), "%d", tableb->v[i].width );

      row[0] = fxy;
      row[1] = tableb->v[i].name;
      row[2] = "";
      row[3] = tableb->v[i].unit;
      row[4] = scale;
      row[5] = ref;
      row[6] = width;
      csv_print_row_n ( out, row, 7 );
    }

  fclose ( out );
  return 0;
}

/*!
 \brief Writes a tablec_list to a CSV file
 \param[in] path The path to the CSV file
 \param[in] codetables_dir The directory containing codetable files
 \param[out] nrows Pointer to store the number of rows written
 \return 0 on success, 1 on failure
*/
static int write_tablec_csv ( const char *path, const char *codetables_dir, size_t *nrows )
{
  FILE *out;
  struct strlist files;
  size_t i;
  const char *hdr[] = {
    "FXY", "CodeFigure", "EntryName_en", "EntryName_sub1_en", "EntryName_sub2_en", "Note_en"
  };

  memset ( &files, 0, sizeof ( files ) );
  if ( collect_codetable_files ( codetables_dir, &files ) )
    return 1;

  if ( ( out = fopen ( path, "w" ) ) == NULL )
    {
      fprintf ( stderr, "%s: Cannot create '%s'\n", SELF, path );
      strlist_free ( &files );
      return 1;
    }

  csv_print_row_n ( out, hdr, 6 );
  *nrows = 0;

  for ( i = 0; i < files.n; i++ )
    {
      FILE *f;
      char line[MAX_LINE];
      uint32_t fxy;
      char fxy_s[8];

      if ( descriptor_from_codetable_filename ( &fxy, files.v[i] ) )
        continue;
      snprintf ( fxy_s, sizeof ( fxy_s ), "%06u", fxy );

      if ( ( f = fopen ( files.v[i], "r" ) ) == NULL )
        {
          fprintf ( stderr, "%s: Cannot open '%s'\n", SELF, files.v[i] );
          fclose ( out );
          strlist_free ( &files );
          return 1;
        }

      while ( fgets ( line, sizeof ( line ), f ) )
        {
          char *s, *p, *code, *desc;
          const char *row[6];

          s = trim ( line );
          if ( s[0] == 0 || s[0] == '#' )
            continue;

          code = s;
          while ( *s && ! isspace ( ( unsigned char ) *s ) )
            s++;
          if ( *s == 0 )
            continue;
          *s++ = 0;
          while ( *s && isspace ( ( unsigned char ) *s ) )
            s++;
          while ( *s && ! isspace ( ( unsigned char ) *s ) )
            s++;
          if ( *s )
            {
              *s++ = 0;
              while ( *s && isspace ( ( unsigned char ) *s ) )
                s++;
            }
          desc = s;

          p = code;
          while ( *p )
            {
              if ( ! isdigit ( ( unsigned char ) *p ) )
                break;
              p++;
            }
          if ( *p )
            continue;

          row[0] = fxy_s;
          row[1] = code;
          row[2] = trim ( desc );
          row[3] = "";
          row[4] = "";
          row[5] = "";
          csv_print_row_n ( out, row, 6 );
          ( *nrows )++;
        }
      fclose ( f );
    }

  fclose ( out );
  strlist_free ( &files );
  return 0;
}

/*!
 \brief Writes a tabled_list to a CSV file
 \param[in] path The path to the CSV file
 \param[in] sequence_def The path to the sequence definition file
 \param[out] nrows Pointer to store the number of rows written
 \return 0 on success, 1 on failure
*/
static int write_tabled_csv ( const char *path, const char *sequence_def, size_t *nrows,
                              const struct tableb_list *local_tableb,
                              const struct text_map *wmo_seq_title,
                              const struct text_map *wmo_desc_name,
                              const struct text_map *wmo_b_desc_name )
{
  FILE *in, *out;
  char line[MAX_LINE];
  const char *hdr[] = { "FXY1", "FXY2", "Title_en", "ElementName_en" };

  if ( ( in = fopen ( sequence_def, "r" ) ) == NULL )
    {
      fprintf ( stderr, "%s: Cannot open '%s'\n", SELF, sequence_def );
      return 1;
    }

  if ( ( out = fopen ( path, "w" ) ) == NULL )
    {
      fprintf ( stderr, "%s: Cannot create '%s'\n", SELF, path );
      fclose ( in );
      return 1;
    }

  csv_print_row_n ( out, hdr, 4 );
  *nrows = 0;

  while ( fgets ( line, sizeof ( line ), in ) )
    {
      const char *s = trim ( line );
      const char *p, *q1, *q2, *b1, *b2;
      char key[8];

      if ( s[0] == 0 || s[0] == '#' )
        continue;

      q1 = strchr ( s, '"' );
      if ( q1 == NULL )
        continue;
      q2 = strchr ( q1 + 1, '"' );
      if ( q2 == NULL )
        continue;

      if ( ( size_t ) ( q2 - q1 - 1 ) != 6 )
        continue;
      memcpy ( key, q1 + 1, 6 );
      key[6] = 0;

      p = key;
      while ( *p )
        {
          if ( ! isdigit ( ( unsigned char ) *p ) )
            break;
          p++;
        }
      if ( *p )
        continue;

      b1 = strchr ( q2, '[' );
      b2 = strrchr ( q2, ']' );
      if ( b1 == NULL || b2 == NULL || b2 <= b1 )
        continue;

      p = b1 + 1;
      while ( p < b2 )
        {
          const char *row[4];
          char desc[8];
          const char *title = NULL;
          const char *ename = NULL;
          int j = 0;

          while ( p < b2 && ! isdigit ( ( unsigned char ) *p ) )
            p++;
          while ( p < b2 && isdigit ( ( unsigned char ) *p ) && j < 6 )
            desc[j++] = *p++;
          if ( j != 6 )
            {
              while ( p < b2 && isdigit ( ( unsigned char ) *p ) )
                p++;
              continue;
            }
          desc[6] = 0;

          title = text_map_find ( wmo_seq_title, key );
          ename = text_map_find ( wmo_desc_name, desc );

          if ( ( ename == NULL || ename[0] == 0 ) && desc[0] == '0' )
            {
              ename = local_tableb_name_from_key ( local_tableb, desc );
            }

          if ( ( ename == NULL || ename[0] == 0 ) && desc[0] == '0' )
            {
              ename = text_map_find ( wmo_b_desc_name, desc );
            }

          if ( ( ename == NULL || ename[0] == 0 ) && desc[0] == '3' )
            {
              ename = text_map_find ( wmo_seq_title, desc );
            }

          row[0] = key;
          row[1] = desc;
          row[2] = title ? title : "";
          row[3] = ename ? ename : "";
          csv_print_row_n ( out, row, 4 );
          ( *nrows )++;
        }
    }

  fclose ( out );
  fclose ( in );
  return 0;
}

/*!
 \brief Prints usage information
*/
static void print_usage ( void )
{
  printf ( "%s %s\n", SELF, PACKAGE_VERSION );
  printf ( "Usage:\n" );
  printf ( "  %s -w workdir [-o output_dir] [-l local_version -c centre -s subcentre] [-m wmo_share_dir]\n", SELF );
  printf ( "\n" );
  printf ( "Where workdir may contain: element.table, sequence.def and codetables/*.table\n" );
}

/*!
 \brief Main function
    \param[in] argc Argument count
    \param[in] argv Argument vector
    \return EXIT_SUCCESS on success, EXIT_FAILURE on failure
*/
int main ( int argc, char **argv )
{
  int iopt;
  int have_element_table;
  int have_sequence_def;
  int have_codetables_dir;
  int generated_b = 0;
  int generated_c = 0;
  int generated_d = 0;
  char workdir[PATH_MAX] = {0};
  char outdir[PATH_MAX] = {0};
  char wmo_dir[PATH_MAX] = {0};
  char local_version[64] = {0};
  char centre[64] = {0};
  char subcentre[64] = {0};

  char element_table[PATH_MAX], sequence_def[PATH_MAX], codetables_dir[PATH_MAX];
  char wmo_tableb[PATH_MAX], wmo_tabled[PATH_MAX];
  char out_b[PATH_MAX], out_c[PATH_MAX], out_d[PATH_MAX];
  struct tableb_list tableb;
  struct text_map wmo_seq_title, wmo_desc_name, wmo_b_desc_name;
  size_t n_c = 0, n_d = 0;

  memset ( &tableb, 0, sizeof ( tableb ) );
  memset ( &wmo_seq_title, 0, sizeof ( wmo_seq_title ) );
  memset ( &wmo_desc_name, 0, sizeof ( wmo_desc_name ) );
  memset ( &wmo_b_desc_name, 0, sizeof ( wmo_b_desc_name ) );

  while ( ( iopt = getopt ( argc, argv, "hw:o:l:c:s:m:" ) ) != -1 )
    {
      switch ( iopt )
        {
        case 'w':
          strncpy_safe ( workdir, optarg, sizeof ( workdir ) );
          break;
        case 'o':
          strncpy_safe ( outdir, optarg, sizeof ( outdir ) );
          break;
        case 'l':
          strncpy_safe ( local_version, optarg, sizeof ( local_version ) );
          break;
        case 'c':
          strncpy_safe ( centre, optarg, sizeof ( centre ) );
          break;
        case 's':
          strncpy_safe ( subcentre, optarg, sizeof ( subcentre ) );
          break;
        case 'm':
          strncpy_safe ( wmo_dir, optarg, sizeof ( wmo_dir ) );
          break;
        case 'h':
        default:
          print_usage();
          return EXIT_SUCCESS;
        }
    }

  if ( workdir[0] == 0 )
    {
      fprintf ( stderr, "%s: Missing -w workdir\n", SELF );
      print_usage();
      return EXIT_FAILURE;
    }

  if ( outdir[0] == 0 )
    strncpy_safe ( outdir, ".", sizeof ( outdir ) );

  if ( wmo_dir[0] == 0 )
    strncpy_safe ( wmo_dir, "share", sizeof ( wmo_dir ) );

  if ( ! is_directory ( workdir ) )
    {
      fprintf ( stderr, "%s: '%s' is not a directory\n", SELF, workdir );
      return EXIT_FAILURE;
    }

  if ( ! is_directory ( outdir ) )
    {
      fprintf ( stderr, "%s: '%s' is not a directory\n", SELF, outdir );
      return EXIT_FAILURE;
    }

  if ( local_version[0] == 0 || centre[0] == 0 || subcentre[0] == 0 )
    {
      if ( derive_ids_from_workdir ( workdir, local_version, sizeof ( local_version ),
                                     centre, sizeof ( centre ), subcentre, sizeof ( subcentre ) ) )
        {
          fprintf ( stderr, "%s: Cannot infer local_version/centre/subcentre from '%s'. Use -l -c -s\n", SELF, workdir );
          return EXIT_FAILURE;
        }
    }

  if ( snprintf ( element_table, sizeof ( element_table ), "%s/element.table", workdir ) >= ( int ) sizeof ( element_table ) ||
       snprintf ( sequence_def, sizeof ( sequence_def ), "%s/sequence.def", workdir ) >= ( int ) sizeof ( sequence_def ) ||
       snprintf ( codetables_dir, sizeof ( codetables_dir ), "%s/codetables", workdir ) >= ( int ) sizeof ( codetables_dir ) )
    {
      fprintf ( stderr, "%s: Path too long\n", SELF );
      return EXIT_FAILURE;
    }

  have_element_table = is_regular_file ( element_table );
  have_sequence_def = is_regular_file ( sequence_def );
  have_codetables_dir = is_directory ( codetables_dir );

  if ( ! have_element_table )
    {
      fprintf ( stderr, "%s: Missing '%s'. Skipping tableB CSV generation.\n", SELF, element_table );
    }
  if ( ! have_sequence_def )
    {
      fprintf ( stderr, "%s: Missing '%s'. Skipping tableD CSV generation.\n", SELF, sequence_def );
    }
  if ( ! have_codetables_dir )
    {
      fprintf ( stderr, "%s: Missing '%s'. Skipping tableC CSV generation.\n", SELF, codetables_dir );
    }

  if ( snprintf ( out_b, sizeof ( out_b ), "%s/tableB_LOCAL_%s_%s_%s.csv", outdir, local_version, centre, subcentre ) >= ( int ) sizeof ( out_b ) ||
       snprintf ( out_c, sizeof ( out_c ), "%s/tableC_LOCAL_%s_%s_%s.csv", outdir, local_version, centre, subcentre ) >= ( int ) sizeof ( out_c ) ||
       snprintf ( out_d, sizeof ( out_d ), "%s/tableD_LOCAL_%s_%s_%s.csv", outdir, local_version, centre, subcentre ) >= ( int ) sizeof ( out_d ) )
    {
      fprintf ( stderr, "%s: Output path too long\n", SELF );
      return EXIT_FAILURE;
    }

  if ( snprintf ( wmo_tableb, sizeof ( wmo_tableb ), "%s/BUFR_45_0_0_TableB_en.csv", wmo_dir ) >= ( int ) sizeof ( wmo_tableb ) ||
       snprintf ( wmo_tabled, sizeof ( wmo_tabled ), "%s/BUFR_45_0_0_TableD_en.csv", wmo_dir ) >= ( int ) sizeof ( wmo_tabled ) )
    {
      fprintf ( stderr, "%s: WMO path too long\n", SELF );
      return EXIT_FAILURE;
    }

  if ( have_element_table && parse_element_table ( element_table, &tableb ) )
    {
      free ( tableb.v );
      return EXIT_FAILURE;
    }

  if ( have_sequence_def && load_wmo_tableb_names ( wmo_tableb, &wmo_b_desc_name ) )
    {
      fprintf ( stderr, "%s: Error loading '%s'\n", SELF, wmo_tableb );
      free ( tableb.v );
      text_map_free ( &wmo_seq_title );
      text_map_free ( &wmo_desc_name );
      text_map_free ( &wmo_b_desc_name );
      return EXIT_FAILURE;
    }

  if ( have_sequence_def && load_wmo_tabled_info ( wmo_tabled, &wmo_seq_title, &wmo_desc_name ) )
    {
      fprintf ( stderr, "%s: Error loading '%s'\n", SELF, wmo_tabled );
      free ( tableb.v );
      text_map_free ( &wmo_seq_title );
      text_map_free ( &wmo_desc_name );
      text_map_free ( &wmo_b_desc_name );
      return EXIT_FAILURE;
    }

  if ( have_sequence_def && ( ! is_regular_file ( wmo_tableb ) || ! is_regular_file ( wmo_tabled ) ) )
    {
      fprintf ( stderr, "%s: Warning: WMO reference tables not found in '%s'. TableD text enrichment will be partial.\n", SELF, wmo_dir );
    }

  if ( have_element_table )
    {
      if ( write_tableb_csv ( out_b, &tableb ) )
        {
          free ( tableb.v );
          text_map_free ( &wmo_seq_title );
          text_map_free ( &wmo_desc_name );
          text_map_free ( &wmo_b_desc_name );
          return EXIT_FAILURE;
        }
      generated_b = 1;
    }

  if ( have_codetables_dir )
    {
      if ( write_tablec_csv ( out_c, codetables_dir, &n_c ) )
        {
          free ( tableb.v );
          text_map_free ( &wmo_seq_title );
          text_map_free ( &wmo_desc_name );
          text_map_free ( &wmo_b_desc_name );
          return EXIT_FAILURE;
        }
      generated_c = 1;
    }

  if ( have_sequence_def )
    {
      if ( write_tabled_csv ( out_d, sequence_def, &n_d, &tableb, &wmo_seq_title, &wmo_desc_name, &wmo_b_desc_name ) )
        {
          free ( tableb.v );
          text_map_free ( &wmo_seq_title );
          text_map_free ( &wmo_desc_name );
          text_map_free ( &wmo_b_desc_name );
          return EXIT_FAILURE;
        }
      generated_d = 1;
    }

  if ( generated_b )
    {
      printf ( "Generated: %s\n", out_b );
      printf ( "TableB elements: %zu\n", tableb.n );
    }
  if ( generated_c )
    {
      printf ( "Generated: %s\n", out_c );
      printf ( "TableC entries:  %zu\n", n_c );
    }
  if ( generated_d )
    {
      printf ( "Generated: %s\n", out_d );
      printf ( "TableD entries:  %zu\n", n_d );
    }
  if ( ! generated_b && ! generated_c && ! generated_d )
    {
      printf ( "No CSV files were generated.\n" );
    }

  free ( tableb.v );
  text_map_free ( &wmo_seq_title );
  text_map_free ( &wmo_desc_name );
  text_map_free ( &wmo_b_desc_name );
  return EXIT_SUCCESS;
}
