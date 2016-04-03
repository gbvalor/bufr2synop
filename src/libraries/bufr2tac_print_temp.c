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
 \file bufr2tac_print_temp.c
 \brief file with the code to print the results in case of temp reposrt
 */
#include "bufr2tac.h"

/*!
  \def check_len(ori,inc)
  \brief cheks if there still memory enough to add \a inc chars
*/
#define check_len(ori,inc) (c - *ori + inc < (int)lmax)

/*!
  \fn int print_temp_raw_data ( struct temp_raw_data *r )
  \brief Prints for debug a struct \ref temp_raw_data
  \param r the pointer of struct to print
*/
int print_temp_raw_data ( struct temp_raw_data *r )
{
  size_t i;

  printf("raw points %lu\n", r->n);
  for ( i = 0; i < r->n ; i++ )
    {
      if ( r->raw[i].T != MISSING_REAL &&
           r->raw[i].Td != MISSING_REAL &&
           r->raw[i].dd != MISSING_REAL &&
           r->raw[i].ff != MISSING_REAL )
        printf ( "i=%3ld, dt=%4d, flags=%08X, P=%6.1lf, h=%6.0lf, dlat=%7.4lf, dlon=%7.4lf, T=%6.1lf, Td=%6.1lf, dd=%03.0lf, ff=%5.1lf\n",
                 i, r->raw[i].dt, r->raw[i].flags, r->raw[i].p * 0.01, r->raw[i].h, r->raw[i].dlat, r->raw[i].dlon,
                 r->raw[i].T - 273.15, r->raw[i].Td - 273.15, r->raw[i].dd, r->raw[i].ff );
      else
        {
          printf ( "i=%3ld, dt=%4d, flags=%08X, P=%6.1lf, h=%6.0lf, dlat=%7.4lf, dlon=%7.4lf, ",
                   i, r->raw[i].dt, r->raw[i].flags, r->raw[i].p * 0.01, r->raw[i].h, r->raw[i].dlat, r->raw[i].dlon );
          if ( r->raw[i].T != MISSING_REAL )
            printf ( "T=%6.1lf, ", r->raw[i].T - 273.15 );
          else
            printf ( "T= ///./, " );

          if ( r->raw[i].Td != MISSING_REAL )
            printf ( "T=%6.1lf, ", r->raw[i].Td - 273.15 );
          else
            printf ( "Td= ///./, " );

          if ( r->raw[i].dd != MISSING_REAL )
            printf ( "dd=%03.0lf, ", r->raw[i].dd );
          else
            printf ( "dd=///, " );

          if ( r->raw[i].dd != MISSING_REAL )
            printf ( "ff=%5.1lf\n", r->raw[i].ff );
          else
            printf ( "ff= //./\n" );
        }
    }
  return 0;
}

/*!
  \fn int print_temp_raw_wind_shear_data ( struct temp_raw_wind_shear_data *w )
  \brief Prints for debug a struct \ref temp_raw_data
  \param w the pointer of struct to print
*/
int print_temp_raw_wind_shear_data ( struct temp_raw_wind_shear_data *w )
{
  size_t i;

  printf("raw share wind points %lu\n", w->n);
  for ( i = 0; i < w->n ; i++ )
    {
      if ( w->raw[i].ws_blw != MISSING_REAL && w->raw[i].ws_abv != MISSING_REAL )
        printf ( "i=%3ld, dt=%4d, flags=%08X, P=%6.1lf, dlat=%7.4lf, dlon=%7.4lf, ws_blw=%5.1lf, ws_abv=%5.1lf\n",
                 i, w->raw[i].dt, w->raw[i].flags, w->raw[i].p * 0.01, w->raw[i].dlat, w->raw[i].dlon,
                 w->raw[i].ws_blw, w->raw[i].ws_abv );
      else
        {
          printf ( "i=%3ld, dt=%4d, flags=%08X, P=%6.1lf, dlat=%7.4lf, dlon=%7.4lf, ",
                   i, w->raw[i].dt, w->raw[i].flags, w->raw[i].p * 0.01, w->raw[i].dlat, w->raw[i].dlon );

          if ( w->raw[i].ws_blw != MISSING_REAL )
            printf ( "ws_blw=%5.1lf, ", w->raw[i].ws_blw );
          else
            printf ( "ws_blw= ///./, " );

          if ( w->raw[i].ws_abv != MISSING_REAL )
            printf ( "ws_abv=%5.1lf\n", w->raw[i].ws_abv );
          else
            printf ( "ws_abv= ///./\n" );
        }
    }
  return 0;
}

