/***************************************************************************
 *   Copyright (C) 2013-2022 by Guillermo Ballester Valor                  *
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
 \file bufr2tac_print.c
 \brief file with the code to print the results
 */
#include "bufr2tac.h"

/*!
  \fn size_t print_synop_sec0 (char **sec0, size_t lmax, struct synop_chunks *syn)
  \brief Prints the synop section 0 (header)
  \param sec0 the pointer where to print section
  \param lmax max length permited
  \param syn pointer to s atruct \ref synop_chunks where the parse results are set
  \return Used bytes
*/
size_t print_synop_sec0 ( char **sec0, size_t lmax, struct synop_chunks *syn )
{
  char *c = *sec0;
  size_t used = 0;

  used += snprintf ( c + used, lmax - used,  "%s%s%s%s%s", syn->e.YYYY, syn->e.MM, syn->e.DD, syn->e.HH, syn->e.mm );

  // Print type
  used += snprintf ( c + used, lmax - used,    " %s%s", syn->s0.MiMi, syn->s0.MjMj );

  if ( syn->s0.D_D[0] )
    {
      used += snprintf ( c + used, lmax - used,  " %s", syn->s0.D_D );
    }
  else if ( syn->s0.A1[0] && syn->s0.bw[0] && syn->s0.nbnbnb[0] )
    {
      used += snprintf ( c + used, lmax - used,  " %s%s%s", syn->s0.A1, syn->s0.bw, syn->s0.nbnbnb );
    }


  // print YYGGiw
  used += snprintf ( c + used, lmax - used, " %s%s%s", syn->s0.YY, syn->s0.GG, syn->s0.iw );

  // print IIiii
  if ( syn->s0.II[0] )
    {
      used += snprintf ( c + used, lmax - used, " %s%s", syn->s0.II, syn->s0.iii );
    }
  else
    {
      if ( syn->s0.LaLaLa[0] )
        {
          used += snprintf ( c + used, lmax - used, " 99%s", syn->s0.LaLaLa );
        }
      else
        {
          used += snprintf ( c + used, lmax - used, " 99///" );
        }

      if ( syn->s0.Qc[0] && syn->s0.LoLoLoLo[0] )
        {
          used += snprintf ( c + used, lmax - used, " %s%s", syn->s0.Qc, syn->s0.LoLoLoLo );
        }
      else
        {
          used += snprintf ( c + used, lmax - used, " /////" );
        }
    }

  if ( strcmp ( syn->s0.MiMi, "OO" ) == 0 )
    {
      if ( syn->s0.MMM[0] && syn->s0.Ula[0] && syn->s0.Ulo[0] )
        {
          used += snprintf ( c + used, lmax - used, " %s%s%s", syn->s0.MMM, syn->s0.Ula, syn->s0.Ulo );
        }
      if ( syn->s0.h0h0h0h0[0] )
        {
          used += snprintf ( c + used, lmax - used, " %s%s", syn->s0.h0h0h0h0, syn->s0.im );
        }

    }

  *sec0 = c + used;
  return used;
}

