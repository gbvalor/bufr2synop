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
 \file bufr2tac_print_temp.c
 \brief file with the code to print the results in case of temp reposrt
 */
#include "bufr2tac.h"

/*!
  \fn int print_temp_raw_data ( struct temp_raw_data *r )
  \brief Prints for debug a struct \ref temp_raw_data
  \param r the pointer of struct to print

  This is used mainly for debug
*/
int print_temp_raw_data ( const struct temp_raw_data *r )
{
  size_t i;

  printf ( "raw points %lu\n", r->n );
  for ( i = 0; i < r->n ; i++ )
    {
      if ( r->raw[i].T != MISSING_REAL &&
           r->raw[i].Td != MISSING_REAL &&
           r->raw[i].dd != MISSING_REAL &&
           r->raw[i].ff != MISSING_REAL )
        printf ( "i=%3lu, dt=%4d, flags=%08X, P=%6.1lf, h=%6.0lf, dlat=%7.4lf, dlon=%7.4lf, T=%6.1lf, Td=%6.1lf, dd=%03.0lf, ff=%5.1lf\n",
                 i, r->raw[i].dt, r->raw[i].flags, r->raw[i].p * 0.01, r->raw[i].h, r->raw[i].dlat, r->raw[i].dlon,
                 r->raw[i].T - 273.15, r->raw[i].Td - 273.15, r->raw[i].dd, r->raw[i].ff );
      else
        {
          printf ( "i=%3lu, dt=%4d, flags=%08X, P=%6.1lf, h=%6.0lf, dlat=%7.4lf, dlon=%7.4lf, ",
                   i, r->raw[i].dt, r->raw[i].flags, r->raw[i].p * 0.01, r->raw[i].h, r->raw[i].dlat, r->raw[i].dlon );
          if ( r->raw[i].T != MISSING_REAL )
            {
              printf ( "T=%6.1lf, ", r->raw[i].T - 273.15 );
            }
          else
            {
              printf ( "T= ///./, " );
            }

          if ( r->raw[i].Td != MISSING_REAL )
            {
              printf ( "T=%6.1lf, ", r->raw[i].Td - 273.15 );
            }
          else
            {
              printf ( "Td= ///./, " );
            }

          if ( r->raw[i].dd != MISSING_REAL )
            {
              printf ( "dd=%03.0lf, ", r->raw[i].dd );
            }
          else
            {
              printf ( "dd=///, " );
            }

          if ( r->raw[i].dd != MISSING_REAL )
            {
              printf ( "ff=%5.1lf\n", r->raw[i].ff );
            }
          else
            {
              printf ( "ff= //./\n" );
            }
        }
    }
  return 0;
}

/*!
  \fn int print_temp_raw_wind_shear_data ( struct temp_raw_wind_shear_data *w )
  \brief Prints for debug a struct \ref temp_raw_data
  \param w the pointer of struct to print

  Used in debug stage
*/
int print_temp_raw_wind_shear_data ( const struct temp_raw_wind_shear_data *w )
{
  size_t i;

  printf ( "raw share wind points %lu\n", w->n );
  for ( i = 0; i < w->n ; i++ )
    {
      if ( w->raw[i].ws_blw != MISSING_REAL && w->raw[i].ws_abv != MISSING_REAL )
        printf ( "i=%3lu, dt=%4d, flags=%08X, P=%6.1lf, dlat=%7.4lf, dlon=%7.4lf, ws_blw=%5.1lf, ws_abv=%5.1lf\n",
                 i, w->raw[i].dt, w->raw[i].flags, w->raw[i].p * 0.01, w->raw[i].dlat, w->raw[i].dlon,
                 w->raw[i].ws_blw, w->raw[i].ws_abv );
      else
        {
          printf ( "i=%3lu, dt=%4d, flags=%08X, P=%6.1lf, dlat=%7.4lf, dlon=%7.4lf, ",
                   i, w->raw[i].dt, w->raw[i].flags, w->raw[i].p * 0.01, w->raw[i].dlat, w->raw[i].dlon );

          if ( w->raw[i].ws_blw != MISSING_REAL )
            {
              printf ( "ws_blw=%5.1lf, ", w->raw[i].ws_blw );
            }
          else
            {
              printf ( "ws_blw= ///./, " );
            }

          if ( w->raw[i].ws_abv != MISSING_REAL )
            {
              printf ( "ws_abv=%5.1lf\n", w->raw[i].ws_abv );
            }
          else
            {
              printf ( "ws_abv= ///./\n" );
            }
        }
    }
  return 0;
}

