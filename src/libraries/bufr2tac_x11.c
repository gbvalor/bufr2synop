/***************************************************************************
 *   Copyright (C) 2013-2022 by Guillermo Ballester Valor                  *
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
 \file bufr2tac_x11.c
 \brief decodes the descriptors with X = 11 (wind)
 */
#include "bufr2tac.h"

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

  if ( secs < 0 )
    {
      i = ( -secs ) / 360;
    }
  else
    {
      i = ( secs ) / 360;
    }

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
  \fn void direction_to_0877 ( char *dd, uint32_t ival );
  \brief encode dd from ival
  \param dd target string according to WMO 306 Vol I.1 (TAC), I.2 (BUFR)
  \param ival wind direction (value range: 0 .. 511)

TAC
===
FM 12 SYNOP, FM 13 SHIP, FM 14 SYNOP MOBIL,
FM 18 BUOY,
FM 35 TEMP, FM 36 TEMP SHIP, FM 37 TEMP DROP, FM 38 TEMP MOBIL

dd
    True direction, in tens of degrees, from which wind is blowing (or will
    blow).
    (Code table 0877; stations within 1° of the North Pole use Code table 0878)
    (FM 12, FM 13, FM 14, FM 18, ...
    (1)
        When encoding wind direction that has been rounded off to the nearest
        5°, the hundreds and tens figures of this rounded direction shall be
        reported by dd and the units figure shall be added to the hundreds
        figure of the wind speed.
    (2)
        Stations within 1° of the South Pole shall use Code table 0877 for
        reporting wind direction.
        These stations shall orient their azimuth rings so that the ring's zero
        coincides with the Greenwich meridian, e.g. wind from 0° longitude is
        coded 36, from 90°E longitude is coded 09, from 180° longitude is coded
        18, and from 90°W longitude is coded 27, etc.

dBdB
    Drift direction of the buoy, expressed in tens of degrees, at the last
    known position of the buoy given in the groups YYMMJ GGgg/.
    (FM 18)

dtdt
    True direction (rounded off to the nearest 5°), in tens of degrees, from
    which wind is blowing at the tropopause level.
    (FM 35, FM 36, FM 37, FM 38)
    (1)
        See Note (1) under dd.

dw1dw1
dw2dw2
    True direction, in tens of degrees, from which swell waves are coming.
    (Code table 0877)
    (FM 12, FM 13, FM 14)

d0d0
...
dndn
    True direction, in tens of degrees, towards which sea current at selected
    and/or significant depths starting with the sea surface is moving.
    (Code table 0877)
    (FM 18, FM 64)

Code table 0877: Direction in two figures
    00 Calm
    01 5° – 14°
    ..
    36 355° – 4°

BUFR
====

    0 11 001 Wind direction
    0 11 011 Wind direction at 10 m
    0 22 001 Direction of waves
    0 22 002 Direction of wind waves
    0 22 003 Direction of swell waves
    0 22 004 Direction towards which current is flowing
    0 22 005 Direction of sea-surface current
UNIT: degree true, SCALE: 0, REFERENCE VALUE: 0, DATA WIDTH (Bits): 9

Wind and wind waves reporting standards:
    Calm: speed: 0, direction: 0°
    Normal observation: speed >0, direction 1° - 360°
*/
void direction_to_0877 ( char *dd, uint32_t ival )
{
  uint32_t ix;

  if ( ival == 0 ) {
      strcpy ( dd, "00" );
  } else {
      ix = (( ival + 5 ) / 10 ) % 36;
      sprintf ( dd, "%02u", ix == 0u ? 36u : ix );
  }
}


/*!
  \fn char * wind_to_dndnfnfnfn( char target, double dd, double ff)
  \brief sets dndnfnfnfn item in a temp report
  \param target string set as resul
  \param dd wind direction (degrees)
  \param vv wind speed
*/
char * wind_to_dndnfnfnfn ( char * target, double dd, double ff )
{
  int ix;

  if ( dd == MISSING_REAL || ff == MISSING_REAL )
    {
      strcpy ( target, "/////" );
      return target;
    }

  ix = ( int ) ( ( dd + 2.5 ) / 5 ) * 5 * 100 + ( int ) ( ff + 0.5 );
  sprintf ( target, "%05d", ix );
  return target;
}

/*!
  \fn int syn_parse_x11 ( struct synop_chunks *syn, struct bufr2tac_subset_state *s )
  \brief Parse a expanded descriptor with X = 11
  \param syn pointer to a struct \ref synop_chunks where to set the results
  \param s pointer to a struct \ref bufr2tac_subset_state where is stored needed information in sequential analysis

  It returns 0 if success, 1 if problems when processing. If a descriptor is not processed returns 0 anyway
*/
int syn_parse_x11 ( struct synop_chunks *syn, struct bufr2tac_subset_state *s )
{

  if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
    {
      return 0;
    }

  switch ( s->a->desc.y )
    {
    case 1: // 0 11 001 . Wind direction
    case 11: // 0 11 011 . Wind direction at 10 meters
      direction_to_0877 ( syn->s1.dd, s->ival );
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
      if (strcmp ( syn->s1.dd, "00" ) == 0 && strcmp ( syn->s1.ff, "00" ) != 0 )
        {
          strcpy ( syn->s1.dd, "99" );
        }
      break;

    case 41: // 0 11 041 . Max wind gust speed
      if ( syn->s3.d9.n == SYNOP_NMISC )
        {
          return 0; // No more groups space
        }
        
      if ( s->itval == -600 )
        {
          if ( syn->mask & SUBSET_MASK_HAVE_GUST10 )
            {
              break; // Already have gust for 10 minutes
            }
          sprintf ( syn->s3.d9.misc[syn->s3.d9.n].SpSp, "910" );
          s->SnSn = 910;
          if ( syn->s0.iw[0] == '4' )
            {
              s->val *= 1.94384449; // original value was in m/s. So it change to knots
            }
          if ( s->val < 100.0 )
            {
              sprintf ( syn->s3.d9.misc[syn->s3.d9.n].spsp, "%02d", ( int ) ( s->val + 0.5 ) );
            }
          else
            {
              // Case more than 99 knots
              sprintf ( syn->s3.d9.misc[syn->s3.d9.n].spsp,"99" );
              syn->s3.d9.n++;
              sprintf ( syn->s3.d9.misc[syn->s3.d9.n].SpSp, "00" );
              sprintf ( syn->s3.d9.misc[syn->s3.d9.n].spsp, "%03d", ( int ) ( s->val + 0.5 ) );
            }
          syn->s3.d9.n++;
          syn->mask |= SUBSET_MASK_HAVE_GUST10;
          syn->mask |= SYNOP_SEC3;
        }
      else if ( s->itval )
        {
          if ( s->mask & SUBSET_MASK_HAVE_GUST )
            {
              sprintf ( syn->s5.d9.misc[syn->s5.d9.n].SpSp, "907" );
              secs_to_tt ( syn->s5.d9.misc[syn->s5.d9.n].spsp, s->itval );
              syn->s5.d9.n++;
              sprintf ( syn->s5.d9.misc[syn->s5.d9.n].SpSp, "911" );
              s->SnSn = 911;
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
              s->SnSn = 911;
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

    case 42: // 0 11 042 . Max wind speed (10-minutes mean)
      if ( syn->s3.d9.n == SYNOP_NMISC )
        {
          return 0;
        }
      sprintf ( syn->s3.d9.misc[syn->s3.d9.n].SpSp, "907" );
      secs_to_tt ( syn->s3.d9.misc[syn->s3.d9.n].spsp, s->itval );
      syn->s3.d9.n++;
      sprintf ( syn->s3.d9.misc[syn->s3.d9.n].SpSp, "912" );
      s->SnSn = 912;
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
      break;

    case 43: // 0 11 043 . Max wind gust direction
      if ( BUFR2TAC_DEBUG_LEVEL > 1 && (s->a->mask & DESCRIPTOR_VALUE_MISSING) == 0 ) 
        bufr2tac_set_error ( s, 0, "syn_parse_x11()", "Descriptor not parsed" );
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
              s->val /= 1.94384449; // original values in knots. passed to m/s
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
          syn->mask |= SYNOP_SEC3;
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
      if ( BUFR2TAC_DEBUG_LEVEL > 1 && (s->a->mask & DESCRIPTOR_VALUE_MISSING) == 0 ) 
        bufr2tac_set_error ( s, 0, "syn_parse_x11()", "Descriptor not parsed" );
      break;
    }
  return 0;
}

/*!
  \fn int buoy_parse_x11 ( struct buoy_chunks *b, struct bufr2tac_subset_state *s )
  \brief Parse a expanded descriptor with X = 11
  \param b pointer to a struct \ref buoy_chunks where to set the results
  \param s pointer to a struct \ref bufr2tac_subset_state where is stored needed information in sequential analysis

  It returns 0 if success, 1 if problems when processing. If a descriptor is not processed returns 0 anyway
*/
int buoy_parse_x11 ( struct buoy_chunks *b, struct bufr2tac_subset_state *s )
{

  if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
    {
      return 0;
    }

  switch ( s->a->desc.y )
    {
    case 1: // 0 11 001 . Wind direction
    case 11: // 0 11 011 . Wind direction at 10m
      direction_to_0877 ( b->s1.dd, s->ival );
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
      if (strcmp ( b->s1.dd, "00" ) == 0 && strcmp ( b->s1.ff, "00" ) != 0 )
        {
          strcpy ( b->s1.dd, "99" );
        }
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
      if ( BUFR2TAC_DEBUG_LEVEL > 1 && (s->a->mask & DESCRIPTOR_VALUE_MISSING) == 0 ) 
        bufr2tac_set_error ( s, 0, "buoy_parse_x11()", "Descriptor not parsed" );
      break;
    }
  return 0;
}

/*!
  \fn int climat_parse_x11 ( struct climat_chunks *c, struct bufr2tac_subset_state *s )
  \brief Parse a expanded descriptor with X = 11
  \param c pointer to a struct \ref climat_chunks where to set the results
  \param s pointer to a struct \ref bufr2tac_subset_state where is stored needed information in sequential analysis

  It returns 0 if success, 1 if problems when processing. If a descriptor is not processed returns 0 anyway
*/
int climat_parse_x11 ( struct climat_chunks *c, struct bufr2tac_subset_state *s )
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
          sprintf ( c->s4.yfx, "%02d", (s->day + 50) % 100 );
        }
      c->mask |= CLIMAT_SEC4;
      break;

    default:
      if ( BUFR2TAC_DEBUG_LEVEL > 1 && (s->a->mask & DESCRIPTOR_VALUE_MISSING) == 0 ) 
        bufr2tac_set_error ( s, 0, "climat_parse_x11()", "Descriptor not parsed" );
      break;
    }
  return 0;
}

