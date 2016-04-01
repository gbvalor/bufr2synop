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
 \file bufr2tac_print.c
 \brief file with the code to print the results
 */
#include "bufr2tac.h"

/*!
  \def check_len(ori,inc)
  \brief cheks if there still memory enough to add \a inc chars
*/
#define check_len(ori,inc) (c - *ori + inc < (int)lmax)

/*!
  \fn char * print_synop_sec0 (char **sec0, size_t lmax, struct synop_chunks *syn)
  \brief Prints the synop section 0 (header)
  \param sec0 the pointer where to print section
  \param lmax max length permited
  \param syn pointer to s atruct \ref synop_chunks where the parse results are set
*/
char * print_synop_sec0 ( char **sec0, size_t lmax, struct synop_chunks *syn )
{
  char *c = *sec0;

  if ( check_len ( sec0,12 ) )
    {
      c += sprintf ( c, "%s%s%s%s%s", syn->e.YYYY, syn->e.MM, syn->e.DD, syn->e.HH, syn->e.mm );
    }

  // Print type
  if ( check_len ( sec0,5 ) )
    {
      c += sprintf ( c, " %s%s", syn->s0.MiMi, syn->s0.MjMj );
    }

  if ( check_len ( sec0,6 ) )
    {
      if ( syn->s0.D_D[0] )
        {
          c += sprintf ( c, " %s", syn->s0.D_D );
        }
      else if ( syn->s0.A1[0] && syn->s0.bw[0] && syn->s0.nbnbnb[0] )
        {
          c += sprintf ( c, " %s%s%s", syn->s0.A1, syn->s0.bw, syn->s0.nbnbnb );
        }

    }

  // print YYGGiw
  if ( check_len ( sec0,6 ) )
    {
      c += sprintf ( c, " %s%s%s", syn->s0.YY, syn->s0.GG, syn->s0.iw );
    }

  // print IIiii
  if ( check_len ( sec0,6 ) && syn->s0.II[0] )
    {
      c += sprintf ( c, " %s%s", syn->s0.II, syn->s0.iii );
    }
  else
    {
      if ( syn->s0.LaLaLa[0] )
        {
          c += sprintf ( c, " 99%s", syn->s0.LaLaLa );
        }
      else
        {
          c += sprintf ( c, " 99///" );
        }

      if ( syn->s0.Qc[0] && syn->s0.LoLoLoLo[0] )
        {
          c += sprintf ( c, " %s%s", syn->s0.Qc, syn->s0.LoLoLoLo );
        }
      else
        {
          c += sprintf ( c, " /////" );
        }
    }

  if ( strcmp ( syn->s0.MiMi, "OO" ) == 0 )
    {
      if ( syn->s0.MMM[0] && syn->s0.Ula && syn->s0.Ulo )
        {
          c += sprintf ( c, " %s%s%s", syn->s0.MMM, syn->s0.Ula, syn->s0.Ulo );
        }
      if ( syn->s0.h0h0h0h0[0] )
        {
          c += sprintf ( c, " %s%s", syn->s0.h0h0h0h0, syn->s0.im );
        }

    }

  *sec0 = c;
  return *sec0;
}

