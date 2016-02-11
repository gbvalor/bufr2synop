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
 \file bufr2syn_x11.c
 \brief decodes the descriptors with X = 11 (wind)
 */
#include "bufr2syn.h"

/*!
  \fn char * secs_to_tt(char *tt, int secs)
  \brief get tt code from seconds
  \param tt string with the resulting table code
  \param secs second

  returns the pointer to \a tt
*/
char * secs_to_tt ( char *tt, int secs )
{
  int i;

  i = ( -secs ) / 360;
  if ( i <= 60 )
    {
      sprintf ( tt, "%02d", i );
    }
  else if ( i < 700 )
    {
      sprintf ( tt, "61" );
    }
  else if ( i < 800 )
    {
      sprintf ( tt, "62" );
    }
  else if ( i < 900 )
    {
      sprintf ( tt, "63" );
    }
  else if ( i < 1000 )
    {
      sprintf ( tt, "64" );
    }
  else if ( i < 1100 )
    {
      sprintf ( tt, "65" );
    }
  else if ( i < 1200 )
    {
      sprintf ( tt, "66" );
    }
  else if ( i < 1800 )
    {
      sprintf ( tt, "67" );
    }
  else
    {
      sprintf ( tt, "68" );
    }
  return tt;
}

/*!
  \fn int syn_parse_x11 ( struct synop_chunks *syn, struct bufr_subset_state *s )
  \brief Parse a expanded descriptor with X = 11
  \param syn pointer to a struct \ref synop_chunks where to set the results
  \param s pointer to a struct \ref bufr_subset_state where is stored needed information in sequential analysis

  It returns 0 if success, 1 if problems when processing. If a descriptor is not processed returns 0 anyway
*/
int syn_parse_x11 ( struct synop_chunks *syn, struct bufr_subset_state *s )
{

  if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
    {
      return 0;
    }

  switch ( s->a->desc.y )
    {
    case 1: // 0 11 001 . Wind direction
    case 11: // 0 11 011 . Wind direction at 10 meters
      sprintf ( syn->s1.dd, "%02d", ( s->ival + 5 ) / 10 );
      syn->mask |= SYNOP_SEC1;
      break;
    case 2: // 0 11 002 . Wind speed
    case 12: // 0 11 012 . Wind speed at 10 meters
      if ( syn->s0.iw[0] == '4' )
        {
          s->val *= 1.94384449;
        }
      if ( s->val < 100.0 )
        {
          sprintf ( syn->s1.ff, "%02d", ( int ) ( s->val + 0.5 ) );
        }
      else
        {
          sprintf ( syn->s1.ff,"99" );
          sprintf ( syn->s1.fff, "%03d", ( int ) ( s->val + 0.5 ) );
        }
      syn->mask |= SYNOP_SEC1;
      break;
    case 41: // 0 11 041 . Max wind gust speed
      if ( syn->s3.d9.n == SYNOP_NMISC )
        {
          return 0;
        }
      if ( s->itval == -600 )
        {
          sprintf ( syn->s3.d9.misc[syn->s3.d9.n].SpSp, "910" );
          if ( syn->s0.iw[0] == '4' )
            {
              s->val *= 1.94384449;
            }
          if ( s->val < 100.0 )
            {
              sprintf ( syn->s3.d9.misc[syn->s3.d9.n].spsp, "%02d", ( int ) ( s->val + 0.5 ) );
            }
          else
            {
              sprintf ( syn->s3.d9.misc[syn->s3.d9.n].spsp,"99" );
              syn->s3.d9.n++;
              sprintf ( syn->s3.d9.misc[syn->s3.d9.n].SpSp, "00" );
              sprintf ( syn->s3.d9.misc[syn->s3.d9.n].spsp, "%03d", ( int ) ( s->val + 0.5 ) );
            }
          syn->s3.d9.n++;
        }
      else if ( s->itval )
        {
          if ( s->mask & SUBSET_MASK_HAVE_GUST )
            {
              sprintf ( syn->s5.d9.misc[syn->s5.d9.n].SpSp, "907" );
              secs_to_tt ( syn->s5.d9.misc[syn->s5.d9.n].spsp, s->itval );
              syn->s5.d9.n++;
              sprintf ( syn->s5.d9.misc[syn->s5.d9.n].SpSp, "911" );
              if ( syn->s0.iw[0] == '4' )
                {
                  s->val *= 1.94384449;
                }
              if ( s->val < 100.0 )
                {
                  sprintf ( syn->s5.d9.misc[syn->s5.d9.n].spsp, "%02d", ( int ) ( s->val + 0.5 ) );
                }
              else
                {
                  sprintf ( syn->s5.d9.misc[syn->s5.d9.n].spsp,"99" );
                  syn->s5.d9.n++;
                  sprintf ( syn->s5.d9.misc[syn->s5.d9.n].SpSp, "00" );
                  sprintf ( syn->s5.d9.misc[syn->s5.d9.n].spsp, "%03d", ( int ) ( s->val + 0.5 ) );
                }
              syn->s5.d9.n++;
              syn->mask |= SYNOP_SEC5;
            }
          else
            {
              sprintf ( syn->s3.d9.misc[syn->s3.d9.n].SpSp, "907" );
              secs_to_tt ( syn->s3.d9.misc[syn->s3.d9.n].spsp, s->itval );
              syn->s3.d9.n++;
              sprintf ( syn->s3.d9.misc[syn->s3.d9.n].SpSp, "911" );
              if ( syn->s0.iw[0] == '4' )
                {
                  s->val *= 1.94384449;
                }
              if ( s->val < 100.0 )
                {
                  sprintf ( syn->s3.d9.misc[syn->s3.d9.n].spsp, "%02d", ( int ) ( s->val + 0.5 ) );
                }
              else
                {
                  sprintf ( syn->s3.d9.misc[syn->s3.d9.n].spsp,"99" );
                  syn->s3.d9.n++;
                  sprintf ( syn->s3.d9.misc[syn->s3.d9.n].SpSp, "00" );
                  sprintf ( syn->s3.d9.misc[syn->s3.d9.n].spsp, "%03d", ( int ) ( s->val + 0.5 ) );
                }
              syn->s3.d9.n++;
              syn->mask |= SYNOP_SEC3;
              s->mask |= SUBSET_MASK_HAVE_GUST;
            }
        }
      break;
    case 43: // 0 11 043 . Max wind gust direction
      break;
    case 84: // 0 11 084 . Wind speed in knots
      if ( syn->s0.iw[0] == '1' )
        {
          s->val /= 1.94384449;
        }
      if ( s->val < 100.0 )
        {
          sprintf ( syn->s1.ff, "%02d", ( int ) ( s->val + 0.5 ) );
        }
      else
        {
          sprintf ( syn->s1.ff,"99" );
          sprintf ( syn->s1.fff, "%03d", ( int ) ( s->val + 0.5 ) );
        }
      syn->mask |= SYNOP_SEC1;
      break;
    case 86: // 0 11 086 . Max wind speed in knots
      if ( syn->s3.d9.n == SYNOP_NMISC )
        {
          return 0;
        }
      if ( s->itval == -600 )
        {
          sprintf ( syn->s3.d9.misc[syn->s3.d9.n].SpSp, "910" );
          if ( syn->s0.iw[0] == '1' )
            {
              s->val /= 1.94384449;
            }
          if ( s->val < 100.0 )
            {
              sprintf ( syn->s3.d9.misc[syn->s3.d9.n].spsp, "%02d", ( int ) ( s->val + 0.5 ) );
            }
          else
            {
              sprintf ( syn->s3.d9.misc[syn->s3.d9.n].spsp,"99" );
              syn->s3.d9.n++;
              sprintf ( syn->s3.d9.misc[syn->s3.d9.n].SpSp, "00" );
              sprintf ( syn->s3.d9.misc[syn->s3.d9.n].spsp, "%03d", ( int ) ( s->val + 0.5 ) );
            }
          syn->s3.d9.n++;
        }
      else if ( s->itval )
        {
          if ( s->mask & SUBSET_MASK_HAVE_GUST )
            {
              sprintf ( syn->s5.d9.misc[syn->s5.d9.n].SpSp, "907" );
              secs_to_tt ( syn->s5.d9.misc[syn->s5.d9.n].spsp, s->itval );
              syn->s5.d9.n++;
              sprintf ( syn->s5.d9.misc[syn->s5.d9.n].SpSp, "911" );
              if ( syn->s0.iw[0] == '1' )
                {
                  s->val /= 1.94384449;
                }
              if ( s->val < 100.0 )
                {
                  sprintf ( syn->s5.d9.misc[syn->s5.d9.n].spsp, "%02d", ( int ) ( s->val + 0.5 ) );
                }
              else
                {
                  sprintf ( syn->s5.d9.misc[syn->s5.d9.n].spsp,"99" );
                  syn->s5.d9.n++;
                  sprintf ( syn->s5.d9.misc[syn->s5.d9.n].SpSp, "00" );
                  sprintf ( syn->s5.d9.misc[syn->s5.d9.n].spsp, "%03d", ( int ) ( s->val + 0.5 ) );
                }
              syn->s5.d9.n++;
              syn->mask |= SYNOP_SEC5;
            }
          else
            {
              sprintf ( syn->s3.d9.misc[syn->s3.d9.n].SpSp, "907" );
              secs_to_tt ( syn->s3.d9.misc[syn->s3.d9.n].spsp, s->itval );
              syn->s3.d9.n++;
              sprintf ( syn->s3.d9.misc[syn->s3.d9.n].SpSp, "911" );
              if ( syn->s0.iw[0] == '1' )
                {
                  s->val /= 1.94384449;
                }
              if ( s->val < 100.0 )
                {
                  sprintf ( syn->s3.d9.misc[syn->s3.d9.n].spsp, "%02d", ( int ) ( s->val + 0.5 ) );
                }
              else
                {
                  sprintf ( syn->s3.d9.misc[syn->s3.d9.n].spsp,"99" );
                  syn->s3.d9.n++;
                  sprintf ( syn->s3.d9.misc[syn->s3.d9.n].SpSp, "00" );
                  sprintf ( syn->s3.d9.misc[syn->s3.d9.n].spsp, "%03d", ( int ) ( s->val + 0.5 ) );
                }
              syn->s3.d9.n++;
              syn->mask |= SYNOP_SEC3;
              s->mask |= SUBSET_MASK_HAVE_GUST;
            }
        }
      break;
    default:
      break;
    }
  return 0;
}