/*!
  \fn size_t print_synop_sec1 (char **sec1, size_t lmax, struct synop_chunks *syn)
  \brief Prints the synop section 1
  \param sec1 the pointer where to print section
  \param lmax max length permited
  \param syn pointer to s atruct \ref synop_chunks where the parse results are set
  \return Used bytes
*/
size_t print_synop_sec1 ( char **sec1, size_t lmax, struct synop_chunks *syn )
{
  char *c = *sec1;
  size_t used = 0;

  if ( syn->mask & SYNOP_SEC1 )
    {
      // printf irixhVV
      used += snprintf ( c + used, lmax - used, " %s%s%s%s", syn->s1.ir, syn->s1.ix, syn->s1.h, syn->s1.VV );


      // printf Nddff
      used += snprintf ( c + used, lmax - used, " %s%s%s", syn->s1.N, syn->s1.dd, syn->s1.ff );
      if ( strlen ( syn->s1.fff ) )
        {
          used += snprintf ( c + used, lmax - used, " 00%s", syn->s1.fff );
        }

      // printf 1snTTT

      if ( syn->s1.TTT[0] )
        {
          used += snprintf ( c + used, lmax - used, " 1%s%s", syn->s1.sn1, syn->s1.TTT );
        }

      // printf 2snTdTdTd or 29UUU
      if ( syn->s1.TdTdTd[0] )
        {
          used += snprintf ( c + used, lmax - used, " 2%s%s", syn->s1.sn2, syn->s1.TdTdTd );
        }
      else if ( syn->s1.UUU[0] )
        {
          used += snprintf ( c + used, lmax - used, " 29%s", syn->s1.UUU );
        }

      // printf 3PoPoPoPo
      if ( syn->s1.PoPoPoPo[0] )
        {
          used += snprintf ( c + used, lmax - used, " 3%s", syn->s1.PoPoPoPo );
        }

      // printf 4PPPP or 4a3hhh
      if ( syn->s1.PPPP[0] )
        {
          used += snprintf ( c + used, lmax - used, " 4%s", syn->s1.PPPP );
        }
      else if ( syn->s1.hhh[0] )
        {
          if ( syn->s1.a3[0] == 0 )
            {
              syn->s1.a3[0] = '/';
            }
          used += snprintf ( c + used, lmax - used, " 4%s%s", syn->s1.a3, syn->s1.hhh );
        }

      // printf 5appp
      if ( syn->s1.a[0] || syn->s1.ppp[0] )
        {
          if ( syn->s1.a[0] == 0 )
            {
              syn->s1.a[0] = '/';
            }
          if ( syn->s1.ppp[0] == 0 )
            {
              strcpy ( syn->s1.ppp, "///" );
            }
          used += snprintf ( c + used, lmax - used, " 5%s%s", syn->s1.a, syn->s1.ppp );
        }

      // printf 6RRRtr
      if ( syn->s1.tr[0] || syn->s1.RRR[0] )
        {
          if ( syn->s1.tr[0] == 0 )
            {
              syn->s1.tr[0] = '/';
            }
          if ( syn->s1.RRR[0] == 0 )
            {
              strcpy ( syn->s1.RRR, "///" );
            }
          used += snprintf ( c + used, lmax - used, " 6%s%s", syn->s1.RRR, syn->s1.tr );
        }

      if ( syn->s1.ww[0] || syn->s1.W1[0] || syn->s1.W2[0] )
        {
          if ( syn->s1.ww[0] == 0 )
            {
              strcpy ( syn->s1.ww, "//" );
            }
          if ( syn->s1.W1[0] == 0 )
            {
              strcpy ( syn->s1.W1, "/" );
            }
          if ( syn->s1.W2[0] == 0 )
            {
              strcpy ( syn->s1.W2, "/" );
            }
          used += snprintf ( c + used, lmax - used, " 7%s%s%s", syn->s1.ww, syn->s1.W1, syn->s1.W2 );
        }

      if ( ( syn->s1.Nh[0] && syn->s1.Nh[0] != '0' && syn->s1.Nh[0] != '/' ) ||
           ( syn->s1.Cl[0] && syn->s1.Cl[0] != '0' && syn->s1.Cl[0] != '/' ) ||
           ( syn->s1.Cm[0] && syn->s1.Cm[0] != '0' && syn->s1.Cm[0] != '/' ) ||
           ( syn->s1.Ch[0] && syn->s1.Ch[0] != '0' && syn->s1.Ch[0] != '/' ) )
        {
          if ( syn->s1.Nh[0] == 0 )
            {
              strcpy ( syn->s1.Nh, "/" );
            }
          if ( syn->s1.Cl[0] == 0 )
            {
              strcpy ( syn->s1.Cl, "/" );
            }
          if ( syn->s1.Cm[0] == 0 )
            {
              strcpy ( syn->s1.Cm, "/" );
            }
          if ( syn->s1.Ch[0] == 0 )
            {
              strcpy ( syn->s1.Ch, "/" );
            }
          used += snprintf ( c + used, lmax - used, " 8%s%s%s%s", syn->s1.Nh, syn->s1.Cl, syn->s1.Cm, syn->s1.Ch );
        }

      if ( syn->s1.GG[0] )
        {
          used += snprintf ( c + used, lmax - used, " 9%s%s", syn->s1.GG, syn->s1.gg );
        }
    }
  *sec1 = c + used;
  return used;
}