/*!
  \fn char * print_synop_sec1 (char **sec1, size_t lmax, struct synop_chunks *syn)
  \brief Prints the synop section 1
  \param sec1 the pointer where to print section
  \param lmax max length permited
  \param syn pointer to s atruct \ref synop_chunks where the parse results are set
*/
char * print_synop_sec1 ( char **sec1, size_t lmax, struct synop_chunks *syn )
{
  char *c = *sec1;

  if ( syn->mask & SYNOP_SEC1 )
    {
      // printf irixhVV
      if ( check_len ( sec1,6 ) )
        {
          c += sprintf ( c, " %s%s%s%s", syn->s1.ir, syn->s1.ix, syn->s1.h, syn->s1.VV );
        }

      // printf Nddff
      if ( check_len ( sec1,6 ) )
        {
          c += sprintf ( c, " %s%s%s", syn->s1.N, syn->s1.dd, syn->s1.ff );
        }

      if ( check_len ( sec1,6 ) && strlen ( syn->s1.fff ) )
        {
          c += sprintf ( c, " 00%s", syn->s1.fff );
        }

      // printf 1snTTT

      if ( check_len ( sec1,6 ) && syn->s1.TTT[0] )
        {
          c += sprintf ( c, " 1%s%s", syn->s1.sn1, syn->s1.TTT );
        }

      // printf 2snTdTdTd
      if ( check_len ( sec1,6 ) && syn->s1.TdTdTd[0] )
        {
          c += sprintf ( c, " 2%s%s", syn->s1.sn2, syn->s1.TdTdTd );
        }

      // printf 3PoPoPoPo
      if ( check_len ( sec1,6 ) && syn->s1.PoPoPoPo[0] )
        {
          c += sprintf ( c, " 3%s", syn->s1.PoPoPoPo );
        }

      // printf 4PPPP
      if ( check_len ( sec1,6 ) && syn->s1.PPPP[0] )
        {
          c += sprintf ( c, " 4%s", syn->s1.PPPP );
        }

      // printf 5appp
      if ( check_len ( sec1,6 ) && ( syn->s1.a[0] || syn->s1.ppp[0] ) )
        {
          if ( syn->s1.a[0] == 0 )
            {
              syn->s1.a[0] = '/';
            }
          if ( syn->s1.ppp[0] == 0 )
            {
              strcpy ( syn->s1.ppp, "///" );
            }
          c += sprintf ( c, " 5%s%s", syn->s1.a, syn->s1.ppp );
        }

      // printf 6RRRtr
      if ( check_len ( sec1,6 ) && ( syn->s1.tr[0] || syn->s1.RRR[0] ) )
        {
          if ( syn->s1.tr[0] == 0 )
            {
               syn->s1.tr[0] = '/';
            }
          if ( syn->s1.RRR[0] == 0 )
            {
              strcpy ( syn->s1.RRR, "///" );
            }
          c += sprintf ( c, " 6%s%s", syn->s1.RRR, syn->s1.tr );
        }

      if ( check_len ( sec1,6 ) && ( syn->s1.ww[0] || syn->s1.W1[0] || syn->s1.W2[0] ) )
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
          c += sprintf ( c, " 7%s%s%s", syn->s1.ww, syn->s1.W1, syn->s1.W2 );
        }

      if ( check_len ( sec1,6 ) && ( ( syn->s1.Nh[0] && syn->s1.Nh[0] != '0' && syn->s1.Nh[0] != '/' ) ||
                                     ( syn->s1.Cl[0] && syn->s1.Cl[0] != '0' && syn->s1.Cl[0] != '/' ) ||
                                     ( syn->s1.Cm[0] && syn->s1.Cm[0] != '0' && syn->s1.Cm[0] != '/' ) ||
                                     ( syn->s1.Ch[0] && syn->s1.Ch[0] != '0' && syn->s1.Ch[0] != '/' ) ) )
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
          c += sprintf ( c, " 8%s%s%s%s", syn->s1.Nh, syn->s1.Cl, syn->s1.Cm, syn->s1.Ch );
        }

      if ( check_len ( sec1,6 ) && syn->s1.GG[0] )
        {
          c+= sprintf ( c, " 9%s%s", syn->s1.GG, syn->s1.gg );
        }
    }
  *sec1 = c;
  return *sec1;
}


