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
  \file bufr2syn_utils.c
  \brief file with the utility routines for binary bufr2synop 
*/
#include "bufr2syn.h"


/*!
  \fn three_bytes_to_uint(const unsigned char *source)
  \brief returns the integer value from an array of three bytes, most significant first
*/
unsigned int three_bytes_to_uint(const unsigned char *source)
{
   return  (source[2] + source[1] * 256 + source[0] * 65536);
}

/*!
  \fn int integer_to_descriptor(struct bufr_descriptor *d, int id)
  \brief parse an integer with a descriptor fom bufr ECWMF libary
  \param d pointer to a struct \ref bufr_descriptor where to set the result on output
  \param id integer with the descriptor from ewcwf
*/
int integer_to_descriptor(struct bufr_descriptor *d, int id)
{
   if (d == NULL)
     return 1;
   d->f = id / 100000;
   d->x = (id % 100000) / 1000;
   d->y = id % 1000;
   sprintf(d->c, "%06d", id);
   return 0;
}

/*!
  \fn char * charray_to_string(char *s, unsigned char *buf, size_t size)
  \brief get a null termitated c-string from an array of unsigned chars
  \param s resulting string
  \param buf pointer to first element in array
  \param size number of chars in array
*/
char * charray_to_string(char *s, unsigned char *buf, size_t size)
{
   // copy
   memcpy(s , buf, size);
   // add final string mark
   s[size] = '\0';
   return s;
}

/*!
  \fn char * adjust_string(char *s)
  \brief Supress trailing blanks of a string
  \param s string to process
*/
char * adjust_string(char *s)
{
   size_t l;
   l = strlen(s);
   while (l && s[--l] == ' ') 
     s[l] = '\0';
   return s;
}

/*! \fn int tokenize_string(char *tk[], size_t ntk, char *target, size_t len, char *blanks)
        \brief Split the report string into tokens
        \param tk array of strings. Memory have to be allocated previously by caller
        \param ntk max amount of elements in tk array. 
        \param target string which is the report to split
        \param len lenght of \a target string
        \param blanks string with the character considered as blanks. If null
                      the default " =\n\r\v\t" is considered
        Returns the number of tokens \a target has been splitted into.
        Retuns 0 in case of a void or too large target.
        NOTE: \a target is changed after calling this routine. In fact, the pointers
        in \a tk[] are linking to some char in string \a target
*/
size_t tokenize_string(char *tk[], size_t ntk, char *target, size_t len, char *blanks)
{
  size_t i = 1, n;
  char d[] = " =\n\r\v\t", *b, *aux;

  // set the blanks array
  if (blanks == NULL) 
    b = &d[0];
  else 
    b = blanks;

  // check target string limits 
  n = strlen(target);
  if (n == 0 || n > len)
    return 0;

  // the split loop 
  tk[0] = strtok_r(target, b, &aux);
  while (i < ntk &&
         ((tk[i] = strtok_r(NULL, b, &aux)) != NULL ) &&
         strlen(tk[i]) > 2 &&
         strlen(tk[i]) < 10)
    i++;
  if (i < ntk)
    return i;
  else
    return ntk - 1;
}

/*!
  \fn int YYYYMMDDHHmm_to_met_datetime(struct met_datetime *t, const char *source)
  \brief Parse the string YYYYMMDDHHmm and set a struct \ref met_datetime
  \param source string with date in YYYYMMDDHHmm format
  \param t pointer to a struct \ref met_datetime where to set the results
*/
int YYYYMMDDHHmm_to_met_datetime(struct met_datetime *t, const char *source)
{
  if (strlen(source) != 12)
    return 1;
  memset ( &t->tim, 0, sizeof ( struct tm ) );
  strptime (source, "%Y%m%d%H%M", &t->tim );
  strcpy(t->datime, source);
  t->t = mktime(&t->tim);
  return 0;
} 


