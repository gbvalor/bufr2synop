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
 \file bufr2syn_x11.c
 \brief decodes the descriptors with X = 11
 */
#include "bufr2synop.h"

int syn_parse_x11 ( struct synop_chunks *syn, struct bufr_subset_state *s, char *err )
{
  switch ( s->a->desc.y )
    {
    case 1: // 0 11 001
    case 11: // 0 11 011
      sprintf ( syn->s1.dd, "%02d", ( s->ival + 5 ) / 10 );
      break;
    case 2: // 0 11 002
    case 12: // 0 11 012
      if ( syn->s0.iw[0] == '4' )
        s->val *= 1.94384449;
      if ( s->val < 100.0 )
        sprintf ( syn->s1.ff, "%02d", ( int ) ( s->val + 0.5 ) );
      else
        {
          sprintf ( syn->s1.ff,"99" );
          sprintf ( syn->s1.fff, "%03d", ( int ) ( s->val + 0.5 ) );
        }
      break;
    default:
      break;
    }
  return 0;
}
