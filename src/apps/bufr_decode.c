/* $Id$ */
/**
* Copyright 1981-2012 ECMWF.
*
* This software is licensed under the terms of the GNU Lesser
* General Public License Version 3 which can be obtained at
* http://www.gnu.org/licenses/lgpl.html.
*
* In applying this licence, ECMWF does not waive the privileges
* and immunities granted to it by virtue of its status as an
* intergovernmental organisation nor does it submit to any
* jurisdiction.
*/
/*!
 \file bufr_decode.c
 \brief decodes a bufr file using ECMWF bufr library

 This file comes from original ECMWF example. Here it is heavily transformed
 Anyway it mantain the original coyright

 Most important about modified is that all integers must have 4 bytes, i.e., need
 to be defined as 'int' or 'unsigned int' instead of 'long' and ' unsigned long'
 for intel/amd X86_64 machines

 To be compiled with gcc we need:
   1) Install ECMWF bufr library
   2) Then compile it as
      gcc bufr_decode.c -o bufr_decode -lgfortran -lbufr -lm

*******************************************************************
*
* Program : Bufr_decode
*
*
* Author: Milan Dragosavac    ECMWF    July 1996
*
* Purpose: Decode bufr message
*
*
*
* Usage:
*
*
*
* References:
*
*
*
*
* File formats:
*
*
*
* Restrictions:
*
*
* Error handling:
*
*
*
* Notes:
* Revised by Guillermo Ballester Valor AEMET (Spain) to be
* compiled with gcc/gfortran in OpenSUSE 12.2.  November 2012
*
*******************************************************************
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*!
  \def KVALS
  \brief dimension of arrays of doubles
*/
#define KVALS 1024000

/*!
  \def KELEM
  \brief max dimension of elements for a single report
*/
#define KELEM 8192

/*!
  Note that big arrays are defined as global here to avoid segfaults
*/

char bufr_message[512000]; /*!< The array where the bufr file will be stored when readed */

static char cnames[KELEM][64]; /*!< Array of strings with name of vars */
static char cunits[KELEM][24]; /*!< Array of strings with name of used units */

char cvals[KVALS][80]; /*!< array of strings with value of data */

double values[KVALS],vals[KVALS];
int ktdlst[KELEM], ktdexp[KELEM];

/*! Added to avoid C compiler warnings */
int bus012_ ( int *, unsigned int *, int *, int *, int *, int *, int * );
int buprs0_ ( int * );
int buprs1_ ( int * );
int buprs3_ ( int *, int *, int *, int *, int *, int *, char ** );
int bufrex_ ( int *, int *, int *, int *, int *, int *, int *, int *, int *, char **, char **, int *, double *, char **,                int * );
int busel_ ( int *, int *, int *, int *, int * );
int busel2_ ( int *, int *, int *,  char **, int *, char **, char **, char **, int * );
int buukey_ ( int *, int *, int *, int *, int * );
int buprt_ ( int *, int *, int *, int *, char **, char **, char **, int *, double *, int *, int *, int * );

int main ( int argc, char *argv[] )
{

  FILE *fp;
  char filename[256];
  int length=512000;
  int status = 0;

  unsigned int *kbuff;
  int ksup[9];
  int ksec0[3];
  int ksec1[40];
  int ksec2[4096];
  int ksec3[4];
  int ksec4[2];
  int key[46];
  int kerr;

  int kelem = KELEM, kvals = KVALS;
  int icode = 0, ktdlen, ktdexl;
  int current_ss;

  /*     Get input and output file name.  */
  /*     -------------------------------  */

  if ( argc != 3 )
    {
      printf ( "Usage: bufr_decode -i infile \n" );
      printf ( "Please try again. \n" );
      exit ( 1 );
    }

  printf ( "%c",argc );

  if ( !strcmp ( argv[1],"-i" ) ) strcpy ( filename,argv[2] );
  else
    {
      printf ( "Usage: bufr_decode -i infile \n" );
      exit ( 1 );
    }

  /*     Open input file       */
  /*     ---------------       */

  if ( ( fp = fopen ( filename,"r" ) ) == NULL )
    {
      printf ( "cannot open file\n" );
      exit ( 1 );
    }

  /*     Read in bufr messages */
  /*     --------------------- */

  while ( status >= 0 )
    {
      status = readbufr ( fp,&bufr_message,&length );

      if ( status == -1 ) printf ( "End of file.\n" );
      else if ( status == -2 ) printf ( "Error in file handling\n" );
      else if ( status == -3 ) printf ( "Too small input array.\n" );
      else if ( status == -4 ) printf ( "Too small input array.\n" );
      else
        {
          printf ( "It is OK.\n" );
          printf ( "message read " );
          printf ( "%d\n",length );
          printf ( "%s\n",&bufr_message[0] );
        }
      status=-1;

      /* Expand bufr message calling fortran program */
      kbuff = ( unsigned int * ) bufr_message;
      length /= 4; // Now length is sized in words

      bus012_ ( &length, kbuff , ksup, ksec0, ksec1, ksec2,  &kerr ) ;
      buprs0_ ( ksec0 );
      buprs1_ ( ksec1 );

      if ( ksup[5] > 1 )
        kelem = kvals/ksup[5];
      else
        kelem = KELEM;

      if ( kelem > KELEM ) kelem = KELEM;
      kerr = 0;
      bufrex_ ( &length, ( int * ) kbuff,ksup,ksec0,ksec1,ksec2,ksec3,ksec4,
                &kelem, ( char ** ) cnames, ( char ** ) cunits,&kvals,
                values, ( char ** ) cvals,&kerr );
      if ( kerr )
        {
          kerr = 0;
        }

      /* NOTE the differences between array indexes in fortran and C
         so fortran KSEC3[3] is C ksec3[2]  */
      printf ( "ksec3[3]=%d\n", ksec3[2] );
      buukey_ ( ksec1,ksec2,key,ksup,&kerr );

      busel_ ( &ktdlen,ktdlst,&ktdexl,ktdexp,&kerr );
      buprs3_ ( ksec3,&ktdlen,ktdlst,&ktdexl,ktdexp,&kelem, ( char ** ) cnames );

      icode = 0;
      current_ss = 1;
      buprt_ ( &icode,&current_ss,&ksec3[2],&kelem, ( char ** ) cnames,
               ( char ** ) cunits, ( char ** ) cvals,
               &kvals,values,ksup,ksec1,&kerr );

    }

  return kerr;

}

