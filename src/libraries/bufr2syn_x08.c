/***************************************************************************
 *   Copyright (C) 2013-2015 by Guillermo Ballester Valor                  *
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
 \file bufr2syn_x08.c
 \brief decodes the descriptors with X = 08 (Significance Qualifiers)

  As there is no significance information in synop we use this descriptors as
  off/on interruptor.
*/
#include "bufr2syn.h"

/*!
  \fn int syn_parse_x08 ( struct synop_chunks *syn, struct bufr_subset_state *s )
  \brief Parse a expanded descriptor with X = 08
  \param syn pointer to a struct \ref synop_chunks where to set the results
  \param s pointer to a struct \ref bufr_subset_state where is stored needed information in sequential analysis

  It returns 0 if success, 1 if problems when processing. If a descriptor is not processed returns 0 anyway
*/
int syn_parse_x08 ( struct synop_chunks *syn, struct bufr_subset_state *s )
{
  if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
    return 0;

  if ( syn == NULL )
    return 1;

  switch ( s->a->desc.y )
    {
    case 2: // 0 08 002 . Vertical significance (surface observations)
      if ( s->ival == 21 )
        s->clayer = 1; // first cloud layer
      else if ( s->ival == 22 )
        s->clayer = 2; // second cloud layer
      else if ( s->ival == 23 )
        s->clayer = 3; // third cloud layer
      else if ( s->ival == 24 )
        s->clayer = 4; // fourth cloud layer
      break;
    case 22:  // 0 08 022 . Total number
    case 23:  // 0 08 023 . First-order statistics
    case 24:  // 0 08 024 . Difference statistics
      if ( s->isq )
        s->isq = 0;
      else
        s->isq = 1;
      break;
    default:
      break;
    }
  return 0;
}

/*!
  \fn int buoy_parse_x08 ( struct buoy_chunks *b, struct bufr_subset_state *s )
  \brief Parse a expanded descriptor with X = 08
  \param b pointer to a struct \ref synop_chunks where to set the results
  \param s pointer to a struct \ref bufr_subset_state where is stored needed information in sequential analysis

  It returns 0 if success, 1 if problems when processing. If a descriptor is not processed returns 0 anyway
*/
int buoy_parse_x08 ( struct buoy_chunks *b, struct bufr_subset_state *s )
{
  if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
    return 0;

  if ( b == NULL )
    return 1;

  switch ( s->a->desc.y )
    {
    case 22:  // 0 08 022 . Total number
    case 23:  // 0 08 023 . First-order statistics
    case 24:  // 0 08 024 . Difference statistics
      if ( s->isq )
        s->isq = 0;
      else
        s->isq = 1;
      break;
    default:
      break;
    }
  return 0;
}
