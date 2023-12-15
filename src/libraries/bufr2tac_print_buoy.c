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
 \file bufr2tac_print_buoy.c
 \brief file with the code to print the results for buoy reports
 */
#include "bufr2tac.h"

/*!
  \fn size_t print_buoy_sec0 (char **sec0, size_t lmax, struct buoy_chunks *b)
  \brief Prints the buoy section 1
  \param sec0 the pointer where to print section
  \param lmax max length permited
  \param b pointer to s atruct \ref buoy_chunks where the parse results are set
*/
size_t print_buoy_sec0 ( char **sec0, size_t lmax, struct buoy_chunks *b )
{
  size_t used = 0;
  char *c = *sec0;

  used += snprintf ( c + used, lmax - used, "%s%s%s%s%s", b->e.YYYY, b->e.MM, b->e.DD, b->e.HH, b->e.mm );

  // Print type
  used += snprintf ( c + used, lmax - used, " %s%s", b->s0.MiMi, b->s0.MjMj );

  if ( b->s0.A1[0] && b->s0.bw[0] && b->s0.nbnbnb[0] )
    {
      used += snprintf ( c + used, lmax - used, " %s%s%s", b->s0.A1, b->s0.bw, b->s0.nbnbnb );
    }
  else if ( b->s0.D_D[0] )
    {
      used += snprintf ( c + used, lmax - used, " %s", b->s0.D_D );
    }


  used += snprintf ( c + used, lmax - used, " %s%s%s", b->s0.YY, b->s0.MM, b->s0.J );

  if ( b->s0.iw[0] )
    {
      used += snprintf ( c + used, lmax - used, " %s%s%s", b->s0.GG, b->s0.gg, b->s0.iw );
    }
  else
    {
      used += snprintf ( c + used, lmax - used, " %s%s/", b->s0.GG, b->s0.gg );
    }

  used += snprintf ( c + used, lmax - used, " %s%s", b->s0.Qc, b->s0.LaLaLaLaLa );


  used += snprintf ( c + used, lmax - used, " %s", b->s0.LoLoLoLoLoLo );

  if ( b->s0.QA[0] || b->s0.Ql[0] || b->s0.Qt[0] )
    {
      if ( b->s0.Ql[0] )
        {
          used += snprintf ( c + used, lmax - used,  " 6%s", b->s0.Ql );
        }
      else
        {
          used += snprintf ( c + used, lmax - used, " 6/" );
        }

      if ( b->s0.Qt[0] )
        {
          used +=snprintf ( c + used, lmax - used, "%s", b->s0.Qt );
        }
      else
        {
          used += snprintf ( c + used, lmax - used,  "/" );
        }

      if ( b->s0.QA[0] )
        {
          used += snprintf ( c + used, lmax - used,  "%s/", b->s0.QA );
        }
      else
        {
          used += snprintf ( c + used, lmax - used, "//" );
        }
    }

  *sec0 = c + used;
  return used;
}