/*!
  \fn size_t print_temp_a_sec1 (char **sec1, size_t lmax, struct temp_chunks *t)
  \brief Prints the section 1 of part A of a TEMP report
  \param sec1 the pointer where to print section
  \param lmax max length permited
  \param t pointer to s atruct \ref temp_chunks where the parse results are set

  returns the string sec1
*/
size_t print_temp_a_sec1 ( char **sec1, size_t lmax, struct temp_chunks *t )
{
  size_t used = 0;
  char *c = *sec1;

  used += snprintf ( c + used, lmax - used, "%s", t->t.datime );

  used += snprintf ( c + used, lmax - used, " %s%s", t->a.s1.MiMi, t->a.s1.MjMj );

  if ( t->a.s1.D_D[0] && t->a.s1.II[0] == 0 )
    {
      used += snprintf ( c + used, lmax - used, " %s", t->a.s1.D_D );
    }

  used += snprintf ( c + used, lmax - used, " %s%s", t->a.s1.YYGG, t->a.s1.id );

  // print IIiii
  if ( t->a.s1.II[0] )
    {
      used += snprintf ( c + used, lmax - used, " %s%s", t->a.s1.II, t->a.s1.iii );
    }
  else
    {
      if ( t->a.s1.LaLaLa[0] )
        {
          used += snprintf ( c + used, lmax - used, " 99%s", t->a.s1.LaLaLa );
        }
      else
        {
          used += snprintf ( c + used, lmax - used, " 99///" );
        }

      if ( t->a.s1.Qc[0] && t->a.s1.LoLoLoLo[0] )
        {
          used += snprintf ( c + used, lmax - used, " %s%s", t->a.s1.Qc, t->a.s1.LoLoLoLo );
        }
      else
        {
          used += snprintf ( c + used, lmax - used, " /////" );
        }

      if ( t->a.s1.MMM[0] && t->a.s1.Ula[0] && t->a.s1.Ulo[0] )
        {
          used += snprintf ( c + used, lmax - used, " %s%s%s", t->a.s1.MMM, t->a.s1.Ula, t->a.s1.Ulo );
        }

      if ( t->a.s1.h0h0h0h0[0] )
        {
          used += snprintf ( c + used, lmax - used, " %s%s", t->a.s1.h0h0h0h0, t->a.s1.im );
        }
    }

  *sec1 = c + used;
  return used;
}


/*!
  \fn size_t print_temp_a_sec2 (char **sec2, size_t lmax, struct temp_chunks *t)
  \brief Prints the section 2 of part A of a TEMP report
  \param sec2 the pointer where to print section
  \param lmax max length permited
  \param t pointer to s atruct \ref temp_chunks where the parse results are set

  returns the string sec2
*/
size_t print_temp_a_sec2 ( char **sec2, size_t lmax, struct temp_chunks *t )
{
  size_t i;
  size_t used = 0;
  char *c = *sec2;

  //Surface level
  used += snprintf ( c + used, lmax - used, " 99%s", t->a.s2.lev0.PnPnPn );
  used += snprintf ( c + used, lmax - used, " %s%s", t->a.s2.lev0.TnTnTan, t->a.s2.lev0.DnDn );
  used += snprintf ( c + used, lmax - used, " %s", t->a.s2.lev0.dndnfnfnfn );

  for ( i = 0; i < t->a.s2.n ; i++ )
    {
      used += snprintf ( c + used, lmax - used, " %s%s", t->a.s2.std[i].PnPn, t->a.s2.std[i].hnhnhn );
      used += snprintf ( c + used, lmax - used, " %s%s", t->a.s2.std[i].TnTnTan, t->a.s2.std[i].DnDn );
      used += snprintf ( c + used, lmax - used, " %s", t->a.s2.std[i].dndnfnfnfn );
    }

  *sec2 = c + used;
  return used;
}

/*!
  \fn size_t print_temp_a_sec3 (char **sec3, size_t lmax, struct temp_chunks *t)
  \brief Prints the section 3 of part A of a TEMP report
  \param sec3 the pointer where to print section
  \param lmax max length permited
  \param t pointer to s atruct \ref temp_chunks where the parse results are set

  returns the string sec3
*/
size_t print_temp_a_sec3 ( char **sec3, size_t lmax, struct temp_chunks *t )
{
  size_t used = 0;
  char *c = *sec3;

  if ( t->a.s3.n == 0 )
    {
      used += snprintf ( c + used, lmax - used, " 88999" );
    }
  else
    {
      for ( size_t i = 0; i < t->a.s3.n ; i++ )
        {
          used += snprintf ( c + used, lmax - used, " 88%s", t->a.s3.trop[i].PnPnPn );
          used += snprintf ( c + used, lmax - used, " %s%s", t->a.s3.trop[i].TnTnTan, t->a.s3.trop[i].DnDn );
          used += snprintf ( c + used, lmax - used, " %s", t->a.s3.trop[i].dndnfnfnfn );
        }
    }

  *sec3 = c + used;
  return used;

}

