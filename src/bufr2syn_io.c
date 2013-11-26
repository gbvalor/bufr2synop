/***************************************************************************
 *   Copyright (C) 2012 by Guillermo Ballester Valor                       *
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
 \file bufr2syn_io.c
 \brief file with the i/o code for binary bufr2synop
 */

#include "bufr2synop.h"

void
print_usage(void)
{
  printf("Usage: \n");
  printf("  bufr2synop -i input [-o output][-v]\n");
  printf(
      "       -i input. Pathname of the file with the bufr message to parse\n");
  printf(
      "       -o output. Pathname of output file. Default is standar output\n");
  printf("       -v. Verbose output\n");
  printf("       -h. Show this help\n");
}

int
read_bufr(unsigned char *bufr, char *filename, int *length)
{
  int aux;
  size_t n = 0;
  FILE *fp;

  /* Open input file */
  if ((fp = fopen(filename, "r")) == NULL )
    {
      fprintf(stderr, "cannot open file '%s'\n", filename);
      exit(EXIT_FAILURE);
    }

  while ((aux = fgetc(fp)) != EOF && n < *length)
    bufr[n++] = aux;
  *length = n;

  // close the file
  fclose(fp);

  if (n < 8) // we need at least 8 bytes
    return -4;

  // check if begins with BUFR 
  if (bufr[0] != 'B' || bufr[1] != 'U' || bufr[2] != 'F' || bufr[3] != 'R')
    return -1;

  // check if end with '7777'
  if (bufr[n - 4] != '7' || bufr[n - 3] != '7' || bufr[n - 2] != '7'
      || bufr[n - 1] != '7')
    return -2;

  printf("%d %d\n", n, three_bytes_to_uint(bufr + 4));
  // check about the expected size
  if (n != three_bytes_to_uint(bufr + 4))
    return -3;

  return 0;
}

/*!
 \fn int read_arguments( int _argc, char * _argv[])
 \brief read input arguments with the aid of getopt
 */
int
read_arguments(int _argc, char * _argv[])
{
  int iopt;

  // Initial and default values
  INPUTFILE[0] = '\0';
  OUTPUTFILE[0] = '\0';
  VERBOSE = 0;

  /*
   Read input arguments using getop library
   */
  while ((iopt = getopt(_argc, _argv, "hi:o:v")) != -1)
    switch (iopt)
      {
    case 'i':
      if (strlen(optarg) < 256)
        strcpy(INPUTFILE, optarg);
      break;
    case 'o':
      if (strlen(optarg) < 256)
        strcpy(OUTPUTFILE, optarg);
      break;
    case 'v':
      VERBOSE = 1;
      break;
    case 'h':
    default:
      print_usage();
      exit(EXIT_SUCCESS);
      }

  if (strlen(INPUTFILE) == 0)
    {
      print_usage();
      return 1;
    }
  return 0;
}
