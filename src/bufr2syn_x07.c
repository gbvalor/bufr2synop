/***************************************************************************
 *   Copyright (C) 2013 by Guillermo Ballester Valor                       *
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
#include "bufr2synop.h"

/*!
  \fn int syn_parse_x07 ( struct synop_chunks *syn, struct bufr_subset_state *s, char *err )
  \brief Parse a expanded descriptor with X = 07
  \param syn pointer to a struct \ref synop_chunks where to set the results
  \param s pointer to a struct \ref bufr_subset_state where is stored needed information in sequential analysis
  \param err string with optional error 

  It returns 0 if success, 1 if problems when processing. If a descriptor is not processed returns 0 anyway
*/
int syn_parse_x07 ( struct synop_chunks *syn, struct bufr_subset_state *s, char *err )
{
  int ia;
  switch ( s->a->desc.y )
    {
    case 1: // 0 07 001
    case 30:
    case 31:
      if (syn->s0.h0h0h0h0[0] == 0)
      {
        sprintf(syn->s0.h0h0h0h0, "%04d", s->ival); 
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
  \fn int buoy_parse_x07 ( struct buoy_chunks *b, struct bufr_subset_state *s, char *err )
  \brief Parse a expanded descriptor with X = 07
  \param b pointer to a struct \ref buoy_chunks where to set the results
  \param s pointer to a struct \ref bufr_subset_state where is stored needed information in sequential analysis
  \param err string with optional error 

  It returns 0 if success, 1 if problems when processing. If a descriptor is not processed returns 0 anyway
*/
int buoy_parse_x07 ( struct buoy_chunks *b, struct bufr_subset_state *s, char *err )
{
  char aux[16];

  switch ( s->a->desc.y )
    {
      case 62:
      case 63: // 0 07 063 Profundity below sea/water level
        if (s->k_rep == (s->i - 1) ) // Case of first layer after a replicator
           s->layer = 0;
        else 
           (s->layer)++;
        s->deep = (int)(s->val + 0.5);
        break;
      default:
      break;
    }
  return 0;
}