/*!
  \fn size_t print_temp_a_sec4 (char **sec4, size_t lmax, struct temp_chunks *t)
  \brief Prints the section 4 of part A of a TEMP report
  \param sec4 the pointer where to print section
  \param lmax max length permited
  \param t pointer to s atruct \ref temp_chunks where the parse results are set

  returns the string sec1
*/
size_t print_temp_a_sec4 ( char **sec4, size_t lmax, struct temp_chunks *t )
{
  size_t used = 0;
  char *c = *sec4;

  if ( t->a.s4.n == 0 )
    {
      used += snprintf ( c + used, lmax - used, " 77999" );
    }
  else
    {
      for ( size_t i = 0; i < t->a.s4.n ; i++ )
        {
          if ( t->a.s4.windx[i].no_last_wind )
            {
              used += snprintf ( c + used, lmax - used, " 77%s", t->a.s4.windx[i].PmPmPm );
            }
          else
            {
              used += snprintf ( c + used, lmax - used, " 66%s", t->a.s4.windx[i].PmPmPm );
            }
          used += snprintf ( c + used, lmax - used, " %s", t->a.s4.windx[i].dmdmfmfmfm );

          if ( t->a.s4.windx[i].vbvb[0] && t->a.s4.windx[i].vava[0] )
            {
              used += snprintf ( c + used, lmax - used, " 4%s%s", t->a.s4.windx[i].vbvb, t->a.s4.windx[i].vava );
            }
        }
    }

  *sec4 = c + used;
  return used;

}

/*!
  \fn size_t print_temp_a_sec7 (char **sec7, size_t lmax, struct temp_chunks *t)
  \brief Prints the section 7 of part A of a TEMP report
  \param sec7 the pointer where to print section
  \param lmax max length permited
  \param t pointer to s atruct \ref temp_chunks where the parse results are set

  returns the string sec7
*/
size_t print_temp_a_sec7 ( char **sec7, size_t lmax, struct temp_chunks *t )
{
  size_t used = 0;
  char *c = *sec7;

  used += snprintf ( c + used, lmax - used, " 31313" );
  used += snprintf ( c + used, lmax - used, " %s%s%s", t->a.s7.sr, t->a.s7.rara, t->a.s7.sasa );
  used += snprintf ( c + used, lmax - used, " 8%s%s", t->a.s7.GG, t->a.s7.gg );

  if ( t->a.s7.TwTwTw[0] )
    {
      used += snprintf ( c + used, lmax - used, " 9%s%s", t->a.s7.sn, t->a.s7.TwTwTw );
    }

  *sec7 = c + used;
  return used;
}

/*!
  \fn size_t print_temp_a (struct metreport *m )
  \brief Prints the part A of a TEMP report into a string
  \param m pointer to struct \ref metreport where are both target and source
  
  If OK returns 0, otherwise 1
*/
int print_temp_a (  struct metreport *m )
{
  char *c = &(m->alphanum[0]);
  size_t used = 0;
  size_t lmax = sizeof(m->alphanum);
  struct temp_chunks *t = &m->temp;


  // Needs time extension
  if ( t->a.e.YYYY[0] == 0  || t->a.e.YYYY[0] == '0')
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

  used += print_temp_a_sec1 ( &c, lmax - used, t );
  used += print_temp_a_sec2 ( &c, lmax - used, t );
  used += print_temp_a_sec3 ( &c, lmax - used, t );
  used += print_temp_a_sec4 ( &c, lmax - used, t );
  used += print_temp_a_sec7 ( &c, lmax - used, t );
  snprintf ( c, lmax - used, "=" );
  return 0;
}

