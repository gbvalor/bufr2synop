/***************************************************************************
 *   Copyright (C) 2013-2017 by Guillermo Ballester Valor                  *
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
#include "bufrdeco.h"

// To use package config.h
#ifndef CONFIG_H
# include "config.h"
# define CONFIG_H
#endif

const char SELF[] = "build_bufrdeco_tables";
char INPUT_FILE[256];
char TABLE_TYPE[8];
int A_FIELDS[8] = {1,2,-1,-1,-1,-1,-1,-1}; // fields selected for table A type
int B_FIELDS[8] = {3,4,5,6,7,8,9,-1}; // fields selected for table B type
int C_FIELDS[8] = {1,3,4,5,6,7,-1,-1}; // Fields selected for CodeFlag (C) type.
int D_FIELDS[8] = {3,6,-1,-1,-1,-1,-1,-1}; // Fields selected for D type (Common sequences)

void print_usage ( void )
{
  printf ( "%s %s\n", SELF, PACKAGE_VERSION );
  printf ( "Usage: \n" );
  printf ( "%s -i input_file [-t table_type][-h]\n" , SELF );
  printf ( "       -h Print this help\n" );
  printf ( "       -t table_type. (A = TableA, B = TableB, C = CodeFlag, D = TableD)\n" );
}


int main ( int argc, char *argv[] )
{
  int iopt, i, *fs;
  FILE *f;
  char lin[CSV_MAXL], caux[CSV_MAXL];
  char *tk[16];
  int nt;

  // Read args
  TABLE_TYPE[0] = 0;
  INPUT_FILE[0] = 0;
  while ( ( iopt = getopt ( argc, argv, "hi:t:" ) ) !=-1 )
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

  switch ( TABLE_TYPE[0] )
    {
    case 'A':
    case 'a':
      fs = & ( A_FIELDS[0] );
      break;
    case 'B':
    case 'b':
      fs = & ( B_FIELDS[0] );
      break;
    case 'C':
    case 'c':
      fs = & ( C_FIELDS[0] );
      break;
    case 'D':
    case 'd':
      fs = & ( D_FIELDS[0] );
      break;
    default:
      fprintf ( stderr, "%s: Error, bad '%s' table type\n", SELF, TABLE_TYPE );
      fclose ( f );
      exit ( EXIT_FAILURE );
    }

  while ( fgets ( lin, CSV_MAXL, f ) != NULL && i < BUFR_MAXLINES_TABLEB )
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
          if (strlen(tk[fs[i]]) )
            printf ( "%s", csv_quoted_string ( caux, tk[fs[i]] ) );
          i++;
        }
      printf ( "\n" );
    }
  fclose ( f );
  exit ( EXIT_SUCCESS );
}
