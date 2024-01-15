/***************************************************************************
 *   Copyright (C) 2013-2024 by Guillermo Ballester Valor                  *
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
 \file bufr2tac_x04.c
 \brief decodes the descriptors with X = 04 (date and time)
 */
#include "bufr2tac.h"

/*!
  \fn int time_period_duration (struct bufr2tac_subset_state *s )
  \brief Get time period duration in seconds
  \param s pinter to struct \ref bufr2tac_subset_state

  Returns -1 if no duration is computed. Otherwise returns time duration in seconds
*/
int time_period_duration ( const struct bufr2tac_subset_state *s )
{
  if ( s->k_itval == ( s->k_jtval + 1 ) )
    {
      // two consecutive time period displacements
      if ( ( s->itmask & DESCRIPTOR_VALUE_MISSING ) == 0  &&
           ( s->jtmask & DESCRIPTOR_VALUE_MISSING ) == 0 )
        {
          return ( abs ( s->itval - s->jtval ) );
        }
      else if ( ( s->itmask & DESCRIPTOR_VALUE_MISSING )  &&
                ( s->jtmask & DESCRIPTOR_VALUE_MISSING ) )
        {
          return 0;
        }
      else if ( ( s->itmask & DESCRIPTOR_VALUE_MISSING ) == 0 &&
                ( s->jtmask & DESCRIPTOR_VALUE_MISSING ) )
        {
          return ( abs ( s->itval ) );
        }
      else if ( ( s->jtmask & DESCRIPTOR_VALUE_MISSING ) == 0 &&
                ( s->itmask & DESCRIPTOR_VALUE_MISSING ) )
        {
          return -1;
        }
    }

  // just a single time period displacement
  if ( ( s->itmask & DESCRIPTOR_VALUE_MISSING ) == 0 )
    {
      return ( abs ( s->itval ) );
    }
  else
    {
      return 0;
    }
}

/*!
  \fn int hour_rounded(const struct synop_chunks *syn)
  \brief Get the rounded hour of a given date
  \param syn pointer to the \ref synop_chunks struct

  It returns the rounded hour if >= 0. If < 0 problems
*/
int hour_rounded ( const struct synop_chunks *syn )
{
  time_t t;
  struct tm tim;
  char aux[32];

  snprintf ( aux, sizeof (aux), "%s%s%s%s%s", syn->e.YYYY,syn->e.MM,syn->e.DD,syn->e.HH, syn->e.mm );
  memset ( &tim, 0, sizeof ( struct tm ) );
  if ( strlen ( aux ) == 12 )
    {
      strptime ( aux, "%Y%m%d%H%M", &tim );
      t = mktime ( &tim );
      t += 1800;
      gmtime_r ( &t, &tim );
      return tim.tm_hour;
    }
  else if ( syn->e.mm[0] == 0 && syn->e.HH[0] )
    {
      return atoi ( syn->e.HH );
    }
  else
    return -1;
}