char * print_temp_a_sec1 ( char **sec1, size_t lmax, struct temp_chunks *t )
{
  char *c = *sec1, *c0 = *sec1;

  c += sprintf ( c, "%s", t->t.datime );

  c += sprintf ( c, " %s%s", t->a.s1.MiMi, t->a.s1.MjMj );

  if ( t->a.s1.D_D[0] && t->a.s1.II[0] == 0)
    c += sprintf ( c, " %s", t->a.s1.D_D );

  c += sprintf ( c, " %s%s", t->a.s1.YYGG, t->a.s1.id );

  // print IIiii
  if ( check_len ( sec1,6 ) && t->a.s1.II[0] )
    {
      c += sprintf ( c, " %s%s", t->a.s1.II, t->a.s1.iii );
    }
  else
    {
      if ( t->a.s1.LaLaLa[0] )
        {
          c += sprintf ( c, " 99%s", t->a.s1.LaLaLa );
        }
      else
        {
          c += sprintf ( c, " 99///" );
        }

      if ( t->a.s1.Qc[0] && t->a.s1.LoLoLoLo[0] )
        {
          c += sprintf ( c, " %s%s", t->a.s1.Qc, t->a.s1.LoLoLoLo );
        }
      else
        {
          c += sprintf ( c, " /////" );
        }

      if ( t->a.s1.MMM[0] && t->a.s1.Ula && t->a.s1.Ulo )
        {
          c += sprintf ( c, " %s%s%s", t->a.s1.MMM, t->a.s1.Ula, t->a.s1.Ulo );
        }

      if ( t->a.s1.h0h0h0h0[0] )
        {
          c += sprintf ( c, " %s%s", t->a.s1.h0h0h0h0, t->a.s1.im );
        }
    }

  if ( c != c0 )
    {
      *sec1 = c;
    }
  return *sec1;
}


char * print_temp_a_sec2 ( char **sec2, size_t lmax, struct temp_chunks *t )
{
  size_t i;
  char *c = *sec2, *c0 = *sec2;

  //Surface level
  if ( check_len ( sec2, 18 ) )
    {
      c += sprintf ( c, " 99%s", t->a.s2.lev0.PnPnPn );
      c += sprintf ( c, " %s%s", t->a.s2.lev0.TnTnTan, t->a.s2.lev0.DnDn );
      c += sprintf ( c, " %s", t->a.s2.lev0.dndnfnfnfn );
    }
  for ( i = 0; i < t->a.s2.n ; i++ )
    {
      if ( check_len ( sec2, 18 ) )
        {
          c += sprintf ( c, " %s%s", t->a.s2.std[i].PnPn, t->a.s2.std[i].hnhnhn );
          c += sprintf ( c, " %s%s", t->a.s2.std[i].TnTnTan, t->a.s2.std[i].DnDn );
          c += sprintf ( c, " %s", t->a.s2.std[i].dndnfnfnfn );
        }
    }

  if ( c != c0 )
    {
      *sec2 = c;
    }
  return *sec2;

}

