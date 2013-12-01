/***************************************************************************
 *   Copyright (C) 2013 by Guillermo Ballester Valor                       *
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
  printf("  bufr2synop -i input [-t bufrtable_dir][-o output][-v]\n");
  printf("       -i input. Pathname of the file with the bufr message to parse\n");
  printf("       -o output. Pathname of output file. Default is standar output\n");
  printf("       -t bufrtable_dir. Pathname of bufr tables directory. Ended with '/'\n");
  printf("       -v. Verbose output\n");
  printf("       -h. Show this help\n");
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
  BUFRTABLES_DIR[0] = '\0';
  VERBOSE = 0;
  
  /*
   Read input arguments using getop library
   */
  while ((iopt = getopt(_argc, _argv, "hi:o:t:v")) != -1)
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
     case 't':
      if (strlen(optarg) < 256)
      {
        strcpy(BUFRTABLES_DIR, optarg);
      }
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

/*!
  \fn char * get_ecmwf_tablename(char *target, char type)
  \brief Get the complete pathname of a table file needed by a bufr message
  \param target the resulting name
  \param type a char with type, i.e, 'B', 'C', or 'D'

  In ECMWF library the name of a table file is Kssswwwwwxxxxxyyyzzz.TXT , where
       - K - type of table, i.e, 'B', 'C', or 'D'
       - sss - Master table number (zero for WMO meteorological tables)
       - wwwww - Originating sub-centre
       - xxxxx - Originating centre
       - yyy - Version number of master table used
       - zzz - Version number of local table used

  If standard WMO tables are used, the Originating centre xxxxx will be set to
*/
char * get_ecmwf_tablename(char *target, char type)
{
   if (KSEC1[13] != 0) // case of not WMO tables
     sprintf(target,"%s%c%03d%05d%05d%03d%03d.TXT", BUFRTABLES_DIR, type, KSEC1[13], KSEC1[15], KSEC1[2], KSEC1[14], KSEC1[7]);
   else
     sprintf(target,"%s%c000%05d00000%03d%03d.TXT", BUFRTABLES_DIR, type, KSEC1[15], KSEC1[14], KSEC1[7]);
   return target;
} 

/*!
  \fn int read_table_c(void)
  \brief read a Table C file with code TABLE and flag descriptors   
*/ 
int read_table_c(void)
{
   char *c, file[256];
   FILE *t;
   size_t i = 0;

   NLINES_TABLEC = 0;

   get_ecmwf_tablename(&file[0], 'C');

   if ((t = fopen(file, "r")) == NULL)
   {
       fprintf(stderr,"Unable to open table C file '%s'\n", file);
       return 0;
   }
   
   while(fgets(TABLEC[i], 90, t) != NULL && i < MAXLINES_TABLEC)
   {
      // supress the newline
      if ((c = strrchr(TABLEC[i],'\n')) != NULL)
        *c = '\0';
      i++;
   }
   fclose(t);
   NLINES_TABLEC = i;
   return i; 
}

/*!
  \fn char * get_explained_table_val(char *expl, size_t dim, struct bufr_descriptor *d, int ival)
  \brief gets a strung with the meaning of a value for a code table descriptor 
  \param expl string with resulting meaning
  \param dim max length alowed for \a expl string
  \param d pointer to the source descriptor
  \param ival integer value for the descriptos

  If something went wrong, it returns NULL . Otherwise it returns \a expl
*/
char * get_explained_table_val(char *expl, size_t dim, struct bufr_descriptor *d, int ival)
{
   char *c;
   long nv, v,  nl;
   size_t i, j;

   // Find first line for descriptor
   for (i = 0; i <  NLINES_TABLEC; i++)
   {
      if(TABLEC[i][0] != d->c[0] ||
         TABLEC[i][1] != d->c[1] ||
         TABLEC[i][2] != d->c[2] ||
         TABLEC[i][3] != d->c[3] ||
         TABLEC[i][4] != d->c[4] ||
         TABLEC[i][5] != d->c[5])
        continue;
      else
        break;
   }

   if (i == NLINES_TABLEC)
   {
     //printf("Descriptor %s No encontrado\n", d->c);
     return NULL; 
   }
   //printf("Descriptor %s en linea %d\n", d->c, i);

   // reads the amount of possible values
   if (TABLEC[i][7] != ' ')
     nv = strtol(&TABLEC[i][7], &c, 10);
   else
     return NULL;

   // read a value
   for (j = 0; j < nv && i < NLINES_TABLEC ; i++)
   {
     if (TABLEC[i][12] != ' ')
     {
        v = strtol(&TABLEC[i][12], &c, 10);
        nv++;
        if (v != ival)
          continue;
        break;
     }
   }

   if (j == nv || i == NLINES_TABLEC)
     return NULL; // Value not found 

   // read how many lines for the descriptors
   nl = strtol(&TABLEC[i][21], &c, 10);


   // if match then we have finished the search
   strcpy(expl, &TABLEC[i][24]);
   if (nl > 1)
   {
     for (nv = 1 ; nv < nl; nv++)
        if ((strlen(expl) + strlen(&TABLEC[i + nv][24])) < dim)
          strcat(expl, &TABLEC[i][24]);
   }

   return expl;
}
