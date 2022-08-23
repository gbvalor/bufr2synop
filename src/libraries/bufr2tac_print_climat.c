/***************************************************************************
 *   Copyright (C) 2013-2018 by Guillermo Ballester Valor                  *
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
 \file bufr2tac_print_climat.c
 \brief file with the code to print the results for climat reports
 */
#include "bufr2tac.h"

/*!
  \def check_len(ori,inc)
  \brief cheks if there still memory enough to add \a inc chars
*/
#define check_len(ori,inc) (c - *ori + inc < (int)lmax)


/*!
  \fn char * print_climat_sec0 (char **sec0, size_t lmax, struct climat_chunks *cl)
  \brief Prints the climat section 0 (header)
  \param sec0 the pointer where to print section
  \param lmax max length permited
  \param cl pointer to s atruct \ref climat_chunks where the parse results are set
*/
char * print_climat_sec0 ( char **sec0, size_t lmax, struct climat_chunks *cl )
{
  char *c = *sec0;

  if ( check_len ( sec0,12 ) )
    {
      c += sprintf ( c, "%s%s%s%s%s", cl->e.YYYY, cl->e.MM, cl->e.DD, cl->e.HH, cl->e.mm );
    }

  // Print type
  if ( check_len ( sec0,6 ) )
    {
      c += sprintf ( c, " CLIMAT" );
    }

  // print MMJJJ
  if ( check_len ( sec0,8 ) )
    {
      c += sprintf ( c, " %s%s", cl->s0.MM, cl->s0.JJJ );
    }

  // print IIiii
  if ( check_len ( sec0,6 ) && cl->s0.II[0] )
    {
      c += sprintf ( c, " %s%s", cl->s0.II, cl->s0.iii );
    }

  *sec0 = c;
  return *sec0;
}