/*!
  \fn size_t print_synop_sec2 (char **sec2, size_t lmax, struct synop_chunks *syn)
  \brief Prints the synop section 2
  \param sec2 the pointer where to print section
  \param lmax max length permited
  \param syn pointer to s atruct \ref synop_chunks where the parse results are set
  \return Used bytes
*/
size_t print_synop_sec2 ( char **sec2, size_t lmax, struct synop_chunks *syn )
{
  char *c = *sec2;
  size_t used = 0;

  if ( syn->mask & SYNOP_SEC2 )
    {
      // 222Dsvs
      used += snprintf ( c + used, lmax - used, " 222" );
      if ( syn->s2.Ds[0] )
        {
          used += snprintf ( c + used, lmax - used, "%s", syn->s2.Ds );
        }
      else
        {
          used += snprintf ( c + used, lmax - used, "/" );
        }

      if ( syn->s2.vs[0] )
        {
          used += snprintf ( c + used, lmax - used, "%s", syn->s2.vs );
        }
      else
        {
          used += snprintf ( c + used, lmax - used, "/" );
        }

      // printf 0ssTwTwTw
      if ( syn->s2.TwTwTw[0] )
        {
          used += snprintf ( c + used, lmax - used, " 0%s%s", syn->s2.ss, syn->s2.TwTwTw );
        }

      if ( syn->s2.PwaPwa[0] || syn->s2.HwaHwa[0] )
        {
          if ( syn->s2.PwaPwa[0] )
            {
              used += snprintf ( c + used, lmax - used, " 1%s", syn->s2.PwaPwa );
            }
          else
            {
              used += snprintf ( c + used, lmax - used, " 1//" );
            }

          if ( syn->s2.HwaHwa[0] )
            {
              used += snprintf ( c + used, lmax - used, "%s", syn->s2.HwaHwa );
            }
          else
            {
              used += snprintf ( c + used, lmax - used, "//" );
            }
        }

      if ( syn->s2.PwPw[0] || syn->s2.HwHw[0] )
        {
          if ( syn->s2.PwPw[0] )
            {
              used += snprintf ( c + used, lmax - used, " 2%s", syn->s2.PwPw );
            }
          else
            {
              used += snprintf ( c + used, lmax - used, " 2//" );
            }

          if ( syn->s2.HwHw[0] )
            {
              used += snprintf ( c + used, lmax - used, "%s", syn->s2.HwHw );
            }
          else
            {
              used += snprintf ( c + used, lmax - used, "//" );
            }
        }

      if ( syn->s2.dw1dw1[0] || syn->s2.dw2dw2[0] )
        {
          if ( syn->s2.dw1dw1[0] )
            {
              used += snprintf ( c + used, lmax - used, " 3%s", syn->s2.dw1dw1 );
            }
          else
            {
              used += snprintf ( c + used, lmax - used, " 3//" );
            }

          if ( syn->s2.dw2dw2[0] )
            {
              used += snprintf ( c + used, lmax - used, "%s", syn->s2.dw2dw2 );
            }
          else
            {
              used += snprintf ( c + used, lmax - used, "//" );
            }
        }

      if ( syn->s2.Pw1Pw1[0] || syn->s2.Hw1Hw1[0] )
        {
          if ( syn->s2.Pw1Pw1[0] )
            {
              used += snprintf ( c + used, lmax - used, " 4%s", syn->s2.Pw1Pw1 );
            }
          else
            {
              used += snprintf ( c + used, lmax - used, " 4//" );
            }

          if ( syn->s2.Hw1Hw1[0] )
            {
              used += snprintf ( c + used, lmax - used, "%s", syn->s2.Hw1Hw1 );
            }
          else
            {
              used += snprintf ( c + used, lmax - used, "//" );
            }
        }


      if ( syn->s2.Pw2Pw2[0] || syn->s2.Hw2Hw2[0] )
        {
          if ( syn->s2.Pw2Pw2[0] )
            {
              used += snprintf ( c + used, lmax - used, " 5%s", syn->s2.Pw2Pw2 );
            }
          else
            {
              used += snprintf ( c + used, lmax - used, " 5//" );
            }

          if ( syn->s2.Hw2Hw2[0] )
            {
              used += snprintf ( c + used, lmax - used, "%s", syn->s2.Hw2Hw2 );
            }
          else
            {
              used += snprintf ( c + used, lmax - used, "//" );
            }
        }

      if ( syn->s2.HwaHwaHwa[0] )
        {
          used += snprintf ( c + used, lmax - used, " 70%s", syn->s2.HwaHwaHwa );
        }


      if ( syn->s2.TbTbTb[0] )
        {
          used += snprintf ( c + used, lmax - used, " 8%s%s", syn->s2.sw, syn->s2.TbTbTb );
        }

    }
  *sec2 = c + used;
  return used;
}

