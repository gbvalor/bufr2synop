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
 \file bufr2tac_x08.c
 \brief decodes the descriptors with X = 08 (Significance Qualifiers)

  As there is no significance information in synop we use this descriptors as
  off/on interruptor.
*/
#include "bufr2tac.h"

/*!
  \fn int syn_parse_x08 ( struct synop_chunks *syn, struct bufr2tac_subset_state *s )
  \brief Parse a expanded descriptor with X = 08
  \param syn pointer to a struct \ref synop_chunks where to set the results
  \param s pointer to a struct \ref bufr2tac_subset_state where is stored needed information in sequential analysis

  It returns 0 if success, 1 if problems when processing. If a descriptor is not processed returns 0 anyway
*/
int syn_parse_x08 ( struct synop_chunks *syn, struct bufr2tac_subset_state *s )
{
  if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
    {
      return 0;
    }

  if ( syn == NULL )
    {
      return 1;
    }

  switch ( s->a->desc.y )
    {
    case 2: // 0 08 002 . Vertical significance (surface observations)
      if ( s->ival == 21 )
        {
          s->clayer = 1;  // first cloud layer
        }
      else if ( s->ival == 22 )
        {
          s->clayer = 2;  // second cloud layer
        }
      else if ( s->ival == 23 )
        {
          s->clayer = 3;  // third cloud layer
        }
      else if ( s->ival == 24 )
        {
          s->clayer = 4;  // fourth cloud layer
        }
      else if (s->ival == 10)
      {
	  s->clayer = -1; // base of cloud layer below level station and top over level station
      }
      else if (s->ival == 11)
      {
	  s->clayer = -2; // base and top of cloud layer below level station       
	}
      break;
    case 22:  // 0 08 022 . Total number
    case 23:  // 0 08 023 . First-order statistics
    case 24:  // 0 08 024 . Difference statistics
      if ( s->isq )
        {
          s->isq = 0;
        }
      else
        {
          s->isq = 1;
        }
      break;
    default:
      break;
    }
  return 0;
}

/*!
  \fn int buoy_parse_x08 ( struct buoy_chunks *b, struct bufr2tac_subset_state *s )
  \brief Parse a expanded descriptor with X = 08
  \param b pointer to a struct \ref buoy_chunks where to set the results
  \param s pointer to a struct \ref bufr2tac_subset_state where is stored needed information in sequential analysis

  It returns 0 if success, 1 if problems when processing. If a descriptor is not processed returns 0 anyway
*/
int buoy_parse_x08 ( struct buoy_chunks *b, struct bufr2tac_subset_state *s )
{
  if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
    {
      return 0;
    }

  if ( b == NULL )
    {
      return 1;
    }

  switch ( s->a->desc.y )
    {
    case 22:  // 0 08 022 . Total number
    case 23:  // 0 08 023 . First-order statistics
    case 24:  // 0 08 024 . Difference statistics
      if ( s->isq )
        {
          s->isq = 0;
        }
      else
        {
          s->isq = 1;
        }
      break;
    default:
      break;
    }
  return 0;
}