/*!
  \fn char * print_climat_sec1 (char **sec1, size_t lmax, struct climat_chunks *cl)
  \brief Prints the climat section 1
  \param sec1 the pointer where to print section
  \param lmax max length permited
  \param cl pointer to s atruct \ref climat_chunks where the parse results are set
*/
char * print_climat_sec1 ( char **sec1, size_t lmax, struct climat_chunks *cl )
{
  char *c = *sec1;

  if ( cl->mask & SYNOP_SEC1 )
    {
      if ( check_len ( sec1,4 ) )
        {
          c += sprintf ( c, " 111" );
        }

      if ( check_len ( sec1,6 ) )
        {
          if ( cl->s1.PoPoPoPo[0] )
            {
              c += sprintf ( c, " 1%s", cl->s1.PoPoPoPo );
            }
        }

      if ( check_len ( sec1,6 ) )
        {
          if ( cl->s1.PPPP[0] )
            {
              c += sprintf ( c, " 2%s", cl->s1.PPPP );
            }
        }

      if ( check_len ( sec1,9 ) )
        {
          if ( cl->s1.TTT[0] || cl->s1.ststst[0] )
            {
              if ( cl->s1.TTT[0] == 0 )
                {
                  strcpy ( cl->s1.s, "/" );
                  strcpy ( cl->s1.TTT, "///" );
                }
              if ( cl->s1.ststst[0] == 0 )
                {
                  strcpy ( cl->s1.ststst, "///" );
                }
              c += sprintf ( c, " 3%s%s%s", cl->s1.s, cl->s1.TTT, cl->s1.ststst );
            }
        }

      if ( check_len ( sec1,10 ) )
        {
          if ( cl->s1.TxTxTx[0] || cl->s1.TnTnTn[0] )
            {
              if ( cl->s1.TxTxTx[0] == 0 )
                {
                  strcpy ( cl->s1.sx, "/" );
                  strcpy ( cl->s1.TxTxTx, "///" );
                }
              if ( cl->s1.TnTnTn[0] == 0 )
                {
                  strcpy ( cl->s1.sn, "/" );
                  strcpy ( cl->s1.TnTnTn, "///" );
                }
              c += sprintf ( c, " 4%s%s%s%s", cl->s1.sx, cl->s1.TxTxTx, cl->s1.sn, cl->s1.TnTnTn );
            }
        }

      if ( check_len ( sec1,5 ) )
        {
          if ( cl->s1.eee[0] )
            {
              c += sprintf ( c, " 5%s", cl->s1.eee );
            }
        }

      if ( check_len ( sec1,9 ) )
        {
          if ( cl->s1.R1R1R1R1[0] ||  cl->s1.Rd[0] ||  cl->s1.nrnr[0] )
            {
              if ( cl->s1.R1R1R1R1[0] == 0 )
                {
                  strcpy ( cl->s1.R1R1R1R1, "////" );
                }
              if ( cl->s1.Rd[0] == 0 )
                {
                  strcpy ( cl->s1.Rd, "/" );
                }
              if ( cl->s1.nrnr[0] == 0 )
                {
                  strcpy ( cl->s1.nrnr, "//" );
                }
              c += sprintf ( c, " 6%s%s%s", cl->s1.R1R1R1R1, cl->s1.Rd, cl->s1.nrnr );
            }
        }

      if ( check_len ( sec1,8 ) )
        {
          if ( cl->s1.S1S1S1[0] ||  cl->s1.pspsps[0] )
            {
              if ( cl->s1.S1S1S1[0] == 0 )
                {
                  strcpy ( cl->s1.S1S1S1, "///" );
                }
              if ( cl->s1.pspsps[0] == 0 )
                {
                  strcpy ( cl->s1.pspsps, "///" );
                }
              c += sprintf ( c, " 7%s%s", cl->s1.S1S1S1, cl->s1.pspsps );
            }
        }

      if ( check_len ( sec1,8 ) )
        {
          if ( cl->s1.mpmp[0] || cl->s1.mtmt[0] ||  cl->s1.mtx[0] ||  cl->s1.mtn[0] )
            {
              if ( cl->s1.mpmp[0] == 0 )
                {
                  strcpy ( cl->s1.mpmp, "//" );
                }
              if ( cl->s1.mtmt[0] == 0 )
                {
                  strcpy ( cl->s1.mtmt, "//" );
                }
              if ( cl->s1.mtx[0] == 0 )
                {
                  strcpy ( cl->s1.mtx, "/" );
                }
              if ( cl->s1.mtn[0] == 0 )
                {
                  strcpy ( cl->s1.mtn, "/" );
                }
              c += sprintf ( c, " 8%s%s%s%s", cl->s1.mpmp, cl->s1.mtmt, cl->s1.mtx, cl->s1.mtn );
            }
        }

      if ( check_len ( sec1,8 ) )
        {
          if ( cl->s1.meme[0] || cl->s1.mrmr[0] || cl->s1.msms[0] )
            {
              if ( cl->s1.meme[0] == 0 )
                {
                  strcpy ( cl->s1.meme, "//" );
                }
              if ( cl->s1.mrmr[0] == 0 )
                {
                  strcpy ( cl->s1.mrmr, "//" );
                }
              if ( cl->s1.msms[0] == 0 )
                {
                  strcpy ( cl->s1.msms, "//" );
                }
              c += sprintf ( c, " 9%s%s%s", cl->s1.meme, cl->s1.mrmr, cl->s1.msms );
            }
        }

    }

  *sec1 = c;
  return *sec1;
}