/*!
  \fn int syn_parse_x04 ( struct synop_chunks *syn, struct bufr2tac_subset_state *s )
  \brief Parse a expanded descriptor with X = 04
  \param syn pointer to a struct \ref synop_chunks where to set the results
  \param s pointer to a struct \ref bufr2tac_subset_state where is stored needed information in sequential analysis

  It returns 0 if success, 1 if problems when processing. If a descriptor is not processed returns 0 anyway
*/
int syn_parse_x04 ( struct synop_chunks *syn, struct bufr2tac_subset_state *s )
{
  switch ( s->a->desc.y )
    {
    case 1: // 0 04 001 .Year
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          return 0;
        }
      else if ( s->ival < 2000 || s->ival > 2050 )
        {
          if ( BUFR2TAC_DEBUG_LEVEL > 0 )
            bufr2tac_set_error ( s, 2, "syn_parse_x04()", "Bad year" );
          return 1;
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
      break;

    case 4: // 0 04 004 . Hour
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          return 0;
        }
      sprintf ( syn->e.HH, "%02d", s->ival );
      s->mask |= SUBSET_MASK_HAVE_HOUR;
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
          s->jtmask = s->itmask;
          s->k_itval = s->i;
          s->itval = 0;
          return 0;
        }
      s->k_jtval = s->k_itval;
      s->jtval = s->itval;
      s->jtmask = s->itmask;
      s->k_itval = s->i;
      s->itmask = s->a->mask;
      s->itval = s->ival * 86400;
      break;

    case 24: // 0 04 024 .  Time period of displacement (hours)
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          s->k_jtval = s->k_itval;
          s->jtval = s->itval;
          s->jtmask = s->itmask;
          s->k_itval = s->i;
          s->itval = 0;
          return 0;
        }
      s->k_jtval = s->k_itval;
      s->jtval = s->itval;
      s->jtmask = s->itmask;
      s->k_itval = s->i;
      s->itmask = s->a->mask;
      s->itval = s->ival * 3600;
      break;

    case 25: // 0 04 025  Time period of displacement (minutes)
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          s->k_jtval = s->k_itval;
          s->jtval = s->itval;
          s->jtmask = s->itmask;
          s->k_itval = s->i;
          s->itval = 0;
          return 0;
        }
      s->k_jtval = s->k_itval;
      s->jtval = s->itval;
      s->jtmask = s->itmask;
      s->k_itval = s->i;
      s->itmask = s->a->mask;
      s->itval = s->ival * 60;
      break;

    case 26: // 0 04 026 .  Time period of displacement (seconds)
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          s->k_jtval = s->k_itval;
          s->jtval = s->itval;
          s->jtmask = s->itmask;
          s->k_itval = s->i;
          s->itval = 0;
          return 0;
        }
      s->k_jtval = s->k_itval;
      s->jtval = s->itval;
      s->jtmask = s->itmask;
      s->k_itval = s->i;
      s->itmask = s->a->mask;
      s->itval = s->ival;
      break;

    default:
      if ( BUFR2TAC_DEBUG_LEVEL > 1 && ( s->a->mask & DESCRIPTOR_VALUE_MISSING ) == 0 )
        bufr2tac_set_error ( s, 0, "syn_parse_x04()", "Descriptor not parsed" );
      break;
    }
  return 0;
}

/*!
  \fn int buoy_parse_x04 ( struct buoy_chunks *b, struct bufr2tac_subset_state *s )
  \brief Parse a expanded descriptor with X = 04
  \param b pointer to a struct \ref buoy_chunks where to set the results
  \param s pointer to a struct \ref bufr2tac_subset_state where is stored needed information in sequential analysis

  It returns 0 if success, 1 if problems when processing. If a descriptor is not processed returns 0 anyway
*/
int buoy_parse_x04 ( struct buoy_chunks *b, struct bufr2tac_subset_state *s )
{

  switch ( s->a->desc.y )
    {
    case 1: // 0 04 001 . Year
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          return 0;
        }
      else if ( s->ival < 2000 || s->ival > 2050 )
        {
          if ( BUFR2TAC_DEBUG_LEVEL > 0 )
            bufr2tac_set_error ( s, 2, "buoy_parse_x04()", "Bad year" );
          return 1;
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
      if ( BUFR2TAC_DEBUG_LEVEL > 1 && ( s->a->mask & DESCRIPTOR_VALUE_MISSING ) == 0 )
        bufr2tac_set_error ( s, 0, "buoy_parse_x04()", "Descriptor not parsed" );
      break;
    }
  return 0;
}

