/* $Id$ */
/***************************************************************************
 *   Copyright (C) 2004-2018 by Guillermo Ballester Valor                  *
 *   gbv@ogimet.com                                                        *
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
   \file metclimat.h
   Main header file for climat related tasks in metdecoder library
*/
#ifndef METCLIMAT_H
#define METCLIMAT_H

/*! \def CLIMAT_SEC0
    \brief mask bit meaning section 0 of climat is solicited to or parsed with success
*/
#define CLIMAT_SEC0 1

/*! \def CLIMAT_SEC1
    \brief mask bit meaning section 1 of climat is solicited to or parsed with success
*/
#define CLIMAT_SEC1 2

/*! \def CLIMAT_SEC2
    \brief mask bit meaning section 2 of climat is solicited to or parsed with success
*/
#define CLIMAT_SEC2 4

/*! \def CLIMAT_SEC3
    \brief mask bit meaning section 3 of climat is solicited to or parsed with success
*/
#define CLIMAT_SEC3 8

/*! \def CLIMAT_SEC4
    \brief mask bit meaning section 4 of climat is solicited to or parsed with success
*/
#define CLIMAT_SEC4 16

/*! \def CLIMAT_EXT
    \brief mask bit meaning date extension is parsed with success
*/
#define CLIMAT_EXT 32

/*! \def CLIMAT_OLD
    \brief mask bit meaning that the climat is an old CLIMAT, usually before Nov 1994
*/
#define CLIMAT_OLD 64


/*! \struct climat_sec0
    \brief contains all possible substrings from section 0 when a report is parsed with success
*/
struct climat_sec0
{
  char MM[4]; /*! MM item (month 01..12) */
  char JJJ[4]; /*! JJJ item (latest 3 digits of year)  */
  char II[4]; /*!< Regional indicator for a synop station index */
  char iii[4]; /*!< Station index  */
  char Reg[4]; /*!< WMO IIiii region 'I','II','III', 'IV', 'V', ... */
};

/*! \struct climat_sec1
    \brief contains all possible substrings from section 1 when a report is parsed with success
*/
struct climat_sec1
{
  char PoPoPoPo[8];
  char PPPP[8];
  char s[2];
  char TTT[4];
  char ststst[4];
  char sx[2];
  char TxTxTx[4];
  char sn[2];
  char TnTnTn[4];
  char eee[4];
  char R1R1R1R1[6];
  char Rd[2];
  char nrnr[4];
  char S1S1S1[4];
  char pspsps[4];
  char mpmp[4];
  char mtmt[4];
  char mtx[2];
  char mtn[2];
  char meme[4];
  char mrmr[4];
  char msms[4];
};

/*! \struct climat_sec2
    \brief contains all possible substrings from section 2 when a report is parsed with success
*/
struct climat_sec2
{
  char YbYb[4];
  char YcYc[4];
  char PoPoPoPo[8];
  char PPPP[8];
  char s[2];
  char TTT[4];
  char ststst[4];
  char sx[2];
  char TxTxTx[4];
  char sn[2];
  char TnTnTn[4];
  char eee[4];
  char R1R1R1R1[6];
  char nrnr[4];
  char S1S1S1[4];
  char ypyp[4];
  char ytyt[4];
  char ytxytx[4];
  char yeye[4];
  char yryr[4];
  char ysys[4];
};

/*! \struct climat_sec3
    \brief contains all possible substrings from section 3 when a report is parsed with success
*/
struct climat_sec3
{
  char T25[4];
  char T30[4];
  char T35[4];
  char T40[4];
  char Tn0[4];
  char Tx0[4];
  char R01[4];
  char R05[4];
  char R10[4];
  char R50[4];
  char R100[4];
  char R150[4];
  char s00[4];
  char s01[4];
  char s10[4];
  char s50[4];
  char f10[4];
  char f20[4];
  char f30[4];
  char V1[4];
  char V2[4];
  char V3[4];
};

/*! \struct climat_sec4
    \brief contains all possible substrings from section 4 when a report is parsed with success
*/
struct climat_sec4
{
  char sx[2];
  char Txd[4];
  char yx[4];
  char sn[2];
  char Tnd[4];
  char yn[4];
  char sax[2];
  char Tax[4];
  char yax[4];
  char san[2];
  char Tan[4];
  char yan[4];
  char RxRxRxRx[8];
  char yr[4];
  char iw[2];
  char fxfxfx[4];
  char yfx[4];
  char Dts[4];
  char Dgr[4];
  char iy[2];
  char GxGx[4];
  char GnGn[4];
};

/*! \struct climat_old
    \brief contains all possible substrings from old CLIMAT format when a report is parsed with success
*/
struct climat_old
{
  char PoPoPoPo[6];
  char PPPP[6];
  char sn[2];
  char TTT[4];
  char eee[4];
  char nrnr[4];
  char R1R1R1R1[6];
  char Rd[4];
  char S1S1S1[4];
  char KsKs[4];
};

/*! \struct climat_chunks
    \brief contains all possible substrings from a synop report is parsed with success
*/
struct climat_chunks
{
  int mask; /*!< Mask with sections parsed succesfully */
  struct report_date_ext e; /*!< struct with the optional extension parsed */
  struct climat_sec0 s0; /*!< struct with parsed header and section 0 */
  struct climat_sec1 s1; /*!< struct with parsed section 1 */
  struct climat_sec2 s2; /*!< struct with parsed section 2 if present */
  struct climat_sec3 s3; /*!< struct with parsed section 3 if present */
  struct climat_sec4 s4; /*!< struct with parsed section 4 if present */
  struct climat_old o; /*!< struct with parsed old format if present */
  char error[128]; /*!< string with error code if a wrong climat report is found */
};





#endif

