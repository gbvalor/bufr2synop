/***************************************************************************
 *   Copyright (C) 2013-2016 by Guillermo Ballester Valor                  *
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
 \file bufr2synop.h
 \brief Include header file for binary bufr2synop
*/

#include "bufr2tac.h"

// Global vars
extern unsigned char BUFR_MESSAGE[BUFR_LEN];
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
extern char BUFRTABLES_DIR[256];
extern char LISTOFFILES[256];
extern char SELF[];
extern int VERBOSE;
extern int SHOW_SEQUENCE;
extern int SHOW_ECMWF_OUTPUT;
extern int DEBUG;
extern int NFILES;
extern int GTS_HEADER;
extern int XML;
extern int JSON;
extern int CSV;

extern struct synop_chunks SYNOP;
extern struct buoy_chunks BUOY;
extern struct temp_chunks TEMP;

extern size_t NLINES_TABLEC;
extern char TABLEC[MAXLINES_TABLEC][92];
extern char DEFAULT_BUFRTABLES[];
extern char TYPE[8];
extern struct metreport REPORT;
extern struct gts_header HEADER;
extern FILE * FL;
extern struct bufr_subset_sequence_data SUBSET;
extern struct bufr_subset_state STATE;


// functions
void print_usage ( void );
int read_args ( int _argc, char * _argv[] );
char * get_bufrfile_path ( char *filename, char *err );