/*!
  \fn char * print_synop_sec2 (char **sec2, size_t lmax, struct synop_chunks *syn)
  \brief Prints the synop section 2
  \param sec2 the pointer where to print section
  \param lmax max length permited
  \param syn pointer to s atruct \ref synop_chunks where the parse results are set
*/
char * print_synop_sec2 ( char **sec2, size_t lmax, struct synop_chunks *syn )
{
  char *c = *sec2;
  if ( syn->mask & SYNOP_SEC2 )
    {
      // 222Dsvs
      if ( check_len ( sec2,46 ) )
        {
          c += sprintf ( c, " 222" );
          if ( syn->s2.Ds[0] )
            {
              c += sprintf ( c, "%s", syn->s2.Ds );
            }
          else
            {
              c += sprintf ( c, "/" );
            }

          if ( syn->s2.vs[0] )
            {
              c += sprintf ( c, "%s", syn->s2.vs );
            }
          else
            {
              c += sprintf ( c, "/" );
            }
        }

      // printf 0ssTwTwTw
      if ( check_len ( sec2,6 ) && syn->s2.TwTwTw[0] )
        {
          c += sprintf ( c, " 0%s%s", syn->s2.ss, syn->s2.TwTwTw );
        }

      if ( check_len ( sec2,6 ) && ( syn->s2.PwaPwa[0] || syn->s2.HwaHwa[0] ) )
        {
          if ( syn->s2.PwaPwa[0] )
            {
              c += sprintf ( c, " 1%s", syn->s2.PwaPwa );
            }
          else
            {
              c += sprintf ( c, " 1//" );
            }

          if ( syn->s2.HwaHwa[0] )
            {
              c += sprintf ( c, "%s", syn->s2.HwaHwa );
            }
          else
            {
              c += sprintf ( c, "//" );
            }
        }

      if ( check_len ( sec2,6 ) && ( syn->s2.PwPw[0] || syn->s2.HwHw[0] ) )
        {
          if ( syn->s2.PwPw[0] )
            {
              c += sprintf ( c, " 2%s", syn->s2.PwPw );
            }
          else
            {
              c += sprintf ( c, " 2//" );
            }

          if ( syn->s2.HwHw[0] )
            {
              c += sprintf ( c, "%s", syn->s2.HwHw );
            }
          else
            {
              c += sprintf ( c, "//" );
            }
        }

      if ( check_len ( sec2,6 ) && ( syn->s2.dw1dw1[0] || syn->s2.dw2dw2[0] ) )
        {
          if ( syn->s2.dw1dw1[0] )
            {
              c += sprintf ( c, " 3%s", syn->s2.dw1dw1 );
            }
          else
            {
              c += sprintf ( c, " 3//" );
            }

          if ( syn->s2.dw2dw2[0] )
            {
              c += sprintf ( c, "%s", syn->s2.dw2dw2 );
            }
          else
            {
              c += sprintf ( c, "//" );
            }
        }

      if ( check_len ( sec2,6 ) && ( syn->s2.Pw1Pw1[0] || syn->s2.Hw1Hw1[0] ) )
        {
          if ( syn->s2.Pw1Pw1[0] )
            {
              c += sprintf ( c, " 4%s", syn->s2.Pw1Pw1 );
            }
          else
            {
              c += sprintf ( c, " 4//" );
            }

          if ( syn->s2.Hw1Hw1[0] )
            {
              c += sprintf ( c, "%s", syn->s2.Hw1Hw1 );
            }
          else
            {
              c += sprintf ( c, "//" );
            }
        }


      if ( check_len ( sec2,6 ) && ( syn->s2.Pw2Pw2[0] || syn->s2.Hw2Hw2[0] ) )
        {
          if ( syn->s2.Pw2Pw2[0] )
            {
              c += sprintf ( c, " 5%s", syn->s2.Pw2Pw2 );
            }
          else
            {
              c += sprintf ( c, " 5//" );
            }

          if ( syn->s2.Hw2Hw2[0] )
            {
              c += sprintf ( c, "%s", syn->s2.Hw2Hw2 );
            }
          else
            {
              c += sprintf ( c, "//" );
            }
        }

      if ( check_len ( sec2,6 ) && syn->s2.HwaHwaHwa[0] )
        {
          c += sprintf ( c, " 70%s", syn->s2.HwaHwaHwa );
        }


      if ( check_len ( sec2,6 ) && syn->s2.TbTbTb[0] )
        {
          c += sprintf ( c, " 8%s%s", syn->s2.sw, syn->s2.TbTbTb );
        }

    }
  *sec2 = c;
  return *sec2;
}

