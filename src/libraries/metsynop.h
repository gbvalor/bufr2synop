/* $Id$ */
/***************************************************************************
 *   Copyright (C) 2004-2015 by Guillermo Ballester Valor                  *
 *   gbv@ogimet.com                                                        *
 *                                                                         *
 *   This file is part of bufr2synop                                       *
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
   \file metsynop.h
   \brief Main header file for synop related tasks
*/
#ifndef METSYNOP_H
#define METSYNOP_H

#include "metcommon.h"

/*! \def SYNOP_SEC0
    \brief mask bit meaning section 0 or synop is solicited to or parsed with success
*/
#define SYNOP_SEC0 1

/*! \def SYNOP_SEC1
    \brief mask bit meaning section 1 or synop is solicited to or parsed with success
*/
#define SYNOP_SEC1 2

/*! \def SYNOP_SEC2
    \brief mask bit meaning section 2 or synop is solicited to or parsed with success
*/
#define SYNOP_SEC2 4

/*! \def SYNOP_SEC3
    \brief mask bit meaning section 3 or synop is solicited to or parsed with success
*/
#define SYNOP_SEC3 8

/*! \def SYNOP_SEC4
    \brief mask bit meaning section 4 or synop is solicited to or parsed with success
*/
#define SYNOP_SEC4 16

/*! \def SYNOP_SEC5
    \brief mask bit meaning section 5 or synop is solicited to or parsed with success
*/
#define SYNOP_SEC5 32

/*! \def SYNOP_EXT
    \brief mask bit meaning date extension is parsed with success
*/
#define SYNOP_EXT 64

/*! \def SYNOP_SEC3_8
    \brief mask bit meaning optional/regional 8 part, section 3 for synop is solicited to or parsed with success
*/
#define SYNOP_SEC3_8 128

/*! \def SYNOP_BUFR
    \brief mask bit meaning date synop is decoded from a BUFR 
*/
#define SYNOP_BUFR 256

/*! \def SYNOP_NNUB
    \brief number of optional nub3 struct to store the parsed results of 8.... groups
*/
#define SYNOP_NNUB 6

/*! \def SYNOP_NMISC
    \brief number of misc3 struct to store the parsed results of 9SpSpspsp groups
*/
#define SYNOP_NMISC 20

/*! \struct synop_sec0
    \brief contains all possible substrings from section 0 when a report is parsed with success
*/
struct synop_sec0
{
  char MiMi[4]; /*!< MiMi item. (Code table 2582) */
  char MjMj[4]; /*!< MjMj item. (Code table 2582) */
  char A1[2]; /*!< A1 item. WMO region */
  char bw[2]; /*!< bw item. WMO subregion */
  char nbnbnb[4]; /*!< kins and serial number for buoy */
  char D_D[10]; /*!< Ship signal or mobile land station indentifier*/
  char YY[4]; /*!< Day (UTC) of observation */
  char GG[4]; /*!< Hour (UTC) of observation */
  char iw[2]; /*!< Indicator for source and units used in speed wind data */
  char II[4]; /*!< Regional indicator for a synop station index */
  char iii[4]; /*!< Station index  */
  char Reg[4]; /*!< WMO IIiii region 'I','II','III', 'IV', 'V', ... */
  char IIIII[10]; /*!< Ship index name */
  char LaLaLa[4]; /*!< Latitude in degree tenths */
  char Qc[2]; /*!< Quadrant of the globe. (Code table 3333) */
  char LoLoLoLo[6]; /*!< Longitude in degree tenths */
  char MMM[4]; /*!< Number of Marsden square (Code table 2590) */
  char Ula[2]; /*!< Unit digit in the reported latitude. */
  char Ulo[2]; /*!< Units digit in the reported longitude. */
  char h0h0h0h0[6]; /*!< Elevation of a mobile land station  */
  char im[2]; /*!< Indicator for units of elevation, and confidence factor for accuracy of elevation. (Code table 1845) */
};

