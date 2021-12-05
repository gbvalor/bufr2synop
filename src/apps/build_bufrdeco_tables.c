/***************************************************************************
 *   Copyright (C) 2013-2021 by Guillermo Ballester Valor                  *
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
   \file build_bufrdeco_tables.c
   \brief This file includes the code for build_bufrdeco_tables binary

   \a build_bufrdeco_tables is the tool to convert original BUFR files from ECMWF package
   and WMO machine readable files into the format used by the decode library \a bufrdeco
   included in this package. \a bufrdeco uses BUFR table files in csv format like WMO but supressing
   some fields from original WMO files.

   Because of WMO has not released csv table files for versions prior to 18, it is needed to build
   them since version 13 using ECMWF table files. Version 13 is backward compatible with prior versions.

   Following the ECMWF way to set the CodeFlag tables as C Tables, the original WMO CodeFlag Tables are named
   as C tables. Original WMO C tables with descriptors operators an A tables are no needed in this package.
*/

#include "bufrdeco.h"

// To use package config.h
#ifndef CONFIG_H
# include "config.h"
# define CONFIG_H
#endif

const char SELF[] = "build_bufrdeco_tables";
char INPUT_FILE[256];
char TABLE_TYPE[8];
int IS_WMO;
int A_FIELDS[8] = {1,2,-1,-1,-1,-1,-1,-1}; // fields selected for table A type
int B_FIELDS[8] = {3,4,5,6,7,8,9,-1}; // fields selected for table B type
int C_FIELDS[8] = {1,3,4,5,6,7,-1,-1}; // Fields selected for CodeFlag (C) type.
int D_FIELDS[8] = {3,6,4,7,-1,-1,-1,-1}; // Fields selected for D type (Common sequences)

int B_FIELDS_2[8] = {2,3,4,5,6,7,8,-1}; // fields selected for table B type for version >= 35
int C_FIELDS_2[8] = {0,2,3,4,5,6,-1,-1}; // Fields selected for CodeFlag (C) type for version >= 35.
int D_FIELDS_2[8] = {2,5,3,6,-1,-1,-1,-1}; // Fields selected for D type (Common sequences)

void print_usage ( void )
{
  printf ( "%s %s\n", SELF, PACKAGE_VERSION );
  printf ( "Usage: \n" );
  printf ( "%s -i input_file -t table_type [-e[-h]\n" , SELF );
  printf ( "       -h Print this help\n" );
  printf ( "       -e Source ECMWF, default WMO\n" );
  printf ( "       -t table_type. (A = TableA, B = TableB, C = CodeFlag, D = TableD)\n" );
  printf ( "       -2 Versión 35 or newest\n");
}


