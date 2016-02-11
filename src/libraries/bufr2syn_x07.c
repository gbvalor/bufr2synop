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
 \file bufr2syn_x07.c
 \brief decodes the descriptors with X = 07 (Vertical Position)
 */
#include "bufr2syn.h"

/*!
  \fn int syn_parse_x07 ( struct synop_chunks *syn, struct bufr_subset_state *s )
  \brief Parse a expanded descriptor with X = 07
  \param syn pointer to a struct \ref synop_chunks where to set the results
  \param s pointer to a struct \ref bufr_subset_state where is stored needed information in sequential analysis

  It returns 0 if success, 1 if problems when processing. If a descriptor is not processed returns 0 anyway
*/
int syn_parse_x07 ( struct synop_chunks *syn, struct bufr_subset_state *s )
{

  if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
    {
      return 0;
    }

  // this is to avoid warning
  if ( syn == NULL )
    {
      return 1;
    }

  switch ( s->a->desc.y )
    {
    case 1: // 0 07 001 . Heigh of station
    case 30: // 0 07 030 . Height of station ground above msl
    case 31: // 0 07 031 . Height of barometer above msl
      if ( syn->s0.h0h0h0h0[0] == 0 )
        {
          sprintf ( syn->s0.h0h0h0h0, "%04d", s->ival );
          syn->s0.im[0] = '1';// set unit as m
          s->mask |= SUBSET_MASK_HAVE_ALTITUDE;
        }
      s->alt = s->val;
      break;
    default:
      break;
    }

  return 0;
}

/*!
  \fn int buoy_parse_x07 ( struct buoy_chunks *b, struct bufr_subset_state *s )
  \brief Parse a expanded descriptor with X = 07
  \param b pointer to a struct \ref buoy_chunks where to set the results
  \param s pointer to a struct \ref bufr_subset_state where is stored needed information in sequential analysis

  It returns 0 if success, 1 if problems when processing. If a descriptor is not processed returns 0 anyway
*/
int buoy_parse_x07 ( struct buoy_chunks *b, struct bufr_subset_state *s )
{

  if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
    {
      return 0;
    }

  if ( b == NULL )
    {
      return 1;
    }

  switch ( s->a->desc.y )
    {
    case 62: // 0 07 062 Depth below sea/water surface
    case 63: // 0 07 063 Profundity below sea/water level
      if ( s->k_rep == ( s->i - 1 ) ) // Case of first layer after a replicator
        {
          s->layer = 0;
        }
      else
        {
          ( s->layer ) ++;
        }
      s->deep = ( int ) ( s->val + 0.5 );
      break;
    default:
      break;
    }
  return 0;
}

/*!
  \fn int climat_parse_x07 ( struct synop_chunks *c, struct bufr_subset_state *s )
  \brief Parse a expanded descriptor with X = 07
  \param c pointer to a struct \ref climat_chunks where to set the results
  \param s pointer to a struct \ref bufr_subset_state where is stored needed information in sequential analysis

  It returns 0 if success, 1 if problems when processing. If a descriptor is not processed returns 0 anyway
*/
int climat_parse_x07 ( struct climat_chunks *c, struct bufr_subset_state *s )
{
  if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
    {
      return 0;
    }

  // this is to avoid warning
  if ( c == NULL )
    {
      return 1;
    }

  switch ( s->a->desc.y )
    {
    case 1: // 0 07 001 . Heigh of station
    case 30: // 0 07 030 . Height of station ground above msl
    case 31: // 0 07 031 . Height of barometer above msl
      s->alt = s->val;
      break;

    case 32: // 0 07 032 . Pressure of standard level
      // Not a useful value for alphanumeric climat
      break;

    case 4: // Pressure of standard level
      // Not a useful value for alphanumeric climat
      break;
    default:
      break;
    }

  return 0;
}

/*!
  \fn int temp_parse_x07 ( struct synop_chunks *c, struct bufr_subset_state *s )
  \brief Parse a expanded descriptor with X = 07
  \param c pointer to a struct \ref temp_chunks where to set the results
  \param s pointer to a struct \ref bufr_subset_state where is stored needed information in sequential analysis

  It returns 0 if success, 1 if problems when processing. If a descriptor is not processed returns 0 anyway
*/
int temp_parse_x07 ( struct temp_chunks *t, struct bufr_subset_state *s )
{
  if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
    {
      return 0;
    }

// this is to avoid warning
  if ( t == NULL )
    {
      return 1;
    }

  switch ( s->a->desc.y )
    {
    case 4:  // 0 07 004 . Pressure
      if ( s->rep > 0 && s->r->n > 0 )
        {
          s->r->raw[s->r->n - 1].p = s->val;
        }
      else if ( s->w->n > 0 )
        {
          s->w->raw[s->w->n - 1].p = s->val;
        }
      break;

    case 30: // 0 07 030 . Height of station ground above msl
      sprintf ( t->a.s1.h0h0h0h0, "%04.0lf" , s->val );
      sprintf ( t->b.s1.h0h0h0h0, "%04.0lf" , s->val );
      sprintf ( t->c.s1.h0h0h0h0, "%04.0lf" , s->val );
      sprintf ( t->d.s1.h0h0h0h0, "%04.0lf" , s->val );
      break;

    case 31: // 0 07 031 . Height of barometer above msl
      s->alt = s->val;
      break;

    default:
      break;
    }

  return 0;
}
