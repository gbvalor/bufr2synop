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
   \file bufrnoaa_io.c
   \brief File with I/O code for bufrnoaa binary
*/
#ifndef CONFIG_H
# include "config.h"
# define CONFIG_H
#endif

#include "bufrnoaa.h"

void print_usage ( void )
{
  print_version();
  printf ( "Usage: \n" );
  printf ( "\n%s -i input_file [-h][-v][-f][-l][-F prefix][-T T2_selection][-O selo][-S sels][-U selu]\n" , OWN);
  printf ( "   -h Print this help\n" );
  printf ( "   -v Print information about build and version\n" );
  printf ( "   -i Input file. Complete input path file for NOAA *.bin bufr archive file\n" );
  printf ( "   -2 Input file is formatted in alternative form: Headers has '#' instead of '*' marks and no sep after '7777'\n");
  printf ( "   -l list the names of reports in input file\n" );
  printf ( "   -f Extract selected reports and write them in files, one per bufr message, as \n" );
  printf ( "      example '20110601213442_ISIE06_SBBR_012100_RRB.bufr'. First field in name is input file timestamp \n" );
  printf ( "      Other fields are from header\n" );
  printf ( "   -F prefix. Builds an archive file with the same format as NOAA one but just with selected messages\n" );
  printf ( "      witgh option  -T. Resulting name is 'prefix_input_filename'\n" );
  printf ( "      If no -F option no archive bin file is created.\n" );
  printf ( "      If no message is selected, the a void file is created.\n" );
  printf ( "      File timestamp is the same than input file\n" );
  printf ( "   -T T2_selection. A string with selection. A character per type (T2 code)\n" );
  printf ( "      'S' = Surface . 'O'= Oceanographic. 'U'= upper air\n" );
  printf ( "      If no -T argument then nothing is selected\n" );
  printf ( "   -S sels. String with selection for A1 when T2='S'\n" );
  printf ( "      By default all A1 are selected\n" );
  printf ( "   -O selo. String with selection for A1 when T2='O'\n" );
  printf ( "      By default all A1 are selected\n" );
  printf ( "   -U sels. String with selection for A1 when T2='U'\n" );
  printf ( "      By default all A1 are selected\n" );
}

/*!
  \fn char *bufrnoaa_get_version(char *version, char *build, char *builder, int *version_major, int *version_minor, int *version_patch)
  \brief Get strings with version information and build date and time
  \param version  pointer to string with version as result if not NULL
  \param build pointer to string with compiler and compilation date and time if not NULL 
  \param builder pointer to string with builder utility. 'cmake' or 'autotools' if not NULL 
  \param version_major pointer to string with version_major component if not NULL
  \param version_minor pointer to string with version_minor component if not NULL
  \param version_patch pointer to string with version_patch component if not NULL
  
  Retuns string pointer \ref version.  
 */
char *bufrnoaa_get_version(char *version, char *build, char *builder, int *version_major, int *version_minor, int *version_patch)
{
  int major = 0, minor = 0, patch = 0;
  char *c;
  
  if (version == NULL)
    return NULL;
  sprintf(version, "%s", VERSION);
  // default values
  sscanf(version, "%d.%d.%d", &major, &minor, &patch);
  if (build)
    {
       c = build;
#ifdef __INTEL_COMPILER
       c += sprintf(build, "using INTEL C compiler icc %s ", __INTEL_COMPILER);
#elifdef __GNUC__ 
       c += sprintf(build, "using GNU C compiler gcc %d.%d.%d ", __GNUC__ , __GNUC_MINOR__ , __GNUC_PATCHLEVEL__);
#endif
       sprintf(c,"at %s %s",__DATE__,__TIME__);
    }

  if (builder)
#ifdef BUILD_USING_CMAKE
    strcpy(builder, "cmake");
#else
    strcpy(builder, "autotools");
#endif    
  if (version_major)
    *version_major = major;
  if (version_minor)
    *version_minor = minor;
  if (version_patch)
    *version_patch = patch;
  return version;  
}

void print_version()
{
   char version[16], build[64], builder[32];

   bufrnoaa_get_version(version, build, builder, NULL, NULL, NULL);
   printf ("%s: Version '%s' built %s and %s.\n" , OWN, VERSION, build, builder);
}

/*!
  \fn int read_args( int _argc, char * _argv[])
  \brief read the arguments from stdio
  \param _argc number of arguments passed
  \param _argv array of arguments

  Returns 1 if succcess, -1 othewise
*/
int read_args ( int _argc, char * _argv[] )
{
  int iopt;

  ENTRADA[0] = '\0';
  SEL[0] = '\0';
  SELS[0] = '\0';
  SELO[0] = '\0';
  SELU[0] = '\0';
  PREFIX[0] =  '\0';
  SELECT = 0;
  INDIVIDUAL = 0;
  COLECT = 0;
  LISTF = 0;
  VERBOSE = 1;
  HEADER_MARK = '*';
  strcpy(FINAL_SEP, SEP);
  
  /*
     Read input options
  */
  while ( ( iopt = getopt ( _argc, _argv, "hv2i:flF:O:qS:T:U:" ) ) !=-1 )
    switch ( iopt )
      {
      case 'i':
        if ( strlen ( optarg ) < 256 )
          strcpy ( ENTRADA, optarg );
        break;
      case '2':
        HEADER_MARK = '#';
        FINAL_SEP[0] = 0;
        break;
      case 'f':
        INDIVIDUAL = 1;
        break;
      case 'q':
        VERBOSE = 0;
        break;
      case 'l':
        LISTF = 1;
        break;
      case 'F':
        if ( strlen ( optarg ) && strlen ( optarg ) < 64 )
          {
            strcpy ( PREFIX, optarg );
            COLECT = 1;
          }
        break;
      case 'T':
        if ( strlen ( optarg ) < 64 )
          {
            strcpy ( SEL, optarg );
            SELECT = 1;
          }
        break;
      case 'S':
        if ( strlen ( optarg ) < 64 )
          {
            strcpy ( SELS, optarg );
          }
        break;
      case 'O':
        if ( strlen ( optarg ) < 64 )
          {
            strcpy ( SELO, optarg );
          }
        break;
      case 'U':
        if ( strlen ( optarg ) < 64 )
          {
            strcpy ( SELU, optarg );
          }
        break;
      case 'v':
          print_version();
          exit ( EXIT_SUCCESS );
        break;  
      case 'h':
      default:
        print_usage();
        exit ( EXIT_SUCCESS );
      }

  if ( ENTRADA[0] == '\0' )
    {
      printf ( "%s: It is needed an input file. Use -i option\n", OWN );
      return -1;
    }
  return 1;
}

