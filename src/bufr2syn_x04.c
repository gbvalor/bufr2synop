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
 \file bufr2syn_x03.c
 \brief decodes the descriptors with X = 03
 */
#include "bufr2synop.h"

int syn_parse_x04 ( struct synop_chunks *syn, struct bufr_subset_state *s, char *err )
{
  switch ( s->a->desc.y )
    {
    case 1: // 0 04 001
      sprintf ( syn->e.YYYY, "%04d", s->ival );
      break;
    case 2: // 0 04 002
      sprintf ( syn->e.MM, "%02d", s->ival );
      break;
    case 3: // 0 04 003
      sprintf ( syn->e.DD, "%02d", s->ival );
      //sprintf(syn->s0.YY, "%02d", (int) sq->sequence[is].val);
      break;
    case 4: // 0 04 004
      sprintf ( syn->e.HH, "%02d", s->ival );
      //sprintf(syn->s0.GG, "%02d", (int) sq->sequence[is].val);
      break;
    case 5: // 0 04 005
      sprintf ( syn->e.mm, "%02d", s->ival );
      break;
      // store latest displacement in seconds
    case 23: // 0 04 023
      s->itval = s->ival * 86400;
      break;
    case 24: // 0 04 024
      s->itval = s->ival * 3600;
      break;
    case 25: // 0 04 025
      s->itval = s->ival * 60;
      break;
    case 26: // 0 04 026
      s->itval = s->ival;
      break;
    default:
      break;
    }
  return 0;
}
