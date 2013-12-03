/* $Id$ */
/***************************************************************************
 *   Copyright (C) 2004-2009 by Guillermo Ballester Valor                  *
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
   \file metbuoy.h
   Main header file for buoy related tasks in metdecoder library
*/
#ifndef METBUOY_H
#define METBUOY_H


/*! \def BUOY_SEC0 
    \brief mask bit meaning section 0 of buoy is solicited to or parsed with success
*/
#define BUOY_SEC0 1

/*! \def BUOY_SEC1 
    \brief mask bit meaning section 1 of buoy is solicited to or parsed with success
*/
#define BUOY_SEC1 2

/*! \def BUOY_SEC2 
    \brief mask bit meaning section 2 of buoy is solicited to or parsed with success
*/
#define BUOY_SEC2 4

/*! \def BUOY_SEC3 
    \brief mask bit meaning section 3 of buoy is solicited to or parsed with success
*/
#define BUOY_SEC3 8

/*! \def BUOY_SEC4 
    \brief mask bit meaning section 4 of buoy is solicited to or parsed with success
*/
#define BUOY_SEC4 16

/*! \def BUOY_EXT 
    \brief mask bit meaning date extension is parsed with success
*/
#define BUOY_EXT 32


/*! \struct buoy_sec0
    \brief contains all possible substrings from section 0 when a report is parsed with success
*/
struct buoy_sec0
{
   char MiMi[4]; /*!< MiMi item. (Code table 2582) */
   char MjMj[4]; /*!< MjMj item. (Code table 2582) */
   char A1[2]; /*!< A1 item. WMO region */
   char bw[2]; /*!< bw item. WMO subregion */
   char nbnbnb[4]; /*!< kins and serial number for buoy */
   char D_D[8]; /*!< Ship signal*/
   char YY[4]; /*!< Day (UTC) of observation */
   char MM[4]; /*!< Month (UTC) of observation 01, 02 .... 12 */
   char J[4]; /*!< Least significant digit of year */
   char GG[4]; /*!< Hour (UTC) of observation */
   char gg[4]; /*!< Minute (UTC) of observation */
   char iw[2]; /*!< Indicator for source and units used in speed wind data */
   char Qc[2]; /*!< Quadrant of the Globe (Code table 3333) */
   char LaLaLaLaLa[8]; /*!< Latitude, precision up to in 0.001 degree */
   char LoLoLoLoLoLo[8]; /*!< Longitude, precision up to 0.001 degree */
   char Ql[2]; /*!< Quality control indicator for position (Code Table 3334) */
   char Qt[2]; /*!< Quality control for time (Code Table 3334) */
   char QA[2]; /*!< Location quality class (Code table 3302) */
};

/*! \struct buoy_sec1
    \brief contains all possible substrings from section 1 when a report is parsed with success
*/
struct buoy_sec1
{
   char Qd[2]; /*!< Quality control indicatitor (Code Table 3334) */
   char Qx[2]; /*!< Indicator of position of group (Regulation 18.3.3) */
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
   char a[2]; /*!< Characteristic of pressure tendency during the three hours preceding the time of observation. (Code table 0200) */
   char ppp[4]; /*!< Amount of pressure tendency at station level during the three hours preceding the time of observation, expressed in tenths of a hectopascal. */
};

/*! \struct buoy_sec2
    \brief contains all possible substrings from section 1 when a report is parsed with success
*/
struct buoy_sec2
{
   char Qd[2]; /*!< Quality control indicatitor (Code Table 3334) */
   char Qx[2]; /*!< Indicator of position of group (Regulation 18.3.3) */
   char sn[2]; /*!< Sign of temperature (Code table 3845) */
   char TwTwTw[4]; /*!< Sea surfacer temperature, in tenths of a degree Celsius, its sign being given by sn. */
   char PwPw[4]; /*!< Period of waves, in seconds */
   char HwHw[4]; /*!< heigh of waves, in units of 0.5 metre */
   char PwPwPw[4]; /*!< Period of waves, in tenths of a second */
   char HwHwHw[4]; /*!< heigh of waves, in tenths of metre */
};

/*! \struct buoy_chunks
    \brief contains all possible substrings from a synop report is parsed with success
*/
struct buoy_chunks
{
   int mask; /*!< Mask with sections parsed succesfully */
   struct report_date_ext e; /*!< struct with the optional extension parsed */
   struct buoy_sec0 s0; /*!< struct with parsed header and section 0 */
   struct buoy_sec1 s1; /*!< struct with parsed section 1 */
   struct buoy_sec2 s2; /*!< struct with parsed section 2 if present */
   char error[128]; /*!< string with error code if a wrong synop report is found */
};

#endif
 