/*!
  \fn int climat_parse_x04 ( struct climat_chunks *c, struct bufr2tac_subset_state *s )
  \brief Parse a expanded descriptor with X = 04
  \param c pointer to a struct \ref climat_chunks where to set the results
  \param s pointer to a struct \ref bufr2tac_subset_state where is stored needed information in sequential analysis

  It returns 0 if success, 1 if problems when processing. If a descriptor is not processed returns 0 anyway
*/
int climat_parse_x04 ( struct climat_chunks *c, struct bufr2tac_subset_state *s )
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
      else if ( s->ival < 2000 || s->ival > 2050 )
        {
          if ( BUFR2TAC_DEBUG_LEVEL > 0 )
            bufr2tac_set_error ( s, 2, "climat_parse_x04()", "Bad year" );
          return 1;
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
          sprintf ( c->s0.JJJ, "%03d", abs ( s->ival ) % 1000 );
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
      if ( s->is_normal == 0 )
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
      if ( BUFR2TAC_DEBUG_LEVEL > 1 && ( s->a->mask & DESCRIPTOR_VALUE_MISSING ) == 0 )
        bufr2tac_set_error ( s, 0, "climat_parse_x04()", "Descriptor not parsed" );
      break;
    }
  return 0;
}

/*!
  \fn int temp_parse_x04 ( struct temp_chunks *t, struct bufr2tac_subset_state *s )
  \brief Parse a expanded descriptor with X = 04
  \param t pointer to a struct \ref temp_chunks where to set the results
  \param s pointer to a struct \ref bufr2tac_subset_state where is stored needed information in sequential analysis

  It returns 0 if success, 1 if problems when processing. If a descriptor is not processed returns 0 anyway
*/
int temp_parse_x04 ( struct temp_chunks *t, struct bufr2tac_subset_state *s )
{
  switch ( s->a->desc.y )
    {
    case 1: // 0 04 001 . Year
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          return 0;
        }
      else if ( s->ival < 2000 || s->ival > 2050 )
        {
          if ( BUFR2TAC_DEBUG_LEVEL > 0 )
            bufr2tac_set_error ( s, 2, "temp_parse_x04()", "Bad year" );
          return 1;
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
          sprintf ( t->a.s7.GG, "%02d", s->ival );
          sprintf ( t->b.s7.GG, "%02d", s->ival );
          sprintf ( t->c.s7.GG, "%02d", s->ival );
          sprintf ( t->d.s7.GG, "%02d", s->ival );
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
          sprintf ( t->a.s7.gg, "%02d", s->ival );
          sprintf ( t->b.s7.gg, "%02d", s->ival );
          sprintf ( t->c.s7.gg, "%02d", s->ival );
          sprintf ( t->d.s7.gg, "%02d", s->ival );
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

      // s->rep is used to check what type of point we are dealing from
      // see repliator 0 31 001 and 0 31 002 to understand it better
      // Because this is the first descriptor in a sequence for a point, we use this
      // descriptor to set properly the current index
      if ( s->rep > 0 )
        {
          // case of Temperature, humidty ... point
          if ( ( int ) s->r->n < s->rep )
            {
              // Because of there are bufr reports with a lot of not significant points
              // we only collect the first one and significant points, i.e. those that flags are non zero
              // When flags are zero we just overwrite points
              if ( s->r->n < ( RAW_TEMP_NMAX_POINTS ) &&
                   ( s->r->n == 0 || s->r->raw[s->r->n - 1].flags ) )
                {
                  s->r->n += 1; // Here we update the index
                }
            }
          s->r->raw[s->r->n - 1].dt = s->ival;
        }
      else
        {
          // case of wind shear point
          if ( ( int ) s->w->n < s->itval )
            {
              s->w->n += 1;
            }
          s->w->raw[s->w->n - 1].dt = s->ival;
        }
      break;

    default:
      if ( BUFR2TAC_DEBUG_LEVEL > 1 && ( s->a->mask & DESCRIPTOR_VALUE_MISSING ) == 0 )
        bufr2tac_set_error ( s, 0, "temp_parse_x04()", "Descriptor not parsed" );
      break;
    }
  return 0;
}