/*!
  \fn size_t print_buoy_sec1 (char **sec1, size_t lmax, struct buoy_chunks *b)
  \brief Prints the buoy section 1
  \param sec1 the pointer where to print section
  \param lmax max length permited
  \param b pointer to s atruct \ref buoy_chunks where the parse results are set
*/
size_t print_buoy_sec1 ( char **sec1, size_t lmax, struct buoy_chunks *b )
{
  size_t used = 0;
  char *c = *sec1;

  if ( b->mask & BUOY_SEC1 )
    {
      // 111QdQx
      used += snprintf ( c + used, lmax - used," 111" );

      if ( b->s1.Qd[0] )
        {
          used += snprintf ( c + used, lmax - used,"%s", b->s1.Qd );
        }
      else
        {
          used += snprintf ( c + used, lmax - used,"/" );
        }

      if ( b->s1.Qx[0] )
        {
          used += snprintf ( c + used, lmax - used,"%s", b->s1.Qx );
        }
      else
        {
          used += snprintf ( c + used, lmax - used,"/" );
        }

      // 0ddff
      if ( b->s1.dd[0] || b->s1.ff[0] )
        {
          used += snprintf ( c + used, lmax - used, " 0" );
          if ( b->s1.dd[0] )
            {
              used += snprintf ( c + used, lmax - used, "%s", b->s1.dd );
            }
          else
            {
              used += snprintf ( c + used, lmax - used, "//" );
            }

          if ( b->s1.ff[0] )
            {
              used += snprintf ( c + used, lmax - used, "%s", b->s1.ff );
            }
          else
            {
              used += snprintf ( c + used, lmax - used, "//" );
            }
        }

      // 1snTTT
      if ( b->s1.TTT[0] )
        {
          used += snprintf ( c + used, lmax - used, " 1%s%s", b->s1.sn1, b->s1.TTT );
        }

      // 2snTdTdTd
      if ( b->s1.TdTdTd[0] )
        {
          used += snprintf ( c + used, lmax - used, " 2%s%s", b->s1.sn2, b->s1.TdTdTd );
        }

      // 3PoPoPoPo
      if ( b->s1.PoPoPoPo[0] )
        {
          used += snprintf ( c + used, lmax - used, " 3%s", b->s1.PoPoPoPo );
        }

      // printf 4PPPP
      if ( b->s1.PPPP[0] )
        {
          used += snprintf ( c + used, lmax - used, " 4%s", b->s1.PPPP );
        }

      // printf 5appp
      if ( b->s1.a[0] || b->s1.ppp[0] )
        {
          if ( b->s1.a[0] == 0 )
            {
              b->s1.a[0] = '/';
            }
          if ( b->s1.ppp[0] == 0 )
            {
              strcpy ( b->s1.ppp, "///" );
            }
          used += snprintf ( c + used, lmax - used, " 5%s%s", b->s1.a, b->s1.ppp );
        }
    }

  *sec1 = c + used;
  return used;
}

/*!
  \fn size_t print_buoy_sec2 (char **sec2, size_t lmax, struct buoy_chunks *b)
  \brief Prints the buoy section 1
  \param sec2 the pointer where to print section
  \param lmax max length permited
  \param b pointer to s atruct \ref buoy_chunks where the parse results are set
*/
size_t print_buoy_sec2 ( char **sec2, size_t lmax, struct buoy_chunks *b )
{
  size_t used = 0;
  char *c = *sec2;

  if ( b->mask & BUOY_SEC2 )
    {
      // 222QdQx
      used += snprintf ( c + used, lmax - used," 222" );

      if ( b->s2.Qd[0] )
        {
          used += snprintf ( c + used, lmax - used,"%s", b->s2.Qd );
        }
      else
        {
          used += snprintf ( c + used, lmax - used,"/" );
        }

      if ( b->s2.Qx[0] )
        {
          used += snprintf ( c + used, lmax - used,"%s", b->s2.Qx );
        }
      else
        {
          used += snprintf ( c + used, lmax - used,"/" );
        }

      // 0snTwTwTw
      if ( b->s2.TwTwTw[0] )
        {
          used += snprintf ( c + used, lmax - used, " 0%s%s", b->s2.sn, b->s2.TwTwTw );
        }

      // 1PwaPwaHwaHwa
      if ( b->s2.PwaPwa[0] || b->s2.HwaHwa[0] )
        {
          used += snprintf ( c + used, lmax - used, " 1" );
          if ( b->s2.PwaPwa[0] )
            {
              used += snprintf ( c + used, lmax - used,"%s", b->s2.PwaPwa );
            }
          else
            {
              used += snprintf ( c + used, lmax - used,"//" );
            }

          if ( b->s2.HwaHwa[0] )
            {
              used += snprintf ( c + used, lmax - used,"%s", b->s2.HwaHwa );
            }
          else
            {
              used += snprintf ( c + used, lmax - used,"//" );
            }
        }

      // 20PwaPwaPwa
      if ( b->s2.PwaPwaPwa[0] )
        {
          used += snprintf ( c + used, lmax - used, " 20%s", b->s2.PwaPwaPwa );
        }

      // 21HwaHwaHwa
      if ( b->s2.HwaHwaHwa[0] )
        {
          used += snprintf ( c + used, lmax - used, " 21%s", b->s2.HwaHwaHwa );
        }


    }

  *sec2 = c + used;
  return used;
}