/*!
  \fn int temp_parse_x11 ( const struct temp_chunks *t, struct bufr2tac_subset_state *s )
  \brief Parse a expanded descriptor with X = 11
  \param t pointer to a struct \ref temp_chunks where to set the results
  \param s pointer to a struct \ref bufr2tac_subset_state where is stored needed information in sequential analysis

  It returns 0 if success, 1 if problems when processing. If a descriptor is not processed returns 0 anyway
*/
int temp_parse_x11 ( const struct temp_chunks *t, struct bufr2tac_subset_state *s )
{

  if ( t == NULL )
    {
      return 1;
    }

  switch ( s->a->desc.y )
    {
    case 1: // 0 11 001. Wind direction
      if ( s->rep > 0 && s->r->n > 0 )
        {
          if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
            {
              s->r->raw[s->r->n - 1].dd = MISSING_REAL;
            }
          else
            {
              s->r->raw[s->r->n - 1].dd = s->val;
            }
        }
      break;

    case 2: // 0 11 002. Wind speed
      if ( s->rep > 0 && s->r->n > 0 )
        {
          if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
            {
              s->r->raw[s->r->n - 1].ff = MISSING_REAL;
            }
          else
            {
              s->r->raw[s->r->n - 1].ff = s->val;
            }
        }
      break;

    case 61: // 0 11 061. Absolute wind shear in 1 km layer below
      if ( s->w->n > 0 )
        {
          if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
            {
              s->w->raw[s->w->n - 1].ws_blw = MISSING_REAL;
            }
          else
            {
              s->w->raw[s->w->n - 1].ws_blw = s->val;
            }
        }
      break;

    case 62: // 0 11 062. Absolute wind shear in 1 km layer above
      if ( s->w->n > 0 )
        {
          if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
            {
              s->w->raw[s->w->n - 1].ws_abv = MISSING_REAL;
            }
          else
            {
              s->w->raw[s->w->n - 1].ws_abv = s->val;
            }
        }
      break;

    default:
      if ( BUFR2TAC_DEBUG_LEVEL > 1 && (s->a->mask & DESCRIPTOR_VALUE_MISSING) == 0 ) 
        bufr2tac_set_error ( s, 0, "temp_parse_x11()", "Descriptor not parsed" );
      break;
    }
  return 0;
}
