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
 \file bufrtotac_io.c
 \brief file with the i/o code for binary bufrtotac
 */
#ifndef CONFIG_H
# include "config.h"
# define CONFIG_H
#endif

#include "bufrtotac.h"

void bufrtotac_print_usage ( void )
{
  bufrtotac_print_version ();
  printf ( "\nUsage: \n" );
  printf ( "%s -i input_file [-i input] [-I list_of_files] [-t bufrtable_dir] [-o output] [-s] [-v][-j][-x][-X][-c][-h][more optional args....]\n", SELF );
  printf ( "       -c. The output is in csv format\n" );
  printf ( "       -D debug level. 0 = No debug, 1 = Debug, 2 = Verbose debug (default = 0)\n" );
  printf ( "       -E. Print expanded tree in json format\n" );
  printf ( "       -G. Print latitude, logitude and altitude \n" );
  printf ( "       -g. Print WIGOS ID\n" );
  printf ( "       -h Print this help\n" );
  printf ( "       -i Input file. Complete input path file for bufr file\n" );
  printf ( "       -I list_of_files. Pathname of a file with the list of files to parse, one filename per line\n" );
  printf ( "       -j. The output is in json format\n" );
  printf ( "       -J. Output expanded subset SEC 4 data in json format\n");
  printf ( "       -n. Do not try to decode to TAC, just parse BUFR report\n" );
  printf ( "       -o output. Pathname of output file. Default is standar output\n" );
  printf ( "       -R. Read bit_offsets file if exists. The path of these files is to add '.offs' to the name of input BUFR file\n");
  printf ( "       -s prints a long output with explained sequence of descriptors\n" );
  printf ( "       -S first..last . Print only results for subsets in range first..last (First subset available is 0). Default is all subsets\n" );
  printf ( "       -t bufrtable_dir. Pathname of bufr tables directory. Ended with '/'\n" );
  printf ( "       -T. Use cache of tables to optimize execution time\n");
  printf ( "       -W. Write bit_offsets file. The path of these files is to add '.offs' to the name of input BUFR file\n");
  printf ( "       -V. Verbose output\n" );
  printf ( "       -v. Print version\n" );
  printf ( "       -x. The output is in xml format\n" );
  printf ( "       -X. Try to extract an embebed bufr in a file seraching for a first '7777' after first 'BUFR'\n" );
  printf ( "       -0. Prints BUFR Sec 0 information in json format\n");
  printf ( "       -1. Prints BUFR Sec 1 information in json format\n");
  printf ( "       -2. Prints BUFR Sec 2 information in json format\n");
  printf ( "       -3. Prints BUFR Sec 3 information in json format\n");
}

