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
/*!
 \file bufr2tac_print_climat.c
 \brief file with the code to print the results for climat reports
 */
#include "bufr2tac.h"

/*!
  \fn size_t print_climat_sec0 (char **sec0, size_t lmax, const struct climat_chunks *cl)
  \brief Prints the climat section 0 (header)
  \param sec0 the pointer where to print section
  \param lmax max length permited
  \param cl pointer to s atruct \ref climat_chunks where the parse results are set
*/
size_t print_climat_sec0 ( char **sec0, size_t lmax, const struct climat_chunks *cl )
{
  char *c = *sec0;
  size_t used = 0;

  used += snprintf ( c + used, lmax - used, "%s%s%s%s%s", cl->e.YYYY, cl->e.MM, cl->e.DD, cl->e.HH, cl->e.mm );

  // Print type
  used += snprintf ( c + used, lmax - used, " CLIMAT" );

  // print MMJJJ
  used += snprintf ( c + used, lmax - used, " %s%s", cl->s0.MM, cl->s0.JJJ );


  // print IIiii
  if ( cl->s0.II[0] )
    {
      used += snprintf ( c + used, lmax - used, " %s%s", cl->s0.II, cl->s0.iii );
    }

  *sec0 = c + used;
  return used;
}

/*!
  \fn size_t print_climat_sec1 (char **sec1, size_t lmax, struct climat_chunks *cl)
  \brief Prints the climat section 1
  \param sec1 the pointer where to print section
  \param lmax max length permited
  \param cl pointer to s atruct \ref climat_chunks where the parse results are set
*/
size_t print_climat_sec1 ( char **sec1, size_t lmax, struct climat_chunks *cl )
{
  char *c = *sec1;
  size_t used = 0;

  if ( cl->mask & SYNOP_SEC1 )
    {
      used += snprintf ( c + used, lmax - used, " 111" );

      if ( cl->s1.PoPoPoPo[0] )
        {
          used += snprintf ( c + used, lmax - used, " 1%s", cl->s1.PoPoPoPo );
        }

      if ( cl->s1.PPPP[0] )
        {
          used += snprintf ( c + used, lmax - used, " 2%s", cl->s1.PPPP );
        }

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
          used += snprintf ( c + used, lmax - used, " 3%s%s%s", cl->s1.s, cl->s1.TTT, cl->s1.ststst );
        }

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
          used += snprintf ( c + used, lmax - used, " 4%s%s%s%s", cl->s1.sx, cl->s1.TxTxTx, cl->s1.sn, cl->s1.TnTnTn );
        }

      if ( cl->s1.eee[0] )
        {
          used += snprintf ( c + used, lmax - used, " 5%s", cl->s1.eee );
        }

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
          used += snprintf ( c + used, lmax - used, " 6%s%s%s", cl->s1.R1R1R1R1, cl->s1.Rd, cl->s1.nrnr );
        }

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
          used += snprintf ( c + used, lmax - used, " 7%s%s", cl->s1.S1S1S1, cl->s1.pspsps );
        }

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
          used += snprintf ( c + used, lmax - used, " 8%s%s%s%s", cl->s1.mpmp, cl->s1.mtmt, cl->s1.mtx, cl->s1.mtn );
        }


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
          used += snprintf ( c + used, lmax - used, " 9%s%s%s", cl->s1.meme, cl->s1.mrmr, cl->s1.msms );
        }
    }

  *sec1 = c + used;
  return used;
}

