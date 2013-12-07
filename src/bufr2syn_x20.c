/***************************************************************************
 *   Copyright (C) 2012 by Guillermo Ballester Valor                       *
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
 \file bufr2syn_x20.c
 \brief decodes the descriptors with X = 20
 */
#include "bufr2synop.h"

char * percent_to_okta ( char *target, double perc )
{
  if ( perc == 0.0 )
    strcpy ( target,"0" );
  else if ( perc <= 12.5 )
    strcpy ( target,"1" );
  else if ( perc <= 25.0 )
    strcpy ( target,"2" );
  else if ( perc <= 37.5 )
    strcpy ( target,"3" );
  else if ( perc <= 50.0 )
    strcpy ( target,"4" );
  else if ( perc <= 62.5 )
    strcpy ( target,"5" );
  else if ( perc <= 75.0 )
    strcpy ( target,"6" );
  else if ( perc <= 87.5 )
    strcpy ( target,"7" );
  else if ( perc <= 100.0 )
    strcpy ( target,"8" );
  else if ( perc == 113.0 )
    strcpy ( target,"/" );
  return target;
}

char * m_to_h ( char *target, double h )
{
  if ( h < 50.0 )
    strcpy ( target,"0" );
  else if ( h < 100.0 )
    strcpy ( target,"1" );
  else if ( h < 200.0 )
    strcpy ( target,"2" );
  else if ( h < 300.0 )
    strcpy ( target,"3" );
  else if ( h < 600.0 )
    strcpy ( target,"4" );
  else if ( h < 1000.0 )
    strcpy ( target,"5" );
  else if ( h < 1500.0 )
    strcpy ( target,"6" );
  else if ( h < 2000.0 )
    strcpy ( target,"7" );
  else if ( h < 2500.0 )
    strcpy ( target,"8" );
  else
    strcpy ( target,"9" );
  return target;

}

char * vism_to_VV ( char *target, double V )
{
  if ( V < 100.0 )
    strcpy ( target, "00" );
  else if ( V <= 5000.0 )
    sprintf ( target, "%02d", ( int ) ( V + 0.1 ) / 100 );
  else if ( V <= 30000.0 )
    sprintf ( target, "%02d", ( int ) ( V + 0.1 ) / 1000 + 50 );
  else if ( V <= 70000.0 )
    sprintf ( target, "%02d", ( int ) ( V - 30000.0 ) / 5000 + 80 );
  else
    strcpy ( target, "89" );
  return target;
}

int syn_parse_x20 ( struct synop_chunks *syn, struct bufr_subset_state *s, char *err )
{
  switch ( s->a->desc.y )
    {
    case 1: // 0 20 001
      vism_to_VV ( syn->s1.VV, s->val );
      break;
    case 3: // 0 20 003
      if ( s->ival < 100 )
        {
          if ( syn->s1.ix[0] == '/' )
            strcpy ( syn->s1.ix,"1" );
          sprintf ( syn->s1.ww, "%02d", s->ival );
        }
      else if ( s->ival < 200 )
        {
          if ( syn->s1.ix[0] == '/' )
            strcpy ( syn->s1.ix,"7" );
          sprintf ( syn->s1.ww, "%02d", s->ival % 100 );
        }
      break;
    case 4: // 0 20 004
      if ( s->ival < 10 )
        {
          if ( syn->s1.ix[0] == '/' )
            strcpy ( syn->s1.ix,"1" );
          sprintf ( syn->s1.W1, "%d", s->ival );
        }
      else
        {
          if ( syn->s1.ix[0] == '/' )
            strcpy ( syn->s1.ix,"7" );
          sprintf ( syn->s1.W1, "%d", s->ival % 10 );
        }
      break;
    case 5: // 0 20 004
      if ( s->ival < 10 )
        {
          if ( syn->s1.ix[0] == '/' )
            strcpy ( syn->s1.ix,"1" );
          sprintf ( syn->s1.W2, "%d", s->ival );
        }
      else
        {
          if ( syn->s1.ix[0] == '/' )
            strcpy ( syn->s1.ix,"7" );
          sprintf ( syn->s1.W2, "%d", s->ival % 10 );
        }
      break;
    case 10: // 0 20 010
      percent_to_okta ( syn->s1.N, s->val );
      break;
    case 11: // 0 20 011
      if ( s->ival <= 8 )
        sprintf ( syn->s1.N, "%1d", s->ival );
      else if ( s->ival <= 10 )
        sprintf ( syn->s1.N, "9", s->ival );
      else if ( s->ival == 15 )
        sprintf ( syn->s1.N, "/");
      break;
    case 12: // 0 20 012
      if (s->ival >= 10 && s->ival < 20)
        sprintf( syn->s1.Ch, "%1d", s->ival % 10);
      else if (s->ival >= 20 && s->ival < 30)
        sprintf( syn->s1.Cm, "%1d", s->ival % 10);
      else if (s->ival >= 30 && s->ival < 40)
        sprintf( syn->s1.Cl, "%1d", s->ival % 10);
      else if (s->ival == 59)
        sprintf( syn->s1.Nh, "/");
      else if (s->ival == 60)
        sprintf( syn->s1.Ch, "/");
      else if (s->ival == 61)
        sprintf( syn->s1.Cm, "/");
      else if (s->ival == 62)
        sprintf( syn->s1.Cl, "/");
      break;
    case 13: // 0 20 013
      m_to_h( syn->s1.h, s->val );
      break;
    default:
      break;
    }
  return 0;
}