/*!
  \fn size_t print_temp_b_sec1 (char **sec1, size_t lmax, struct temp_chunks *t)
  \brief Prints the section 1 of part B of a TEMP report
  \param sec1 the pointer where to print section
  \param lmax max length permited
  \param t pointer to s atruct \ref temp_chunks where the parse results are set

  returns the string sec1
*/
size_t print_temp_b_sec1 ( char **sec1, size_t lmax, struct temp_chunks *t )
{
  size_t used = 0;
  char *c = *sec1;

  used += snprintf ( c + used, lmax - used, "%s", t->t.datime );

  used += snprintf ( c + used, lmax - used, " %s%s", t->b.s1.MiMi, t->b.s1.MjMj );

  if ( t->b.s1.D_D[0] && t->a.s1.II[0] == 0 )
    {
      used += snprintf ( c + used, lmax - used, " %s", t->b.s1.D_D );
    }

  used += snprintf ( c + used, lmax - used, " %s%s", t->b.s1.YYGG, t->b.s1.a4 );

  // print IIiii
  if ( t->b.s1.II[0] )
    {
      used += snprintf ( c + used, lmax - used, " %s%s", t->b.s1.II, t->b.s1.iii );
    }
  else
    {
      if ( t->b.s1.LaLaLa[0] )
        {
          used += snprintf ( c + used, lmax - used, " 99%s", t->b.s1.LaLaLa );
        }
      else
        {
          used += snprintf ( c + used, lmax - used, " 99///" );
        }

      if ( t->b.s1.Qc[0] && t->b.s1.LoLoLoLo[0] )
        {
          used += snprintf ( c + used, lmax - used, " %s%s", t->b.s1.Qc, t->b.s1.LoLoLoLo );
        }
      else
        {
          used += snprintf ( c + used, lmax - used, " /////" );
        }

      if ( t->b.s1.MMM[0] && t->b.s1.Ula[0] && t->b.s1.Ulo[0] )
        {
          used += snprintf ( c + used, lmax - used, " %s%s%s", t->b.s1.MMM, t->b.s1.Ula, t->b.s1.Ulo );
        }

      if ( t->b.s1.h0h0h0h0[0] )
        {
          used += snprintf ( c + used, lmax - used, " %s%s", t->b.s1.h0h0h0h0, t->b.s1.im );
        }
    }

  *sec1= c + used;
  return used;
}

/*!
  \fn size_t print_temp_b_sec5 (char **sec5, size_t lmax, struct temp_chunks *t)
  \brief Prints the section 5 of part B of a TEMP report
  \param sec5 the pointer where to print section
  \param lmax max length permited
  \param t pointer to s atruct \ref temp_chunks where the parse results are set

  returns the string sec5
*/
size_t print_temp_b_sec5 ( char **sec5, size_t lmax, struct temp_chunks *t )
{
  size_t used = 0;
  size_t i;
  char *c = *sec5;

  for ( i = 0; i < t->b.s5.n && i < TEMP_NMAX_POINTS ; i++ )
    {
      used += snprintf ( c + used, lmax - used, " %s%s", t->b.s5.th[i].nini, t->b.s5.th[i].PnPnPn );
      used += snprintf ( c + used, lmax - used, " %s%s", t->b.s5.th[i].TnTnTan, t->b.s5.th[i].DnDn );
    }

  *sec5 = c + used;
  return used;
}

/*!
  \fn size_t print_temp_b_sec6 (char **sec6, size_t lmax, struct temp_chunks *t)
  \brief Prints the section 6 of part B of a TEMP report
  \param sec6 the pointer where to print section
  \param lmax max length permited
  \param t pointer to s atruct \ref temp_chunks where the parse results are set

  returns the string sec6
*/
size_t print_temp_b_sec6 ( char **sec6, size_t lmax, struct temp_chunks *t )
{
  size_t i;
  size_t used = 0;
  char *c = *sec6;

  used += snprintf ( c + used, lmax - used, " 21212" );

  for ( i = 0; i < t->b.s6.n && i < TEMP_NMAX_POINTS ; i++ )
    {
      used += snprintf ( c + used, lmax - used, " %s%s", t->b.s6.wd[i].nini, t->b.s6.wd[i].PnPnPn );
      used += snprintf ( c + used, lmax - used, " %s", t->b.s6.wd[i].dndnfnfnfn );
    }

  *sec6 = c + used;
  return used;
}

/*!
  \fn size_t print_temp_b_sec7 (char **sec7, size_t lmax, struct temp_chunks *t)
  \brief Prints the section 7 of part B of a TEMP report
  \param sec7 the pointer where to print section
  \param lmax max length permited
  \param t pointer to s atruct \ref temp_chunks where the parse results are set

  returns the string sec7
*/
size_t print_temp_b_sec7 ( char **sec7, size_t lmax, struct temp_chunks *t )
{
  size_t used = 0;
  char *c = *sec7;

  used += snprintf ( c + used, lmax - used, " 31313" );
  used += snprintf ( c + used, lmax - used, " %s%s%s", t->b.s7.sr, t->b.s7.rara, t->b.s7.sasa );
  used += snprintf ( c + used, lmax - used, " 8%s%s", t->b.s7.GG, t->b.s7.gg );

  if ( t->b.s7.TwTwTw[0] )
    {
      used += snprintf ( c + used, lmax - used, " 9%s%s", t->b.s7.sn, t->b.s7.TwTwTw );
    }

  *sec7 = c + used;
  return used;
}

