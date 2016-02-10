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
 \file bufr2syn_x04.c
 \brief decodes the descriptors with X = 04 (date and time)
 */
#include "bufr2syn.h"

/*!
  \fn int syn_parse_x04 ( struct synop_chunks *syn, struct bufr_subset_state *s )
  \brief Parse a expanded descriptor with X = 04
  \param syn pointer to a struct \ref synop_chunks where to set the results
  \param s pointer to a struct \ref bufr_subset_state where is stored needed information in sequential analysis

  It returns 0 if success, 1 if problems when processing. If a descriptor is not processed returns 0 anyway
*/
int syn_parse_x04 ( struct synop_chunks *syn, struct bufr_subset_state *s )
{
  switch ( s->a->desc.y )
    {
    case 1: // 0 04 001 .Year
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          return 0;
        }
      sprintf ( syn->e.YYYY, "%04d", s->ival );
      s->mask |= SUBSET_MASK_HAVE_YEAR;
      break;
    case 2: // 0 04 002 . Month
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          return 0;
        }
      sprintf ( syn->e.MM, "%02d", s->ival );
      s->mask |= SUBSET_MASK_HAVE_MONTH;
      break;
    case 3: // 0 04 003 . Day of month
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          return 0;
        }
      sprintf ( syn->e.DD, "%02d", s->ival );
      s->mask |= SUBSET_MASK_HAVE_DAY;
      //sprintf(syn->s0.YY, "%02d", (int) sq->sequence[is].val);
      break;
    case 4: // 0 04 004 . Hour
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          return 0;
        }
      sprintf ( syn->e.HH, "%02d", s->ival );
      s->mask |= SUBSET_MASK_HAVE_HOUR;
      //sprintf(syn->s0.GG, "%02d", (int) sq->sequence[is].val);
      break;
    case 5: // 0 04 005 . Minute
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          return 0;
        }
      sprintf ( syn->e.mm, "%02d", s->ival );
      s->mask |= SUBSET_MASK_HAVE_MINUTE;
      break;
      // store latest displacement in seconds
    case 23: // 0 04 023 . Time period of displacement (days)
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          s->k_jtval = s->k_itval;
          s->jtval = s->itval;
          s->k_itval = s->i;
          s->itval = 0;
          return 0;
        }
      s->k_jtval = s->k_itval;
      s->jtval = s->itval;
      s->k_itval = s->i;
      s->itval = s->ival * 86400;
      break;
    case 24: // 0 04 024 .  Time period of displacement (hours)
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          s->k_jtval = s->k_itval;
          s->jtval = s->itval;
          s->k_itval = s->i;
          s->itval = 0;
          return 0;
        }
      s->k_jtval = s->k_itval;
      s->jtval = s->itval;
      s->k_itval = s->i;
      s->itval = s->ival * 3600;
      break;
    case 25: // 0 04 025  Time period of displacement (minutes)
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          s->k_jtval = s->k_itval;
          s->jtval = s->itval;
          s->k_itval = s->i;
          s->itval = 0;
          return 0;
        }
      s->k_jtval = s->k_itval;
      s->jtval = s->itval;
      s->k_itval = s->i;
      s->itval = s->ival * 60;
      break;
    case 26: // 0 04 026 .  Time period of displacement (seconds)
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          s->k_jtval = s->k_itval;
          s->jtval = s->itval;
          s->k_itval = s->i;
          s->itval = 0;
          return 0;
        }
      s->k_jtval = s->k_itval;
      s->jtval = s->itval;
      s->k_itval = s->i;
      s->itval = s->ival;
      break;
    default:
      break;
    }
  return 0;
}

