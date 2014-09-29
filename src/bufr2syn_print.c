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
 \file bufr2syn_print.c
 \brief file with the code to print the results
 */
#include "bufr2synop.h"

/*!
  \def check_len(ori,inc)
  \brief cheks if there still memory enough to add \a inc chars 
*/
#define check_len(ori,inc) (c - *ori + inc < lmax)

/*!
  \fn char * print_synop_sec0 (char **sec0, size_t lmax, struct synop_chunks *syn)
  \brief Prints the synop section 0 (header)
  \param sec0 the pointer where to print section
  \param lmax max length permited
  \param syn pointer to s atruct \ref synop_chunks where the parse results are set
*/
char * print_synop_sec0 (char **sec0, size_t lmax, struct synop_chunks *syn)
{
  size_t i;
  char *c = *sec0;

  if (check_len(sec0,12))
    c += sprintf ( c, "%s%s%s%s%s", syn->e.YYYY, syn->e.MM, syn->e.DD, syn->e.HH, syn->e.mm );

  // Print type
  if (check_len(sec0,5))
    c += sprintf ( c, " %s%s", syn->s0.MiMi, syn->s0.MjMj );

  if (check_len(sec0,6))
    {
      if (syn->s0.D_D[0])
        c += sprintf ( c, " %s", syn->s0.D_D);
      else if (syn->s0.A1[0] && syn->s0.bw[0] && syn->s0.nbnbnb[0])
        c += sprintf ( c, " %s%s%s", syn->s0.A1, syn->s0.bw, syn->s0.nbnbnb);

    }

  // print YYGGiw
  if (check_len(sec0,6))
    c += sprintf ( c, " %s%s%s", syn->s0.YY, syn->s0.GG, syn->s0.iw );

  // print IIiii
  if (check_len(sec0,6) && syn->s0.II[0])
    c += sprintf ( c, " %s%s", syn->s0.II, syn->s0.iii );
  else
    {
      if (syn->s0.LaLaLa[0])
        c += sprintf ( c, " 99%s", syn->s0.LaLaLa );
      else
        c += sprintf ( c, " 99///");

      if (syn->s0.Qc[0] && syn->s0.LoLoLoLo[0])
        c += sprintf ( c, " %s%s", syn->s0.Qc, syn->s0.LoLoLoLo);
      else
        c += sprintf ( c, " /////");
    }

  if (strcmp(syn->s0.MiMi, "OO") == 0)
    {
      if (syn->s0.MMM[0] && syn->s0.Ula && syn->s0.Ulo)
        c += sprintf ( c, " %s%s%s", syn->s0.MMM, syn->s0.Ula, syn->s0.Ulo);
      if (syn->s0.h0h0h0h0[0])
        c += sprintf ( c, " %s%s", syn->s0.h0h0h0h0, syn->s0.im);

    }

  *sec0 = c;
  return *sec0;
}

