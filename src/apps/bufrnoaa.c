/***************************************************************************
 *   Copyright (C) 2013-2024 by Guillermo Ballester Valor                  *
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
/*! \file bufrnoaa.c
    \brief This file includes the code to extract BUFR messages from NOAA bin files

    <pre>
    *.bin files have the following sequence
    header: in the form '****NNNNNNN****
    new line
    Name: This is taken as name
    \r\r\n
    bufr report
    \r\r\n

    First, it parses bufr reports from file with path included with argument -i

    Then for every report several decisions are taken depending on arguments
      1) Select or discard depending on the type of BUFR
         To select we have to set our choice with the aid of argument -T.
         The argument supplied with '-t' will have one or more chars matching with T2 indexes (see below).
         As example, '-T USO' will select all the BUFR messages with T2 as U (upper level), S (surface) or
         O (oceanographic)
         If no '-t' argument is set it means NO SELECCTION i.e. we almost do nothing

      2) For every selected BUFR message we can do any of these task, (they are not excluyent)
         - To write individual files, one per bufr message, with names as 'YYYYMMDDHHmmss_ISIE06_SBBR_012100_RRB.bufr'.
           Date and time are source file timestamp and other items from bufr header.
           To do this we have to use '-f' option.
         - To write an archive file as the original NOAA bin file, but just with selected messages. The option
           is '-F prefix' where prefix is the string to add. Resulting file names are in the form prefix_original_nane
           if no -F option then no archive bin file is generated. timestamp of resulting file is the same than the input
           file. In case of no bufr selected it just create a void file.

    Second item of resulting name file is 6 characters long:

    For observed data T1 is 'I'

    Possible values for T2 when T1 = 'I'
       O	 Oceanographic/limnographic (water properties)
       P	 Pictorial
       S	 Surface/sea level
       T	 Text (plain language information)
       U	 Upper air
       X	 Other data types
       Z	 Mixed data types

    When T1T2 = 'IS' values for A1:

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

    When T1T2 = 'IU'  valores for A1:
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

    for T1T2 = 'IO' values for A1:
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



    </pre>
*/
#include "bufrnoaa.h"

int STAGE, SELECT, INDIVIDUAL, COLECT, VERBOSE;
int  LISTF; /*!< if != then a list of messages in bin file is generated */
unsigned char BUFR[BUFRLEN];
unsigned char BUF[BLEN];
char ENTRADA[256];
char SEL[64]; /*!< Selection string for argument -T according with T1 */
char SELS[64]; /*!< Selection string for A1 when T2='S' (argument -S) */
char SELU[64]; /*!< Selection string for A1 when T2='U' (argument -U) */
char SELO[64]; /*!< Selection string for A1 when T2='O' (argument -O) */
char SELP[64]; /*!< Selection string for A1 when T2='P' (argument -O) */
char SELT[64]; /*!< Selection string for A1 when T2='T' (argument -O) */
char SELX[64]; /*!< Selection string for A1 when T2='X' (argument -O) */
char SELZ[64]; /*!< Selection string for A1 when T2='Z' (argument -O) */
char PREFIX[64];
char HEADER_MARK; /*!< Header mark character who is repeated four times at the begining */

struct stat INSTAT;
char OWN[] = "bufrnoaa";
char SEP[] = "\r\r\n";
char FINAL_SEP[4];