/*!
  \fn size_t print_temp_b_sec8 (char **sec8, size_t lmax, struct temp_chunks *t)
  \brief Prints the section 8 of part B of a TEMP report
  \param sec8 the pointer where to print section
  \param lmax max length permited
  \param t pointer to s atruct \ref temp_chunks where the parse results are set

  returns the string sec8
*/
size_t print_temp_b_sec8 ( char **sec8, size_t lmax, struct temp_chunks *t )
{
  size_t used = 0;
  char *c = *sec8;

  if ( t->b.s8.h[0] || t->b.s8.Nh[0] || t->b.s8.Cl[0] || t->b.s8.Cm[0] || t->b.s8.Ch[0] )
    {
      used += snprintf ( c + used, lmax - used, " 41414 " );
      if ( t->b.s8.Nh[0] )
        {
          used += snprintf ( c + used, lmax - used, "%s", t->b.s8.Nh );
        }
      else
        {
          used += snprintf ( c + used, lmax - used, "/" );
        }

      if ( t->b.s8.Cl[0] )
        {
          used += snprintf ( c + used, lmax - used, "%s", t->b.s8.Cl );
        }
      else
        {
          used += snprintf ( c + used, lmax - used, "/" );
        }

      if ( t->b.s8.h[0] )
        {
          used += snprintf ( c + used, lmax - used, "%s", t->b.s8.h );
        }
      else
        {
          used += snprintf ( c + used, lmax - used, "/" );
        }

      if ( t->b.s8.Cm[0] )
        {
          used += snprintf ( c + used, lmax - used, "%s", t->b.s8.Cm );
        }
      else
        {
          used += snprintf ( c + used, lmax - used, "/" );
        }

      if ( t->b.s8.Ch[0] )
        {
          used += snprintf ( c + used, lmax - used, "%s", t->b.s8.Ch );
        }
      else
        {
          used += snprintf ( c + used, lmax - used, "/" );
        }

      //used += snprintf ( c + used, lmax - used, " %s%s%s%s%s", t->b.s8.Nh, t->b.s8.Cl, t->b.s8.h, t->b.s8.Cm, t->b.s8.Ch );
    }

  *sec8 = c + used;
  return used;
}


/*!
  \fn int print_temp_b (struct metreport *m )
  \brief Prints the part B of a TEMP report into a string
  \param m pointer to struct \ref metreport where are both target and source

  If OK returns 0, otherwise 1
 */
int print_temp_b ( struct metreport *m )
{
  char *c = &(m->alphanum2[0]);
  size_t used = 0;
  size_t lmax = sizeof(m->alphanum);
  struct temp_chunks *t = &m->temp;


  // Needs time extension
  if ( t->b.e.YYYY[0] == 0  || t->b.e.YYYY[0] == '0')
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

  used += print_temp_b_sec1 ( &c, lmax - used, t );
  used += print_temp_b_sec5 ( &c, lmax - used, t );
  used += print_temp_b_sec6 ( &c, lmax - used, t );
  used += print_temp_b_sec7 ( &c, lmax - used, t );
  used += print_temp_b_sec8 ( &c, lmax - used, t );
  snprintf ( c, lmax - used, "=" );
  return 0;
}


/*!
  \fn size_t print_temp_c_sec1 (char **sec1, size_t lmax, struct temp_chunks *t)
  \brief Prints the section 1 of part C of a TEMP report
  \param sec1 the pointer where to print section
  \param lmax max length permited
  \param t pointer to s atruct \ref temp_chunks where the parse results are set

  returns the string sec1
*/
size_t print_temp_c_sec1 ( char **sec1, size_t lmax, struct temp_chunks *t )
{
  size_t used = 0;
  char *c = *sec1;

  used += snprintf ( c + used, lmax - used, "%s", t->t.datime );

  used += snprintf ( c + used, lmax - used, " %s%s", t->c.s1.MiMi, t->c.s1.MjMj );

  if ( t->c.s1.D_D[0] && t->a.s1.II[0] == 0 )
    {
      used += snprintf ( c + used, lmax - used, " %s", t->c.s1.D_D );
    }

  used += snprintf ( c + used, lmax - used, " %s%s", t->c.s1.YYGG, t->c.s1.id );

  // print IIiii
  if ( t->c.s1.II[0] )
    {
      used += snprintf ( c + used, lmax - used, " %s%s", t->c.s1.II, t->c.s1.iii );
    }
  else
    {
      if ( t->c.s1.LaLaLa[0] )
        {
          used += snprintf ( c + used, lmax - used, " 99%s", t->c.s1.LaLaLa );
        }
      else
        {
          used += snprintf ( c + used, lmax - used, " 99///" );
        }

      if ( t->c.s1.Qc[0] && t->c.s1.LoLoLoLo[0] )
        {
          used += snprintf ( c + used, lmax - used, " %s%s", t->c.s1.Qc, t->c.s1.LoLoLoLo );
        }
      else
        {
          used += snprintf ( c + used, lmax - used, " /////" );
        }

      if ( t->c.s1.MMM[0] && t->c.s1.Ula[0] && t->c.s1.Ulo[0] )
        {
          used += snprintf ( c + used, lmax - used, " %s%s%s", t->c.s1.MMM, t->c.s1.Ula, t->c.s1.Ulo );
        }

      if ( t->c.s1.h0h0h0h0[0] )
        {
          used += snprintf ( c + used, lmax - used, " %s%s", t->c.s1.h0h0h0h0, t->c.s1.im );
        }
    }

  *sec1 = c + used;
  return used;
}