char * print_temp_a_sec3 ( char **sec3, size_t lmax, struct temp_chunks *t )
{
  size_t i;
  char *c = *sec3, *c0 = *sec3;

  if ( t->a.s3.n == 0 )
    {
      if ( check_len ( sec3, 6 ) )
        c += sprintf ( c, " 88999" );
    }
  else
    {
      for ( i = 0; i < t->a.s3.n ; i++ )
        {
          if ( check_len ( sec3, 18 ) )
            {
              c += sprintf ( c, " 88%s", t->a.s3.trop[i].PnPnPn );
              c += sprintf ( c, " %s%s", t->a.s3.trop[i].TnTnTan, t->a.s3.trop[i].DnDn );
              c += sprintf ( c, " %s", t->a.s3.trop[i].dndnfnfnfn );
            }
        }
    }

  if ( c != c0 )
    {
      *sec3 = c;
    }
  return *sec3;

}

char * print_temp_a_sec4 ( char **sec4, size_t lmax, struct temp_chunks *t )
{
  size_t i;
  char *c = *sec4, *c0 = *sec4;

  if ( t->a.s4.n == 0 )
    {
      if ( check_len ( sec4, 6 ) )

        c += sprintf ( c, " 77999" );
    }
  else
    {
      for ( i = 0; i < t->a.s4.n ; i++ )
        {
          if ( check_len ( sec4, 18 ) )
            {
              if ( t->a.s4.windx[i].no_last_wind )
                c += sprintf ( c, " 77%s", t->a.s4.windx[i].PmPmPm );
              else
                c += sprintf ( c, " 66%s", t->a.s4.windx[i].PmPmPm );
              c += sprintf ( c, " %s", t->a.s4.windx[i].dmdmfmfmfm );
              c += sprintf ( c, " 4%s%s", t->a.s4.windx[i].vbvb, t->a.s4.windx[i].vava );
            }
        }
    }


  if ( c != c0 )
    {
      *sec4 = c;
    }
  return *sec4;

}

char * print_temp_a_sec7 ( char **sec7, size_t lmax, struct temp_chunks *t )
{
  char *c = *sec7, *c0 = *sec7;

  if ( check_len ( sec7, 18 ) )
    {
      c += sprintf ( c, " 31313" );
      c += sprintf ( c, " %s%s%s", t->a.s7.sr, t->a.s7.rara, t->a.s7.sasa );
      c += sprintf ( c, " 8%s%s", t->a.s7.GG, t->a.s7.gg );
    }
  if ( t->a.s7.TwTwTw[0] && check_len ( sec7, 6 ) )
    c += sprintf ( c, " 9%s%s", t->a.s7.sn, t->a.s7.TwTwTw );

  if ( c != c0 )
    {
      *sec7 = c;
    }
  return *sec7;
}

int print_temp_a ( char *report, size_t lmax, struct temp_chunks *t )
{
  char *c;

  c = report;

  // Needs time extension
  if ( t->a.e.YYYY[0] == 0 )
    {
      return 1;
    }

  print_temp_a_sec1 ( &c, lmax, t );
  print_temp_a_sec2 ( &c, lmax - strlen ( report ), t );
  print_temp_a_sec3 ( &c, lmax - strlen ( report ), t );
  print_temp_a_sec4 ( &c, lmax - strlen ( report ), t );
  print_temp_a_sec7 ( &c, lmax - strlen ( report ), t );
  c += sprintf ( c, "=" );
  return 0;
}