/*!
  \fn int climat_parse_x08 ( struct climat_chunks *c, struct bufr2tac_subset_state *s )
  \brief Parse a expanded descriptor with X = 08
  \param c pointer to a struct \ref climat_chunks where to set the results
  \param s pointer to a struct \ref bufr2tac_subset_state where is stored needed information in sequential analysis

  It returns 0 if success, 1 if problems when processing. If a descriptor is not processed returns 0 anyway
*/
int climat_parse_x08 ( struct climat_chunks *c, struct bufr2tac_subset_state *s )
{
  if ( c == NULL )
    {
      return 1;
    }

  switch ( s->a->desc.y )
    {
    case 20: // 0 08 020 . Total number of missing entities (days)
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          return 0;
        }
      if ( s->is_normal == 0 )
        {
          switch ( s->isq_val )
            {
              // days of missing data
            case 1: // pressure
              sprintf ( c->s1.mpmp, "%02d", s->ival );
              c->mask |= CLIMAT_SEC1;
              break;
            case 2: // temperature
              sprintf ( c->s1.mtmt, "%02d", s->ival );
              c->mask |= CLIMAT_SEC1;
              break;
            case 4: // vapour pressure
              sprintf ( c->s1.meme, "%02d", s->ival );
              c->mask |= CLIMAT_SEC1;
              break;
            case 5: // precipitation
              sprintf ( c->s1.mrmr, "%02d", s->ival );
              c->mask |= CLIMAT_SEC1;
              break;
            case 6: // sunshine duration
              sprintf ( c->s1.msms, "%02d", s->ival );
              c->mask |= CLIMAT_SEC1;
              break;
            case 7: // Maximum temperature
              if ( s->ival < 10 )
                {
                  sprintf ( c->s1.mtx, "%d", s->ival );
                }
              else
                {
                  c->s1.mtx[0] = '9';
                }
              c->mask |= CLIMAT_SEC1;
              break;
            case 8: // Minimum temperature
              if ( s->ival < 10 )
                {
                  sprintf ( c->s1.mtn, "%d", s->ival );
                }
              else
                {
                  c->s1.mtn[0] = '9';
                }
              c->mask |= CLIMAT_SEC1;
              break;
            default:
              break;
            }
        }
      else
        {
          switch ( s->isq_val )
            {
              // years of missing data
            case 1: // pressure
              sprintf ( c->s2.ypyp, "%02d", s->ival );
              c->mask |= CLIMAT_SEC2;
              break;
            case 2: // temperature
              sprintf ( c->s2.ytyt, "%02d", s->ival );
              c->mask |= CLIMAT_SEC2;
              break;
            case 3: // extreme temperature
              sprintf ( c->s2.ytxytx, "%02d", s->ival );
              c->mask |= CLIMAT_SEC2;
              break;
            case 4: // vapour pressure
              sprintf ( c->s2.yeye, "%02d", s->ival );
              c->mask |= CLIMAT_SEC2;
              break;
            case 5: // precipitation
              sprintf ( c->s2.yryr, "%02d", s->ival );
              c->mask |= CLIMAT_SEC2;
              break;
            case 6: // sunshine duration
              sprintf ( c->s2.ysys, "%02d", s->ival );
              c->mask |= CLIMAT_SEC2;
              break;
            default:
              break;
            }
        }
      break;

    case 22:
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          return 0;
        }
      switch ( s->isq_val )
        {
        case 0:
          sprintf ( c->s3.f10, "%02d", s->ival );
          c->mask |= CLIMAT_SEC3;
          break;
        case 1:
          sprintf ( c->s3.f20, "%02d", s->ival );
          c->mask |= CLIMAT_SEC3;
          break;
        case 2:
          sprintf ( c->s3.f30, "%02d", s->ival );
          c->mask |= CLIMAT_SEC3;
          break;
        case 3:
          sprintf ( c->s3.Tx0, "%02d", s->ival );
          c->mask |= CLIMAT_SEC3;
          break;
        case 4:
          sprintf ( c->s3.T25, "%02d", s->ival );
          c->mask |= CLIMAT_SEC3;
          break;
        case 5:
          sprintf ( c->s3.T30, "%02d", s->ival );
          c->mask |= CLIMAT_SEC3;
          break;
        case 6:
          sprintf ( c->s3.T35, "%02d", s->ival );
          c->mask |= CLIMAT_SEC3;
          break;
        case 7:
          sprintf ( c->s3.T40, "%02d", s->ival );
          c->mask |= CLIMAT_SEC3;
          break;
        case 8:
          sprintf ( c->s3.Tn0, "%02d", s->ival );
          c->mask |= CLIMAT_SEC3;
          break;
        case 10:
          sprintf ( c->s3.R01, "%02d", s->ival );
          c->mask |= CLIMAT_SEC3;
          break;
        case 11:
          sprintf ( c->s3.R05, "%02d", s->ival );
          c->mask |= CLIMAT_SEC3;
          break;
        case 12:
          sprintf ( c->s3.R10, "%02d", s->ival );
          c->mask |= CLIMAT_SEC3;
          break;
        case 13:
          sprintf ( c->s3.R50, "%02d", s->ival );
          c->mask |= CLIMAT_SEC3;
          break;
        case 14:
          sprintf ( c->s3.R100, "%02d", s->ival );
          c->mask |= CLIMAT_SEC3;
          break;
        case 15:
          sprintf ( c->s3.R150, "%02d", s->ival );
          c->mask |= CLIMAT_SEC3;
          break;
        case 16:
          sprintf ( c->s3.s00, "%02d", s->ival );
          c->mask |= CLIMAT_SEC3;
          break;
        case 17:
          sprintf ( c->s3.s01, "%02d", s->ival );
          c->mask |= CLIMAT_SEC3;
          break;
        case 18:
          sprintf ( c->s3.s10, "%02d", s->ival );
          c->mask |= CLIMAT_SEC3;
          break;
        case 19:
          sprintf ( c->s3.s50, "%02d", s->ival );
          c->mask |= CLIMAT_SEC3;
          break;
        case 20:
          sprintf ( c->s3.V1, "%02d", s->ival );
          c->mask |= CLIMAT_SEC3;
          break;
        case 21:
          sprintf ( c->s3.V2, "%02d", s->ival );
          c->mask |= CLIMAT_SEC3;
          break;
        case 22:
          sprintf ( c->s3.V3, "%02d", s->ival );
          c->mask |= CLIMAT_SEC3;
          break;
        case 23:
          sprintf ( c->s4.Dgr, "%02d", s->ival );
          c->mask |= CLIMAT_SEC4;
          break;
        case 24:
          sprintf ( c->s4.Dts, "%02d", s->ival );
          c->mask |= CLIMAT_SEC4;
          break;
        default:
          break;
        }
      break;
    case 23: // 0 08 023 . First-order statistics.
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          s->isq = 0;
          s->isq_val = 0;
        }
      else
        {
          s->isq = 1;
          s->isq_val = s->ival;
        }
      break;

    case 50: // 0 08 050 . Qualifier for number of missing values in calculation of statistic
    case 52: // 0 08 052 . Conditions for which number of days of occurrence follows
    case 53: // 0 08 053 . Day of occurrence qualifier (= 0; on 1 day only) (= 1; on 2 or more days)
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          s->isq = 0;
          s->isq_val = -1;
          return 0;
        }
      s->isq = 1;
      s->isq_val = s->ival;
      break;

    default:
      break;
    }

  return 0;
}

