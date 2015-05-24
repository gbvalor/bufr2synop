/* $Id$ */
/***************************************************************************
 *   Copyright (C) 2004-2015 by Guillermo Ballester Valor                  *
 *   gbv@oxixares.com                                                      *
 *                                                                         *
 *   This file is part of Ogimet                                           *
 *                                                                         *
 *   Ogimet is free software; you can redistribute it and/or modify        *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   Ogimet is distributed in the hope that it will be useful,             *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, see <http://www.gnu.org/licenses/>.  *
 *                                                                         *
 ***************************************************************************/
/*!
   \file bufrnoaa.h
   \brief inclusion file for binary bufrnoaa
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <time.h>
#include <utime.h>
#include <sys/time.h>
#include <sys/stat.h>

#define BLEN 1024
// longitud maxima de un bufr 8 MB
#define BUFRLEN 8388608

extern int STAGE, SELECT, INDIVIDUAL, COLECT, VERBOSE;
extern int LISTF;
extern unsigned char BUFR[BUFRLEN];
extern unsigned char BUF[BLEN];
extern char ENTRADA[256], PREFIX[64];
extern struct stat INSTAT;
extern char SEL[64], SELS[64], SELO[64], SELU[64];
extern char OWN[];

// Functions
int is_bufr ( unsigned char *b );
int is_endb ( unsigned char *b );
int is_head ( unsigned char *b );
int timeval_substract ( struct timeval *result, struct timeval *x, struct timeval *y );
int read_args ( int _argc, char * _argv[] );
void print_usage ( void );
int bufr_is_selected ( char *name );
int date_mtime_from_stat ( char *date, struct stat *st );
int mtime_from_stat ( char *filename, struct stat *st );





