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
 \file bufr2tac_print_buoy.c
 \brief file with the code to print the results for buoy reports
 */
#include "bufr2tac.h"

/*!
  \def check_len(ori,inc)
  \brief cheks if there still memory enough to add \a inc chars
*/
#define check_len(ori,inc) (c - *ori + inc < (int)lmax)

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

  if ( check_len ( sec0,8 ) && b->s0.A1[0] && b->s0.bw[0] && b->s0.nbnbnb[0] )
    {
      c += sprintf ( c, " %s%s%s", b->s0.A1, b->s0.bw, b->s0.nbnbnb );
    }
  else if ( check_len ( sec0, 10 ) && b->s0.D_D[0] )
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
 \fn int print_buoy(char *report, size_t lmax, struct buoy_chunks *b, int mode  )
 \brief prints a buoy into a string
 \param report target string
 \param lmax max size of string
 \param b pointer to a struct \ref buoy_chunks with the result of parse tasks
 \param mode If == 0 legacy mode. If == 1 the print WIGOS identifier

 returns 0 if all went right
*/
int print_buoy ( char *report, size_t lmax, struct buoy_chunks *b, int mode   )
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


