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
 \file bufr2syn_x01.c
 \brief decodes the descriptors with X = 01
*/
#include "bufr2synop.h"


int syn_parse_x01 ( struct synop_chunks *syn, struct bufr_subset_state *s, char *err )
{
  switch ( s->a->desc.y )
    {
    case 1: // 0 01 001
      sprintf ( syn->s0.II, "%02d", s->ival );
      break;
    case 2: // 0 01 002
      sprintf ( syn->s0.iii, "%03d", s->ival );
      break;
    default:
      break;
    }
  return 0;
}