/*!
  \fn size_t print_synop_sec3 (char **sec3, size_t lmax, struct synop_chunks *syn)
  \brief Prints the synop section 3
  \param sec3 the pointer where to print section
  \param lmax max length permited
  \param syn pointer to s atruct \ref synop_chunks where the parse results are set
  \return Used bytes
*/
size_t print_synop_sec3 ( char **sec3, size_t lmax, struct synop_chunks *syn )
{
  size_t i;
  char *c = *sec3;
  size_t used = 0, used0;

  if ( syn->mask & SYNOP_SEC3 )
    {
      used += snprintf ( c + used, lmax - used, " 333" );

      // init point to write info.
      // in case we finally write nothing in this section
      used0 = used;

      // printf 0XoXoXoXo
      if ( syn->s3.XoXoXoXo[0] && ( strstr ( syn->s3.XoXoXoXo,"///" ) == NULL ) )
        {
          if ( syn->s3.XoXoXoXo[0] == 0 )
            {
              syn->s3.XoXoXoXo[0] = '/';
            }
          if ( syn->s3.XoXoXoXo[1] == 0 )
            {
              syn->s3.XoXoXoXo[1] = '/';
            }
          if ( syn->s3.XoXoXoXo[2] == 0 )
            {
              syn->s3.XoXoXoXo[2] = '/';
            }
          if ( syn->s3.XoXoXoXo[3] == 0 )
            {
              syn->s3.XoXoXoXo[3] = '/';
            }
          used += snprintf ( c + used, lmax - used, " 0%s", syn->s3.XoXoXoXo );
        }

      // printf 1snxTxTxTx
      if ( syn->s3.snx[0] )
        {
          used += snprintf ( c + used, lmax - used, " 1%s%s", syn->s3.snx, syn->s3.TxTxTx );
        }

      // printf 1snnTnTnTn
      if ( syn->s3.snn[0] )
        {
          used += snprintf ( c + used, lmax - used, " 2%s%s", syn->s3.snn, syn->s3.TnTnTn );
        }

      // printf 3Ejjj
      if ( syn->s3.E[0] || syn->s3.jjj[0] )
        {
          if ( syn->s3.E[0] == 0 )
            {
              syn->s3.E[0] = '/';
            }
          if ( syn->s3.jjj[0] == 0 )
            {
              strcpy ( syn->s3.jjj, "///" );
            }
          used += snprintf ( c + used, lmax - used, " 3%s%s", syn->s3.E, syn->s3.jjj );
        }

      // printf 4E1sss
      if ( syn->s3.E1[0] || syn->s3.sss[0] )
        {
          if ( syn->s3.E1[0] == 0 )
            {
              syn->s3.E1[0] = '/';
            }
          if ( syn->s3.sss[0] == 0 )
            {
              strcpy ( syn->s3.sss, "///" );
            }
          if ( syn->s3.E1[0] != '/' || strcmp ( syn->s3.sss, "999" ) )
            {
              used += snprintf ( c + used, lmax - used, " 4%s%s", syn->s3.E1, syn->s3.sss );
            }
        }

      /**** Radiation Sunshine gropus ***/

      // print 55SSS
      if ( syn->s3.SSS[0] )
        {
          if ( strcmp ( syn->s3.SSS, "///" ) )
            {
              used += snprintf ( c + used, lmax - used, " 55%s", syn->s3.SSS );
            }
          else if ( syn->s3.j524[0][0] || syn->s3.j524[1][0] ||
                    syn->s3.j524[2][0] || syn->s3.j524[3][0] ||
                    syn->s3.j524[4][0] || syn->s3.j524[5][0] ||
                    syn->s3.j524[6][0] )
            {
              used += snprintf ( c + used, lmax - used, " 55%s", syn->s3.SSS );
            }

          for ( i = 0; i < 7; i++ )
            {
              if ( syn->s3.j524[i][0] )
                {
                  used += snprintf ( c + used, lmax - used, " %s%s", syn->s3.j524[i], syn->s3.FFFF24[i] );
                }
            }
        }

      // print 553SS
      if ( syn->s3.SS[0] )
        {
          if ( strcmp ( syn->s3.SS, "//" ) )
            {
              used += snprintf ( c + used, lmax - used, " 553%s", syn->s3.SS );
            }
          else if ( syn->s3.j5[0][0] || syn->s3.j5[1][0] ||
                    syn->s3.j5[2][0] || syn->s3.j5[3][0] ||
                    syn->s3.j5[4][0] || syn->s3.j5[5][0] ||
                    syn->s3.j5[6][0] )
            {
              used += snprintf ( c + used, lmax - used, " 553%s", syn->s3.SS );
            }

          for ( i = 0; i < 7; i++ )
            {
              if ( syn->s3.j5[i][0] )
                {
                  used += snprintf ( c + used, lmax - used, " %s%s", syn->s3.j5[i], syn->s3.FFFF[i] );
                }
            }
        }

      // print 55407
      if ( syn->s3.FFFF407[0] )
        {
          used += snprintf ( c + used, lmax - used, " 55407 4%s", syn->s3.FFFF407 );
        }

      // print 55408
      if ( syn->s3.FFFF408[0] )
        {
          used += snprintf ( c + used, lmax - used, " 55408 4%s", syn->s3.FFFF408 );
        }

      // print 55507
      if ( syn->s3.FFFF507[0] )
        {
          used += snprintf ( c + used, lmax - used, " 55507 4%s", syn->s3.FFFF507 );
        }

      // print 55507
      if ( syn->s3.FFFF508[0] )
        {
          used += snprintf ( c + used, lmax - used, " 55508 4%s", syn->s3.FFFF508 );
        }

      // print 56DlDmDh
      if ( syn->s3.Dl[0] || syn->s3.Dm[0] || syn->s3.Dh[0] )
        {
          if ( syn->s3.Dl[0] == 0 )
            {
              syn->s3.Dl[0] = '/';
            }
          if ( syn->s3.Dm[0] == 0 )
            {
              syn->s3.Dm[0] = '/';
            }
          if ( syn->s3.Dh[0] == 0 )
            {
              syn->s3.Dh[0] = '/';
            }

          used += snprintf ( c + used, lmax - used, " 56%s%s%s", syn->s3.Dl, syn->s3.Dm, syn->s3.Dh );
        }

      // print 57CDeec
      if ( syn->s3.Da[0] || syn->s3.ec[0] )
        {
          if ( syn->s3.Da[0] == 0 )
            {
              syn->s3.Da[0] = '/';
            }
          if ( syn->s3.ec[0] == 0 )
            {
              syn->s3.ec[0] = '/';
            }

          used += snprintf ( c + used, lmax - used, " 57%s%s%s", syn->s3.C, syn->s3.Da, syn->s3.ec );
        }

      // print 58ppp24 or 59ppo24
      if ( syn->s3.ppp24[0] )
        {
          used += snprintf ( c + used, lmax - used, " 5%s%s", syn->s3.snp24, syn->s3.ppp24 );
        }

      // printf 6RRRtr
      if ( syn->s3.tr[0] || syn->s3.RRR[0] )
        {
          if ( syn->s3.tr[0] == 0 )
            {
              syn->s3.tr[0] = '/';
            }
          if ( syn->s3.RRR[0] == 0 )
            {
              strcpy ( syn->s3.RRR, "///" );
            }
          used += snprintf ( c + used, lmax - used, " 6%s%s", syn->s3.RRR,syn->s3.tr );
        }

      if ( syn->s3.RRRR24[0] )
        {
          used += snprintf ( c + used, lmax - used, " 7%s", syn->s3.RRRR24 );
        }

      // additional cloud layers
      for ( i = 0; i < 4 ; i++ )
        {
          if ( syn->s3.nub[i].hshs[0] )
            {
              if ( syn->s3.nub[i].Ns[0] == 0 && syn->s3.nub[i].C[0] == 0 )
                {
                  used += snprintf ( c + used, lmax - used, " 8//%s", syn->s3.nub[i].hshs ); // Detected cloud but no anymore info
                }
              else
                {
                  used += snprintf ( c + used, lmax - used, " 8" );
                  if ( syn->s3.nub[i].Ns[0] )
                    {
                      used += snprintf ( c + used, lmax - used, "%s", syn->s3.nub[i].Ns );
                    }
                  else
                    {
                      used += snprintf ( c + used, lmax - used, "/" );
                    }

                  if ( syn->s3.nub[i].C[0] )
                    {
                      used += snprintf ( c + used, lmax - used, "%s", syn->s3.nub[i].C );
                    }
                  else
                    {
                      used += snprintf ( c + used, lmax - used, "/" );
                    }

                  used += snprintf ( c + used, lmax - used, "%s", syn->s3.nub[i].hshs );

                }
            }
        }

      // additional info
      for ( i = 0; i < syn->s3.d9.n && i < SYNOP_NMISC ; i++ )
        {
          if ( syn->s3.d9.misc[i].SpSp[0] && syn->s3.d9.misc[i].spsp[0] )
            {
              used += snprintf ( c + used, lmax - used, " %s%s", syn->s3.d9.misc[i].SpSp, syn->s3.d9.misc[i].spsp );
            }
        }


      // aditional regional info
      if ( syn->mask & SYNOP_SEC3_8 )
        {
          used += snprintf ( c + used, lmax - used, " 80000" );
          for ( i = 0; i < SYNOP_NMISC; i++ )
            {
              if ( syn->s3.R8[i][0] || syn->s3.R8[i][0] || syn->s3.R8[i][0] || syn->s3.R8[i][0] )
                {
                  if ( syn->s3.R8[i][0] == 0 )
                    {
                      syn->s3.R8[i][0] = '/';
                    }
                  if ( syn->s3.R8[i][1] == 0 )
                    {
                      syn->s3.R8[i][1] = '/';
                    }
                  if ( syn->s3.R8[i][2] == 0 )
                    {
                      syn->s3.R8[i][2] = '/';
                    }
                  if ( syn->s3.R8[i][3] == 0 )
                    {
                      syn->s3.R8[i][3] = '/';
                    }
                  used += snprintf ( c + used, lmax - used, " %ld%s", i, syn->s3.R8[i] );
                }
            }
        }

      if ( used != used0 )
        {
          *sec3 = c + used;
        }
      else
        used = 0;

    }
  return used;
}