/*!
  \fn char *bufrtotac_get_version(char *version, char *build, char *builder, int *version_major, int *version_minor, int *version_patch)
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
char *bufrtotac_get_version(char *version, size_t dversion, char *build, size_t dbuild, char *builder, size_t dbuilder, 
                           int *version_major, int *version_minor, int *version_patch)
{
  int major = 0, minor = 0, patch = 0;
  size_t used;

  if (version == NULL)
    return NULL;
  snprintf(version, dversion, "%s", VERSION);
  // default  
  sscanf(version, "%d.%d.%d", &major, &minor, &patch);

  if (build)
    {
       used = 0;
#if defined(__INTEL_COMPILER)
       used += snprintf(build + used, dbuild - used, "using INTEL C compiler icc %d.%d ", __INTEL_COMPILER, __INTEL_COMPILER_UPDATE);
#elif defined(__clang_version__) 
       used += snprintf(build + used, dbuild - used, "using clang C compiler %s ", __clang_version__);
#elif defined(__GNUC__) 
       used += snprintf(build + used, dbuild - used, "using GNU C compiler gcc %d.%d.%d ", __GNUC__ , __GNUC_MINOR__ , __GNUC_PATCHLEVEL__);
#elif defined(_MSC_VER) 
       used += snprintf(build + used, dbuild - used, "using MICROSOFT C compiler %d ", _MSC_VER);
#else
       used += snprintf(build + used, dbuild - used, "using an unknown C compiler ");
#endif
       snprintf(build + used, dbuild - used,"at %s %s",__DATE__,__TIME__);
    }
    
  if (builder)
#ifdef BUILD_USING_CMAKE
    strncpy(builder, "cmake", dbuilder);
#else
    strncpy(builder, "autotools", dbuilder);
#endif    
  if (version_major)
    *version_major = major;
  if (version_minor)
    *version_minor = minor;
  if (version_patch)
    *version_patch = patch;
  return version;  
}


void bufrtotac_print_version()
{
  char version[16], build[128], builder[32];

  bufrtotac_get_version ( version, sizeof (version), build, sizeof (build), builder, sizeof (builder), NULL, NULL, NULL );
  printf ( "%s: Version '%s' built %s and %s.\n", SELF, VERSION, build, builder );
  bufr2tac_get_version ( version, sizeof (version), build, sizeof (build), builder, sizeof (builder), NULL, NULL, NULL );
  printf ( "Linked to bufr2tac library version '%s' built %s and %s.\n", version, build, builder );
  bufrdeco_get_version ( version, sizeof (version), build, sizeof (build), builder, sizeof (builder), NULL, NULL, NULL );
  printf ( "Linked to bufrdeco library version '%s' built %s and %s.\n", version, build, builder );
}


/*!
  \fn int read_args( int _argc, char * _argv[])
  \brief read the arguments from stdio
  \param _argc number of arguments passed
  \param _argv array of arguments

  Returns 1 if succcess, -1 othewise
*/
int bufrtotac_read_args ( int _argc, char * _argv[] )
{
  int iopt;
  char *c;
  char aux[128];

  // Initial and default values
  INPUTFILE[0] = '\0';
  OUTPUTFILE[0] = '\0';
  LISTOFFILES[0] = '\0';
  BUFRTABLES_DIR[0] = '\0';
  VERBOSE = 0;
  SHOW_SEQUENCE = 0;
  DEBUG = 0;
  NFILES = 0;
  XML = 0;
  JSON = 0;
  CSV= 0;
  EXTRACT = 0;
  HTML = 0;
  NOTAC = 0;
  FIRST_SUBSET = 0;
  LAST_SUBSET = -1;
  PRINT_WIGOS_ID = 0;
  READ_OFFSETS = 0;
  WRITE_OFFSETS = 0;
  USE_CACHE = 0;
  PRINT_JSON_DATA = 0;
  PRINT_JSON_SEC1 = 0;
  PRINT_JSON_SEC2 = 0;
  PRINT_JSON_SEC3 = 0;
  PRINT_JSON_EXPANDED_TREE = 0;
  
  /*
     Read input options
  */
  while ( ( iopt = getopt ( _argc, _argv, "cD:Ehi:jJHI:no:S:st:TvgGVWRxX0123" ) ) !=-1 )
    switch ( iopt )
      {
      case 'i':
        if ( strlen ( optarg ) < 256 )
        {
          strcpy ( INPUTFILE, optarg );
          snprintf( OFFSETFILE, sizeof (OFFSETFILE), "%s.offs", INPUTFILE);
        }
        break;
        
      case 'E':
        PRINT_JSON_EXPANDED_TREE = 1;
        break;

      case 'j':
        JSON = 1;
        break;
        
      case 'J':
        PRINT_JSON_DATA = 1;
        break;
        
      case 'I':
        if ( strlen ( optarg ) < 256 )
          strcpy ( LISTOFFILES, optarg );
        break;
        
      case 'n':
        NOTAC = 1;
        break;
        
      case 'o':
        if ( strlen ( optarg ) < 256 )
          strcpy ( OUTPUTFILE, optarg );
        break;
        
      case 't':
        if ( strlen ( optarg ) < 256 )
          {
            strcpy ( BUFRTABLES_DIR, optarg );
          }
        break;
        
      case 'T':
        USE_CACHE = 1;
        break;

      case 'D':
        if ( strlen ( optarg ) < 2 )
          {
            DEBUG = atoi ( optarg );
            if (DEBUG < 3)
             bufr2tac_set_debug_level ( DEBUG ); // set debug level to bufr2tac library
          }
        break;
        
      case 'H':
        HTML = 1;
        break;
        
      case 'V':
        VERBOSE = 1;
        break;
        
      case 'v':
        bufrtotac_print_version();
        exit ( EXIT_SUCCESS );
        break;
        
      case 's':
        SHOW_SEQUENCE = 1;
        break;
        
      case 'S':
        // Process subset sequence to show
        if ( strlen ( optarg ) < 128 &&
             strspn ( optarg, "0123456789." ) == strlen ( optarg ) )
          {
            if ( strstr ( optarg, ".." ) != NULL )
              {
                strcpy ( aux, optarg );
                c = strstr ( aux, ".." );
                if ( strlen ( c ) > 2 )
                  {
                    LAST_SUBSET = atoi ( c + 2 );
                  }
                *c = '\0';
                if ( aux[0] )
                  {
                    FIRST_SUBSET = atoi ( aux );
                  }
              }
            else
              {
                FIRST_SUBSET = atoi ( optarg );
                LAST_SUBSET = FIRST_SUBSET;
              }
          }
        break;
        
      case 'x':
        XML = 1;
        break;
        
      case 'c':
        CSV = 1;
        break;
        
      case 'X':
        EXTRACT = 1;
        break;
        
      case 'g':
        PRINT_WIGOS_ID = 1;
        break;
        
      case 'G':
        PRINT_GEO = 1;
        break;
        
      case 'W':
        WRITE_OFFSETS = 1;
        break;
        
      case 'R':
         READ_OFFSETS = 1;
         break;
         
      case '0':
         PRINT_JSON_SEC0 = 1;
         break;

      case '1':
         PRINT_JSON_SEC1 = 1;
         break;
         
      case '2':
         PRINT_JSON_SEC2 = 1;
         break;
         
      case '3':
         PRINT_JSON_SEC3 = 1;
         break;
         
      case 'h':
      default:
        bufrtotac_print_usage();
        exit ( EXIT_SUCCESS );
      }

  if ( INPUTFILE[0] == 0 && LISTOFFILES[0] == 0 )
    {
      printf ( "read_args(): It is needed an input file. Use -i or -I option\n" );
      bufrtotac_print_usage();
      return -1;
    }
    
  // Open oututfile if needed or stdout
  if (OUTPUTFILE[0])
  {
    if ((OUT = fopen (OUTPUTFILE,"w")) == NULL)
    {
      printf ( "%s(): Cannot open file %s to write to\n", __func__ , OUTPUTFILE);
      bufrtotac_print_usage();
      return -1;
    }
  }
  else
    OUT = stdout;
  
  return 1;
}