/*!
  \fn char * print_climat_sec2 (char **sec2, size_t lmax, struct climat_chunks *cl)
  \brief Prints the climat section 2
  \param sec2 the pointer where to print section
  \param lmax max length permited
  \param cl pointer to s atruct \ref climat_chunks where the parse results are set
*/
char * print_climat_sec2 ( char **sec2, size_t lmax, struct climat_chunks *cl )
{
  char *c = *sec2, *c0 = *sec2;

  if ( cl->mask & SYNOP_SEC2 )
    {
      //c += sprintf ( c, "\r\n      222" );
      c += sprintf ( c, " 222" );

      // init point to write info.
      // in case we finally write nothing in this section
      c0 = c;

      if ( check_len ( sec2,6 ) && ( cl->s2.YbYb[0] || cl->s2.YcYc[0] ) )
        {
          if ( cl->s2.YbYb[0] == 0 )
            {
              strcpy ( cl->s2.YbYb, "//" );
            }
          if ( cl->s2.YcYc[0] == 0 )
            {
              strcpy ( cl->s2.YcYc, "//" );
            }
          c += sprintf ( c, " 0%s%s", cl->s2.YbYb, cl->s2.YcYc );
        }

      if ( check_len ( sec2,6 ) && cl->s2.PoPoPoPo[0] )
        {
          c += sprintf ( c, " 1%s", cl->s2.PoPoPoPo );
        }

      if ( check_len ( sec2,6 ) && cl->s2.PPPP[0] )
        {
          c += sprintf ( c, " 2%s", cl->s2.PPPP );
        }

      if ( check_len ( sec2,9 ) && ( cl->s2.s[0] || cl->s2.TTT[0] || cl->s2.ststst[0] ) )
        {
          if ( cl->s2.s[0] == 0 )
            {
              strcpy ( cl->s2.s, "/" );
            }
          if ( cl->s2.TTT[0] == 0 )
            {
              strcpy ( cl->s2.TTT, "///" );
            }
          if ( cl->s2.ststst[0] == 0 )
            {
              strcpy ( cl->s2.ststst, "///" );
            }

          c += sprintf ( c, " 3%s%s%s", cl->s2.s, cl->s2.TTT, cl->s2.ststst );
        }

      if ( check_len ( sec2,10 ) && ( cl->s2.sx[0] || cl->s2.TxTxTx[0] || cl->s2.sn[0] || cl->s2.TnTnTn[0] ) )
        {
          if ( cl->s2.sx[0] == 0 )
            {
              strcpy ( cl->s2.sx, "/" );
            }
          if ( cl->s2.TxTxTx[0] == 0 )
            {
              strcpy ( cl->s2.TxTxTx, "///" );
            }
          if ( cl->s2.sn[0] == 0 )
            {
              strcpy ( cl->s2.sn, "/" );
            }
          if ( cl->s2.TnTnTn[0] == 0 )
            {
              strcpy ( cl->s2.TnTnTn, "///" );
            }

          c += sprintf ( c, " 4%s%s%s%s", cl->s2.sx, cl->s2.TxTxTx, cl->s2.sn, cl->s2.TnTnTn );
        }

      if ( check_len ( sec2,5 ) && cl->s2.eee[0] )
        {
          c += sprintf ( c, " 5%s", cl->s2.eee );
        }

      if ( check_len ( sec2,8 ) && ( cl->s2.R1R1R1R1[0] || cl->s2.nrnr[0] ) )
        {
          if ( cl->s2.R1R1R1R1[0] == 0 )
            {
              strcpy ( cl->s2.R1R1R1R1, "////" );
            }
          if ( cl->s2.nrnr[0] == 0 )
            {
              strcpy ( cl->s2.nrnr, "//" );
            }
          c += sprintf ( c, " 6%s%s", cl->s2.R1R1R1R1, cl->s2.nrnr );
        }

      if ( check_len ( sec2,5 ) && cl->s2.S1S1S1[0] )
        {
          c += sprintf ( c, " 7%s", cl->s2.S1S1S1 );
        }

      if ( check_len ( sec2,8 ) && ( cl->s2.ypyp[0] || cl->s2.ytyt[0] || cl->s2.ytxytx[0] ) )
        {
          if ( cl->s2.ypyp[0] == 0 )
            {
              strcpy ( cl->s2.ypyp, "//" );
            }
          if ( cl->s2.ytyt[0] == 0 )
            {
              strcpy ( cl->s2.ytyt, "//" );
            }
          if ( cl->s2.ytxytx[0] == 0 )
            {
              strcpy ( cl->s2.ytxytx, "//" );
            }

          c += sprintf ( c, " 8%s%s%s", cl->s2.ypyp, cl->s2.ytyt, cl->s2.ytxytx );
        }

      if ( check_len ( sec2,8 ) && ( cl->s2.yeye[0] || cl->s2.yryr[0] || cl->s2.ysys[0] ) )
        {
          if ( cl->s2.yeye[0] == 0 )
            {
              strcpy ( cl->s2.yeye, "//" );
            }
          if ( cl->s2.yryr[0] == 0 )
            {
              strcpy ( cl->s2.yryr, "//" );
            }
          if ( cl->s2.ysys[0] == 0 )
            {
              strcpy ( cl->s2.ysys, "//" );
            }

          c += sprintf ( c, " 9%s%s%s", cl->s2.yeye, cl->s2.yryr, cl->s2.ysys );
        }

    }

  if ( c != c0 )
    {
      *sec2 = c;
    }
  return *sec2;

}