char * print_temp_b_sec1 ( char **sec1, size_t lmax, struct temp_chunks *t )
{
  char *c = *sec1, *c0 = *sec1;

  c += sprintf ( c, "%s", t->t.datime );

  c += sprintf ( c, " %s%s", t->b.s1.MiMi, t->b.s1.MjMj );

  if ( t->b.s1.D_D[0] && t->a.s1.II[0] == 0)
    c += sprintf ( c, " %s", t->b.s1.D_D );

  c += sprintf ( c, " %s%s", t->b.s1.YYGG, t->b.s1.a4 );

  // print IIiii
  if ( check_len ( sec1,6 ) && t->b.s1.II[0] )
    {
      c += sprintf ( c, " %s%s", t->b.s1.II, t->b.s1.iii );
    }
  else
    {
      if ( t->b.s1.LaLaLa[0] )
        {
          c += sprintf ( c, " 99%s", t->b.s1.LaLaLa );
        }
      else
        {
          c += sprintf ( c, " 99///" );
        }

      if ( t->b.s1.Qc[0] && t->b.s1.LoLoLoLo[0] )
        {
          c += sprintf ( c, " %s%s", t->b.s1.Qc, t->b.s1.LoLoLoLo );
        }
      else
        {
          c += sprintf ( c, " /////" );
        }

      if ( t->b.s1.MMM[0] && t->b.s1.Ula && t->b.s1.Ulo )
        {
          c += sprintf ( c, " %s%s%s", t->b.s1.MMM, t->b.s1.Ula, t->b.s1.Ulo );
        }

      if ( t->b.s1.h0h0h0h0[0] )
        {
          c += sprintf ( c, " %s%s", t->b.s1.h0h0h0h0, t->b.s1.im );
        }
    }

  if ( c != c0 )
    {
      *sec1 = c;
    }
  return *sec1;
}

char * print_temp_b_sec5 ( char **sec5, size_t lmax, struct temp_chunks *t )
{
  size_t i;
  char *c = *sec5, *c0 = *sec5;

  for ( i = 0; i < t->b.s5.n ; i++ )
    {
      if ( check_len ( sec5, 12 ) )
        {
          c += sprintf ( c, " %s%s", t->b.s5.th[i].nini, t->b.s5.th[i].PnPnPn );
          c += sprintf ( c, " %s%s", t->b.s5.th[i].TnTnTan, t->b.s5.th[i].DnDn );
        }
    }

  if ( c != c0 )
    {
      *sec5 = c;
    }
  return *sec5;
}

char * print_temp_b_sec6 ( char **sec6, size_t lmax, struct temp_chunks *t )
{
  size_t i;
  char *c = *sec6, *c0 = *sec6;

  if ( check_len ( sec6, 6 ) )
    c += sprintf ( c, " 21212" );

  for ( i = 0; i < t->b.s6.n ; i++ )
    {
      if ( check_len ( sec6, 12 ) )
        {
          c += sprintf ( c, " %s%s", t->b.s6.wd[i].nini, t->b.s6.wd[i].PnPnPn );
          c += sprintf ( c, " %s", t->b.s6.wd[i].dndnfnfnfn );
        }
    }

  if ( c != c0 )
    {
      *sec6 = c;
    }
  return *sec6;
}

char * print_temp_b_sec7 ( char **sec7, size_t lmax, struct temp_chunks *t )
{
  char *c = *sec7, *c0 = *sec7;

  if ( check_len ( sec7, 18 ) )
    {
      c += sprintf ( c, " 31313" );
      c += sprintf ( c, " %s%s%s", t->b.s7.sr, t->b.s7.rara, t->b.s7.sasa );
      c += sprintf ( c, " 8%s%s", t->b.s7.GG, t->b.s7.gg );
    }
  if ( t->b.s7.TwTwTw[0] && check_len ( sec7, 6 ) )
    c += sprintf ( c, " 9%s%s", t->b.s7.sn, t->b.s7.TwTwTw );

  if ( c != c0 )
    {
      *sec7 = c;
    }
  return *sec7;
}

char * print_temp_b_sec8 ( char **sec8, size_t lmax, struct temp_chunks *t )
{
  char *c = *sec8, *c0 = *sec8;

  if ( check_len ( sec8, 12 ) && t->b.s8.h[0] )
    {
      c += sprintf ( c, " 41414" );
      c += sprintf ( c, " %s%s%s%s%s", t->b.s8.Nh, t->b.s8.Cl, t->b.s8.h, t->b.s8.Cm, t->b.s8.Ch );
    }

  if ( c != c0 )
    {
      *sec8 = c;
    }
  return *sec8;
}