/* this is an interface to use bufr2tac */
int bufrtotac_parse_subset_sequence ( struct metreport *m, struct bufr2tac_subset_state *st,
                                      struct bufrdeco *b, char *err )
{
  size_t i;
  int ksec1[40], res;
  int *kdtlst;
  size_t nlst = ( size_t ) b->sec3.ndesc;

  // memory for kdtlst
  if ( ( kdtlst = ( int * ) calloc ( 1, nlst * sizeof ( int ) ) ) == NULL )
    {
      sprintf ( err, "decobufr_parse_subset_sequence(): cannot alloc memory for kdtlst\n" );
      return 1;
    }

  // sets descriptor as integer according to ECMWF
  for ( i = 0; i < nlst ; i++ )
    {
      descriptor_to_integer ( &kdtlst[i], &b->sec3.unexpanded[i] );
    }

  // And now set only used ksec1 elements
  ksec1[5] = b->sec1.category;
  ksec1[6] = b->sec1.subcategory_local;

  // Finaly we call to bufr2tac library
  bufr2tac_clean_metreport( m );
  
  // Set the subset being parsed
  m->subset = SUBSET;
  
  if (PRINT_WIGOS_ID)
    m->print_mask |= PRINT_BITMASK_WIGOS;
  
  if (PRINT_GEO)
    m->print_mask |= PRINT_BITMASK_GEO;
  
  
  m->h = &b->header;
  res = parse_subset_sequence ( m, &b->seq, st, kdtlst, nlst, ksec1, err );
  free ( ( void * ) kdtlst );
  return res;
}