/*!
  \fn size_t print_climat_sec2 (char **sec2, size_t lmax, struct climat_chunks *cl)
  \brief Prints the climat section 2
  \param sec2 the pointer where to print section
  \param lmax max length permited
  \param cl pointer to s atruct \ref climat_chunks where the parse results are set
*/
size_t print_climat_sec2 ( char **sec2, size_t lmax, struct climat_chunks *cl )
{
  char *c = *sec2;
  size_t used = 0, used0 = 0;

  if ( cl->mask & SYNOP_SEC2 )
    {
      //used += snprintf ( c + used, lmax - used, "\r\n      222" );
      used += snprintf ( c + used, lmax - used, " 222" );

      // init point to write info.
      // in case we finally write nothing in this section
      used0 = used;

      if (  cl->s2.YbYb[0] || cl->s2.YcYc[0] )
        {
          if ( cl->s2.YbYb[0] == 0 )
            {
              strcpy ( cl->s2.YbYb, "//" );
            }
          if ( cl->s2.YcYc[0] == 0 )
            {
              strcpy ( cl->s2.YcYc, "//" );
            }
          used += snprintf ( c + used, lmax - used, " 0%s%s", cl->s2.YbYb, cl->s2.YcYc );
        }

      if ( cl->s2.PoPoPoPo[0] )
        {
          used += snprintf ( c + used, lmax - used, " 1%s", cl->s2.PoPoPoPo );
        }

      if ( cl->s2.PPPP[0] )
        {
          used += snprintf ( c + used, lmax - used, " 2%s", cl->s2.PPPP );
        }

      if (  cl->s2.s[0] || cl->s2.TTT[0] || cl->s2.ststst[0] )
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

          used += snprintf ( c + used, lmax - used, " 3%s%s%s", cl->s2.s, cl->s2.TTT, cl->s2.ststst );
        }

      if (  cl->s2.sx[0] || cl->s2.TxTxTx[0] || cl->s2.sn[0] || cl->s2.TnTnTn[0] )
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

          used += snprintf ( c + used, lmax - used, " 4%s%s%s%s", cl->s2.sx, cl->s2.TxTxTx, cl->s2.sn, cl->s2.TnTnTn );
        }

      if (  cl->s2.eee[0] )
        {
          used += snprintf ( c + used, lmax - used, " 5%s", cl->s2.eee );
        }

      if ( cl->s2.R1R1R1R1[0] || cl->s2.nrnr[0]  )
        {
          if ( cl->s2.R1R1R1R1[0] == 0 )
            {
              strcpy ( cl->s2.R1R1R1R1, "////" );
            }
          if ( cl->s2.nrnr[0] == 0 )
            {
              strcpy ( cl->s2.nrnr, "//" );
            }
          used += snprintf ( c + used, lmax - used, " 6%s%s", cl->s2.R1R1R1R1, cl->s2.nrnr );
        }

      if ( cl->s2.S1S1S1[0] )
        {
          used += snprintf ( c + used, lmax - used, " 7%s", cl->s2.S1S1S1 );
        }

      if ( cl->s2.ypyp[0] || cl->s2.ytyt[0] || cl->s2.ytxytx[0] )
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

          used += snprintf ( c + used, lmax - used, " 8%s%s%s", cl->s2.ypyp, cl->s2.ytyt, cl->s2.ytxytx );
        }

      if (  cl->s2.yeye[0] || cl->s2.yryr[0] || cl->s2.ysys[0] )
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

          used += snprintf ( c + used, lmax - used, " 9%s%s%s", cl->s2.yeye, cl->s2.yryr, cl->s2.ysys );
        }

    }

  if ( used != used0 )
    {
      *sec2 = c + used;
    }
  else
    used = 0;

  return used;
}