/*!
  \fn char * print_climat_sec3 (char **sec3, size_t lmax, struct climat_chunks *cl)
  \brief Prints the climat section 3
  \param sec3 the pointer where to print section
  \param lmax max length permited
  \param cl pointer to s atruct \ref climat_chunks where the parse results are set
*/
char * print_climat_sec3 ( char **sec3, size_t lmax, struct climat_chunks *cl )
{
  char *c = *sec3, *c0 = *sec3;

  if ( cl->mask & SYNOP_SEC3 )
    {
      //c += sprintf ( c, "\r\n      333" );
      c += sprintf ( c, " 333" );

      // init point to write info.
      // in case we finally write nothing in this section
      c0 = c;

      if ( check_len ( sec3,8 ) &&
           (
             ( cl->s3.T25[0] && strcmp ( cl->s3.T25, "00" ) ) ||
             ( cl->s3.T30[0] && strcmp ( cl->s3.T30, "00" ) )
           )
         )
        {
          if ( cl->s3.T25[0] == 0 )
            {
              strcpy ( cl->s3.T25,"//" );
            }
          if ( cl->s3.T30[0] == 0 )
            {
              strcpy ( cl->s3.T30,"//" );
            }
          c += sprintf ( c, " 0%s%s", cl->s3.T25, cl->s3.T30 );
        }

      if ( check_len ( sec3,8 ) &&
           (
             ( cl->s3.T35[0] && strcmp ( cl->s3.T35, "00" ) ) ||
             ( cl->s3.T40[0] && strcmp ( cl->s3.T40, "00" ) )
           )
         )
        {
          if ( cl->s3.T35[0] == 0 )
            {
              strcpy ( cl->s3.T35,"//" );
            }
          if ( cl->s3.T40[0] == 0 )
            {
              strcpy ( cl->s3.T40,"//" );
            }
          c += sprintf ( c, " 1%s%s", cl->s3.T35, cl->s3.T40 );
        }

      if ( check_len ( sec3,8 ) &&
           (
             ( cl->s3.Tn0[0] && strcmp ( cl->s3.Tn0, "00" ) ) ||
             ( cl->s3.Tx0[0] && strcmp ( cl->s3.Tx0, "00" ) )
           )
         )
        {
          if ( cl->s3.Tn0[0] == 0 )
            {
              strcpy ( cl->s3.Tn0,"//" );
            }
          if ( cl->s3.Tx0[0] == 0 )
            {
              strcpy ( cl->s3.Tx0,"//" );
            }
          c += sprintf ( c, " 2%s%s", cl->s3.Tn0, cl->s3.Tx0 );
        }

      if ( check_len ( sec3,8 ) &&
           (
             ( cl->s3.R01[0] && strcmp ( cl->s3.R01, "00" ) ) ||
             ( cl->s3.R05[0] && strcmp ( cl->s3.R05, "00" ) )
           )
         )
        {
          if ( cl->s3.R01[0] == 0 )
            {
              strcpy ( cl->s3.R01,"//" );
            }
          if ( cl->s3.R05[0] == 0 )
            {
              strcpy ( cl->s3.R05,"//" );
            }
          c += sprintf ( c, " 3%s%s", cl->s3.R01, cl->s3.R05 );
        }

      if ( check_len ( sec3,8 ) &&
           (
             ( cl->s3.R10[0] && strcmp ( cl->s3.R10, "00" ) ) ||
             ( cl->s3.R50[0] && strcmp ( cl->s3.R50, "00" ) )
           )
         )
        {
          if ( cl->s3.R10[0] == 0 )
            {
              strcpy ( cl->s3.R10,"//" );
            }
          if ( cl->s3.R50[0] == 0 )
            {
              strcpy ( cl->s3.R50,"//" );
            }
          c += sprintf ( c, " 4%s%s", cl->s3.R10, cl->s3.R50 );
        }

      if ( check_len ( sec3,8 ) &&
           (
             ( cl->s3.R100[0] && strcmp ( cl->s3.R100, "00" ) ) ||
             ( cl->s3.R150[0] && strcmp ( cl->s3.R150, "00" ) )
           )
         )
        {
          if ( cl->s3.R100[0] == 0 )
            {
              strcpy ( cl->s3.R100,"//" );
            }
          if ( cl->s3.R150[0] == 0 )
            {
              strcpy ( cl->s3.R150,"//" );
            }
          c += sprintf ( c, " 5%s%s", cl->s3.R100, cl->s3.R150 );
        }

      if ( check_len ( sec3,8 ) &&
           (
             ( cl->s3.s00[0] && strcmp ( cl->s3.s00, "00" ) ) ||
             ( cl->s3.s01[0] && strcmp ( cl->s3.s01, "00" ) )
           )
         )
        {
          if ( cl->s3.s00[0] == 0 )
            {
              strcpy ( cl->s3.s00,"//" );
            }
          if ( cl->s3.s01[0] == 0 )
            {
              strcpy ( cl->s3.s01,"//" );
            }
          c += sprintf ( c, " 6%s%s", cl->s3.s00, cl->s3.s01 );
        }

      if ( check_len ( sec3,8 ) &&
           (
             ( cl->s3.s10[0] && strcmp ( cl->s3.s10, "00" ) ) ||
             ( cl->s3.s50[0] && strcmp ( cl->s3.s50, "00" ) )
           )
         )
        {
          if ( cl->s3.s10[0] == 0 )
            {
              strcpy ( cl->s3.s10,"//" );
            }
          if ( cl->s3.s50[0] == 0 )
            {
              strcpy ( cl->s3.s50,"//" );
            }
          c += sprintf ( c, " 7%s%s", cl->s3.s10, cl->s3.s50 );
        }

      if ( check_len ( sec3,10 ) &&
           (
             ( cl->s3.f10[0] && strcmp ( cl->s3.f10, "00" ) ) ||
             ( cl->s3.f20[0] && strcmp ( cl->s3.f20, "00" ) ) ||
             ( cl->s3.f30[0] && strcmp ( cl->s3.f30, "00" ) )
           )
         )
        {
          if ( cl->s3.f10[0] == 0 )
            {
              strcpy ( cl->s3.f10,"//" );
            }
          if ( cl->s3.f20[0] == 0 )
            {
              strcpy ( cl->s3.f20,"//" );
            }
          if ( cl->s3.f30[0] == 0 )
            {
              strcpy ( cl->s3.f30,"//" );
            }
          c += sprintf ( c, " 8%s%s%s", cl->s3.f10, cl->s3.f20, cl->s3.f30 );
        }

      if ( check_len ( sec3,10 ) &&
           (
             ( cl->s3.V1[0] && strcmp ( cl->s3.V1, "00" ) ) ||
             ( cl->s3.V2[0] && strcmp ( cl->s3.V2, "00" ) ) ||
             ( cl->s3.V3[0] && strcmp ( cl->s3.V3, "00" ) )
           )
         )
        {
          if ( cl->s3.V1[0] == 0 )
            {
              strcpy ( cl->s3.V1,"//" );
            }
          if ( cl->s3.V2[0] == 0 )
            {
              strcpy ( cl->s3.V2,"//" );
            }
          if ( cl->s3.V3[0] == 0 )
            {
              strcpy ( cl->s3.V3,"//" );
            }
          c += sprintf ( c, " 9%s%s%s", cl->s3.V1, cl->s3.V2, cl->s3.V3 );
        }

    }

  if ( c != c0 )
    {
      *sec3 = c;
    }
  return *sec3;

}