/*!
  \fn char * print_synop_sec3 (char **sec3, size_t lmax, struct synop_chunks *syn)
  \brief Prints the synop section 3
  \param sec3 the pointer where to print section
  \param lmax max length permited
  \param syn pointer to s atruct \ref synop_chunks where the parse results are set
*/
char * print_synop_sec3 ( char **sec3, size_t lmax, struct synop_chunks *syn )
{
  size_t i;
  char *c = *sec3, *c0;
  if ( syn->mask & SYNOP_SEC3 )
    {
      if ( check_len ( sec3,4 ) )
        {
          c += sprintf ( c, " 333" );
        }

      // init point to write info.
      // in case we finally write nothing in this section
      c0 = c;

      // printf 1snxTxTxTx
      if ( check_len ( sec3,6 ) && syn->s3.XoXoXoXo[0] )
        {
          c += sprintf ( c, " 0%s", syn->s3.XoXoXoXo );
        }

      // printf 1snxTxTxTx
      if ( check_len ( sec3,6 ) && syn->s3.snx[0] )
        {
          c += sprintf ( c, " 1%s%s", syn->s3.snx, syn->s3.TxTxTx );
        }

      // printf 1snnTnTnTn
      if ( check_len ( sec3,6 ) && syn->s3.snn[0] )
        {
          c += sprintf ( c, " 2%s%s", syn->s3.snn, syn->s3.TnTnTn );
        }

      // printf 3Ejjj
      if ( check_len ( sec3,6 ) && ( syn->s3.E[0] || syn->s3.jjj[0] ) )
        {
          if ( syn->s3.E[0] == 0 )
            {
              syn->s3.E[0] = '/';
            }
          if ( syn->s3.jjj[0] == 0 )
            {
              strcpy ( syn->s3.jjj, "///" );
            }
          c += sprintf ( c, " 3%s%s", syn->s3.E, syn->s3.jjj );
        }

      /**** Radiation Sunshine gropus ***/

      // print 55SSS
      if ( check_len ( sec3,6 ) && syn->s3.SSS[0] && strcmp ( syn->s3.SSS,"///" ) )
        {
          c += sprintf ( c, " 55%s", syn->s3.SSS );
          for ( i = 0; i < 7; i++ )
            {
              if ( syn->s3.j524[i][0] )
                {
                  sprintf ( c, " %s%s", syn->s3.j524[i], syn->s3.FFFF24[i] );
                }
            }
        }

      // print 553SS
      if ( check_len ( sec3,6 ) && syn->s3.SS[0] && strcmp ( syn->s3.SS,"//" ) )
        {
          c += sprintf ( c, " 553%s", syn->s3.SS );
          for ( i = 0; i < 7; i++ )
            {
              if ( syn->s3.j5[i][0] )
                {
                  sprintf ( c, " %s%s", syn->s3.j5[i], syn->s3.FFFF[i] );
                }
            }

        }
      // printf 6RRRtr
      if ( check_len ( sec3,6 ) && ( syn->s3.tr[0] || syn->s3.RRR[0] ) )
        {
          if ( syn->s3.tr[0] == 0 )
            {
              syn->s3.tr[0] = '/';
            }
          if ( syn->s3.RRR[0] == 0 )
            {
              strcpy ( syn->s3.RRR, "///" );
            }
          c += sprintf ( c, " 6%s%s", syn->s3.RRR ,syn->s3.tr );
        }

      if ( check_len ( sec3,6 ) && syn->s3.RRRR24[0] )
        {
          c += sprintf ( c, " 7%s", syn->s3.RRRR24 );
        }

      // additional cloud layers
      i = 0;
      while ( i < 4 && syn->s3.nub[i].hshs[0] )
        {
          c += sprintf ( c, " 8%s%s%s", syn->s3.nub[i].Ns, syn->s3.nub[i].C, syn->s3.nub[i].hshs );
          i++;
        }

      // additional info
      for ( i = 0; i < syn->s3.d9.n ; i++ )
        {
          c += sprintf ( c, " %s%s", syn->s3.d9.misc[i].SpSp, syn->s3.d9.misc[i].spsp );
        }

      if ( c != c0 )
        {
          *sec3 = c;
        }
    }
  return *sec3;
}

/*!
  \fn char * print_synop_sec5 (char **sec5, size_t lmax, struct synop_chunks *syn)
  \brief Prints the synop section 5
  \param sec5 the pointer where to print section
  \param lmax max length permited
  \param syn pointer to s atruct \ref synop_chunks where the parse results are set
*/
char * print_synop_sec5 ( char **sec5, size_t lmax, struct synop_chunks *syn )
{
  size_t i;
  char *c = *sec5, *c0;

  if ( syn->mask & SYNOP_SEC5 )
    {
      if ( check_len ( sec5,4 ) )
        {
          c += sprintf ( c, " 555" );
        }

      // init point to write info.
      // in case we finally write nothing in this section
      c0 = c;

      // printf 6RRRtr
      if ( check_len ( sec5,6 ) && ( syn->s5.tr[0] || syn->s5.RRR[0] ) )
        {
          if ( syn->s5.tr[0] == 0 )
            {
              syn->s5.tr[0] = '/';
            }
          if ( syn->s5.RRR[0] == 0 )
            {
              strcpy ( syn->s5.RRR, "///" );
            }
          c += sprintf ( c, " 6%s%s", syn->s5.RRR ,syn->s5.tr );
        }

      // additional info
      for ( i = 0; i < syn->s5.d9.n ; i++ )
        {
          c += sprintf ( c, " %s%s", syn->s5.d9.misc[i].SpSp, syn->s5.d9.misc[i].spsp );
        }

      if ( c != c0 )
        {
          *sec5 = c;
        }
    }
  return *sec5;
}