int print_temp_b ( char *report, size_t lmax, struct temp_chunks *t )
{
  char *c;

  c = report;

  // Needs time extension
  if ( t->b.e.YYYY[0] == 0 )
    {
      return 1;
    }

  print_temp_b_sec1 ( &c, lmax, t );
  print_temp_b_sec5 ( &c, lmax - strlen ( report ), t );
  print_temp_b_sec6 ( &c, lmax - strlen ( report ), t );
  print_temp_b_sec7 ( &c, lmax - strlen ( report ), t );
  print_temp_b_sec8 ( &c, lmax - strlen ( report ), t );
  c += sprintf ( c, "=" );
  return 0;
}


char * print_temp_c_sec1 ( char **sec1, size_t lmax, struct temp_chunks *t )
{
  char *c = *sec1, *c0 = *sec1;

  c += sprintf ( c, "%s", t->t.datime );

  c += sprintf ( c, " %s%s", t->c.s1.MiMi, t->c.s1.MjMj );

  if ( t->c.s1.D_D[0] && t->a.s1.II[0] == 0)
    c += sprintf ( c, " %s", t->c.s1.D_D );

  c += sprintf ( c, " %s%s", t->c.s1.YYGG, t->c.s1.id );

  // print IIiii
  if ( check_len ( sec1,6 ) && t->c.s1.II[0] )
    {
      c += sprintf ( c, " %s%s", t->c.s1.II, t->c.s1.iii );
    }
  else
    {
      if ( t->c.s1.LaLaLa[0] )
        {
          c += sprintf ( c, " 99%s", t->c.s1.LaLaLa );
        }
      else
        {
          c += sprintf ( c, " 99///" );
        }

      if ( t->c.s1.Qc[0] && t->c.s1.LoLoLoLo[0] )
        {
          c += sprintf ( c, " %s%s", t->c.s1.Qc, t->c.s1.LoLoLoLo );
        }
      else
        {
          c += sprintf ( c, " /////" );
        }

      if ( t->c.s1.MMM[0] && t->c.s1.Ula && t->c.s1.Ulo )
        {
          c += sprintf ( c, " %s%s%s", t->c.s1.MMM, t->c.s1.Ula, t->c.s1.Ulo );
        }

      if ( t->c.s1.h0h0h0h0[0] )
        {
          c += sprintf ( c, " %s%s", t->c.s1.h0h0h0h0, t->c.s1.im );
        }
    }

  if ( c != c0 )
    {
      *sec1 = c;
    }
  return *sec1;
}

char * print_temp_c_sec2 ( char **sec2, size_t lmax, struct temp_chunks *t )
{
  size_t i;
  char *c = *sec2, *c0 = *sec2;

  for ( i = 0; i < t->c.s2.n ; i++ )
    {
      if ( check_len ( sec2, 18 ) )
        {
          c += sprintf ( c, " %s%s", t->c.s2.std[i].PnPn, t->c.s2.std[i].hnhnhn );
          c += sprintf ( c, " %s%s", t->c.s2.std[i].TnTnTan, t->c.s2.std[i].DnDn );
          c += sprintf ( c, " %s", t->c.s2.std[i].dndnfnfnfn );
        }
    }

  if ( c != c0 )
    {
      *sec2 = c;
    }
  return *sec2;

}

char * print_temp_c_sec3 ( char **sec3, size_t lmax, struct temp_chunks *t )
{
  size_t i;
  char *c = *sec3, *c0 = *sec3;

  if ( t->c.s3.n == 0 )
    {
      if ( check_len ( sec3, 6 ) )
        c += sprintf ( c, " 88999" );
    }
  else
    {
      for ( i = 0; i < t->c.s3.n ; i++ )
        {
          if ( check_len ( sec3, 18 ) )
            {
              c += sprintf ( c, " 88%s", t->c.s3.trop[i].PnPnPn );
              c += sprintf ( c, " %s%s", t->c.s3.trop[i].TnTnTan, t->c.s3.trop[i].DnDn );
              c += sprintf ( c, " %s", t->c.s3.trop[i].dndnfnfnfn );
            }
        }
    }

  if ( c != c0 )
    {
      *sec3 = c;
    }
  return *sec3;

}

