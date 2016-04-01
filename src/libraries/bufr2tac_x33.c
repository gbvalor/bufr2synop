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
 \file bufr2tac_x33.c
 \brief decodes the descriptors with X = 33 (Quality data)
*/
#include "bufr2tac.h"

/*!
  \fn int buoy_parse_x33 ( struct buoy_chunks *b, struct bufr2tac_subset_state *s )
  \brief Parse a expanded descriptor with X = 33
  \param b pointer to a struct \ref buoy_chunks where to set the results
  \param s pointer to a struct \ref bufr2tac_subset_state where is stored needed information in sequential analysis

  It returns 0 if success, 1 if problems when processing. If a descriptor is not processed returns 0 anyway
*/
int buoy_parse_x33 ( struct buoy_chunks *b, struct bufr2tac_subset_state *s )
{

  if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
    return 0;


  switch ( s->a->desc.y )
    {
    case 20: // 0 33 020. Quality control indication of followinf value
      sprintf ( b->s0.Qt, "%d", s->ival );
      b->mask |= BUOY_SEC1;
      break;
    case 21: // 0 33 021. Quality control of following value
      sprintf ( b->s4.Qp, "%d", s->ival );
      b->mask |= BUOY_SEC1;
      break;
    case 22: // 0 33 022. Quality of buoy satellite transmission
      if ( s->ival == 0 )
        {
          if ( b->s0.Qt[0] == 0 )
            sprintf ( b->s0.Qt, "1" );
          if ( b->s1.Qd[0] == 0 )
            sprintf ( b->s1.Qd, "1" );
          if ( b->s2.Qd[0] == 0 )
            sprintf ( b->s2.Qd, "1" );
        }
      if ( s->ival == 2 )
        {
          if ( b->s0.Qt[0] == 0 )
            sprintf ( b->s0.Qt, "3" );
          if ( b->s1.Qd[0] == 0 )
            sprintf ( b->s1.Qd, "3" );
          if ( b->s2.Qd[0] == 0 )
            sprintf ( b->s2.Qd, "3" );
        }
      break;
    case 23: // 0 33 023 . Quality of buoy location
      sprintf ( b->s0.Ql, "%d", s->ival );
      sprintf ( b->s4.QL, "%d", s->ival );
      break;
    case 27: // 0 33 027. Location quality class (range of ratiuos of 66% confidence)
      sprintf ( b->s0.QA, "%d", s->ival );
      sprintf ( b->s4.QA, "%d", s->ival );
      break;
    default:
      break;
    }
  return 0;
}

/*!
  \fn int temp_parse_x33 ( struct temp_chunks *b, struct bufr2tac_subset_state *s )
  \brief Parse a expanded descriptor with X = 33
  \param t pointer to a struct \ref temp_chunks where to set the results
  \param s pointer to a struct \ref bufr2tac_subset_state where is stored needed information in sequential analysis

  It returns 0 if success, 1 if problems when processing. If a descriptor is not processed returns 0 anyway
*/
int temp_parse_x33 ( struct temp_chunks *t, struct bufr2tac_subset_state *s )
{

  switch ( s->a->desc.y )
    {
    case 24: // 0 33 024. Station elevation quality mark (for mobile stations)
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          strcpy ( t->a.s1.im, "/" );
          strcpy ( t->b.s1.im, "/" );
          strcpy ( t->c.s1.im, "/" );
          strcpy ( t->d.s1.im, "/" );
          return 0;
        }

      if ( s->ival >= 0 && s->ival < 9 )
        {
          sprintf ( t->a.s1.im, "%d", s->ival );
          sprintf ( t->a.s1.im, "%d", s->ival );
          sprintf ( t->a.s1.im, "%d", s->ival );
          sprintf ( t->a.s1.im, "%d", s->ival );
        }
      else
        {
          strcpy ( t->a.s1.im, "/" );
          strcpy ( t->b.s1.im, "/" );
          strcpy ( t->c.s1.im, "/" );
          strcpy ( t->d.s1.im, "/" );
        }
    default:
      break;
    }
  return 0;
}
