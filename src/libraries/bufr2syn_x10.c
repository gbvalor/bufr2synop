/***************************************************************************
 *   Copyright (C) 2013-2014 by Guillermo Ballester Valor                  *
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
 \brief decodes the descriptors with X = 10 (air pressure)
 */
#include "bufr2syn.h"

/*!
  \fn char * pascal_to_ppp ( char *target, double P )
  \brief Converts pascal values (variation) into a ppp string
  \param P the pressure variation in Pascal units
  \param target string with the result
*/
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


/*!
  \fn char * pascal_to_PPPP ( char *target, double P )
  \brief Converts pascal values into a PPPP string
  \param P the pressure variation in Pascal units
  \param target string with the result
*/
char * pascal_to_PPPP ( char *target, double P )
{
  int ic;
  ic = ( int ) ( P * 0.1 );
  sprintf ( target, "%04d", ic % 10000 );
  return target;
}

/*!
  \fn int syn_parse_x10 ( struct synop_chunks *syn, struct bufr_subset_state *s )
  \brief Parse a expanded descriptor with X = 10
  \param syn pointer to a struct \ref synop_chunks where to set the results
  \param s pointer to a struct \ref bufr_subset_state where is stored needed information in sequential analysis

  It returns 0 if success, 1 if problems when processing. If a descriptor is not processed returns 0 anyway
*/
int syn_parse_x10 ( struct synop_chunks *syn, struct bufr_subset_state *s )
{
  char aux[16];

  if ( s->a->mask & DESCRIPTOR_VALUE_MISSING)
    return 0;

  switch ( s->a->desc.y )
    {
    case 4: // 0 10 004
      pascal_to_PPPP ( aux, s->val );
      strcpy ( syn->s1.PoPoPoPo, aux );
      syn->mask |= SYNOP_SEC1;
      break;
    case 51: // 0 10 051
      pascal_to_PPPP ( aux, s->val );
      strcpy ( syn->s1.PPPP, aux );
      syn->mask |= SYNOP_SEC1;
      break;
    case 63: // 0 10 063
      sprintf ( syn->s1.a, "%1d",s->ival );
      syn->mask |= SYNOP_SEC1;
      break;
    case 61: // 0 10 061
      pascal_to_ppp ( aux, s->val );
      sprintf ( syn->s1.ppp, "%s", aux );
      syn->mask |= SYNOP_SEC1;
      break;
    default:
      break;
    }
  return 0;
}

/*!
  \fn int buoy_parse_x10 ( struct buoy_chunks *b, struct bufr_subset_state *s )
  \brief Parse a expanded descriptor with X = 10
  \param b pointer to a struct \ref buoy_chunks where to set the results
  \param s pointer to a struct \ref bufr_subset_state where is stored needed information in sequential analysis

  It returns 0 if success, 1 if problems when processing. If a descriptor is not processed returns 0 anyway
*/
int buoy_parse_x10 ( struct buoy_chunks *b, struct bufr_subset_state *s )
{
  char aux[16];

  if ( s->a->mask & DESCRIPTOR_VALUE_MISSING)
    return 0;

  switch ( s->a->desc.y )
    {

    case 4: // 0 10 004
      pascal_to_PPPP ( aux, s->val );
      strcpy ( b->s1.PoPoPoPo, aux );
      b->mask |= BUOY_SEC1;
      break;
    case 51: // 0 10 051
      pascal_to_PPPP ( aux, s->val );
      strcpy ( b->s1.PPPP, aux );
      b->mask |= BUOY_SEC1;
      break;
    case 63: // 0 10 063
      sprintf ( b->s1.a, "%1d",s->ival );
      b->mask |= BUOY_SEC1;
      break;
    case 61: // 0 10 061
      pascal_to_ppp ( aux, s->val );
      sprintf ( b->s1.ppp, "%s", aux );
      b->mask |= BUOY_SEC1;
      break;

    default:
      break;
    }
  return 0;
}