/*!
  \fn size_t print_synop_sec4 (char **sec4, size_t lmax, struct synop_chunks *syn)
  \brief Prints the synop section 4
  \param sec4 the pointer where to print section
  \param lmax max length permited
  \param syn pointer to s atruct \ref synop_chunks where the parse results are set
  \return Used bytes
*/
size_t print_synop_sec4 ( char **sec4, size_t lmax, struct synop_chunks *syn )
{
  char *c = *sec4;
  size_t used = 0, used0;

  if ( syn->mask & SYNOP_SEC5 )
    {
      used += snprintf ( c + used, lmax - used, " 444" );

      // init point to write info.
      // in case we finally write nothing in this section
      used0 = used;

      // printf N1C1H1H1Ct
      if ( syn->s4.N1[0] || syn->s4.C1[0] || syn->s4.H1H1[0] || syn->s4.Ct[0] )
        {
          if ( syn->s4.N1[0] == 0 )
            {
              syn->s4.N1[0] = '/';
            }
          if ( syn->s4.C1[0] == 0 )
            {
              syn->s4.C1[0] = '/';
            }
          if ( syn->s4.Ct[0] == 0 )
            {
              syn->s4.Ct[0] = '/';
            }
          if ( syn->s4.H1H1[0] == 0 )
            {
              strcpy ( syn->s4.H1H1, "//" );
            }
          used += snprintf ( c + used, lmax - used, " %s%s%s%s", syn->s4.N1, syn->s4.C1, syn->s4.H1H1, syn->s4.Ct );
        }

      if ( used != used0 )
        {
          *sec4 = c + used;
        }
      else
        used = 0;
    }
  return used;
}