/*!
  \fn size_t print_climat_sec3 (char **sec3, size_t lmax, struct climat_chunks *cl)
  \brief Prints the climat section 3
  \param sec3 the pointer where to print section
  \param lmax max length permited
  \param cl pointer to s atruct \ref climat_chunks where the parse results are set
*/
size_t print_climat_sec3 ( char **sec3, size_t lmax, struct climat_chunks *cl )
{
  char *c = *sec3;
  size_t used0 = 0, used = 0;

  if ( cl->mask & SYNOP_SEC3 )
    {
      //used += snprintf ( c + used, lmax - used, "\r\n      333" );
      used += snprintf ( c + used, lmax - used, " 333" );

      // init point to write info.
      // in case we finally write nothing in this section
      used0 = used;

      if (   ( cl->s3.T25[0] && strcmp ( cl->s3.T25, "00" ) ) ||
             ( cl->s3.T30[0] && strcmp ( cl->s3.T30, "00" ) )
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
          used += snprintf ( c + used, lmax - used, " 0%s%s", cl->s3.T25, cl->s3.T30 );
        }

      if ( 
             ( cl->s3.T35[0] && strcmp ( cl->s3.T35, "00" ) ) ||
             ( cl->s3.T40[0] && strcmp ( cl->s3.T40, "00" ) )
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
          used += snprintf ( c + used, lmax - used, " 1%s%s", cl->s3.T35, cl->s3.T40 );
        }

      if ( 
             ( cl->s3.Tn0[0] && strcmp ( cl->s3.Tn0, "00" ) ) ||
             ( cl->s3.Tx0[0] && strcmp ( cl->s3.Tx0, "00" ) )
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
          used += snprintf ( c + used, lmax - used, " 2%s%s", cl->s3.Tn0, cl->s3.Tx0 );
        }

      if (   ( cl->s3.R01[0] && strcmp ( cl->s3.R01, "00" ) ) ||
             ( cl->s3.R05[0] && strcmp ( cl->s3.R05, "00" ) )
           
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
          used += snprintf ( c + used, lmax - used, " 3%s%s", cl->s3.R01, cl->s3.R05 );
        }

      if (   ( cl->s3.R10[0] && strcmp ( cl->s3.R10, "00" ) ) ||
             ( cl->s3.R50[0] && strcmp ( cl->s3.R50, "00" ) )
           
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
          used += snprintf ( c + used, lmax - used, " 4%s%s", cl->s3.R10, cl->s3.R50 );
        }

      if (   ( cl->s3.R100[0] && strcmp ( cl->s3.R100, "00" ) ) ||
             ( cl->s3.R150[0] && strcmp ( cl->s3.R150, "00" ) )
           
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
          used += snprintf ( c + used, lmax - used, " 5%s%s", cl->s3.R100, cl->s3.R150 );
        }

      if (   ( cl->s3.s00[0] && strcmp ( cl->s3.s00, "00" ) ) ||
             ( cl->s3.s01[0] && strcmp ( cl->s3.s01, "00" ) )
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
          used += snprintf ( c + used, lmax - used, " 6%s%s", cl->s3.s00, cl->s3.s01 );
        }

      if (   ( cl->s3.s10[0] && strcmp ( cl->s3.s10, "00" ) ) ||
             ( cl->s3.s50[0] && strcmp ( cl->s3.s50, "00" ) )
           
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
          used += snprintf ( c + used, lmax - used, " 7%s%s", cl->s3.s10, cl->s3.s50 );
        }

      if ( 
             ( cl->s3.f10[0] && strcmp ( cl->s3.f10, "00" ) ) ||
             ( cl->s3.f20[0] && strcmp ( cl->s3.f20, "00" ) ) ||
             ( cl->s3.f30[0] && strcmp ( cl->s3.f30, "00" ) )
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
          used += snprintf ( c + used, lmax - used, " 8%s%s%s", cl->s3.f10, cl->s3.f20, cl->s3.f30 );
        }

      if ( 
             ( cl->s3.V1[0] && strcmp ( cl->s3.V1, "00" ) ) ||
             ( cl->s3.V2[0] && strcmp ( cl->s3.V2, "00" ) ) ||
             ( cl->s3.V3[0] && strcmp ( cl->s3.V3, "00" ) )
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
          used += snprintf ( c + used, lmax - used, " 9%s%s%s", cl->s3.V1, cl->s3.V2, cl->s3.V3 );
        }

    }

  if ( used != used0 )
    {
      *sec3 = c + used;
    }
  used = 0;

  return used;

}