/*!
  \fn size_t print_temp_c_sec2 (char **sec2, size_t lmax, struct temp_chunks *t)
  \brief Prints the section 2 of part C of a TEMP report
  \param sec2 the pointer where to print section
  \param lmax max length permited
  \param t pointer to s atruct \ref temp_chunks where the parse results are set

  returns the string sec2
*/
size_t print_temp_c_sec2 ( char **sec2, size_t lmax, struct temp_chunks *t )
{
  size_t i;
  size_t used = 0;
  char *c = *sec2;

  for ( i = 0; i < t->c.s2.n ; i++ )
    {
      used += snprintf ( c + used, lmax - used, " %s%s", t->c.s2.std[i].PnPn, t->c.s2.std[i].hnhnhn );
      used += snprintf ( c + used, lmax - used, " %s%s", t->c.s2.std[i].TnTnTan, t->c.s2.std[i].DnDn );
      used += snprintf ( c + used, lmax - used, " %s", t->c.s2.std[i].dndnfnfnfn );
    }

  *sec2 = c + used;
  return used;

}

/*!
  \fn size_t print_temp_c_sec3 (char **sec3, size_t lmax, struct temp_chunks *t)
  \brief Prints the section 3 of part C of a TEMP report
  \param sec3 the pointer where to print section
  \param lmax max length permited
  \param t pointer to s atruct \ref temp_chunks where the parse results are set

  returns the string sec3
*/
size_t print_temp_c_sec3 ( char **sec3, size_t lmax, struct temp_chunks *t )
{
  size_t used = 0;
  char *c = *sec3;

  if ( t->c.s3.n == 0 )
    {
      used += snprintf ( c + used, lmax - used, " 88999" );
    }
  else
    {
      for ( size_t i = 0; i < t->c.s3.n ; i++ )
        {
          used += snprintf ( c + used, lmax - used, " 88%s", t->c.s3.trop[i].PnPnPn );
          used += snprintf ( c + used, lmax - used, " %s%s", t->c.s3.trop[i].TnTnTan, t->c.s3.trop[i].DnDn );
          used += snprintf ( c + used, lmax - used, " %s", t->c.s3.trop[i].dndnfnfnfn );
        }
    }

  *sec3 = c + used;
  return used;

}

/*!
  \fn size_t print_temp_c_sec4 (char **sec4, size_t lmax, struct temp_chunks *t)
  \brief Prints the section 4 of part C of a TEMP report
  \param sec4 the pointer where to print section
  \param lmax max length permited
  \param t pointer to s atruct \ref temp_chunks where the parse results are set

  returns the string sec4
*/
size_t print_temp_c_sec4 ( char **sec4, size_t lmax, struct temp_chunks *t )
{
  size_t used = 0;
  char *c = *sec4;

  if ( t->c.s4.n == 0 )
    {
      used += snprintf ( c + used, lmax - used, " 77999" );
    }
  else
    {
      for (size_t i = 0; i < t->c.s4.n ; i++ )
        {
          if ( t->c.s4.windx[i].no_last_wind )
            {
              used += snprintf ( c + used, lmax - used, " 77%s", t->c.s4.windx[i].PmPmPm );
            }
          else
            {
              used += snprintf ( c + used, lmax - used, " 66%s", t->c.s4.windx[i].PmPmPm );
            }
          used += snprintf ( c + used, lmax - used, " %s", t->c.s4.windx[i].dmdmfmfmfm );
          if ( t->c.s4.windx[i].vbvb[0] && t->c.s4.windx[i].vava[0] )
            {
              used += snprintf ( c + used, lmax - used, " 4%s%s", t->c.s4.windx[i].vbvb, t->c.s4.windx[i].vava );
            }
        }
    }

  *sec4 = c + used;
  return used;

}

