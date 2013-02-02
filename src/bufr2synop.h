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
 \file bufr2synop.h
 \brief Include header file for binary bufr2synop 
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

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
  \def BUFR_LEN
  \brief max length of a bufrfile 
*/
#define BUFR_LEN 512000

extern char BUFR_MESSAGE[BUFR_LEN]; 
extern char CNAMES[KELEM][64]; 
extern char CUNITS[KELEM][24]; 

extern int KSUP[9]; /*!< array containing supplementary information */
extern int KSEC0[3];
extern int KSEC1[40];
extern int KSEC2[4096];
extern int KSEC3[4];
extern int KSEC4[2];
extern int KEY[46];
extern int KERR;


extern char CVALS[KVALS][80]; /*!< array of strings with value of data */

extern double VALUES[KVALS], VALS[KVALS];
extern int KTDLST[KELEM], KTDEXP[KELEM];

extern char INPUTFILE[256];
extern char OUTPUTFILE[256];
extern char SELF[];
extern int VERBOSE;