/*!
  \fn size_t print_climat_sec4 (char **sec4, size_t lmax, struct climat_chunks *cl)
  \brief Prints the climat section 4
  \param sec4 the pointer where to print section
  \param lmax max length permited
  \param cl pointer to s atruct \ref climat_chunks where the parse results are set
*/
size_t print_climat_sec4 ( char **sec4, size_t lmax, struct climat_chunks *cl )
{
  char *c = *sec4;
  size_t used0 = 0, used = 0;

  if ( cl->mask & CLIMAT_SEC4 )
    {
      //used += snprintf ( c + used, lmax - used, "\r\n      444" );
      used += snprintf ( c + used, lmax - used, " 444" );

      // init point to write info.
      // in case we finally write nothing in this section
      used0 = used;

      if (  cl->s4.sx[0] || cl->s4.Txd[0] || cl->s4.yx[0] )
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
          used += snprintf ( c + used, lmax - used, " 0%s%s%s", cl->s4.sx, cl->s4.Txd, cl->s4.yx );
        }

      if ( cl->s4.sn[0] || cl->s4.Tnd[0] || cl->s4.yn[0] )
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
          used += snprintf ( c + used, lmax - used, " 1%s%s%s", cl->s4.sn, cl->s4.Tnd, cl->s4.yn );
        }

      if (  cl->s4.sax[0] || cl->s4.Tax[0] || cl->s4.yax[0] )
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
          used += snprintf ( c + used, lmax - used, " 2%s%s%s", cl->s4.sax, cl->s4.Tax, cl->s4.yax );
        }

      if (  cl->s4.san[0] || cl->s4.Tan[0] || cl->s4.yan[0] )
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
          used += snprintf ( c + used, lmax - used, " 3%s%s%s", cl->s4.san, cl->s4.Tan, cl->s4.yan );
        }

      if (  cl->s4.RxRxRxRx[0] || cl->s4.yr[0] )
        {
          if ( cl->s4.RxRxRxRx[0] == 0 )
            {
              strcpy ( cl->s4.RxRxRxRx, "////" );
            }
          if ( cl->s4.yx[0] == 0 )
            {
              strcpy ( cl->s4.Tan, "//" );
            }
          used += snprintf ( c + used, lmax - used, " 4%s%s", cl->s4.RxRxRxRx, cl->s4.yr );
        }

      if (  cl->s4.fxfxfx[0] || cl->s4.yfx[0] )
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
          used += snprintf ( c + used, lmax - used, " 5%s%s%s", cl->s4.iw, cl->s4.fxfxfx, cl->s4.yfx );
        }

      if ( cl->s4.Dts[0] || cl->s4.Dgr[0] )
        {
          if ( cl->s4.Dts[0] == 0 )
            {
              strcpy ( cl->s4.Dts,"//" );
            }
          if ( cl->s4.Dgr[0] == 0 )
            {
              strcpy ( cl->s4.Dgr,"//" );
            }
          used += snprintf ( c + used, lmax - used, " 6%s%s", cl->s4.Dts, cl->s4.Dgr );
        }

      if (  cl->s4.iy[0] || cl->s4.GxGx[0] || cl->s4.GnGn[0] )
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
          used += snprintf ( c + used, lmax - used, " 7%s%s%s", cl->s4.iy, cl->s4.GxGx, cl->s4.GnGn );
        }


    }

  if ( used != used0 )
    {
      *sec4 = c + used;
    }
  else
    used = 0;

  return used;
}


/*!
 \fn int print_climat_report ( struct metreport *m )
 \brief prints a climat into a string
 \param m pointer to struct \ref metreport where are both target and source

  If OK returns 0, otherwise 1
*/
int print_climat_report ( struct metreport *m )
{
  char *c = &(m->alphanum[0]);
  size_t used = 0;
  size_t lmax = sizeof(m->alphanum);
  struct climat_chunks *cl = &m->climat;

  // Needs time extension
  if ( cl->e.YYYY[0] == 0  || cl->e.YYYY[0] == '0' || cl->e.MM[0] == 0 )
    {
      return 1;
    }

  if ( m->print_mask & PRINT_BITMASK_WIGOS )
    {
      used += print_wigos_id ( &c, lmax, m );
    }

  if ( m->print_mask & PRINT_BITMASK_GEO )
    {
      used += print_geo ( &c, lmax, m );
    }

  used += print_climat_sec0 ( &c, lmax - used, cl );

  if ( cl->mask & ( CLIMAT_SEC1 | CLIMAT_SEC2 | CLIMAT_SEC3 | CLIMAT_SEC4 ) )
    {
      used += print_climat_sec1 ( &c, lmax - used, cl );

      used += print_climat_sec2 ( &c, lmax - used, cl );

      used += print_climat_sec3 ( &c, lmax - used, cl );

      used += print_climat_sec4 ( &c, lmax - used, cl );
    }
  else
    {
      c += snprintf ( c, lmax - used, " NIL" );
    }
  snprintf ( c, lmax - used - 1, "=" );

  return 0;

}
