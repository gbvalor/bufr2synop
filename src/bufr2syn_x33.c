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
 \file bufr2syn_x33.c
 \brief decodes the descriptors with X = 33 (Quality data)
*/
#include "bufr2synop.h"

/*!
  \fn int buoy_parse_x33 ( struct buoy_chunks *b, struct bufr_subset_state *s, char *err )
  \brief Parse a expanded descriptor with X = 33
  \param b pointer to a struct \ref buoy_chunks where to set the results
  \param s pointer to a struct \ref bufr_subset_state where is stored needed information in sequential analysis
  \param err string with optional error

  It returns 0 if success, 1 if problems when processing. If a descriptor is not processed returns 0 anyway
*/
int buoy_parse_x33 ( struct buoy_chunks *b, struct bufr_subset_state *s, char *err )
{
  char aux[16];

  switch ( s->a->desc.y )
    {
    case 20:
      sprintf(b->s0.Qt, "%d", s->ival);
      b->mask |= BUOY_SEC1;
      break;
    case 21:
      sprintf(b->s4.Qp, "%d", s->ival);
      b->mask |= BUOY_SEC1;
      break;
    case 22:
      if (s->ival == 0)
        {
          if ( b->s0.Qt[0] == 0)
            sprintf(b->s0.Qt, "1", s->ival);
          if ( b->s1.Qd[0] == 0)
            sprintf(b->s1.Qd, "1", s->ival);
          if ( b->s2.Qd[0] == 0)
            sprintf(b->s2.Qd, "1", s->ival);
        }
      if (s->ival == 2)
        {
          if (b->s0.Qt[0] == 0)
            sprintf(b->s0.Qt, "3", s->ival);
          if ( b->s1.Qd[0] == 0)
            sprintf(b->s1.Qd, "3", s->ival);
          if ( b->s2.Qd[0] == 0)
            sprintf(b->s2.Qd, "3", s->ival);
        }
      break;
    case 23:
      sprintf(b->s0.Ql, "%d", s->ival);
      sprintf(b->s4.QL, "%d", s->ival);
      break;
    case 27:
      sprintf(b->s0.QA, "%d", s->ival);
      sprintf(b->s4.QA, "%d", s->ival);
      break;
    default:
      break;
    }
  return 0;
}