/*!
 \fn int print_synop(char *report, size_t lmax, struct synop_chunks *syn)
 \brief prints a synop into a string
 \param report target string
 \param lmax max size of string
 \param syn pointer to a struct \ref synop_chunks with the result of parse tasks

 returns 0 if all went right
*/
int print_synop ( char *report, size_t lmax, struct synop_chunks *syn )
{
  char *c;

  c = report;

  // Needs time extension
  if ( syn->e.YYYY[0] == 0 )
    {
      return 1;
    }

  print_synop_sec0 ( &c, lmax, syn );

  if ( syn->mask & ( SYNOP_SEC1 | SYNOP_SEC2 | SYNOP_SEC3 | SYNOP_SEC5 ) )
    {
      print_synop_sec1 ( &c, lmax - strlen ( report ), syn );

      print_synop_sec2 ( &c, lmax - strlen ( report ), syn );

      print_synop_sec3 ( &c, lmax - strlen ( report ), syn );

      print_synop_sec5 ( &c, lmax - strlen ( report ), syn );
    }
  else
    {
      c += sprintf ( c, " NIL" );
    }
  c += sprintf ( c, "=" );

  return 0;
}


/*!
  \fn char * print_buoy_sec0 (char **sec0, size_t lmax, struct buoy_chunks *b)
  \brief Prints the buoy section 1
  \param sec0 the pointer where to print section
  \param lmax max length permited
  \param b pointer to s atruct \ref buoy_chunks where the parse results are set
*/
char * print_buoy_sec0 ( char **sec0, size_t lmax, struct buoy_chunks *b )
{
  char *c = *sec0;

  if ( check_len ( sec0,12 ) )
    {
      c += sprintf ( c, "%s%s%s%s%s", b->e.YYYY, b->e.MM, b->e.DD, b->e.HH, b->e.mm );
    }

  // Print type
  if ( check_len ( sec0,5 ) )
    {
      c += sprintf ( c, " %s%s", b->s0.MiMi, b->s0.MjMj );
    }

  if ( check_len ( sec0,6 ) && b->s0.A1[0] && b->s0.bw[0] && b->s0.nbnbnb[0] )
    {
      c += sprintf ( c, " %s%s%s", b->s0.A1, b->s0.bw, b->s0.nbnbnb );
    }
  else if ( check_len ( sec0, 10 ) && b->s0.D_D )
    {
      c += sprintf ( c, " %s", b->s0.D_D );
    }


  if ( check_len ( sec0,6 ) )
    {
      c += sprintf ( c, " %s%s%s", b->s0.YY, b->s0.MM, b->s0.J );
    }

  if ( check_len ( sec0,6 ) )
    {
      if ( b->s0.iw[0] )
        {
          c += sprintf ( c, " %s%s%s", b->s0.GG, b->s0.gg, b->s0.iw );
        }
      else
        {
          c += sprintf ( c, " %s%s/", b->s0.GG, b->s0.gg );
        }
    }

  if ( check_len ( sec0,7 ) )
    {
      c += sprintf ( c, " %s%s", b->s0.Qc, b->s0.LaLaLaLaLa );
    }


  if ( check_len ( sec0,7 ) )
    {
      c += sprintf ( c, " %s", b->s0.LoLoLoLoLoLo );
    }

  if ( b->s0.QA[0] || b->s0.Ql[0] || b->s0.Qt[0] )
    {
      if ( b->s0.Ql[0] )
        {
          c +=sprintf ( c, " 6%s", b->s0.Ql );
        }
      else
        {
          c +=sprintf ( c, " 6/" );
        }

      if ( b->s0.Qt[0] )
        {
          c +=sprintf ( c, "%s", b->s0.Qt );
        }
      else
        {
          c +=sprintf ( c, "/" );
        }

      if ( b->s0.QA[0] )
        {
          c +=sprintf ( c, "%s/", b->s0.QA );
        }
      else
        {
          c +=sprintf ( c, "//" );
        }
    }

  *sec0 = c;
  return *sec0;
}