/*!
  \fn char * print_climat_sec4 (char **sec4, size_t lmax, struct climat_chunks *cl)
  \brief Prints the climat section 4
  \param sec4 the pointer where to print section
  \param lmax max length permited
  \param cl pointer to s atruct \ref climat_chunks where the parse results are set
*/
char * print_climat_sec4 ( char **sec4, size_t lmax, struct climat_chunks *cl )
{
  char *c = *sec4, *c0 = *sec4;

  if ( cl->mask & CLIMAT_SEC4 )
    {
      //c += sprintf ( c, "\r\n      444" );
      c += sprintf ( c, " 444" );

      // init point to write info.
      // in case we finally write nothing in this section
      c0 = c;

      if ( check_len ( sec4,8 ) && ( cl->s4.sx[0] || cl->s4.Txd[0] || cl->s4.yx[0] ) )
        {
          if ( cl->s4.sx[0] == 0 )
            {
              strcpy ( cl->s4.sx, "/" );
            }
          if ( cl->s4.Txd[0] == 0 )
            {
              strcpy ( cl->s4.Txd, "///" );
            }
          if ( cl->s4.yx[0] == 0 )
            {
              strcpy ( cl->s4.yx, "//" );
            }
          c += sprintf ( c, " 0%s%s%s", cl->s4.sx, cl->s4.Txd, cl->s4.yx );
        }

      if ( check_len ( sec4,8 ) && ( cl->s4.sn[0] || cl->s4.Tnd[0] || cl->s4.yn[0] ) )
        {
          if ( cl->s4.sn[0] == 0 )
            {
              strcpy ( cl->s4.sn, "/" );
            }
          if ( cl->s4.Tnd[0] == 0 )
            {
              strcpy ( cl->s4.Tnd, "///" );
            }
          if ( cl->s4.yn[0] == 0 )
            {
              strcpy ( cl->s4.yn, "//" );
            }
          c += sprintf ( c, " 1%s%s%s", cl->s4.sn, cl->s4.Tnd, cl->s4.yn );
        }

      if ( check_len ( sec4,8 ) && ( cl->s4.sax[0] || cl->s4.Tax[0] || cl->s4.yax[0] ) )
        {
          if ( cl->s4.sax[0] == 0 )
            {
              strcpy ( cl->s4.sax, "/" );
            }
          if ( cl->s4.Tax[0] == 0 )
            {
              strcpy ( cl->s4.Tax, "///" );
            }
          if ( cl->s4.yax[0] == 0 )
            {
              strcpy ( cl->s4.yax, "//" );
            }
          c += sprintf ( c, " 2%s%s%s", cl->s4.sax, cl->s4.Tax, cl->s4.yax );
        }

      if ( check_len ( sec4,8 ) && ( cl->s4.san[0] || cl->s4.Tan[0] || cl->s4.yan[0] ) )
        {
          if ( cl->s4.san[0] == 0 )
            {
              strcpy ( cl->s4.san, "/" );
            }
          if ( cl->s4.Tan[0] == 0 )
            {
              strcpy ( cl->s4.Tan, "///" );
            }
          if ( cl->s4.yan[0] == 0 )
            {
              strcpy ( cl->s4.yan, "//" );
            }
          c += sprintf ( c, " 3%s%s%s", cl->s4.san, cl->s4.Tan, cl->s4.yan );
        }

      if ( check_len ( sec4,8 ) && ( cl->s4.RxRxRxRx[0] || cl->s4.yr[0] ) )
        {
          if ( cl->s4.RxRxRxRx[0] == 0 )
            {
              strcpy ( cl->s4.RxRxRxRx, "////" );
            }
          if ( cl->s4.yx[0] == 0 )
            {
              strcpy ( cl->s4.Tan, "//" );
            }
          c += sprintf ( c, " 4%s%s", cl->s4.RxRxRxRx, cl->s4.yr );
        }

      if ( check_len ( sec4,8 ) && ( cl->s4.fxfxfx[0] || cl->s4.yfx[0] ) )
        {
          if ( cl->s4.iw[0] == 0 )
            {
              strcpy ( cl->s4.iw, "/" );
            }
          if ( cl->s4.fxfxfx[0] == 0 )
            {
              strcpy ( cl->s4.fxfxfx, "///" );
            }
          if ( cl->s4.yfx[0] == 0 )
            {
              strcpy ( cl->s4.yfx, "//" );
            }
          c += sprintf ( c, " 5%s%s%s", cl->s4.iw, cl->s4.fxfxfx, cl->s4.yfx );
        }

      if ( check_len ( sec4,8 ) && ( cl->s4.Dts[0] || cl->s4.Dgr[0] ) )
        {
          if ( cl->s4.Dts[0] == 0 )
            {
              strcpy ( cl->s4.Dts,"//" );
            }
          if ( cl->s4.Dgr[0] == 0 )
            {
              strcpy ( cl->s4.Dgr,"//" );
            }
          c += sprintf ( c, " 6%s%s", cl->s4.Dts, cl->s4.Dgr );
        }

      if ( check_len ( sec4,8 ) && ( cl->s4.iy[0] || cl->s4.GxGx[0] || cl->s4.GnGn[0] ) )
        {
          if ( cl->s4.iw[0] == 0 )
            {
              strcpy ( cl->s4.iy, "/" );
            }
          if ( cl->s4.GxGx[0] == 0 )
            {
              strcpy ( cl->s4.GxGx, "///" );
            }
          if ( cl->s4.GnGn[0] == 0 )
            {
              strcpy ( cl->s4.GnGn, "//" );
            }
          c += sprintf ( c, " 7%s%s%s", cl->s4.iy, cl->s4.GxGx, cl->s4.GnGn );
        }


    }

  if ( c != c0 )
    {
      *sec4 = c;
    }
  return *sec4;

}