char * print_temp_c_sec4 ( char **sec4, size_t lmax, struct temp_chunks *t )
{
  size_t i;
  char *c = *sec4, *c0 = *sec4;

  if ( t->c.s4.n == 0 )
    {
      if ( check_len ( sec4, 6 ) )

        c += sprintf ( c, " 77999" );
    }
  else
    {
      for ( i = 0; i < t->c.s4.n ; i++ )
        {
          if ( check_len ( sec4, 18 ) )
            {
              if ( t->c.s4.windx[i].no_last_wind )
                c += sprintf ( c, " 77%s", t->c.s4.windx[i].PmPmPm );
              else
                c += sprintf ( c, " 66%s", t->c.s4.windx[i].PmPmPm );
              c += sprintf ( c, " %s", t->c.s4.windx[i].dmdmfmfmfm );
	      if (t->c.s4.windx[i].vbvb[0] && t->c.s4.windx[i].vava[0])
                c += sprintf ( c, " 4%s%s", t->c.s4.windx[i].vbvb, t->c.s4.windx[i].vava );
            }
        }
    }


  if ( c != c0 )
    {
      *sec4 = c;
    }
  return *sec4;

}

char * print_temp_c_sec7 ( char **sec7, size_t lmax, struct temp_chunks *t )
{
  char *c = *sec7, *c0 = *sec7;

  if ( check_len ( sec7, 18 ) )
    {
      c += sprintf ( c, " 31313" );
      c += sprintf ( c, " %s%s%s", t->c.s7.sr, t->c.s7.rara, t->c.s7.sasa );
      c += sprintf ( c, " 8%s%s", t->c.s7.GG, t->c.s7.gg );
    }
  if ( t->c.s7.TwTwTw[0] && check_len ( sec7, 6 ) )
    c += sprintf ( c, " 9%s%s", t->c.s7.sn, t->c.s7.TwTwTw );

  if ( c != c0 )
    {
      *sec7 = c;
    }
  return *sec7;
}

int print_temp_c ( char *report, size_t lmax, struct temp_chunks *t )
{
  char *c;

  c = report;

  // Needs time extension
  if ( t->b.e.YYYY[0] == 0 )
    {
      return 1;
    }

  print_temp_c_sec1 ( &c, lmax, t );
  print_temp_c_sec2 ( &c, lmax - strlen ( report ), t );
  print_temp_c_sec3 ( &c, lmax - strlen ( report ), t );
  print_temp_c_sec4 ( &c, lmax - strlen ( report ), t );
  print_temp_c_sec7 ( &c, lmax - strlen ( report ), t );
  c += sprintf ( c, "=" );
  return 0;
}

char * print_temp_d_sec1 ( char **sec1, size_t lmax, struct temp_chunks *t )
{
  char *c = *sec1, *c0 = *sec1;

  c += sprintf ( c, "%s", t->t.datime );

  c += sprintf ( c, " %s%s", t->d.s1.MiMi, t->d.s1.MjMj );

  if ( t->d.s1.D_D[0] && t->a.s1.II[0] == 0)
    c += sprintf ( c, " %s", t->d.s1.D_D );

  c += sprintf ( c, " %s/", t->d.s1.YYGG);

  // print IIiii
  if ( check_len ( sec1,6 ) && t->d.s1.II[0] )
    {
      c += sprintf ( c, " %s%s", t->d.s1.II, t->d.s1.iii );
    }
  else
    {
      if ( t->d.s1.LaLaLa[0] )
        {
          c += sprintf ( c, " 99%s", t->d.s1.LaLaLa );
        }
      else
        {
          c += sprintf ( c, " 99///" );
        }

      if ( t->d.s1.Qc[0] && t->d.s1.LoLoLoLo[0] )
        {
          c += sprintf ( c, " %s%s", t->d.s1.Qc, t->d.s1.LoLoLoLo );
        }
      else
        {
          c += sprintf ( c, " /////" );
        }

      if ( t->d.s1.MMM[0] && t->d.s1.Ula && t->d.s1.Ulo )
        {
          c += sprintf ( c, " %s%s%s", t->d.s1.MMM, t->d.s1.Ula, t->d.s1.Ulo );
        }

      if ( t->d.s1.h0h0h0h0[0] )
        {
          c += sprintf ( c, " %s%s", t->d.s1.h0h0h0h0, t->d.s1.im );
        }
    }

  if ( c != c0 )
    {
      *sec1 = c;
    }
  return *sec1;
}