/*!
  \fn char * print_buoy_sec1 (char **sec1, size_t lmax, struct buoy_chunks *b)
  \brief Prints the buoy section 1
  \param sec1 the pointer where to print section
  \param lmax max length permited
  \param b pointer to s atruct \ref buoy_chunks where the parse results are set
*/
char * print_buoy_sec1 ( char **sec1, size_t lmax, struct buoy_chunks *b )
{
  char *c = *sec1;

  if ( b->mask & BUOY_SEC1 )
    {
      // 111QdQx
      c += sprintf ( c," 111" );

      if ( b->s1.Qd[0] )
        {
          c += sprintf ( c,"%s", b->s1.Qd );
        }
      else
        {
          c += sprintf ( c,"/" );
        }

      if ( b->s1.Qx[0] )
        {
          c += sprintf ( c,"%s", b->s1.Qx );
        }
      else
        {
          c += sprintf ( c,"/" );
        }

      // 0ddff
      if ( check_len ( sec1,6 ) )
        {
          if ( b->s1.dd[0] || b->s1.ff[0] )
            {
              c += sprintf ( c, " 0" );
              if ( b->s1.dd[0] )
                {
                  c += sprintf ( c, "%s", b->s1.dd );
                }
              else
                {
                  c += sprintf ( c, "//" );
                }

              if ( b->s1.ff[0] )
                {
                  c += sprintf ( c, "%s", b->s1.ff );
                }
              else
                {
                  c += sprintf ( c, "//" );
                }
            }
        }

      // 1snTTT
      if ( check_len ( sec1,6 ) && b->s1.TTT[0] )
        {
          c += sprintf ( c, " 1%s%s", b->s1.sn1, b->s1.TTT );
        }

      // 2snTdTdTd
      if ( check_len ( sec1,6 ) && b->s1.TdTdTd[0] )
        {
          c += sprintf ( c, " 2%s%s", b->s1.sn2, b->s1.TdTdTd );
        }

      // 3PoPoPoPo
      if ( check_len ( sec1,6 ) && b->s1.PoPoPoPo[0] )
        {
          c += sprintf ( c, " 3%s", b->s1.PoPoPoPo );
        }

      // printf 4PPPP
      if ( check_len ( sec1,6 ) && b->s1.PPPP[0] )
        {
          c += sprintf ( c, " 4%s", b->s1.PPPP );
        }

      // printf 5appp
      if ( check_len ( sec1,6 ) && ( b->s1.a[0] || b->s1.ppp[0] ) )
        {
          if ( b->s1.a[0] == 0 )
            {
              b->s1.a[0] = '/';
            }
          if ( b->s1.ppp[0] == 0 )
            {
              strcpy ( b->s1.ppp, "///" );
            }
          c += sprintf ( c, " 5%s%s", b->s1.a, b->s1.ppp );
        }
    }

  *sec1 = c;
  return *sec1;
}

/*!
  \fn char * print_buoy_sec2 (char **sec2, size_t lmax, struct buoy_chunks *b)
  \brief Prints the buoy section 1
  \param sec2 the pointer where to print section
  \param lmax max length permited
  \param b pointer to s atruct \ref buoy_chunks where the parse results are set
*/
char * print_buoy_sec2 ( char **sec2, size_t lmax, struct buoy_chunks *b )
{
  char *c = *sec2;

  if ( b->mask & BUOY_SEC2 )
    {
      // 222QdQx
      c += sprintf ( c," 222" );

      if ( b->s2.Qd[0] )
        {
          c += sprintf ( c,"%s", b->s2.Qd );
        }
      else
        {
          c += sprintf ( c,"/" );
        }

      if ( b->s2.Qx[0] )
        {
          c += sprintf ( c,"%s", b->s2.Qx );
        }
      else
        {
          c += sprintf ( c,"/" );
        }

      // 0snTwTwTw
      if ( check_len ( sec2,6 ) && b->s2.TwTwTw[0] )
        {
          c += sprintf ( c, " 0%s%s", b->s2.sn, b->s2.TwTwTw );
        }

      // 1PwaPwaHwaHwa
      if ( check_len ( sec2,6 ) && ( b->s2.PwaPwa[0] || b->s2.HwaHwa[0] ) )
        {
          c += sprintf ( c, " 1" );
          if ( b->s2.PwaPwa[0] )
            {
              c += sprintf ( c,"%s", b->s2.PwaPwa );
            }
          else
            {
              c += sprintf ( c,"//" );
            }

          if ( b->s2.HwaHwa[0] )
            {
              c += sprintf ( c,"%s", b->s2.HwaHwa );
            }
          else
            {
              c += sprintf ( c,"//" );
            }
        }

      // 20PwaPwaPwa
      if ( check_len ( sec2,6 ) && b->s2.PwaPwaPwa[0] )
        {
          c += sprintf ( c, " 20%s", b->s2.PwaPwaPwa );
        }

      // 21HwaHwaHwa
      if ( check_len ( sec2,6 ) && b->s2.HwaHwaHwa[0] )
        {
          c += sprintf ( c, " 21%s", b->s2.HwaHwaHwa );
        }


    }

  *sec2 = c;
  return *sec2;
}