/*! \struct synop_sec1
    \brief contains all possible substrings from section 1 when a report is parsed with success

    To check if a substring has been parsed after a succeeded return we need to compute the
    lengh of the corresponding member. If zero the it is not parsed.
*/
struct synop_sec1
{
  char ir[2]; /*!< indicator for inclusion or omission of precipitation data. (Code table 1819) */
  char ix[2]; /*!< Indicator for type of station operation (manned or automatic) and for present and past weather data. (Code table 1860) */
  char h[2]; /*!< Height above surface of the base of the lowest cloud seen. (Code table 1600) */
  char VV[4]; /*!< Horizontal visibility at surface. (Code table 4377)  */
  char N[2]; /*!< Total cloud cover. (Code table 2700) */
  char dd[4]; /*!< True direction, in tens of degrees, from which wind is blowing (Code table 0877) */
  char ff[4]; /*!< Wind speed, in units indicated by iw. */
  char fff[4]; /*!< Wind speed, in units indicated by iw, of 99 units or more. */
  char sn1[2]; /*!< Sign of temperature (Code table 3845) */
  char TTT[4]; /*!< Air temperature, in tenths of a degree Celsius, its sign being given by sn1. */
  char sn2[2]; /*!< Sign of dewpoint temperature (Code table 3845)  */
  char TdTdTd[4]; /*!< Dew-point temperature, in tenths of a degree Celsius, its sign being given by sn2. */
  char UUU[4]; /*!< Relative humidity of the air, in per cent, the first figure being zero except for UUU = 100 percent.*/
  char PoPoPoPo[6]; /*!< Pressure at station level, in tenths of a hectopascal, omitting thousands digit of hectopascals of the pressure value. */
  char PPPP[6]; /*!< Pressure at mean sea level, in tenths of a hectopascal, omitting the thousands digit of hectopascals of the pressure value. */
  char a3[2]; /*!< Standard isobaric surface for which the geopotential is reported. (Code table 0264) */
  char hhh[4]; /*!< Geopotential of an agreed standard isobaric surface given by a3, in standard geopotential metres, omitting the thousands digit.*/
  char a[2]; /*!< Characteristic of pressure tendency during the three hours preceding the time of observation. (Code table 0200) */
  char ppp[4]; /*!< Amount of pressure tendency at station level during the three hours preceding the time of observation, expressed in tenths of a hectopascal. */
  char RRR[4]; /*!< Amount of precipitation which has fallen during the period preceding the time of observation, as indicated by tR. (Code table 3590) */
  char tr[2]; /*!< Duration of period of reference for amount of precipitation, ending at the time of the report. (Code table 4019) */
  char ww[4]; /*!< Present weather reported from a manned weather station. (Code table 4677) or
Present weather reported from an automatic weather station. (Code table 4680), depending on ix */
  char W1[2]; /*!< Past weather. (Code table 4561) or Past weather reported from an automatic weather station. (Code table 4531), depending on ix */
  char W2[2]; /*!< Past weather. (Code table 4561) or Past weather reported from an automatic weather station. (Code table 4531), depending on ix */
  char Nh[2]; /*!< Amount of all the CL cloud present or, if no CL cloud is present, the amount of all the CM cloud present. (Code table 2700) */
  char Cl[2]; /*!< Clouds of the genera Stratocumulus, Stratus, Cumulus and Cumulonimbus. (Code table 0513) */
  char Cm[2]; /*!< Clouds of the genera Altocumulus, Altostratus and Nimbostratus. (Code table 0515) */
  char Ch[2]; /*!< Clouds of the genera Cirrus, Cirrocumulus and Cirrostratus. (Code table 0509)  */
  char GG[4]; /*!< Actual time of observation, to the nearest whole hour UTC. */
  char gg[4]; /*!< Actual minutes of observation, to the nearest minute UTC.  */
};


/*! \struct synop_sec2
    \brief contains all possible substrings from section 2 when a report is parsed with success (still incomplete)

    To check if a substring has been parsed after a succeeded return we need to compute the
    lengh of the corresponding member. If zero the it is not parsed.
*/
struct synop_sec2
{
  char Ds[2]; /*!< True direction of resultant displacement of the ship during the three hours preceding the time of observation. (Code table 0700) */
  char vs[2]; /*!< Ship’s average speed made good during the three hours preceding the time of observation. (Code table 4451) */
  char ss[2]; /*!< Sign of Sea-surface temperature */
  char TwTwTw[6]; /*!< Sea-surface temperature, in tenths of degrees Celsius, its sign being given by ss.*/
  char PwaPwa[4]; /*!< Period of wind waves, obtained by instrumental methods, in seconds. */
  char HwaHwa[4]; /*!< Heigh of wind waves, in units of 0.5 metre. */
  char PwPw[4]; /*!< Period of waves, in seconds. */
  char HwHw[4]; /*!< Height of waves, in units of 0.5 metre.*/
  char dw1dw1[4]; /*!< Direction of swell waves, in units of 0.5 metre.*/
  char dw2dw2[4]; /*!< Direction of swell waves, in units of 0.5 metre.*/
  char Pw1Pw1[4]; /*!< Period of swell waves, in seconds.*/
  char Hw1Hw1[4]; /*!< Height of swell waves, in units of 0.5 metre. */
  char Pw2Pw2[4]; /*!< Period of swell waves, in seconds. */
  char Hw2Hw2[4]; /*!< Height of swell waves, in units of 0.5 metre. */
  char ICING[32]; /*!< String with icing description in clear text */
  char HwaHwaHwa[6];  /*!< heigh of wind waves, in tenths of metre */
  char sw[2]; /*!< Sign of wet bulb temperature */
  char TbTbTb[6]; /*!< Wet-bulb temperature, in tenths of degrees Celsius, its sign being given by sw. */
  char ICE[32]; /*!< String with ice description in clear text */
};