/*!
  \fn size_t print_buoy_sec3 (char **sec3, size_t lmax, struct buoy_chunks *b)
  \brief Prints the buoy section 3
  \param sec3 the pointer where to print section
  \param lmax max length permited
  \param b pointer to s atruct \ref buoy_chunks where the parse results are set
*/
size_t print_buoy_sec3 ( char **sec3, size_t lmax, struct buoy_chunks *b )
{
  size_t used = 0;
  char *c = *sec3;
  size_t l;

  if ( b->mask & BUOY_SEC3 )
    {
      used += snprintf ( c + used, lmax - used, " 333%s%s", b->s3.Qd1, b->s3.Qd2 );

      // check if has 8887k2
      l = 0;
      while ( l < 32 && b->s3.l1[l].zzzz[0] )
        {
          if ( l == 0 )
            {
              used += snprintf ( c + used, lmax - used, " 8887%s", b->s3.k2 );
            }
          used += snprintf ( c + used, lmax - used,  " 2%s", b->s3.l1[l].zzzz );

          if ( b->s3.l1[l].TTTT[0] )
            {
              used += snprintf ( c + used, lmax - used,  " 3%s", b->s3.l1[l].TTTT );
            }

          if ( b->s3.l1[l].SSSS[0] )
            {
              used += snprintf ( c + used, lmax - used,  " 4%s", b->s3.l1[l].SSSS );
            }
          l++;
        }

      l = 0;
      while ( l < 32 && b->s3.l2[l].zzzz[0] )
        {
          if ( l == 0 )
            {
              used += snprintf ( c + used, lmax - used, " 66%s9%s", b->s3.k6, b->s3.k3 );
            }
          used += snprintf ( c + used, lmax - used,  " 2%s", b->s3.l2[l].zzzz );

          if ( b->s3.l2[l].dd[0] || b->s3.l2[l].ccc[0] )
            {
              if ( b->s3.l2[l].dd[0] )
                {
                  used += snprintf ( c + used, lmax - used,  " %s", b->s3.l2[l].dd );
                }
              else
                {
                  used += snprintf ( c + used, lmax - used,  " //" );
                }
              if ( b->s3.l2[l].ccc[0] )
                {
                  used += snprintf ( c + used, lmax - used,  "%s", b->s3.l2[l].ccc );
                }
              else
                {
                  used += snprintf ( c + used, lmax - used,  "///" );
                }
            }
          l++;
        }
    }

  *sec3 = c + used;
  return used;
}

/*!
 *  \fn size_t print_synop_wigos_id ( char **wid,  size_t lmax, struct buoy_chunks *b )
 *  \brief Prints a WIGOS identifier in a buoy report
 */
size_t print_buoy_wigos_id ( char **wid,  size_t lmax, struct buoy_chunks *b )
{
  char aux[40];
  size_t used = 0;
  char sep = '|';

  if ( b->wid.series == 0 && b->wid.issuer == 0 && b->wid.issue == 0 && b->wid.local_id[0] == '\0' )
    sprintf ( aux,"0-0-0-MISSING" );
  else
    sprintf ( aux,"%d-%d-%d-%s", b->wid.series, b->wid.issuer, b->wid.issue, b->wid.local_id );

  used = snprintf ( *wid, lmax, "%-32s%c", aux, sep );
  *wid += used;
  return used;
}


/*!
 \fn int print_buoy_report(struct metreport *m )
 \brief prints a buoy into a string
 \param m pointer to struct \ref metreport where are both target and source


  If OK returns 0, otherwise 1
*/
int print_buoy_report ( struct metreport *m )
{
  char *c = & ( m->alphanum[0] );
  size_t used = 0;
  size_t lmax = sizeof ( m->alphanum );
  struct buoy_chunks *b = &m->buoy;

  // Needs time extension
  if ( b->e.YYYY[0] == 0  || b->e.YYYY[0] == '0' )
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

  print_buoy_sec0 ( &c, lmax, b );

  if ( b->mask & ( BUOY_SEC1 | BUOY_SEC2 | BUOY_SEC3 ) )
    {
      used += print_buoy_sec1 ( &c, lmax - used, b );

      used += print_buoy_sec2 ( &c, lmax - used, b );

      used += print_buoy_sec3 ( &c, lmax - used, b );
    }
  else
    {
      c += snprintf ( c, lmax - used, " NIL" );
    }
  snprintf ( c, lmax - used - 1, "=" );

  return 0;
}