/*!
  \fn int buoy_parse_x04 ( struct buoy_chunks *b, struct bufr_subset_state *s )
  \brief Parse a expanded descriptor with X = 04
  \param b pointer to a struct \ref buoy_chunks where to set the results
  \param s pointer to a struct \ref bufr_subset_state where is stored needed information in sequential analysis

  It returns 0 if success, 1 if problems when processing. If a descriptor is not processed returns 0 anyway
*/
int buoy_parse_x04 ( struct buoy_chunks *b, struct bufr_subset_state *s )
{

  switch ( s->a->desc.y )
    {
    case 1: // 0 04 001 . Year
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          return 0;
        }
      if ( b->e.YYYY[0] == 0 )
        {
          sprintf ( b->e.YYYY, "%04d", s->ival );
        }
      s->mask |= SUBSET_MASK_HAVE_YEAR;
      break;
    case 2: // 0 04 002 . Month
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          return 0;
        }
      if ( b->e.MM[0] == 0 )
        {
          sprintf ( b->e.MM, "%02d", s->ival );
        }
      s->mask |= SUBSET_MASK_HAVE_MONTH;
      break;
    case 3: // 0 04 003 . Day of month
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          return 0;
        }
      if ( b->e.DD[0] == 0 )
        {
          sprintf ( b->e.DD, "%02d", s->ival );
        }
      s->mask |= SUBSET_MASK_HAVE_DAY;
      //sprintf(b->s0.YY, "%02d", (int) sq->sequence[is].val);
      break;
    case 4: // 0 04 004 . Hour
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          return 0;
        }
      if ( b->e.HH[0] == 0 )
        {
          sprintf ( b->e.HH, "%02d", s->ival );
        }
      s->mask |= SUBSET_MASK_HAVE_HOUR;
      //sprintf(b->s0.GG, "%02d", (int) sq->sequence[is].val);
      break;
    case 5: // 0 04 005 . Minute
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          return 0;
        }
      if ( b->e.mm[0] == 0 )
        {
          sprintf ( b->e.mm, "%02d", s->ival );
        }
      s->mask |= SUBSET_MASK_HAVE_MINUTE;
      break;
      // store latest displacement in seconds
    case 23: // 0 04 023 . Time period of displacement (days)
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          s->k_jtval = s->k_itval;
          s->jtval = s->itval;
          s->k_itval = s->i;
          s->itval = 0;
          return 0;
        }
      s->k_jtval = s->k_itval;
      s->jtval = s->itval;
      s->k_itval = s->i;
      s->itval = s->ival * 86400;
      break;
    case 24: // 0 04 024 . Time period of displacement (hours)
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          s->k_jtval = s->k_itval;
          s->jtval = s->itval;
          s->k_itval = s->i;
          s->itval = 0;
          return 0;
        }
      s->k_jtval = s->k_itval;
      s->jtval = s->itval;
      s->k_itval = s->i;
      s->itval = s->ival * 3600;
      break;
    case 25: // 0 04 025 . Time period of displacement (minutes)
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          s->k_jtval = s->k_itval;
          s->jtval = s->itval;
          s->k_itval = s->i;
          s->itval = 0;
          return 0;
        }
      s->k_jtval = s->k_itval;
      s->jtval = s->itval;
      s->k_itval = s->i;
      s->itval = s->ival * 60;
      break;
    case 26: // 0 04 026 .  Time period of displacement (seconds)
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          s->k_jtval = s->k_itval;
          s->jtval = s->itval;
          s->k_itval = s->i;
          s->itval = 0;
          return 0;
        }
      s->k_jtval = s->k_itval;
      s->jtval = s->itval;
      s->k_itval = s->i;
      s->itval = s->ival;
      break;
    default:
      break;
    }
  return 0;
}