/*!
  \fn int buoy_parse_x11 ( struct buoy_chunks *b, struct bufr_subset_state *s )
  \brief Parse a expanded descriptor with X = 11
  \param b pointer to a struct \ref buoy_chunks where to set the results
  \param s pointer to a struct \ref bufr_subset_state where is stored needed information in sequential analysis

  It returns 0 if success, 1 if problems when processing. If a descriptor is not processed returns 0 anyway
*/
int buoy_parse_x11 ( struct buoy_chunks *b, struct bufr_subset_state *s )
{

  if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
    {
      return 0;
    }

  switch ( s->a->desc.y )
    {
    case 1: // 0 11 001 . Wind direction
    case 11: // 0 11 011 . Wind direction at 10m
      sprintf ( b->s1.dd, "%02d", ( s->ival + 5 ) / 10 );
      b->mask |= BUOY_SEC1;
      break;
    case 2: // 0 11 002 . Wind speed
    case 12: // 0 11 012 . Wind speed at 10 meters
      if ( b->s0.iw[0] == '4' )
        {
          s->val *= 1.94384449;
        }
      if ( s->val < 100.0 )
        {
          sprintf ( b->s1.ff, "%02d", ( int ) ( s->val + 0.5 ) );
        }
      else
        {
          sprintf ( b->s1.ff,"99" );
          sprintf ( b->s1.fff, "%03d", ( int ) ( s->val + 0.5 ) );
        }
      b->mask |= BUOY_SEC1;
      break;
    case 84: // 0 11 084  (wind in knots)
      if ( b->s0.iw[0] == '1' )
        {
          s->val /= 1.94384449;
        }
      if ( s->val < 100.0 )
        {
          sprintf ( b->s1.ff, "%02d", ( int ) ( s->val + 0.5 ) );
        }
      else
        {
          sprintf ( b->s1.ff,"99" );
          sprintf ( b->s1.fff, "%03d", ( int ) ( s->val + 0.5 ) );
        }
      b->mask |= BUOY_SEC1;
      break;

    default:
      break;
    }
  return 0;
}