/*!
  \fn size_t print_synop_sec5 (char **sec5, size_t lmax, struct synop_chunks *syn)
  \brief Prints the synop section 5
  \param sec5 the pointer where to print section
  \param lmax max length permited
  \param syn pointer to s atruct \ref synop_chunks where the parse results are set
  \return Used bytes
*/
size_t print_synop_sec5 ( char **sec5, size_t lmax, struct synop_chunks *syn )
{
  size_t i;
  char *c = *sec5;
  size_t used = 0, used0;

  if ( syn->mask & SYNOP_SEC5 )
    {
      used += snprintf ( c + used, lmax - used, " 555" );

      // init point to write info.
      // in case we finally write nothing in this section
      used0 = used;

      // printf 6RRRtr
      if ( syn->s5.tr[0] || syn->s5.RRR[0] )
        {
          if ( syn->s5.tr[0] == 0 )
            {
              syn->s5.tr[0] = '/';
            }
          if ( syn->s5.RRR[0] == 0 )
            {
              strcpy ( syn->s5.RRR, "///" );
            }
          used += snprintf ( c + used, lmax - used, " 6%s%s", syn->s5.RRR,syn->s5.tr );
        }

      // additional info
      for ( i = 0; i < syn->s5.d9.n ; i++ )
        {
          used += snprintf ( c + used, lmax - used, " %s%s", syn->s5.d9.misc[i].SpSp, syn->s5.d9.misc[i].spsp );
        }

      if ( used != used0 )
        {
          *sec5 = c + used;
        }
      else
        used = 0;
    }
  return used;
}