/*!
  \fn int climat_parse_x04 ( struct climat_chunks *c, struct bufr_subset_state *s )
  \brief Parse a expanded descriptor with X = 04
  \param c pointer to a struct \ref climat_chunks where to set the results
  \param s pointer to a struct \ref bufr_subset_state where is stored needed information in sequential analysis

  It returns 0 if success, 1 if problems when processing. If a descriptor is not processed returns 0 anyway
*/
int climat_parse_x04 ( struct climat_chunks *c, struct bufr_subset_state *s )
{
  if ( c == NULL )
    {
      return 1;
    }


  switch ( s->a->desc.y )
    {
    case 1: // 0 04 001 .Year
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          return 0;
        }
      if ( s->i && ( s->a1->desc.x == 4 ) && ( s->a1->desc.y == 1 ) )
        {
          // this is the final year of a normal period
          s->is_normal = 1;
          sprintf ( c->s2.YcYc, "%02d", s->ival % 100 );
          // then we fill the begin from tatest descriptor
          sprintf ( c->s2.YbYb, "%02d", ( int ) s->a1->val % 100 );
        }
      else if ( c->e.YYYY[0] == '\0' )
        {
          sprintf ( c->e.YYYY, "%04d", s->ival );
          s->mask |= SUBSET_MASK_HAVE_YEAR;
          sprintf ( c->s0.JJJ, "%03d", s->ival % 1000 );
          c->mask |= CLIMAT_SEC0;
        }
      break;

    case 2: // 0 04 002 . Month
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          return 0;
        }
      sprintf ( c->e.MM, "%02d", s->ival );
      s->mask |= SUBSET_MASK_HAVE_MONTH;
      sprintf ( c->s0.MM, "%02d", s->ival );
      c->mask |= CLIMAT_SEC0;
      break;

    case 3: // 0 04 003 . Day of month
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          return 0;
        }
      if ( c->e.DD[0] == 0 )
        {
          sprintf ( c->e.DD, "%02d", s->ival );
        }
      if ( s->a1->desc.x == 8 && s->a1->desc.y == 53 )
        {
          s->more_days = s->isq_val;
        }
      s->day = s->ival;
      s->mask |= SUBSET_MASK_HAVE_DAY;
      break;

    case 4: // 0 04 004 . Hour
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          return 0;
        }
      if ( c->e.HH[0] == 0 )
        {
          sprintf ( c->e.HH, "%02d", s->ival );
        }
      s->mask |= SUBSET_MASK_HAVE_HOUR;
      break;

    case 5: // 0 04 005 . Minute
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          return 0;
        }
      if ( c->e.mm[0] == 0 )
        {
          sprintf ( c->e.mm, "%02d", s->ival );
        }
      s->mask |= SUBSET_MASK_HAVE_MINUTE;
      break;

    case 22: // 0 04 022 . Num month. Case of sec2 with normals
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          return 0;
        }
      s->month = s->ival;
      break;

    case 23: // 0 04 023 . Num days in month
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          return 0;
        }
      if ( s->nday == 0 )
        {
          s->nday = s->ival;
        }
      break;

    case 51: // 0 04 051 . Principal time of daily reading of maximum
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          return 0;
        }
      sprintf ( c->s4.GxGx, "%02d", s->ival );
      s->mask |= CLIMAT_SEC4;
      break;

    case 52: // 0 04 052 . Principal time of daily reading of minimum
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          return 0;
        }
      sprintf ( c->s4.GnGn, "%02d", s->ival );
      s->mask |= CLIMAT_SEC4;
      break;

    case 53: // 0 04 053 . Number of days with precipitation equal to or more than 1 mm
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          return 0;
        }
      if (s->is_normal == 0)
      {
	sprintf ( c->s1.nrnr, "%02d", s->ival );
        s->mask |= CLIMAT_SEC1;
      }
      else
      {
	sprintf ( c->s2.nrnr, "%02d", s->ival );
        s->mask |= CLIMAT_SEC2;
      }
      break;


    case 74: // 0 04 074 . (UTC - LST)
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          return 0;
        }
      s->dift = s->ival;
      break;


    default:
      break;
    }
  return 0;
}