/*!
 \fn int print_climat(char *report, size_t lmax, struct climat_chunks *cl, int mode)
 \brief prints a climat into a string
 \param report target string
 \param lmax max size of string
 \param cl pointer to a struct \ref climat_chunks with the result of parse tasks
 \param mode If == 0 legacy mode. If == 1 the print WIGOS identifier

 returns 0 if all went right
*/
int print_climat ( char *report, size_t lmax, struct climat_chunks *cl, int mode )
{
  char *c;

  c = report;

  // Needs time extension
  if ( cl->e.YYYY[0] == 0 || cl->e.MM[0] == 0 )
    {
      return 1;
    }

  print_climat_sec0 ( &c, lmax, cl );

  if ( cl->mask & ( CLIMAT_SEC1 | CLIMAT_SEC2 | CLIMAT_SEC3 | CLIMAT_SEC4 ) )
    {
      print_climat_sec1 ( &c, lmax - strlen ( report ), cl );

      print_climat_sec2 ( &c, lmax - strlen ( report ), cl );

      print_climat_sec3 ( &c, lmax - strlen ( report ), cl );

      print_climat_sec4 ( &c, lmax - strlen ( report ), cl );
    }
  else
    {
      c += sprintf ( c, " NIL" );
    }


  c += sprintf ( c, "=" );
  return 0;

}