/*!
  \fn int temp_parse_x08 ( struct temp_chunks *t, struct bufr2tac_subset_state *s )
  \brief Parse a expanded descriptor with X = 08
  \param t pointer to a struct \ref temp_chunks where to set the results
  \param s pointer to a struct \ref bufr2tac_subset_state where is stored needed information in sequential analysis

  It returns 0 if success, 1 if problems when processing. If a descriptor is not processed returns 0 anyway
*/
int temp_parse_x08 ( struct temp_chunks *t, struct bufr2tac_subset_state *s )
{
  if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
    {
      return 0;
    }

  if ( t == NULL )
    {
      return 1;
    }

  switch ( s->a->desc.y )
    {
    case 2: // 0 08 002.
      // this is to mark of the begining or end of cloud surface data
      if ( s->isq == 0 )
        {
          s->isq = 1;
        }
      else
        {
          s->isq = 0;
        }
      break;
    case 21: // 0 08 021. Time significance
      if ( s->ival != 18 )
        {
          return 1;  // it should be 18 (launch date/time)
        }
      break;

    case 42: // 0 08 042. Extended vertical sounding significance
      if ( s->rep > 0 && s->r->n > 0 )
        {
          s->r->raw[s->r->n - 1].flags = s->ival;
        }
      else if ( s->w->n > 0 )
        {
          // wind shear case
          s->w->raw[s->w->n - 1].flags = s->ival;
        }
      break;
    default:
      break;
    }
  return 0;
}