/*!
 \fn int print_synop_report(struct metreport *m)
 \brief prints a synop into a string
 \param m pointer to struct \ref metreport where are both target and source

  If OK returns 0, otherwise 1
*/
int print_synop_report (struct metreport *m)
{
  char *c = &(m->alphanum[0]);
  size_t used = 0;
  size_t lmax = sizeof(m->alphanum);
  struct synop_chunks *syn = &m->synop;
  
  // Needs time extension
  if ( syn->e.YYYY[0] == 0 || syn->e.YYYY[0] == '0' )
    {
      return 1;
    }

  if ( m->print_mask & PRINT_BITMASK_WIGOS )
    {
      used += print_wigos_id ( &c, lmax, m );
    }
  else
    {
      // Do not print a synop if no IIiii identifier when mode = 0
      if ( ( strcmp(syn->s0.MiMi, "AA") == 0 && strcmp(syn->s0.MjMj, "XX") == 0) && 
        ( syn->s0.II[0] == '\0' || syn->s0.iii[0] == '\0' || strcmp ( syn->s0.II, "00" ) == 0 ))
        return 1;
    }

  if ( m->print_mask & PRINT_BITMASK_GEO )
    {
      used += print_geo ( &c, lmax, m );
    }
    
  used += print_synop_sec0 ( &c, lmax - used, syn );

  if ( syn->mask & ( SYNOP_SEC1 | SYNOP_SEC2 | SYNOP_SEC3 | SYNOP_SEC4 | SYNOP_SEC5 ) )
    {
      used += print_synop_sec1 ( &c, lmax - used, syn );

      used += print_synop_sec2 ( &c, lmax - used, syn );

      used += print_synop_sec3 ( &c, lmax - used, syn );

      used += print_synop_sec4 ( &c, lmax - used, syn );

      used += print_synop_sec5 ( &c, lmax - used, syn );
    }
  else
    {
      c += snprintf ( c, lmax - used, " NIL" );
    }
  snprintf ( c, lmax - used - 1, "=" );

  return 0;
}
 
