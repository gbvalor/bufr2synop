/***************************************************************************
 *   Copyright (C) 2012 by Guillermo Ballester Valor   *
 *   gbv@ogimet.com   *
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

void print_usage(void)
{
  printf("Usage: \n");
  printf("  bufr2synop -i input [-o output][-v]\n");
  printf("       -i input. Pathname of the file with the bufr message to parse\n");
  printf("       -o output. Pathname of output file. Default is standar output\n");
  printf("       -v. Verbose output\n");
  printf("       -h. Show this help\n");
}

int read_arguments( int _argc, char * _argv[])
{
  int iopt;

  // Initial and default values
  INPUTFILE[0] = '\0';
  OUTPUTFILE[0] = '\0';
  VERBOSE = 0;

  /*
     Read input arguments using getop library 
  */
  while ((iopt = getopt(_argc, _argv, "hi:o:v"))!=-1)
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
      exit (EXIT_SUCCESS);
    }

  if (strlen(INPUTFILE) == 0)
  {
    print_usage();
    return 1;
  }
  return 0;
}