int main ( int argc, char *argv[] )
{
  int iopt, i, *fs;
  FILE *f;
  char lin[CSV_MAXL], caux[CSV_MAXL], caux2[32];
  char *tk[16], *c;
  int nt;
  uint32_t ix = 0;
  long int il;
  struct bufr_descriptor desc;

  // Read args
  TABLE_TYPE[0] = 0;
  INPUT_FILE[0] = 0;
  IS_WMO = 1;
  while ( ( iopt = getopt ( argc, argv, "hei:t:2" ) ) !=-1 )
    switch ( iopt )
      {
      case 'i':
        if ( strlen ( optarg ) < 256 )
          strcpy ( INPUT_FILE, optarg );
        break;
      case 't':
        if ( strlen ( optarg ) < 8 )
          strcpy ( TABLE_TYPE, optarg );
        break;
      case '2':
        IS_WMO = 2;
        break;
      case 'e':
      case 'E':
        IS_WMO = 0;
        break;
      case 'h':
      default:
        print_usage();
        exit ( EXIT_SUCCESS );
      }

  if ( TABLE_TYPE[0] == 0 )
    {
      fprintf ( stderr, "%s: Error. Need to provide Table Type with arg -t\n", SELF );
      exit ( EXIT_FAILURE );
    }

  if ( INPUT_FILE[0] == 0 )
    {
      fprintf ( stderr, "%s: Error. Need to provide input file with arg -i\n", SELF );
      exit ( EXIT_FAILURE );
    }

  if ( ( f = fopen ( INPUT_FILE, "r" ) ) == NULL )
    {
      fprintf ( stderr, "%s: Error. Cannot open file '%s'\n", SELF, INPUT_FILE );
      exit ( EXIT_FAILURE );
    }

  if ( IS_WMO )
    {
      switch ( TABLE_TYPE[0] )
        {
        case 'A':
        case 'a':
          fs = & ( A_FIELDS[0] );
          break;
        case 'B':
        case 'b':
          if (IS_WMO == 1)  
            fs = & ( B_FIELDS[0] );
          else
            fs = & ( B_FIELDS_2[0] );
          break;
        case 'C':
        case 'c':
          if (IS_WMO == 1)  
            fs = & ( C_FIELDS[0] );
          else
            fs = & ( C_FIELDS_2[0] );
          break;
        case 'D':
        case 'd':
          if (IS_WMO == 1)  
            fs = & ( D_FIELDS[0] );
          else
            fs = & ( D_FIELDS_2[0] );
          break;
        default:
          fprintf ( stderr, "%s: Error, bad '%s' table type\n", SELF, TABLE_TYPE );
          fclose ( f );
          exit ( EXIT_FAILURE );
        }

      while ( fgets ( lin, CSV_MAXL, f ) != NULL )
        {
          // Parse line
          if ( parse_csv_line ( &nt, tk, lin ) < 0 )
            {
              fprintf ( stderr, "%s: Error parsing csv line '%s' from file '%s'\n", SELF, lin, INPUT_FILE );
              fclose ( f );
              exit ( EXIT_FAILURE );
            }

          i = 0;
          while ( fs[i] >= 0 )
            {
              if ( i > 0 )
                printf ( "," );
              if ( strlen ( tk[fs[i]] ) )
                printf ( "%s", csv_quoted_string ( caux, tk[fs[i]] ) );
              i++;
            }
          printf ( "\n" );
        }
    }
  else
    {
      // Decode ECMWF tables
      // first line with the name of fields
      switch ( TABLE_TYPE[0] )
        {
        case 'B':
        case 'b':
          printf ( "\"FXY\",\"ElementName_en\",\"Note_en\",\"BUFR_Unit\",\"BUFR_Scale\",\"BUFR_ReferenceValue\",\"BUFR_DataWidth_Bits\"\n" );
          break;
        case 'C':
        case 'c':
          printf ( "\"FXY\",\"CodeFigure\",\"EntryName_en\",\"EntryName_sub1_en\",\"EntryName_sub2_en\",\"Note_en\"\n" );
          break;
        case 'D':
        case 'd':
          printf ( "\"FXY1\",\"FXY2\",\"Title_en\",\"ElementName_en\"\n" );
          break;
        }

      caux[0] = 0;
      while ( fgets ( lin, CSV_MAXL, f ) != NULL )
        {
          // supress the newline
          if ( ( c = strrchr ( lin,'\n' ) ) != NULL )
            {
              *c = '\0';
            }

          switch ( TABLE_TYPE[0] )
            {
            case 'B':
            case 'b':
              // Descriptor
              ix = strtoul ( &lin[0], &c, 10 );
              uint32_t_to_descriptor ( &desc, ix );
              printf ( "\"%s\",", desc.c );

              // detailed name and a void note
              bufr_charray_to_string ( caux, &lin[8], 64 );
              bufr_adjust_string ( caux ); // supress trailing blanks
              printf ( "\"%s\",,", caux );

              // type
              bufr_charray_to_string ( caux ,&lin[73], 24 );
              bufr_adjust_string ( caux );
              printf ( "\"%s\",", caux );

              // escale
              il = strtol ( &lin[97], &c, 10 );
              printf ( "\"%ld\",", il );

              // reference
              il = strtol ( &lin[102], &c, 10 );
              printf ( "\"%ld\",", il );

              // bits
              il = strtol ( &lin[115], &c, 10 );
              printf ( "\"%ld\"\n", il );
              break;

            case 'C':
            case 'c':

              // write csv line with stored data if finished
              if ( lin[12] != ' ' && caux[0] )
                {
                  printf ( "\"%s\",\"%u\",\"%s\",,,\n", caux2, ix, caux );
                  caux[0] = 0;
                }

              if ( lin[0] != ' ' )
                {
                  memcpy ( caux2, lin, 6 );
                  caux2[6] = '\0';
                }

              if ( lin[12] != ' ' )
                {
                  ix = strtol ( lin + 12, &c, 10 );
                  strcpy ( caux, lin + 24 );
                }
              else
                {
                  if ( lin[22] != ' ' )
                    {
                      if ( strlen ( caux ) + strlen ( lin + 22 ) < BUFR_EXPLAINED_LENGTH )
                        strcat ( caux, lin + 22 );
                      else
                        {
                          // cut the explanation
                          * ( lin + 22 + BUFR_EXPLAINED_LENGTH - strlen ( caux ) - 1 ) = '\0';
                          strcat ( caux, lin + 22 );
                        }
                    }
                  else
                    {
                      // this is valid for versions < 15
                      if ( strlen ( caux ) + strlen ( lin + 24 ) < BUFR_EXPLAINED_LENGTH )
                        strcat ( caux, lin + 24 );
                      else
                        {
                          // cut the explanation
                          * ( lin + 24 + BUFR_EXPLAINED_LENGTH - strlen ( caux ) - 1 ) = '\0';
                          strcat ( caux, lin + 24 );
                        }
                    }
                }

              break;

            case 'D':
            case 'd':

              if ( lin[1] != ' ' )
                {
                  memcpy ( caux2, lin + 1, 6 );
                  caux2[6] = '\0';
                }

              memcpy ( caux, lin + 11, 6 );
              caux[6] = '\0';
              printf ( "\"%s\",\"%s\",,\n", caux2, caux );
              break;

            default:
              fprintf ( stderr, "%s: Error, bad '%s' table type\n", SELF, TABLE_TYPE );
              fclose ( f );
              exit ( EXIT_FAILURE );
            }
        }

      // In case of table C (codeFlag) we still have to write last line
      if ( TABLE_TYPE[0] == 'C' || TABLE_TYPE[0] == 'c' )
        {
          printf ( "\"%s\",\"%u\",\"%s\",,,\n", caux2, ix, caux );
        }
    }
  fclose ( f );
  exit ( EXIT_SUCCESS );
}