int main ( int argc, char *argv[] )
{
  size_t nb = 0, nc, nx = 0, nbuf = 0, nsel = 0, nerr = 0, i, nh = 0, nw;
  FILE* ficin;
  FILE* ficout = NULL;
  FILE* ficol = NULL;
  unsigned char b[5], header[256];
  unsigned int expected = 0;
  char name[256], namex[512], namec[512];
  struct timeval tini, tfin, tt;

  // Initial time
  gettimeofday ( &tini, NULL );

  // read args from stdio
  if ( read_args ( argc, argv ) < 0 )
    {
      exit ( EXIT_FAILURE );
    }

  // Check input
  if ( stat ( ENTRADA, &INSTAT ) )
    {
      printf ( "%s: Cannot stat  %s\n", OWN, ENTRADA );
      exit ( EXIT_FAILURE );
    }

  // Open output file
  if ( ( ficin = fopen ( ENTRADA,"r" ) ) == NULL )
    {
      printf ( "%s: Cannot open %s\n", OWN, ENTRADA );
      exit ( EXIT_FAILURE );
    }

  if ( COLECT )
    {
      // To make an archive

      // build a name
      strcpy ( namec, PREFIX );
      strcat ( namec, ENTRADA );

      // open the file
      if ( ( ficol = fopen ( namec, "w" ) ) == NULL )
        {
          printf ( "%s: Cannot open %s\n", OWN, namec );
          exit ( EXIT_FAILURE );
        }
    }


  STAGE = 0;

  memset ( &b, 0, 4 * sizeof ( unsigned char ) );

  while ( ( nc = fread ( &BUF[0], sizeof ( unsigned char ), BLEN, ficin ) ) > 0 )
    {
      for ( i = 0; i < nc ; i++ )
        {
          // ingest a byte
          b[4] = b[3];
          b[3] = b[2];
          b[2] = b[1];
          b[1] = b[0];
          b[0] = BUF[i];

          switch ( STAGE )
            {
            case 0: // begining, searching header init
              if ( is_head_custom ( &b[0] , HEADER_MARK ) )
                {
                  memset ( &header[0], 0, 64 * sizeof ( unsigned char ) );
                  header[0] = HEADER_MARK;
                  header[1] = HEADER_MARK;
                  header[2] = HEADER_MARK;
                  header[3] = HEADER_MARK;
                  nh = 4;
                  STAGE = 1;
                }
              break;
            case 1: // header found, filling
              if ( nh < 255 )
                header[nh++] = b[0];
              else
                {
                  STAGE = 0;
                  nerr++;
                  break;
                }
              if ( is_head_custom ( &b[0] , HEADER_MARK ) )
                {
                  STAGE = 2;
                  //header[nh++] = '\0';
                  //printf("Header: '%s'\n",header);
                }
              break;
            case 2: // header already finished
              if ( nh < 255 )
                header[nh++] = b[0];
              else
                {
                  STAGE = 0;
                  nerr++;
                  break;
                }
              if ( b[0] != 0x0a && b[0] != 0x0d )
                {
                  STAGE = 3;
                  memset ( &name[0], 0, 128 * sizeof ( unsigned char ) );
                  name[0] = b[0];
                  nx = 1;
                }
              break;
            case 3: // name already inited
              if ( nh < 255 )
                header[nh++] = b[0];
              else
                {
                  STAGE = 0;
                  nerr++;
                  break;
                }
              if ( nx >= 255 )
                {
                  STAGE = 0;
                  break;
                }
              if ( b[0] == 0x01a || b[0] == 0x0d )
                {
                  STAGE = 4;
                  name[nx++] = '\0';
                  //printf("Name: '%s'\n",name);
                }
              else if ( b[0] == ' ' )
                name[nx++] = '_'; // substitute a space by '_'
              else
                name[nx++] = b[0];
              break;
            case 4: // Waiting BUFR message begin
              if ( nh < 255 )
                header[nh++] = b[0];
              else
                {
                  STAGE = 0;
                  nerr++;
                  break;
                }
              if ( is_bufr ( &b[0] ) )
                {
                  STAGE = 5;
                  expected = 0;
                  // printf("Leyendo BUFR\n");
                  memset ( &BUFR[0], 0, BUFRLEN );
                  nb = 4;
                  BUFR[0]='B';
                  BUFR[1]='U';
                  BUFR[2]='F';
                  BUFR[3]='R';
                  nh -= 4;
                  header[nh] = '\0';
                }
              break;
            case 5: // Filling BUFR message till final '7777'
              if ( nb < ( BUFRLEN - 1 ) )
                BUFR[nb++] = b[0];
              else
                {
                  printf ( "Error: Bufr message length > %d", BUFRLEN );
                  fclose ( ficin );
                  exit ( EXIT_FAILURE );
                }
              // check expected length
              if ( nb == 7 )
                {
                  expected = ( unsigned int ) b[0] + ( unsigned int ) b[1] * 256 + ( unsigned int ) b[2] * 65536;
                }

              // Has been detected some void and fakes bufr
              if ( b[0] == '0' && // This is supossed to be the first '0' in the extension of next BUFR
                   b[1] == HEADER_MARK &&
                   b[2] == HEADER_MARK &&
                   b[3] == HEADER_MARK &&
                   b[4] == HEADER_MARK 
                 )
                {
                  // Ooops. a fake bufr
                  // it seems a new header has been found before '7777'
                  STAGE = 0;
                  nerr++;
                  break;
                }
              if ( nb == expected )
                {
                  if ( is_endb ( &b[0] ) )
                    {
                      STAGE = 0;
                      nbuf++;
                      if ( LISTF )
                        printf ( "%s\n", name );
                      if ( bufr_is_selected ( name ) )
                        {
                          nsel++;
                          if ( INDIVIDUAL )
                            {
                              // prefix with input file timestamp
                              date_mtime_from_stat ( namex, &INSTAT );
                              strcat ( namex,"_" );
                              strcat ( namex, name );
                              strcat ( namex, ".bufr" );
                              if ( ( ficout = fopen ( namex, "w" ) ) == NULL )
                                {
                                  printf ( "Error: cannot open %s\n", name );
                                  fclose ( ficin );
                                  exit ( EXIT_FAILURE );
                                }
                              if ( ( nw = fwrite ( &BUFR[0], sizeof ( unsigned char ), nb, ficout ) ) != nb )
                                {
                                  printf ( "Error: Writen %zu bytes instead of %zu in %s file\n", nw, nb, namex );
                                  fclose ( ficin );
                                  fclose ( ficout );
                                  exit ( EXIT_FAILURE );
                                }
                              // close an individual fileq
                              fclose ( ficout );
                              // change individual file timestamp
                              mtime_from_stat ( namex, &INSTAT );
                            }
                          if ( COLECT )
                            {
                              // first write header
                              if ( ( nw = fwrite ( &header[0], sizeof ( char ), nh, ficol ) ) != nh )
                                {
                                  printf ( "%s: Error: Writen %zu bytes instead of %zu in %s file\n", OWN, nw, nh, namec );
                                  fclose ( ficin );
                                  fclose ( ficol );
                                  exit ( EXIT_FAILURE );
                                }

                              // then bufr message
                              if ( ( nw = fwrite ( &BUFR[0], sizeof ( unsigned char ), nb, ficol ) ) != nb )
                                {
                                  printf ( "%s: Error: Writen %zu bytes instead of %zu in %s file\n", OWN, nw, nb, namex );
                                  fclose ( ficin );
                                  fclose ( ficout );
                                  exit ( EXIT_FAILURE );
                                }
                              // finally \r\r\n
                              if (FINAL_SEP[0])
                              {
                              if ( ( nw = fwrite ( &FINAL_SEP[0], sizeof ( char ), 3, ficol ) ) != 3 )
                                {
                                  printf ( "%s: Error: Writen %zu bytes instead of 3 chars separing messages in %s\n", OWN, nw, namex );
                                  fclose ( ficin );
                                  fclose ( ficout );
                                  exit ( EXIT_FAILURE );
                                }
                              }
                            }
                        }
                    }
                  else
                    {
                      // reached the expected end of BUFR without a '7777'
                      STAGE = 0;
                      nerr++;
                      break;
                    }
                }
            }
        }
    }

  if ( COLECT )
    {
      fclose ( ficol );
      // change archive file timestamp
      mtime_from_stat ( namec, &INSTAT );
    }

  // Final time
  gettimeofday ( &tfin, NULL );
  fclose ( ficin );

  // A brief stat output
  if ( VERBOSE )
    {
      double tx;
      printf ( "Found %zu bufr reports. Selected: %zu. Wrong: %zu\n", nbuf, nsel, nerr );
      timeval_substract ( &tt, &tfin, &tini );
      tx = ( double ) tt.tv_sec + ( double ) tt.tv_usec *1e-6;
      printf ( "%lf seg.  ", tx );
      if ( nbuf && tx != 0.0 )
        printf ( "%lf reports/sec.\n", ( double ) nbuf / tx );
      else
        printf ( "\n" );
    }
  exit ( EXIT_SUCCESS );
}