/*!
   \struct nub3
   \brief struct with info for a cloud group at section 3
*/
struct nub3
{
  char Ns[2]; /*!< Amount of individual cloud layer or mass whose genus is indicated by C. (Code table 2700) */
  char C[2]; /*!< Genus of cloud. (Code table 0500) */
  char hshs[4]; /*!< Height of base of cloud layer or mass whose genus is indicated by C. (Code table 1677) */
};

/*!
   \struct misc3
   \brief struct with mixed info for group SpSpspsp at section at section 3.
*/
struct misc3
{
  char SpSp[4]; /*!< Supplementary information. (Code table 3778) */
  char spsp[4]; /*!< Supplementary information. (Code table 3778) */
};

struct data9
{
  size_t n; /*!< amount of used structs in array */
  struct misc3 misc[SYNOP_NMISC]; /*!< array of optional \ref misc3 struct */
};

/*!
    \struct synop_sec3
    \brief contains most of substrings from section 3 when a report is parsed with success

    To check if a substring has been parsed after a succeeded return we need to compute the
    lengh of the corresponding member. If zero the it is not parsed.
*/
struct synop_sec3
{
  char XoXoXoXo[6]; /*!< regional optative information */
  char snx[2]; /*!< Sign of Maximum temperature (Code table 3845)  */
  char TxTxTx[4]; /*!< Maximum air temperature, in tenths of a degree Celsius, its sign being given by snx.   */
  char snn[2]; /*!< Sign of Minimum temperature (Code table 3845) */
  char TnTnTn[4]; /*!< Minimum air temperature, in tenths of a degree Celsius, its sign being given by snn. */
  char E[2]; /*!< State of the ground without snow or measurable ice cover. (Code table 0901) */
  char jjj[4]; /*!< Supplementary information to be developed regionally (see Volume II). */
  char E1[2]; /*!< State of the ground with snow or measurable ice cover. (Code table 0975) */
  char sss[4]; /*!< Total depth of snow. (Code table 3889) */
  char EEE[4]; /*!< Amount of either evaporation or evapotranspiration, in tenths of a millimetre, during the preceding 24 hours. */
  char ie[2]; /*!< Indicator of type of instrumentation for evaporation measurement or type of crop for which evapotranspiration is reported. (Code table 1806) */
  char go[2]; /*!< Period of time, in hours, between the time of the observation and the time of the wind change, the time of occurrence of the maximum mean wind speed, or the time of tempera-
ture change. */
  char snt[2]; /*!< Sign of temperature change */
  char dt[2]; /*!<  Amount of temperature change, the sign of the change being given by snt. (Code table 0822) */
  char SSS[4]; /*!< Duration of sunshine, in hours and tenths of an hour. */
  char SS[4]; /*!< Duration of sunshine in the past hour, in tenths of an hour. */
  char FFFF407[6]; /*!< Net short radiation, in kilojoules per square meter, over precedent 1 hour period */
  char FFFF408[6]; /*!< Direct solar radiation, in kilojoules per square meter, over precedent 1 hour period */
  char FFFF507[6]; /*!< Net short radiation, in Joules per square cm, over precedent 24 hour period */
  char FFFF508[6]; /*!< Direct solar radiation, in Joules per square cm, over precedent 24 hour period */
  char j5[7][2]; /*!<  Suplementary info index for 1-hour period radiation data */
  char FFFF[7][6]; /*!< Amount of radiation, in kilojoules per square meter, over a 1-hour period. */
  char j524[7][2]; /*!<  Suplementary info index for 24-hour period radiation data */
  char FFFF24[7][6]; /*!< Amount of radiation, in joules per square centimetre, over a 24-hour period. */
  char Dl[2]; /*!< True direction from which CL clouds are moving. (Code table 0700)   */
  char Dm[2]; /*!< True direction from which Cm clouds are moving. (Code table 0700)   */
  char Dh[2]; /*!< True direction from which Ch clouds are moving. (Code table 0700)   */
  char C[2]; /*!< Genus of cloud. (Code table 0500)  */
  char Da[2]; /*!< True direction in which orographic clouds or clouds with vertical development are seen. (Code table 0700) */
  char ec[2]; /*!< Elevation angle of the top of the cloud indicated by C. (Code table 1004) */
  char snp24[2]; /*!< Sign of pressure change */
  char ppp24[4]; /*!< Amount of surface pressure change during last 24 hours either positive, zero or negative, in tenths of a hectopascal.*/
  char RRR[4]; /*!< Amount of precipitation which has fallen during the period preceding the time of observation, as indicated by tR. (Code table 3590) */
  char tr[2]; /*!< Duration of period of reference for amount of precipitation, ending at the time of the report. (Code table 4019)  */
  char RRRR24[6]; /*!< Total amount of precipitation during the 24-hour period ending at the time of observation, in tenths of a millimetre. */
  char R8[SYNOP_NMISC][6]; /*!< Optional regional groups after 80000 */
  struct nub3 nub[SYNOP_NNUB]; /*!< array of optional nub3 structs  */
  struct data9 d9; /*!< struct with optional 9SpSpss items */
};