/*!
  \fn int guess_gts_header(struct gts_header *h, const char *f)
  \brief Guess the WMO GTS header from filename

  \param h pointer to a struct \ref gts_header where to set the results
  \param f pathname of bufrfile

  It returns 1 if guessed, 0 otherwise

  <pre>
  This routine assume the filename format of a bufr file is

    YYYYMMDDHHmmss_BULLNN_ICAO_YYGGgg_ORD.bufr 

  where 

   YYYYMMDDHHmmss 

  is the timestamp (UTC) of the file in GTS (as example, in NOAA gateway) 

  BULLNN is the bulletin identifier in GTS. It is a six chars lenght in the form T1 T2 A1 A2 N1 N2

    For observations, T1 is 'I'
    Possible values of T2 for T1 = 'I' are
       O	 Oceanographic/limnographic (water properties)
       P	 Pictorial
       S	 Surface/sea level
       T	 Text (plain language information)
       U	 Upper air
       X	 Other data types
       Z	 Mixed data types


    For T1T2 = 'IS' Then A1:

       IS A 01–29 Routinely scheduled observations for n/a 000/006
                  distribution from automatic (fixed or mobile)
                  land stations (e.g. 0000, 0100, … or 0220, 0240,
                  0300, …, or 0715, 0745, ... UTC)
       IS A 30–59 N-minute observations from automatic (fixed n/a 000/007
                  or mobile) land stations
       IS B   Radar reports (parts A and B) RADOB 006/003
       IS C 01–45 Climatic observations from land stations CLIMAT 000/020
       IS C 46–59 Climatic observations from marine stations CLIMAT SHIP 001/0202009 edition
                  ATTACHmENT II-5 I
       IS D   Radiological observation RADREP 010/001
       IS E   Ozone measurement at surface n/a 008/000
       IS F   Source of atmospherics SFAZI, SFLOC, SFAZU 000/030
       IS I 01–45 Intermediate synoptic observations from fixed SYNOP (SIxx) 000/001
              land stations 000/051
       IS I 46–59 Intermediate synoptic observations from mobile SYNOP mOBIL 000/004
              land stations
       IS M 01–45 Main synoptic observations from fixed land SYNOP (SMxx) 000/002
              stations 000/052
       IS M 46–59 Main synoptic observations from mobile land SYNOP mOBIL 000/005
               stations
       IS N 01–45 Synoptic observations from fixed land stations SYNOP (SNxx) 000/000
              at non-standard time (i.e. 0100, 0200, 0400,   000/050
              0500, ... UTC)
       IS N 46–59 Synoptic observations from mobile land stations SYNOP mOBIL 000/003
              at non-standard time (i.e. 0100, 0200, 0400,
              0500, ... UTC)
       IS R   Hydrologic reports HYDRA 000/040
       IS S 01–19 Synoptic observations from marine stations SHIP 001/000
       IS S 20–39 One-hour observations from automatic marine n/a 001/006
             stations
       IS S 40–59 N-minute observations from automatic marine n/a 001/007
             stations
       IS T 01–19 Tide gauge observations n/a 001/030
       IS T 20–39 Observed water level time series n/a 001/031
       IS V   Special aeronautical observations (SPECI) SPECI 000/011
       IS W   Aviation routine weather observations (mETAR) mETAR 000/010
       IS X   Other surface data IAC, IAC FLEET

    For T1T2 = 'IU'  then A1:
       IU A   Single level aircraft reports (automatic) AmDAR 004/000
       IU A   Single level aircraft reports (manual) AIREP/PIREP 004/001
       IU B   Single level balloon reports n/a
       IU C   (used for single level satellite-derived SAREP/SATOB 005/000
               reports – see Note 3)
       IU D   Dropsonde/Dropwindsondes TEmP DROP 002/007
       IU E   Ozone vertical sounding n/a 008/001
       IU I   Dispersal and transport analysis n/a 009/000
       IU J 01–19 Upper wind from fixed land stations (entire PILOT (parts A, 002/001
              sounding) B, C, D)
       IU J 20–39 Upper wind from mobile land stations (entire PILOT mOBIL 002/003
              sounding) (parts A, B, C, D)
       IU J 40–59 Upper wind from marine stations (entire PILOT SHIP 002/002
             sounding) (parts A, B, C, D)
       IU K 01–19 Radio soundings from fixed land stations TEmP (parts A, B) 002/004
             (up to 100 hPa)
       IU K 20–39 Radio soundings from mobile land stations TEmP mOBIL 002/006
            (up to 100 hPa) (parts A, B)
       IU K 40–59 Radio soundings from marine stations TEmP SHIP (parts A, B) 002/005
             (up to 100 hPa)
       IU M   Model derived sondes
       IU N   Rocketsondes
       IU O   Profiles of aircraft observations in ascending/ AmDAR 002/020
              descending
       IU P   Profilers PILOT 002/010
       IU Q   RASS temperature profilers TEmP 002/011
       IU R   (used for radiance data – see Note 3)
       IU S   01–19 Radiosondes/pibal reports from fixed land TEmP (parts A, B, C, D) 002/004
               stations (entire sounding)
       IU S 20–39 Radio soundings from mobile land stations TEmP mOBIL (parts A, 002/006
               (entire sounding) B, C, D)
       IU S 40–59 Radio soundings from marine stations TEmP SHIP (parts A, 002/005
               (entire sounding) B, C, D)
       IU T   (used for satellite-derived sondes – see Note 3) SATEm, SARAD, SATOB
       IU U 01–45 Monthly statistics of data from upper-air stations CLImAT TEmP 002/025
       IU U 46–59 Monthly statistics of data from marine stations CLImAT TEmP, SHIP 002/026
       IU W 01–19 Upper wind from fixed land stations (up to PILOT (parts A, B) 002/001
               100 hPa)
       IU W 20–39 Upper wind from mobile land stations (up to PILOT mOBIL 002/003
               100 hPa) (parts A, B)
       IU W 40–59 Upper wind from marine stations (up to PILOT SHIP 002/002
               100 hPa) (parts A, B)
       IU X   Other upper-air reports

    for T1T2 = 'IO' then  A1:
       IO B   Buoy observations BUOY 001/025
       IO I   Sea ice
       IO P   Sub-surface profiling floats TESAC 031/004
       IO R   Sea surface observations TRACKOB 031/001
       IO S   Sea surface and below soundings BATHY, TESAC 031/005
       IO T   Sea surface temperature
       IO W   Sea surface waves WAVEOB 031/002
       IO X   Other sea environmental

    About A2 
       A	 0 - 90W northern hemisphere
       B	 90W - 180 northern hemisphere
       C	 180 - 90E northern hemisphere
       D	 90E - 0 northern hemisphere
       E	 0 - 90W tropical belt
       F	 90W - 180 tropical belt
       G	 180 - 90E tropical belt
       H	 90E - 0 tropical belt
       I	 0 - 90W southern hemisphere
       J	 90W - 180 southern hemisphere
       K	 180 - 90E southern hemisphere
       L	 90E - 0 southern hemisphere
       M	 Lower left 10S 100E/upper right 70N 110W
       N	 Northern hemisphere
       P	 Area between 64.69N - 136.76W, 55.61N - 13.43W
       64.69N - 156.76W, 55.61N - 33.43W
       S	 Southern hemisphere
       T	 45W - 180 northern hemisphere
       U	 Area between 21.0N - 128.1W, 36.0N - 130.9W
                 21.1N - 113.0W, 36.2N - 110.5W
       V	 Area between 30.3N - 83.7W, 51.0N - 68.9W
                 19.8N - 64.5W, 33.3N - 47.1W
       X	 Global Area (area not definable)


   ICAO is the ICAO index for release center.

   YYGGgg is the nominal release time (day, hour, minute) UTC

   ORD is the order sequence as RRA, RRB, CCA .... is optional
  </pre>
*/
int guess_gts_header(struct gts_header *h, const char *f)
{
   size_t nt;
   char aux[256], *tk[16], *c;

   // parse file name
   strcpy(h->filename, f);
   strcpy(aux, f);
 
   // check latest '/' in filename
   if ((c = strrchr(aux,'/')) == NULL)
     c = &aux[0];
   else
     c++;

   nt = tokenize_string(tk, 16, aux, strlen(c), "_.");
   // parse filenames with format as example 'AAAAMMDDHHmmss_ISIE06_SBBR_012100_RRB.bufr'

   // 5 or 6 items 
   if ( nt < 5 || nt > 6)
     return 0;

   // extension bufr
   if (strcmp(tk[nt - 1],"bufr"))
     return 0;

   // item 0 the timestamp of file in NOAA GTS gateway
   if (strlen(tk[0]) != 14 || strspn(tk[0], "0123456789") != 14)
     return 0;
   strcpy(h->timestamp, tk[0]);

   // item 1 
   if (strlen(tk[1]) != 6 || strspn(&tk[1][4],"0123456789") != 2)
     return 0;
   strcpy(h->bname, tk[1]);

   // item 2
   if (strlen(tk[2]) != 4)
     return 0;
   strcpy(h->center, tk[2]);

   if (strlen(tk[3]) != 6 || strspn(tk[3], "0123456789") != 6)
     return 0;
   strcpy(h->dtrel, tk[3]);

   if (nt == 5)
   {
      strcpy(h->order, "BBB");
   }
   else
   {
     if (strlen(tk[4]) == 3)
      strcpy(h->order, tk[4]);
     else
      return 0;
   }

  return 1;
}