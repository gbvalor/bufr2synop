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
 \file bufr2tac_x31.c
 \brief decodes the descriptors with X = 31 (Control)
*/
#include "bufr2tac.h"


/*!
  \fn int syn_parse_x31 ( struct synop_chunks *syn, struct bufr_subset_state *s )
  \brief Parse a expanded descriptor with X = 31
  \param syn pointer to a struct \ref synop_chunks where to set the results
  \param s pointer to a struct \ref bufr_subset_state where is stored needed information in sequential analysis

  It returns 0 if success, 1 if problems when processing. If a descriptor is not processed returns 0 anyway
*/
int syn_parse_x31 ( struct synop_chunks *syn, struct bufr_subset_state *s )
{
  if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
    {
      return 0;
    }

  if ( syn == NULL )
    {
      return 1;
    }

  switch ( s->a->desc.y )
    {
    case 1:
      s->rep = s->ival;
      s->k_rep = s->i;
      break;
    default:
      break;
    }
  return 0;
}


/*!
  \fn int buoy_parse_x31 ( struct buoy_chunks *b, struct bufr_subset_state *s )
  \brief Parse a expanded descriptor with X = 31
  \param b pointer to a struct \ref buoy_chunks where to set the results
  \param s pointer to a struct \ref bufr_subset_state where is stored needed information in sequential analysis

  It returns 0 if success, 1 if problems when processing. If a descriptor is not processed returns 0 anyway
*/
int buoy_parse_x31 ( struct buoy_chunks *b, struct bufr_subset_state *s )
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
    case 1:
      s->rep = s->ival;
      s->k_rep = s->i;
      break;
    default:
      break;
    }
  return 0;
}

/*!
  \fn int temp_parse_x31 ( struct temp_chunks *t, struct bufr_subset_state *s )
  \brief Parse a expanded descriptor with X = 31
  \param t pointer to a struct \ref temp_chunks where to set the results
  \param s pointer to a struct \ref bufr_subset_state where is stored needed information in sequential analysis

  It returns 0 if success, 1 if problems when processing. If a descriptor is not processed returns 0 anyway
*/
int temp_parse_x31 ( struct temp_chunks *t, struct bufr_subset_state *s )
{
  if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
    {
      return 0;
    }

  if ( t == NULL )
    {
      return 1;
    }

  switch ( s->a->desc.y )
    {
    case 1: // 0 31 001 .  Replicator
      // It is supposed that it is the extended replicator used when describing
      // wind shear points in raiosonde
      // It is the amount of points of wind shear data at pressure level
      if ( s->ival < TEMP_NMAX_POINTS )
        {
          s->itval = s->ival; 
          s->rep = 0;  // used to mark it is a share point in sequent descriptors 
        }
      else
        {
          return 1;  // too much points
        }
      s->k_itval = s->i;
      s->w->n = 0;
      break;

    case 2: // 0 31 002 . Extended replicator
      // It is supposed that it is the extended replicator used when describing
      // Temperature, dew-point and wind data at a pressure level with radiosonde position
      // So the integer value of repliactor IS the amount of points
      s->rep = s->ival; // replications and points we need. Also Used to mark this type of point 
      s->itval = 0;
      s->k_rep = s->i;
      s->r->n = 0;
      break;
    default:
      break;
    }
  return 0;
}