/*!
  \fn char * print_buoy_sec3 (char **sec3, size_t lmax, struct buoy_chunks *b)
  \brief Prints the buoy section 3
  \param sec3 the pointer where to print section
  \param lmax max length permited
  \param b pointer to s atruct \ref buoy_chunks where the parse results are set
*/
char * print_buoy_sec3 ( char **sec3, size_t lmax, struct buoy_chunks *b )
{
  char *c = *sec3;
  size_t l;

  if ( check_len ( sec3,6 ) && ( b->mask & BUOY_SEC3 ) )
    {
      c += sprintf ( c, " 333%s%s", b->s3.Qd1, b->s3.Qd2 );

      // check if has 8887k2
      l = 0;
      while ( b->s3.l1[l].zzzz[0] && l < 32 )
        {
          if ( l == 0 )
            {
              c += sprintf ( c, " 8887%s", b->s3.k2 );
            }
          c += sprintf ( c,  " 2%s", b->s3.l1[l].zzzz );

          if ( b->s3.l1[l].TTTT[0] )
            {
              c += sprintf ( c,  " 3%s", b->s3.l1[l].TTTT );
            }

          if ( b->s3.l1[l].SSSS[0] )
            {
              c += sprintf ( c,  " 4%s", b->s3.l1[l].SSSS );
            }
          l++;
        }

      l = 0;
      while ( b->s3.l2[l].zzzz[0] && l < 32 )
        {
          if ( l == 0 )
            {
              c += sprintf ( c, " 66%s9%s", b->s3.k6, b->s3.k3 );
            }
          c += sprintf ( c,  " 2%s", b->s3.l2[l].zzzz );

          if ( b->s3.l2[l].dd[0] || b->s3.l2[l].ccc[0] )
            {
              if ( b->s3.l2[l].dd[0] )
                {
                  c += sprintf ( c,  " %s", b->s3.l2[l].dd );
                }
              else
                {
                  c += sprintf ( c,  " //" );
                }
              if ( b->s3.l2[l].ccc[0] )
                {
                  c += sprintf ( c,  "%s", b->s3.l2[l].ccc );
                }
              else
                {
                  c += sprintf ( c,  "///" );
                }
            }
          l++;
        }
    }

  *sec3 = c;
  return *sec3;
}

/*!
 \fn int print_buoy(char *report, size_t lmax, struct buoy_chunks *b)
 \brief prints a buoy into a string
 \param report target string
 \param lmax max size of string
 \param b pointer to a struct \ref buoy_chunks with the result of parse tasks

 returns 0 if all went right
*/
int print_buoy ( char *report, size_t lmax, struct buoy_chunks *b )
{
  char *c;

  c = report;

  // Needs time extension
  if ( b->e.YYYY[0] == 0 )
    {
      return 1;
    }

  print_buoy_sec0 ( &c, lmax, b );

  if ( b->mask & ( BUOY_SEC1 | BUOY_SEC2 | BUOY_SEC3 ) )
    {
      print_buoy_sec1 ( &c, lmax - strlen ( report ), b );

      print_buoy_sec2 ( &c, lmax - strlen ( report ), b );

      print_buoy_sec3 ( &c, lmax - strlen ( report ), b );
    }
  else
    {
      c+= sprintf ( c, " NIL" );
    }
  c += sprintf ( c, "=" );

  return 0;
}


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
 \fn int print_climat(char *report, size_t lmax, struct climat_chunks *cl)
 \brief prints a climat into a string
 \param report target string
 \param lmax max size of string
 \param cl pointer to a struct \ref climat_chunks with the result of parse tasks

 returns 0 if all went right
*/
int print_climat ( char *report, size_t lmax, struct climat_chunks *cl )
{
  char *c;

  c = report;

  // Needs time extension
  if ( cl->e.YYYY[0] == 0 || cl->e.MM[0] == 0)
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

