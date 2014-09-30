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
 \file bufr2syn_x02.c
 \brief decodes the descriptors with X = 02
 */
#include "bufr2syn.h"

/*!
  \fn int syn_parse_x02 ( struct synop_chunks *syn, struct bufr_subset_state *s, char *err )
  \brief Parse a expanded descriptor with X = 02
  \param syn pointer to a struct \ref synop_chunks where to set the results
  \param s pointer to a struct \ref bufr_subset_state where is stored needed information in sequential analysis
  \param err string with optional error 

  It returns 0 if success, 1 if problems when processing. If a descriptor is not processed returns 0 anyway
*/
int syn_parse_x02 ( struct synop_chunks *syn, struct bufr_subset_state *s, char *err )
{

  if ( s->a->mask & DESCRIPTOR_VALUE_MISSING)
    return 0;

  switch ( s->a->desc.y )
    {
    case 1: // 0 02 001
      s->type = s->ival;
      s->mask |= SUBSET_MASK_HAVE_TYPE_STATION;
      break;
    case 2: // 0 02 002
      if ( s->ival & 4 )
        strcpy ( syn->s0.iw, "4" );
      else
        strcpy ( syn->s0.iw, "1" );
      break;
    default:
      break;
    }
  return 0;
}

/*!
  \fn int buoy_parse_x02 ( struct buoy_chunks *b, struct bufr_subset_state *s, char *err )
  \brief Parse a expanded descriptor with X = 02
  \param b pointer to a struct \ref buoy_chunks where to set the results
  \param s pointer to a struct \ref bufr_subset_state where is stored needed information in sequential analysis
  \param err string with optional error 

  It returns 0 if success, 1 if problems when processing. If a descriptor is not processed returns 0 anyway
*/
int buoy_parse_x02 ( struct buoy_chunks *b, struct bufr_subset_state *s, char *err )
{
  char aux[16];

  if ( s->a->mask & DESCRIPTOR_VALUE_MISSING)
    return 0;

  switch ( s->a->desc.y )
    {
    case 1: // 0 02 001
      s->type = s->ival;
      s->mask |= SUBSET_MASK_HAVE_TYPE_STATION;
      break;
    case 2: // 0 02 002
      if ( s->ival & 4 )
        strcpy ( b->s0.iw, "4" );
      else
        strcpy ( b->s0.iw, "1" );
      break;
    case 31: // 0 02 031
      if (b->s3.k3[0] == 0 && s->ival < 10)
        sprintf(b->s3.k3 ,"%d",s->ival);
      b->mask |= BUOY_SEC3;
      break;
    case 33: // 0 02 033
      sprintf(b->s3.k2 ,"%d",s->ival);
      b->mask |= BUOY_SEC3;
      break;
    case 40: // 0 02 040
      if (b->s3.k6[0] == 0)
        sprintf(b->s3.k6 ,"%d",s->ival);
      b->mask |= BUOY_SEC3;
      break;
      default:
      break;
    }
  return 0;
}
