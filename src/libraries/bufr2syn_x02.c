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
 \file bufr2syn_x02.c
 \brief decodes the descriptors with X = 02 (intrumentation and station type)
 */
#include "bufr2syn.h"

/*!
  \fn int syn_parse_x02 ( struct synop_chunks *syn, struct bufr_subset_state *s )
  \brief Parse a expanded descriptor with X = 02
  \param syn pointer to a struct \ref synop_chunks where to set the results
  \param s pointer to a struct \ref bufr_subset_state where is stored needed information in sequential analysis

  It returns 0 if success, 1 if problems when processing. If a descriptor is not processed returns 0 anyway
*/
int syn_parse_x02 ( struct synop_chunks *syn, struct bufr_subset_state *s )
{


  switch ( s->a->desc.y )
    {
    case 1: // 0 02 001 Type of station
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          strcpy ( syn->s1.ix,"/" );
          return 0;
        }
      s->type = s->ival;
      s->mask |= SUBSET_MASK_HAVE_TYPE_STATION;
      break;
    case 2: // 0 02 002 . Type of instrumentation for wind measurement
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        return 0;
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
  \fn int buoy_parse_x02 ( struct buoy_chunks *b, struct bufr_subset_state *s )
  \brief Parse a expanded descriptor with X = 02
  \param b pointer to a struct \ref buoy_chunks where to set the results
  \param s pointer to a struct \ref bufr_subset_state where is stored needed information in sequential analysis

  It returns 0 if success, 1 if problems when processing. If a descriptor is not processed returns 0 anyway
*/
int buoy_parse_x02 ( struct buoy_chunks *b, struct bufr_subset_state *s )
{
  if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
    return 0;

  switch ( s->a->desc.y )
    {
    case 1: // 0 02 001 . Type of station
      s->type = s->ival;
      s->mask |= SUBSET_MASK_HAVE_TYPE_STATION;
      break;
    case 2: // 0 02 002 . Type of instrumentation for wind measurement
      if ( s->ival & 4 )
        strcpy ( b->s0.iw, "4" );
      else
        strcpy ( b->s0.iw, "1" );
      break;
    case 31: // 0 02 031 . Duration and time of current measurement
      if ( b->s3.k3[0] == 0 && s->ival < 10 )
        sprintf ( b->s3.k3 ,"%d",s->ival );
      b->mask |= BUOY_SEC3;
      break;
    case 33: // 0 02 033 . Method of salinity depth measure
      sprintf ( b->s3.k2 ,"%d",s->ival );
      b->mask |= BUOY_SEC3;
      break;
    case 40: // 0 02 040 .Method of removing velocity and motion of platform from current
      if ( b->s3.k6[0] == 0 )
        sprintf ( b->s3.k6 ,"%d",s->ival );
      b->mask |= BUOY_SEC3;
      break;
    default:
      break;
    }
  return 0;
}

/*!
  \fn int climat_parse_x02 ( struct climat_chunks *c, struct bufr_subset_state *s )
  \brief Parse a expanded descriptor with X = 02
  \param c pointer to a struct \ref climat_chunks where to set the results
  \param s pointer to a struct \ref bufr_subset_state where is stored needed information in sequential analysis

  It returns 0 if success, 1 if problems when processing. If a descriptor is not processed returns 0 anyway
*/
int climat_parse_x02 ( struct climat_chunks *c, struct bufr_subset_state *s )
{
  if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
    return 0;

  if ( c == NULL )
    return 1;

  switch ( s->a->desc.y )
    {
    case 1: // 0 02 001 . Type of station
      s->type = s->ival;
      s->mask |= SUBSET_MASK_HAVE_TYPE_STATION;
      break;

    case 2: // 0 02 002 . Type of instrumentation for wind measurement
      if ( s->ival & 4 )
        strcpy ( c->s4.iw, "4" );
      else
        strcpy ( c->s4.iw, "1" );
      break;

    case 51: // 0 02 051 . Observing method for extreme temperatures
      sprintf ( c->s4.iy,"%d",s->ival );
      break;
    default:
      break;
    }
  return 0;
}

/*!
  \fn int temp_parse_x02 ( struct temp_chunks *c, struct bufr_subset_state *s )
  \brief Parse a expanded descriptor with X = 02
  \param t pointer to a struct \ref temp_chunks where to set the results
  \param s pointer to a struct \ref bufr_subset_state where is stored needed information in sequential analysis

  It returns 0 if success, 1 if problems when processing. If a descriptor is not processed returns 0 anyway
*/
int temp_parse_x02 ( struct temp_chunks *t, struct bufr_subset_state *s )
{
  if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
    return 0;

  if ( t == NULL )
    return 1;

  switch ( s->a->desc.y )
    {
    case 3:  // 0 02 003 . Type of measuring equipment used
      switch ( s->ival )
        {
        case 0:
        case 1:
        case 2:
        case 3:
          sprintf ( t->b.s1.a4,"%d", s->ival );
          break;
        case 4:
        case 5:
        case 6:
        case 7:
          sprintf ( t->b.s1.a4,"%d", s->ival + 1 );
          break;
        default:
          strcpy ( t->b.s1.a4, "9" ); // reserved
          break;
        }
      break;

    case 11: // 0 02 011 . Radiosonde type
      if ( s->ival >= 0 && s->ival < 100 )
        {
          sprintf ( t->a.s7.rara, "%02d", s->ival );
          sprintf ( t->b.s7.rara, "%02d", s->ival );
          sprintf ( t->c.s7.rara, "%02d", s->ival );
          sprintf ( t->d.s7.rara, "%02d", s->ival );
        }
      break;

    case 13: // 0 02 013 . Solar and infrared radiation correction
      if ( s->ival >= 0 && s->ival <= 7 )
        {
          sprintf ( t->a.s7.sr, "%d", s->ival );
          sprintf ( t->b.s7.sr, "%d", s->ival );
          sprintf ( t->c.s7.sr, "%d", s->ival );
          sprintf ( t->d.s7.sr, "%d", s->ival );
        }
      else
        {
          // case of missing data
          strcpy ( t->a.s7.sr, "/" );
          strcpy ( t->b.s7.sr, "/" );
          strcpy ( t->c.s7.sr, "/" );
          strcpy ( t->d.s7.sr, "/" );
        }
      break;

    case 14: // 0 02 014 . Tracking technique/status of system used
      if ( s->ival >= 0 && s->ival < 100 )
        {
          sprintf ( t->a.s7.sasa, "%02d", s->ival );
          sprintf ( t->b.s7.sasa, "%02d", s->ival );
          sprintf ( t->c.s7.sasa, "%02d", s->ival );
          sprintf ( t->d.s7.sasa, "%02d", s->ival );
        }
      else
        {
          strcpy ( t->a.s7.sasa, "//" );
          strcpy ( t->b.s7.sasa, "//" );
          strcpy ( t->c.s7.sasa, "//" );
          strcpy ( t->d.s7.sasa, "//" );
        }
      break;

    default:
      break;
    }
  return 0;
}