/*!
  \fn size_t print_temp_c_sec7 (char **sec7, size_t lmax, struct temp_chunks *t)
  \brief Prints the section 7 of part C of a TEMP report
  \param sec7 the pointer where to print section
  \param lmax max length permited
  \param t pointer to s atruct \ref temp_chunks where the parse results are set

  returns the string sec7
*/
size_t print_temp_c_sec7 ( char **sec7, size_t lmax, struct temp_chunks *t )
{
  size_t used = 0;
  char *c = *sec7;

  used += snprintf ( c + used, lmax - used, " 31313" );
  used += snprintf ( c + used, lmax - used, " %s%s%s", t->c.s7.sr, t->c.s7.rara, t->c.s7.sasa );
  used += snprintf ( c + used, lmax - used, " 8%s%s", t->c.s7.GG, t->c.s7.gg );

  if ( t->c.s7.TwTwTw[0] )
    {
      used += snprintf ( c + used, lmax - used, " 9%s%s", t->c.s7.sn, t->c.s7.TwTwTw );
    }

  *sec7 = c + used;
  return used;
}

/*!
  \fn int print_temp_c (struct metreport *m )
  \brief Prints the part C of a TEMP report into a string
  \param m pointer to struct \ref metreport where are both target and source

  If OK returns 0, otherwise 1
*/
int print_temp_c ( struct metreport *m )
{
  char *c = &(m->alphanum3[0]);
  size_t used = 0;
  size_t lmax = sizeof(m->alphanum);
  struct temp_chunks *t = &m->temp;


  // Needs time extension
  if ( t->c.e.YYYY[0] == 0  || t->c.e.YYYY[0] == '0')
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

  used += print_temp_c_sec1 ( &c, lmax - used, t );
  used += print_temp_c_sec2 ( &c, lmax - used, t );
  used += print_temp_c_sec3 ( &c, lmax - used, t );
  used += print_temp_c_sec4 ( &c, lmax - used, t );
  used += print_temp_c_sec7 ( &c, lmax - used, t );
  snprintf ( c, lmax - used, "=" );
  return 0;
}

/*!
  \fn size_t print_temp_d_sec1 (char **sec1, size_t lmax, struct temp_chunks *t)
  \brief Prints the section 1 of part D of a TEMP report
  \param sec1 the pointer where to print section
  \param lmax max length permited
  \param t pointer to s atruct \ref temp_chunks where the parse results are set

  returns the string sec1
*/
size_t print_temp_d_sec1 ( char **sec1, size_t lmax, struct temp_chunks *t )
{
  size_t used = 0;
  char *c = *sec1;

  used += snprintf ( c + used, lmax - used, "%s", t->t.datime );

  used += snprintf ( c + used, lmax - used, " %s%s", t->d.s1.MiMi, t->d.s1.MjMj );

  if ( t->d.s1.D_D[0] && t->a.s1.II[0] == 0 )
    {
      used += snprintf ( c + used, lmax - used, " %s", t->d.s1.D_D );
    }

  used += snprintf ( c + used, lmax - used, " %s/", t->d.s1.YYGG );

  // print IIiii
  if ( t->d.s1.II[0] )
    {
      used += snprintf ( c + used, lmax - used, " %s%s", t->d.s1.II, t->d.s1.iii );
    }
  else
    {
      if ( t->d.s1.LaLaLa[0] )
        {
          used += snprintf ( c + used, lmax - used, " 99%s", t->d.s1.LaLaLa );
        }
      else
        {
          used += snprintf ( c + used, lmax - used, " 99///" );
        }

      if ( t->d.s1.Qc[0] && t->d.s1.LoLoLoLo[0] )
        {
          used += snprintf ( c + used, lmax - used, " %s%s", t->d.s1.Qc, t->d.s1.LoLoLoLo );
        }
      else
        {
          used += snprintf ( c + used, lmax - used, " /////" );
        }

      if ( t->d.s1.MMM[0] && t->d.s1.Ula[0] && t->d.s1.Ulo[0] )
        {
          used += snprintf ( c + used, lmax - used, " %s%s%s", t->d.s1.MMM, t->d.s1.Ula, t->d.s1.Ulo );
        }

      if ( t->d.s1.h0h0h0h0[0] )
        {
          used += snprintf ( c + used, lmax - used, " %s%s", t->d.s1.h0h0h0h0, t->d.s1.im );
        }
    }

  *sec1 = c + used;
  return used;
}