/*!
  \struct synop_sec4
  \brief contains some of substrings from section 4 when a report is parsed with success
*/
struct synop_sec4
{
  char N1[2]; /*!< Amount of cloud whose base is below the level of the station. (Code table 2700) */
  char C1[2]; /*!< Genus of cloud whose base is below the level of the station. (Code table 0500) */
  char H1H1[4]; /*!< Altitude of the upper surface of clouds reported by C, in hundreds of metres */
  char Ct[2]; /*!< Description of the top of cloud whose base is below the level of the station. (Code table 0552) */
};

/*!
    \struct synop_sec5
    \brief contains some of substrings from section 5 when a report is parsed with success

    To check if a substring has been parsed after a succeeded return we need to compute the
    lengh of the corresponding member. If zero the it is not parsed.
*/
struct synop_sec5
{
  char RRR[4]; /*!< Amount of precipitation which has fallen during the period preceding the time of observation, as indicated by tR. (Code table 3590) */
  char tr[2]; /*!< Duration of period of reference for amount of precipitation, ending at the time of the report. (Code table 4019)  */
  struct data9 d9; /*!< struct with optional 9SpSpss items */
};

/*! \struct synop_chunks
    \brief contains all possible substrings from a synop report is parsed with success
*/
struct synop_chunks
{
  int mask; /*!< Mask with sections parsed succesfully */
  struct report_date_ext e; /*!< struct with the optional extension parsed */
  struct synop_sec0 s0; /*!< struct with parsed header and section 0 */
  struct synop_sec1 s1; /*!< struct with parsed section 1 */
  struct synop_sec2 s2; /*!< struct with parsed section 2 if present */
  struct synop_sec3 s3; /*!< struct with parsed section 3 if present */
  struct synop_sec4 s4; /*!< struct with parsed section 4 if present */
  struct synop_sec5 s5; /*!< struct with parsed section 5 if present */
  char error[128]; /*!< string with error code if a wrong synop report is found */
};


/*!
  \struct wmo_index_estado
  \brief contains an index interval associated with a country
*/
struct wmo_index_estado
{
  char first[8]; /*!< first index of interval */
  char last[8]; /*!< last index of interval */
  char estado[128]; /*! country assigned to the interval */
};

void clean_report_date_ext ( struct report_date_ext *s );
void clean_synop_chunks ( struct synop_chunks *syn );
void clean_syn_sec0 ( struct synop_sec0 *s );
void clean_syn_sec1 ( struct synop_sec1 *s );
void clean_syn_sec2 ( struct synop_sec2 *s );
void clean_syn_sec3 ( struct synop_sec3 *s );
void clean_syn_sec4 ( struct synop_sec4 *s );
void clean_syn_sec5 ( struct synop_sec5 *s );

#endif