char * print_temp_d_sec5 ( char **sec5, size_t lmax, struct temp_chunks *t )
{
  size_t i;
  char *c = *sec5, *c0 = *sec5;

  for ( i = 0; i < t->d.s5.n ; i++ )
    {
      if ( check_len ( sec5, 12 ) )
        {
          c += sprintf ( c, " %s%s", t->d.s5.th[i].nini, t->d.s5.th[i].PnPnPn );
          c += sprintf ( c, " %s%s", t->d.s5.th[i].TnTnTan, t->d.s5.th[i].DnDn );
        }
    }

  if ( c != c0 )
    {
      *sec5 = c;
    }
  return *sec5;
}

char * print_temp_d_sec6 ( char **sec6, size_t lmax, struct temp_chunks *t )
{
  size_t i;
  char *c = *sec6, *c0 = *sec6;

  if ( check_len ( sec6, 6 ) )
    c += sprintf ( c, " 21212" );

  for ( i = 0; i < t->d.s6.n ; i++ )
    {
      if ( check_len ( sec6, 12 ) )
        {
          c += sprintf ( c, " %s%s", t->d.s6.wd[i].nini, t->d.s6.wd[i].PnPnPn );
          c += sprintf ( c, " %s", t->d.s6.wd[i].dndnfnfnfn );
        }
    }

  if ( c != c0 )
    {
      *sec6 = c;
    }
  return *sec6;
}

char * print_temp_d_sec7 ( char **sec7, size_t lmax, struct temp_chunks *t )
{
  char *c = *sec7, *c0 = *sec7;

  if ( check_len ( sec7, 18 ) )
    {
      c += sprintf ( c, " 31313" );
      c += sprintf ( c, " %s%s%s", t->d.s7.sr, t->d.s7.rara, t->d.s7.sasa );
      c += sprintf ( c, " 8%s%s", t->d.s7.GG, t->d.s7.gg );
    }
  if ( t->d.s7.TwTwTw[0] && check_len ( sec7, 6 ) )
    c += sprintf ( c, " 9%s%s", t->d.s7.sn, t->d.s7.TwTwTw );

  if ( c != c0 )
    {
      *sec7 = c;
    }
  return *sec7;
}


int print_temp_d ( char *report, size_t lmax, struct temp_chunks *t )
{
  char *c;

  c = report;

  // Needs time extension
  if ( t->b.e.YYYY[0] == 0 )
    {
      return 1;
    }

  print_temp_d_sec1 ( &c, lmax, t );
  print_temp_d_sec5 ( &c, lmax - strlen ( report ), t );
  print_temp_d_sec6 ( &c, lmax - strlen ( report ), t );
  print_temp_d_sec7 ( &c, lmax - strlen ( report ), t );
  c += sprintf ( c, "=" );
  return 0;
}

int print_temp ( struct metreport *m )
{
  print_temp_a ( m->alphanum, REPORT_LENGTH, &m->temp );
  print_temp_b ( m->alphanum2, REPORT_LENGTH, &m->temp );
  print_temp_c ( m->alphanum3, REPORT_LENGTH, &m->temp );
  print_temp_d ( m->alphanum4, REPORT_LENGTH, &m->temp );

  return 0;
}