/*!
  \fn int climat_parse_x11 ( struct climat_chunks *b, struct bufr_subset_state *s )
  \brief Parse a expanded descriptor with X = 11
  \param b pointer to a struct \ref climat_chunks where to set the results
  \param s pointer to a struct \ref bufr_subset_state where is stored needed information in sequential analysis

  It returns 0 if success, 1 if problems when processing. If a descriptor is not processed returns 0 anyway
*/
int climat_parse_x11 ( struct climat_chunks *c, struct bufr_subset_state *s )
{

  if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
    {
      return 0;
    }

  switch ( s->a->desc.y )
    {
    case 46: // 0 11 046 . Maximum wind speed
      if ( c->s4.iw[0] == '4' )
        {
          sprintf ( c->s4.fxfxfx, "%03d", ( int ) ( s->val * 10.0 * 1.94384449 + 0.5 ) );
        }
      else
        {
          sprintf ( c->s4.fxfxfx, "%03d", ( int ) ( s->val * 10.0 + 0.5 ) );
        }
      if ( s->isq_val == 0 )
        {
          sprintf ( c->s4.yfx, "%02d", s->day );
        }
      else
        {
          sprintf ( c->s4.yfx, "%02d", s->day + 50 );
        }
      c->mask |= CLIMAT_SEC4;
      break;
    default:
      break;
    }
  return 0;
}