/*!
  \fn int temp_parse_x04 ( struct temp_chunks *t, struct bufr_subset_state *s )
  \brief Parse a expanded descriptor with X = 04
  \param t pointer to a struct \ref temp_chunks where to set the results
  \param s pointer to a struct \ref bufr_subset_state where is stored needed information in sequential analysis

  It returns 0 if success, 1 if problems when processing. If a descriptor is not processed returns 0 anyway
*/
int temp_parse_x04 ( struct temp_chunks *t, struct bufr_subset_state *s )
{
    switch ( s->a->desc.y )
    {
    case 1: // 0 04 001 . Year
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          return 0;
        }
      if ( t->a.e.YYYY[0] == 0 )
        {
          sprintf ( t->a.e.YYYY, "%04d", s->ival );
          sprintf ( t->b.e.YYYY, "%04d", s->ival );
          sprintf ( t->c.e.YYYY, "%04d", s->ival );
          sprintf ( t->d.e.YYYY, "%04d", s->ival );
        }
      s->mask |= SUBSET_MASK_HAVE_YEAR;
      break;
    case 2: // 0 04 002 . Month
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          return 0;
        }
      if ( t->a.e.MM[0] == 0 )
        {
          sprintf ( t->a.e.MM, "%02d", s->ival );
          sprintf ( t->b.e.MM, "%02d", s->ival );
          sprintf ( t->c.e.MM, "%02d", s->ival );
          sprintf ( t->d.e.MM, "%02d", s->ival );
        }
      s->mask |= SUBSET_MASK_HAVE_MONTH;
      break;
    case 3: // 0 04 003 . Day of month
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          return 0;
        }
      if ( t->a.e.DD[0] == 0 )
        {
          sprintf ( t->a.e.DD, "%02d", s->ival );
          sprintf ( t->b.e.DD, "%02d", s->ival );
          sprintf ( t->c.e.DD, "%02d", s->ival );
          sprintf ( t->d.e.DD, "%02d", s->ival );
        }
      s->mask |= SUBSET_MASK_HAVE_DAY;
      //sprintf(t->s0.YY, "%02d", (int) sq->sequence[is].val);
      break;
    case 4: // 0 04 004 . Hour
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          return 0;
        }
      if ( t->a.e.HH[0] == 0 )
        {
          sprintf ( t->a.e.HH, "%02d", s->ival );
          sprintf ( t->b.e.HH, "%02d", s->ival );
          sprintf ( t->c.e.HH, "%02d", s->ival );
          sprintf ( t->d.e.HH, "%02d", s->ival );
        }
      s->mask |= SUBSET_MASK_HAVE_HOUR;
      //sprintf(t->s0.GG, "%02d", (int) sq->sequence[is].val);
      break;
    case 5: // 0 04 005 . Minute
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          return 0;
        }
      if ( t->a.e.mm[0] == 0 )
        {
          sprintf ( t->a.e.mm, "%02d", s->ival );
          sprintf ( t->b.e.mm, "%02d", s->ival );
          sprintf ( t->c.e.mm, "%02d", s->ival );
          sprintf ( t->d.e.mm, "%02d", s->ival );
        }
      s->mask |= SUBSET_MASK_HAVE_MINUTE;
      break;
    case 6: // 0 04 006 . Second
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          return 0;
        }
      if ( t->a.e.ss[0] == 0 )
        {
          sprintf ( t->a.e.ss, "%02d", s->ival );
          sprintf ( t->b.e.ss, "%02d", s->ival );
          sprintf ( t->c.e.ss, "%02d", s->ival );
          sprintf ( t->d.e.ss, "%02d", s->ival );
        }
      s->mask |= SUBSET_MASK_HAVE_SECOND;
      break;
      
    case 86: // 0 04 086 . Long time period or displacement (since launch time)
      if (s->rep > 0)
      { // case of Temperature, humidty ... point
	if (s->r->n < s->rep)
	   s->r->n += 1;
	s->r->raw[s->r->n - 1].dt = s->ival; 
      }
      else
      { // case of wind shear point
	if (s->w->n < s->itval) 
	  s->w->n += 1;
	s->w->raw[s->w->n - 1].dt = s->ival; 
      }
      break;
    default:
      break;
    }
  return 0;
}