/*!
  \fn char * get_bufrfile_path( char *filename, char *fileoffset, *err)
  \brief Get bufr file names to parse
  \param filename string where the file is on output
  \param err string where to set the error if any

  If success, it returns a pointer to filename. If there is no more files
  to parse, or error returns NULL.

  This routine is also opens the file with file list and close it when no more
  file available in list

  The content format in LISTOFFILES is compatible with 'ls -1'. Example assume we
  want to parse all *.bufr files in current directory, we can do

  \code
  ls -1 *.bufr > list_file && bufrtotac -I list_file
  \endcode
*/
char * get_bufrfile_path ( char *filename, char *fileoffset, char *err )
{
  char aux[256];
  if ( LISTOFFILES[0] == 0 )
    {
      if ( NFILES == 0 )
        {
          if ( filename != INPUTFILE )
            strlcpy ( filename, INPUTFILE, sizeof (INPUTFILE) );
          return filename;
        }
      else
        return 0;
    }
  else if ( NFILES == 0 )
    {
      if ( ( FL = fopen ( LISTOFFILES,"r" ) ) == NULL )
        {
          snprintf ( err, ERR_SIZE, "Cannot open '%s'", LISTOFFILES );
          return NULL;
        }
    }
  if ( fgets ( aux, 256, FL ) )
    {
      char *c =  strrchr ( aux,'\n' );
      if ( c )
        *c = 0;
      strlcpy ( filename, aux, sizeof (aux) );
      snprintf (fileoffset, BUFRDECO_PATH_LENGTH , "%s.offs", filename);
      return filename;
    }
  else
    {
      fclose ( FL );
      return NULL;
    }
}

/*!
 * \fn int bufrtotac_set_bufrdeco_bitmask (struct bufrdeco *b)
 * \brief Set the bufrdeco struct bitmask according with readed args from shell
 * \param b Pointer to struct \ref bufrdeco already inited
 * \return 0 if succeded 
 */
int bufrtotac_set_bufrdeco_bitmask (struct bufrdeco *b)
{
  bufrdeco_assert ( b != NULL);
  
  if ( HTML )
    b->mask |= BUFRDECO_OUTPUT_HTML;
  
  if (USE_CACHE)
    b->mask |= BUFRDECO_USE_TABLES_CACHE;

  if (PRINT_JSON_DATA)
    b->mask |= BUFRDECO_OUTPUT_JSON_SUBSET_DATA;

  if (PRINT_JSON_SEC0)
    b->mask |= BUFRDECO_OUTPUT_JSON_SEC0;

  if (PRINT_JSON_SEC1)
    b->mask |= BUFRDECO_OUTPUT_JSON_SEC1;
  
  if (PRINT_JSON_SEC2)
    b->mask |= BUFRDECO_OUTPUT_JSON_SEC2;

  if (PRINT_JSON_SEC3)
    b->mask |= BUFRDECO_OUTPUT_JSON_SEC3;
  
  if (PRINT_JSON_EXPANDED_TREE)
    b->mask |= BUFRDECO_OUTPUT_JSON_EXPANDED_TREE;
  
  return 0;
}
