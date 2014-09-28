/***************************************************************************
 *   Copyright (C) 2013-2014 by Guillermo Ballester Valor                  *
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

void print_usage(void)
{
  printf("Usage: \n");
  printf("  bufr2synop [-i input] [-I list_of_files] [-t bufrtable_dir][-o output][-s][-v][-j][-x][-c]\n");
  printf("       -e Print some original output from ECMWF library\n");
  printf("       -h. Show this help\n");
  printf("       -i input. Pathname of the file with the bufr message to parse\n");
  printf("       -j. The output is in json format\n");
  printf("       -c. The output is in csv format\n");
  printf("       -I list_of_files. Pathname of a file with the list of files to parse, one filename per line\n");
  printf("       -o output. Pathname of output file. Default is standar output\n");
  printf("       -s prints a long output with explained sequence of descriptors\n");
  printf("       -t bufrtable_dir. Pathname of bufr tables directory. Ended with '/'\n");
  printf("       -v. Verbose output\n");
  printf("       -x. The output is in xml format\n");
}

/*!
  \fn int read_bufr(unsigned char *bufr, char *filename, int *length)
  \brief read a bufr file as an array of unsigned chars
  \param bufr pointer to an array of unsigned chars. On output it will contain the bufr
  \param filename string with complete pathname of bufr file to read
  \param length On input max length allocated by caller. On output real length of bufr
*/
int read_bufr(unsigned char *bufr, char *filename, int *length)
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

  //printf("%d %d\n", n, three_bytes_to_uint(bufr + 4));
  // check about the expected size
  if (n != three_bytes_to_uint(bufr + 4))
    return -3;

  return 0;
}

/*!
 \fn int read_arguments( int _argc, char * _argv[])
 \brief read input arguments with the aid of getopt
 */
int read_arguments(int _argc, char * _argv[])
{
  int iopt;
  char aux[256];

  // Initial and default values
  INPUTFILE[0] = '\0';
  OUTPUTFILE[0] = '\0';
  LISTOFFILES[0] = '\0';
  BUFRTABLES_DIR[0] = '\0';
  VERBOSE = 0;
  SHOW_SEQUENCE = 0;
  SHOW_ECMWF_OUTPUT = 0;
  DEBUG = 0;
  NFILES = 0;
  XML = 0;
  JSON = 0;
  CSV= 0;

  /*
   Read input arguments using getop library
   */
  while ((iopt = getopt(_argc, _argv, "cDehi:jI:o:st:vx")) != -1)
    switch (iopt)
      {
      case 'i':
        if (strlen(optarg) < 256)
          strcpy(INPUTFILE, optarg);
        break;
      case 'j':
        JSON = 1;
        break;
      case 'I':
        if (strlen(optarg) < 256)
          strcpy(LISTOFFILES, optarg);
        break;
      case 'o':
        if (strlen(optarg) < 256)
          strcpy(OUTPUTFILE, optarg);
        break;
      case 't':
        if (strlen(optarg) < 256)
          {
            strcpy(BUFRTABLES_DIR, optarg);
          }
        break;
      case 'D':
        DEBUG = 1;
        break;
      case 'e':
        SHOW_ECMWF_OUTPUT = 1;
        break;
      case 'v':
        VERBOSE = 1;
        break;
      case 's':
        SHOW_SEQUENCE = 1;
        break;
      case 'x':
        XML = 1;
        break;
      case 'c':
        CSV = 1;
        break;
      case 'h':
      default:
        print_usage();
        exit(EXIT_SUCCESS);
      }

  if (INPUTFILE[0] == 0 && LISTOFFILES[0] == 0)
    {
      print_usage();
      return 1;
    }
  return 0;
}


/*!
  \fn char * get_bufrfile_path( char *filename, char *err)
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
  ls -1 *.bufr > list_file && bufr2synop -I list_file
  \endcode 
*/
char * get_bufrfile_path( char *filename, char *err)
{
   char aux[256], *c;
   if (LISTOFFILES[0] == 0) 
   {
      if (NFILES == 0)
      {
         strcpy(filename, INPUTFILE);
         return filename;
      }
      else
         return 0;
   }
   else if (NFILES == 0)
   {
     if ((FL = fopen(LISTOFFILES,"r")) == NULL)
     {
        sprintf(err,"Cannot open '%s'", LISTOFFILES);
        return NULL;
     }
   }
   if (fgets(aux, 256, FL))
   {
      c =  strrchr(aux,'\n');
      if (c)
       *c = 0;
      strcpy(filename, aux);
      return filename;
   }
   else
   {
      fclose(FL);
      return NULL;
   }
}