/*!
  \fn size_t print_temp_d_sec5 (char **sec5, size_t lmax, struct temp_chunks *t)
  \brief Prints the section 5 of part D of a TEMP report
  \param sec5 the pointer where to print section
  \param lmax max length permited
  \param t pointer to s atruct \ref temp_chunks where the parse results are set

  returns the string sec5
*/
size_t print_temp_d_sec5 ( char **sec5, size_t lmax, struct temp_chunks *t )
{
  size_t i;
  size_t used = 0;
  char *c = *sec5;

  for ( i = 0; i < t->d.s5.n && i < TEMP_NMAX_POINTS ; i++ )
    {
      used += snprintf ( c + used, lmax - used, " %s%s", t->d.s5.th[i].nini, t->d.s5.th[i].PnPnPn );
      used += snprintf ( c + used, lmax - used, " %s%s", t->d.s5.th[i].TnTnTan, t->d.s5.th[i].DnDn );
    }

  *sec5 = c + used;
  return used;
}

/*!
  \fn size_t print_temp_d_sec6 (char **sec6, size_t lmax, struct temp_chunks *t)
  \brief Prints the section 6 of part D of a TEMP report
  \param sec6 the pointer where to print section
  \param lmax max length permited
  \param t pointer to s atruct \ref temp_chunks where the parse results are set

  returns the string sec6
*/
size_t print_temp_d_sec6 ( char **sec6, size_t lmax, struct temp_chunks *t )
{
  size_t i;
  size_t used = 0;
  char *c = *sec6;

  used += snprintf ( c + used, lmax - used, " 21212" );

  for ( i = 0; i < t->d.s6.n && i < TEMP_NMAX_POINTS ; i++ )
    {
      used += snprintf ( c + used, lmax - used, " %s%s", t->d.s6.wd[i].nini, t->d.s6.wd[i].PnPnPn );
      used += snprintf ( c + used, lmax - used, " %s", t->d.s6.wd[i].dndnfnfnfn );
    }

  *sec6 = c + used;
  return used;
}

/*!
  \fn size_t print_temp_d_sec7 (char **sec7, size_t lmax, struct temp_chunks *t)
  \brief Prints the section 7 of part D of a TEMP report
  \param sec7 the pointer where to print section
  \param lmax max length permited
  \param t pointer to s atruct \ref temp_chunks where the parse results are set

  returns the string sec7
*/
size_t print_temp_d_sec7 ( char **sec7, size_t lmax, struct temp_chunks *t )
{
  size_t used = 0;
  char *c = *sec7;

  used += snprintf ( c + used, lmax - used, " 31313" );
  used += snprintf ( c + used, lmax - used, " %s%s%s", t->d.s7.sr, t->d.s7.rara, t->d.s7.sasa );
  used += snprintf ( c + used, lmax - used, " 8%s%s", t->d.s7.GG, t->d.s7.gg );

  if ( t->d.s7.TwTwTw[0] )
    {
      used += snprintf ( c + used, lmax - used, " 9%s%s", t->d.s7.sn, t->d.s7.TwTwTw );
    }

  *sec7 = c + used;
  return used;
}

/*!
  \fn int print_temp_d (struct metreport *m)
  \brief Prints the part D of a TEMP report into a string
  \param m pointer to struct \ref metreport where are both target and source

  If OK returns 0, otherwise 1
*/
int print_temp_d ( struct metreport *m )
{
  char *c = &(m->alphanum4[0]);
  size_t used = 0;
  size_t lmax = sizeof(m->alphanum);
  struct temp_chunks *t = &m->temp;

  // Needs time extension
  if ( t->d.e.YYYY[0] == 0  || t->d.e.YYYY[0] == '0')
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

  used += print_temp_d_sec1 ( &c, lmax - used, t );
  used += print_temp_d_sec5 ( &c, lmax - used, t );
  used += print_temp_d_sec6 ( &c, lmax - used, t );
  used += print_temp_d_sec7 ( &c, lmax - used, t );
  snprintf ( c, lmax - used, "=" );
  return 0;
}

/*!
   \fn int print_temp_report ( struct metreport *m )
   \brief print the four parts of a decoded TEMP report from a BUFR file into strings
   \param m pointer to a struct \ref metreport in which alphanumeric string members stores the reults
*/
int print_temp_report ( struct metreport *m )
{
  // It is required that al lesat a standard level where decoded in SEC A
  if ( m->temp.a.mask & TEMP_SEC_2 )
    {
      print_temp_a ( m );
    }

  // It is required a significant TH or wind poind for sec B
  if ( m->temp.b.mask & ( TEMP_SEC_5 | TEMP_SEC_6 ) )
    {
      print_temp_b ( m );
    }

  // It is required a standard level for SEC C
  if ( m->temp.c.mask & TEMP_SEC_2 )
    {
      print_temp_c ( m );
    }

  // It is required a significant TH or wind poind for sec B
  if ( m->temp.d.mask & ( TEMP_SEC_5 | TEMP_SEC_6 ) )
    {
      print_temp_d ( m );
    }

  return 0;
}
