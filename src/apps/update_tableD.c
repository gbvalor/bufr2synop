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
/*!
   \file update_tableD.c
   \brief This file includes the code for update_TableD binary

   It try to fill some field definitions for TableD from ECMWF. It fills
   the 'Title_en' for revisions prior 18_0_0 from this version.
*/


#include "bufrdeco.h"

// To use package config.h
#ifndef CONFIG_H
# include "config.h"
# define CONFIG_H
#endif

const char SELF[] = "update_tableD";
char INPUT_FILE[256];
char REFERENCE_FILE[256];
char *TK[BUFR_MAXLINES_TABLED][16];
char REF[BUFR_MAXLINES_TABLED][CSV_MAXL];
char LIN[CSV_MAXL];
FILE *F, *R;


void print_usage ( void )
{
  printf ( "%s %s\n", SELF, PACKAGE_VERSION );
  printf ( "Usage: \n" );
  printf ( "%s -i input_file -r reference_file [-h]\n" , SELF );
  printf ( "       -h Print this help\n" );
}

int main ( int argc, char *argv[] )
{
  int iopt, i, j, k, nt;
  char caux[CSV_MAXL], caux2[CSV_MAXL];
  char *tk[16];

  // read arguments
  INPUT_FILE[0] = 0;
  REFERENCE_FILE[0] = 0;

  while ( ( iopt = getopt ( argc, argv, "hi:r:" ) ) !=-1 )
    switch ( iopt )
      {
      case 'i':
        if ( strlen ( optarg ) < 256 )
          strcpy ( INPUT_FILE, optarg );
        break;
      case 'r':
        if ( strlen ( optarg ) < 256 )
          strcpy ( REFERENCE_FILE, optarg );
        break;
      case 'h':
      default:
        print_usage();
        exit ( EXIT_SUCCESS );
      }

  // Open reference file
  if ( ( R = fopen ( REFERENCE_FILE, "r" ) ) == NULL )
    {
      fprintf ( stderr, "%s: Error. Cannot open file '%s'\n", SELF, REFERENCE_FILE );
      exit ( EXIT_FAILURE );
    }

  // read and proccess refence file
  i = 0;
  while ( fgets ( REF[i], CSV_MAXL, R ) != NULL )
    {
      // Parse line
      if ( parse_csv_line ( &nt, TK[i], REF[i] ) < 0 )
        {
          fprintf ( stderr, "%s: Error parsing csv line '%s' from file '%s'\n", SELF, LIN, INPUT_FILE );
          fclose ( R );
          exit ( EXIT_FAILURE );
        }
      i++;
    }
  fclose ( R );

  // Open source file
  if ( ( F = fopen ( INPUT_FILE, "r" ) ) == NULL )
    {
      fprintf ( stderr, "%s: Error. Cannot open file '%s'\n", SELF, INPUT_FILE );
      exit ( EXIT_FAILURE );
    }

  // Read and process file
  j = 0;
  while ( fgets ( LIN, CSV_MAXL, F ) != NULL )
    {
      // Parse line
      if ( parse_csv_line ( &nt, tk, LIN ) < 0 )
        {
          fprintf ( stderr, "%s: Error parsing csv line '%s' from file '%s'\n", SELF, LIN, INPUT_FILE );
          fclose ( F );
          exit ( EXIT_FAILURE );
        }

      printf ( "%s,", csv_quoted_string ( caux, tk[0] ) );
      printf ( "%s,", csv_quoted_string ( caux, tk[1] ) );


      if ( tk[2][0] == 0 ) //case of void field, lets go
        {
          caux[0] = 0;
          // Search the same key tk[0] in reference file
          for ( k = j; k < i ; k++ )
            {
              if ( strcmp ( TK[k][0], tk[0] ) == 0 )
                {
                  strcpy ( caux2, TK[k][2] );
                  j = k;
                  break;
                }
            }
          printf ( "%s,", csv_quoted_string ( caux, caux2 ) );

        }
      else
        printf ( "%s,", csv_quoted_string ( caux, tk[2] ) );


      printf ( "%s\n", csv_quoted_string ( caux, tk[3] ) );
    }
  fclose ( F );

  exit ( EXIT_SUCCESS );
}
