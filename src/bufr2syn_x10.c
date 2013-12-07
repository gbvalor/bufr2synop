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
 \file bufr2syn_x10.c
 \brief decodes the descriptors with X = 10
 */
#include "bufr2synop.h"

char * pascal_to_ppp ( char *target, double P )
{
  int ic;
  if ( P > 0 )
    ic = ( int ) ( P * 0.1 );
  else
    ic = ( int ) ( -P * 0.1 );
  sprintf ( target, "%03d", ic % 1000 );
  return target;
}

char * pascal_to_PPPP ( char *target, double P )
{
  int ic;
  ic = ( int ) ( P * 0.1 );
  sprintf ( target, "%04d", ic % 10000 );
  return target;
}

int syn_parse_x10 ( struct synop_chunks *syn, struct bufr_subset_state *s, char *err )
{
  char aux[16];

  switch ( s->a->desc.y )
    {
    case 4: // 0 10 004
      pascal_to_PPPP ( aux, s->val );
      strcpy ( syn->s1.PoPoPoPo, aux );
      break;
    case 51: // 0 10 051
      pascal_to_PPPP ( aux, s->val );
      strcpy ( syn->s1.PPPP, aux );
      break;
    case 63: // 0 10 063
      sprintf ( syn->s1.a, "%1d",s->ival );
      break;
    case 61: // 0 10 061
      pascal_to_ppp ( aux, s->val );
      sprintf ( syn->s1.ppp, "%s", aux );
      break;
    default:
      break;
    }
  return 0;
}