/*!
  \fn int temp_parse_x11 ( struct temp_chunks *b, struct bufr_subset_state *s )
  \brief Parse a expanded descriptor with X = 11
  \param t pointer to a struct \ref temp_chunks where to set the results
  \param s pointer to a struct \ref bufr_subset_state where is stored needed information in sequential analysis

  It returns 0 if success, 1 if problems when processing. If a descriptor is not processed returns 0 anyway
*/
int temp_parse_x11 ( struct temp_chunks *t, struct bufr_subset_state *s )
{

  if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
    {
      return 0;
    }

  if ( t == NULL )
    return 1;

  switch ( s->a->desc.y )
    {
    case 1: // 0 11 001. Wind direction
      if ( s->rep > 0 && s->r->n > 0 )
        {
          s->r->raw[s->r->n - 1].dd = s->val;
        }
      break;

    case 2: // 0 11 002. Wind speed
      if ( s->rep > 0 && s->r->n > 0 )
        {
          s->r->raw[s->r->n - 1].ff = s->val;
        }
      break;

    case 61: // 0 11 061. Absolute wind shear in 1 km layer below
      if ( s->rep > 0 && s->w->n > 0 )
        {
          s->w->raw[s->w->n - 1].ws_blw = s->val;
        }
      break;

    case 62: // 0 11 062. Absolute wind shear in 1 km layer above
      if ( s->rep > 0 && s->w->n > 0 )
        {
          s->w->raw[s->w->n - 1].ws_abv = s->val;
        }
      break;

    default:
      break;
    }
  return 0;
}