/*!
  \fn char * print_synop_sec1 (char **sec0, size_t lmax, struct synop_chunks *syn)
  \brief Prints the synop section 1
  \param sec0 the pointer where to print section
  \param lmax max length permited
  \param syn pointer to s atruct \ref synop_chunks where the parse results are set
*/
char * print_synop_sec1 (char **sec1, size_t lmax, struct synop_chunks *syn)
{
  size_t i;
  char *c = *sec1;

  if (syn->mask & SYNOP_SEC1)
    {
      // printf Nddff
      if (check_len(sec1,6))
        c += sprintf ( c, " %s%s%s%s", syn->s1.ir, syn->s1.ix, syn->s1.h, syn->s1.VV );

      // printf Nddff
      if (check_len(sec1,6))
        c += sprintf ( c, " %s%s%s", syn->s1.N, syn->s1.dd, syn->s1.ff );

      if (check_len(sec1,6) && strlen ( syn->s1.fff ) )
        c += sprintf ( c, " 00%s", syn->s1.fff );

      // printf 1snTTT

      if (check_len(sec1,6) && syn->s1.TTT[0] )
        c += sprintf ( c, " 1%s%s", syn->s1.sn1, syn->s1.TTT );

      // printf 2snTdTdTd
      if ( check_len(sec1,6) && syn->s1.TdTdTd[0] )
        c += sprintf ( c, " 2%s%s", syn->s1.sn2, syn->s1.TdTdTd );

      // printf 3PoPoPoPo
      if ( check_len(sec1,6) && syn->s1.PoPoPoPo[0] )
        c += sprintf ( c, " 3%s", syn->s1.PoPoPoPo );

      // printf 4PPPP
      if ( check_len(sec1,6) && syn->s1.PPPP[0] )
        c += sprintf ( c, " 4%s", syn->s1.PPPP );

      // printf 5appp
      if ( check_len(sec1,6) && syn->s1.a[0] || syn->s1.ppp[0] )
        {
          if ( syn->s1.a[0] == 0 )
            syn->s1.a[0] = '/';
          if ( syn->s1.ppp[0] == 0 )
            strcpy ( syn->s1.ppp, "///" );
          c += sprintf ( c, " 5%s%s", syn->s1.a, syn->s1.ppp );
        }

      // printf 6RRRtr
      if ( check_len(sec1,6) && (syn->s1.tr[0] || syn->s1.RRR[0]))
        {
          if ( syn->s1.tr[0] == 0 )
            syn->s1.tr[0] = '/';
          if ( syn->s1.RRR[0] == 0 )
            strcpy ( syn->s1.RRR, "///" );
          c += sprintf ( c, " 6%s%s", syn->s1.RRR, syn->s1.tr);
        }

      if ( check_len(sec1,6) && (syn->s1.ww[0] || syn->s1.W1[0] || syn->s1.W2[0]))
        {
          if ( syn->s1.ww[0] == 0 )
            strcpy ( syn->s1.ww, "//" );
          if ( syn->s1.W1[0] == 0 )
            strcpy ( syn->s1.W1, "/" );
          if ( syn->s1.W2[0] == 0 )
            strcpy ( syn->s1.W2, "/" );
          c += sprintf ( c, " 7%s%s%s", syn->s1.ww, syn->s1.W1, syn->s1.W2 );
        }

      if ( check_len(sec1,6) && ((syn->s1.Nh[0] && syn->s1.Nh[0] != '0' && syn->s1.Nh[0] != '/') ||
                                 (syn->s1.Cl[0] && syn->s1.Cl[0] != '0' && syn->s1.Cl[0] != '/') ||
                                 (syn->s1.Cm[0] && syn->s1.Cm[0] != '0' && syn->s1.Cm[0] != '/') ||
                                 (syn->s1.Ch[0] && syn->s1.Ch[0] != '0' && syn->s1.Ch[0] != '/')))
        {
          if ( syn->s1.Nh[0] == 0 )
            strcpy ( syn->s1.Nh, "/" );
          if ( syn->s1.Cl[0] == 0 )
            strcpy ( syn->s1.Cl, "/" );
          if ( syn->s1.Cm[0] == 0 )
            strcpy ( syn->s1.Cm, "/" );
          if ( syn->s1.Ch[0] == 0 )
            strcpy ( syn->s1.Ch, "/" );
          c += sprintf(c, " 8%s%s%s%s", syn->s1.Nh, syn->s1.Cl, syn->s1.Cm, syn->s1.Ch);
        }

      if ( check_len(sec1,6) && syn->s1.GG[0] )
        c+= sprintf ( c, " 9%s%s", syn->s1.GG, syn->s1.gg );
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
char * print_synop_sec2 (char **sec2, size_t lmax, struct synop_chunks *syn)
{
  size_t i;
  char *c = *sec2;
  if ( syn->mask & SYNOP_SEC2 )
    {
      // 222Dsvs
      if (check_len(sec2,46))
        {
          c += sprintf ( c, " 222");
          if (syn->s2.Ds[0])
            c += sprintf ( c, "%s", syn->s2.Ds);
          else
            c += sprintf ( c, "/");

          if (syn->s2.vs[0])
            c += sprintf ( c, "%s", syn->s2.vs);
          else
            c += sprintf ( c, "/");
        }

      // printf 0ssTwTwTw
      if (check_len(sec2,6) && syn->s2.TwTwTw[0] )
        c += sprintf ( c, " 0%s%s", syn->s2.ss, syn->s2.TwTwTw );

      if (check_len(sec2,6) && (syn->s2.PwaPwa[0] || syn->s2.HwaHwa[0]))
        {
          if (syn->s2.PwaPwa[0])
            c += sprintf ( c, " 1%s", syn->s2.PwaPwa);
          else
            c += sprintf ( c, " 1//");

          if (syn->s2.HwaHwa[0])
            c += sprintf ( c, "%s", syn->s2.HwaHwa);
          else
            c += sprintf ( c, "//");
        }

      if (check_len(sec2,6) && (syn->s2.PwPw[0] || syn->s2.HwHw[0]))
        {
          if (syn->s2.PwPw[0])
            c += sprintf ( c, " 2%s", syn->s2.PwPw);
          else
            c += sprintf ( c, " 2//");

          if (syn->s2.HwHw[0])
            c += sprintf ( c, "%s", syn->s2.HwHw);
          else
            c += sprintf ( c, "//");
        }

      if (check_len(sec2,6) && (syn->s2.dw1dw1[0] || syn->s2.dw2dw2[0]))
        {
          if (syn->s2.dw1dw1[0])
            c += sprintf ( c, " 3%s", syn->s2.dw1dw1);
          else
            c += sprintf ( c, " 3//");

          if (syn->s2.dw2dw2[0])
            c += sprintf ( c, "%s", syn->s2.dw2dw2);
          else
            c += sprintf ( c, "//");
        }

      if (check_len(sec2,6) && (syn->s2.Pw1Pw1[0] || syn->s2.Hw1Hw1[0]))
        {
          if (syn->s2.Pw1Pw1[0])
            c += sprintf ( c, " 4%s", syn->s2.Pw1Pw1);
          else
            c += sprintf ( c, " 4//");

          if (syn->s2.Hw1Hw1[0])
            c += sprintf ( c, "%s", syn->s2.Hw1Hw1);
          else
            c += sprintf ( c, "//");
        }


      if (check_len(sec2,6) && (syn->s2.Pw2Pw2[0] || syn->s2.Hw2Hw2[0]))
        {
          if (syn->s2.Pw2Pw2[0])
            c += sprintf ( c, " 5%s", syn->s2.Pw2Pw2);
          else
            c += sprintf ( c, " 5//");

          if (syn->s2.Hw2Hw2[0])
            c += sprintf ( c, "%s", syn->s2.Hw2Hw2);
          else
            c += sprintf ( c, "//");
        }

      if (check_len(sec2,6) && syn->s2.HwaHwaHwa[0])
        {
          c += sprintf(c, " 70%s", syn->s2.HwaHwaHwa);
        }


      if (check_len(sec2,6) && syn->s2.TbTbTb[0] )
        c += sprintf ( c, " 8%s%s", syn->s2.sw, syn->s2.TbTbTb );

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
char * print_synop_sec3 (char **sec3, size_t lmax, struct synop_chunks *syn)
{
  size_t i;
  char *c = *sec3;
  if ( syn->mask & SYNOP_SEC3 )
    {
      if (check_len(sec3,4))
        c += sprintf ( c, " 333" );

      // printf 1snxTxTxTx
      if (check_len(sec3,6) && syn->s3.XoXoXoXo[0])
        {
          c += sprintf(c, " 0%s", syn->s3.XoXoXoXo);
        }

      // printf 1snxTxTxTx
      if (check_len(sec3,6) && syn->s3.snx[0])
        {
          c += sprintf(c, " 1%s%s", syn->s3.snx, syn->s3.TxTxTx);
        }

      // printf 1snnTnTnTn
      if (check_len(sec3,6) && syn->s3.snn[0])
        {
          c += sprintf(c, " 2%s%s", syn->s3.snn, syn->s3.TnTnTn);
        }

      // printf 3Ejjj
      if (check_len(sec3,6) && (syn->s3.E[0] || syn->s3.jjj[0]))
        {
          if (syn->s3.E[0] == 0)
            syn->s3.E[0] = '/';
          if (syn->s3.jjj[0] == 0)
            strcpy(syn->s3.jjj, "///");
          c += sprintf(c, " 3%s%s", syn->s3.E, syn->s3.jjj);
        }

      /**** Radiation Sunshine gropus ***/

      // print 55SSS
      if (check_len(sec3,6) && syn->s3.SSS[0])
        {
          c += sprintf(c, " 55%s", syn->s3.SSS);
          for (i = 0; i < 7; i++)
            {
              if (syn->s3.j524[i][0])
                sprintf(c, " %s%s", syn->s3.j524[i], syn->s3.FFFF24[i]);
            }
        }

      // print 553SS
      if (check_len(sec3,6) && syn->s3.SS[0])
        {
          c += sprintf(c, " 553%s", syn->s3.SS);
          for (i = 0; i < 7; i++)
            {
              if (syn->s3.j5[i][0])
                sprintf(c, " %s%s", syn->s3.j5[i], syn->s3.FFFF[i]);
            }

        }
      // printf 6RRRtr
      if (check_len(sec3,6) && ( syn->s3.tr[0] || syn->s3.RRR[0]) )
        {
          if ( syn->s3.tr[0] == 0 )
            syn->s3.tr[0] = '/';
          if ( syn->s3.RRR[0] == 0 )
            strcpy ( syn->s3.RRR, "///" );
          c += sprintf ( c, " 6%s%s", syn->s3.RRR ,syn->s3.tr);
        }

      if ( check_len(sec3,6) && syn->s3.RRRR24[0] )
        c += sprintf ( c, " 7%s", syn->s3.RRRR24);

      // additional cloud layers
      i = 0;
      while (i < 4 && syn->s3.nub[i].hshs[0])
      {
        c += sprintf ( c, " 8%s%s%s", syn->s3.nub[i].Ns, syn->s3.nub[i].C, syn->s3.nub[i].hshs);
        i++;
      }
    }
  *sec3 = c;
  return *sec3;
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
  size_t i;
  char *c;

  c = report;

  // Needs time extension
  if (syn->e.YYYY[0] == 0)
    return 1;

  print_synop_sec0(&c, lmax, syn);

  if (syn->mask & (SYNOP_SEC1 | SYNOP_SEC2 | SYNOP_SEC3))
    {
      print_synop_sec1(&c, lmax - strlen(report), syn);

      print_synop_sec2(&c, lmax - strlen(report), syn);

      print_synop_sec3(&c, lmax - strlen(report), syn);
    }
  else
    c += sprintf(c, " NIL");
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
char * print_buoy_sec0 (char **sec0, size_t lmax, struct buoy_chunks *b)
{
  size_t i;
  char *c = *sec0;

  if (check_len(sec0,12))
    c += sprintf ( c, "%s%s%s%s%s", b->e.YYYY, b->e.MM, b->e.DD, b->e.HH, b->e.mm );

  // Print type
  if (check_len(sec0,5))
    c += sprintf ( c, " %s%s", b->s0.MiMi, b->s0.MjMj );

  if (check_len(sec0,6) && b->s0.A1[0] && b->s0.bw[0] && b->s0.nbnbnb[0])
    {
      c += sprintf ( c, " %s%s%s", b->s0.A1, b->s0.bw, b->s0.nbnbnb);
    }

  if (check_len(sec0,6))
    c += sprintf(c, " %s%s%s", b->s0.YY, b->s0.MM, b->s0.J);

  if (check_len(sec0,6))
    {
      if (  b->s0.iw[0])
        c += sprintf(c, " %s%s%s", b->s0.GG, b->s0.gg, b->s0.iw);
      else
        c += sprintf(c, " %s%s/", b->s0.GG, b->s0.gg);
    }

  if (check_len(sec0,7))
    c += sprintf(c, " %s%s", b->s0.Qc, b->s0.LaLaLaLaLa);


  if (check_len(sec0,7))
    c += sprintf(c, " %s", b->s0.LoLoLoLoLoLo);

  if (b->s0.QA[0] || b->s0.Ql[0] || b->s0.Qt[0])
    {
      if (b->s0.Ql[0])
        c +=sprintf(c, " 6%s", b->s0.Ql);
      else
        c +=sprintf(c, " 6/");

      if (b->s0.Qt[0])
        c +=sprintf(c, "%s", b->s0.Qt);
      else
        c +=sprintf(c, "/");

      if (b->s0.QA[0])
        c +=sprintf(c, "%s/", b->s0.QA);
      else
        c +=sprintf(c, "//");
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
char * print_buoy_sec1 (char **sec1, size_t lmax, struct buoy_chunks *b)
{
  char *c = *sec1;

  if ( b->mask & BUOY_SEC1)
    {
      // 111QdQx
      c += sprintf(c," 111");

      if (b->s1.Qd[0])
        c += sprintf(c,"%s", b->s1.Qd);
      else
        c += sprintf(c,"/");

      if (b->s1.Qx[0])
        c += sprintf(c,"%s", b->s1.Qx);
      else
        c += sprintf(c,"/");

      // 0ddff
      if (check_len(sec1,6))
        {
          if (b->s1.dd[0] || b->s1.ff[0])
            {
              c += sprintf(c, " 0");
              if (b->s1.dd[0])
                c += sprintf ( c, "%s", b->s1.dd);
              else
                c += sprintf ( c, "//");

              if (b->s1.ff[0])
                c += sprintf ( c, "%s", b->s1.ff);
              else
                c += sprintf ( c, "//");
            }
        }

      // 1snTTT
      if (check_len(sec1,6) && b->s1.TTT[0])
        c += sprintf(c, " 1%s%s", b->s1.sn1, b->s1.TTT);

      // 2snTdTdTd
      if (check_len(sec1,6) && b->s1.TdTdTd[0])
        c += sprintf(c, " 2%s%s", b->s1.sn2, b->s1.TdTdTd);

      // 3PoPoPoPo
      if (check_len(sec1,6) && b->s1.PoPoPoPo[0])
        c += sprintf(c, " 3%s", b->s1.PoPoPoPo);

      // printf 4PPPP
      if ( check_len(sec1,6) && b->s1.PPPP[0] )
        c += sprintf ( c, " 4%s", b->s1.PPPP );

      // printf 5appp
      if ( check_len(sec1,6) && b->s1.a[0] || b->s1.ppp[0] )
        {
          if ( b->s1.a[0] == 0 )
            b->s1.a[0] = '/';
          if ( b->s1.ppp[0] == 0 )
            strcpy ( b->s1.ppp, "///" );
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
char * print_buoy_sec2 (char **sec2, size_t lmax, struct buoy_chunks *b)
{
  char *c = *sec2;

  if (b->mask & BUOY_SEC2)
    {
      // 222QdQx
      c += sprintf(c," 222");

      if (b->s2.Qd[0])
        c += sprintf(c,"%s", b->s2.Qd);
      else
        c += sprintf(c,"/");

      if (b->s2.Qx[0])
        c += sprintf(c,"%s", b->s2.Qx);
      else
        c += sprintf(c,"/");

      // 0snTwTwTw
      if (check_len(sec2,6) && b->s2.TwTwTw[0] )
        c += sprintf ( c, " 0%s%s", b->s2.sn, b->s2.TwTwTw );

      // 1PwaPwaHwaHwa
      if (check_len(sec2,6) && (b->s2.PwaPwa[0] || b->s2.HwaHwa[0]))
        {
          c += sprintf(c, " 1");
          if (b->s2.PwaPwa[0])
            c += sprintf(c,"%s", b->s2.PwaPwa);
          else
            c += sprintf(c,"//");

          if (b->s2.HwaHwa[0])
            c += sprintf(c,"%s", b->s2.HwaHwa);
          else
            c += sprintf(c,"//");
        }

      // 20PwaPwaPwa
      if (check_len(sec2,6) && b->s2.PwaPwaPwa[0])
        c += sprintf(c, " 20%s", b->s2.PwaPwaPwa);

      // 21HwaHwaHwa
      if (check_len(sec2,6) && b->s2.HwaHwaHwa[0])
        c += sprintf(c, " 21%s", b->s2.HwaHwaHwa);


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
char * print_buoy_sec3 (char **sec3, size_t lmax, struct buoy_chunks *b)
{
  char *c = *sec3;
  size_t l;

  if (b->mask & BUOY_SEC3)
    {
      c += sprintf(c, " 333%s%s", b->s3.Qd1, b->s3.Qd2);

      // check if has 8887k2
      l = 0;
      while (b->s3.l1[l].zzzz[0] && l < 32)
        {
          if (l == 0)
            c += sprintf(c, " 8887%s", b->s3.k2);
          c += sprintf(c,  " 2%s", b->s3.l1[l].zzzz);

          if (b->s3.l1[l].TTTT[0])
            c += sprintf(c,  " 3%s", b->s3.l1[l].TTTT);

          if (b->s3.l1[l].SSSS[0])
            c += sprintf(c,  " 4%s", b->s3.l1[l].SSSS);
          l++;
        }

      l = 0;
      while (b->s3.l2[l].zzzz[0] && l < 32)
        {
          if (l == 0)
            c += sprintf(c, " 66%s9%s", b->s3.k6, b->s3.k3);
          c += sprintf(c,  " 2%s", b->s3.l2[l].zzzz);

          if (b->s3.l2[l].dd[0] || b->s3.l2[l].ccc[0])
            {
              if (b->s3.l2[l].dd[0])
                c += sprintf(c,  " %s", b->s3.l2[l].dd);
              else
                c += sprintf(c,  " //");
              if (b->s3.l2[l].ccc[0])
                c += sprintf(c,  "%s", b->s3.l2[l].ccc);
              else
                c += sprintf(c,  "///");
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
 \param syn pointer to a struct \ref buoy_chuncks with the result of parse tasks

 returns 0 if all went right
*/
int print_buoy ( char *report, size_t lmax, struct buoy_chunks *b )
{
  char *c;

  c = report;

  // Needs time extension
  if (b->e.YYYY[0] == 0)
    return 1;

  print_buoy_sec0(&c, lmax, b);

  if (b->mask & (BUOY_SEC1 | BUOY_SEC2 | BUOY_SEC3))
    {
      print_buoy_sec1(&c, lmax - strlen(report), b);

      print_buoy_sec2(&c, lmax - strlen(report), b);

      print_buoy_sec3(&c, lmax - strlen(report), b);
    }
  else
    c+= sprintf(c, " NIL");
  c += sprintf ( c, "=" );

  return 0;
}


