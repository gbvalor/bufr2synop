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
 \file bufr2tac_x10.c
 \brief decodes the descriptors with X = 10 (air pressure)
 */
#include "bufr2tac.h"

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
    {
      ic = ( int ) ( P * 0.1 );
    }
  else
    {
      ic = ( int ) ( -P * 0.1 );
    }
  sprintf ( target, "%03d", ic % 1000 );
  return target;
}

/*!
  \fn char * pascal_to_pnpnpn ( char *target, double P )
  \brief Converts pascal values into a pnpnpn string
  \param P the pressure variation in Pascal units
  \param target string with the result
  If the value is below 10000.0 it should be expressed in decapascal units
  otherwise in hectopascal units
*/
char * pascal_to_pnpnpn ( char *target, double P )
{
  int ic;
  if ( P >= 10000.0 ) 
    {
      ic = ( int ) ( P * 0.01 + 0.5 );
    }
  else
    {
      ic = ( int ) ( P * 0.1 + 0.5 );
      if (ic == 1000)
        ic = 999;
    }
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
  \fn int syn_parse_x10 ( struct synop_chunks *syn, struct bufr2tac_subset_state *s )
  \brief Parse a expanded descriptor with X = 10
  \param syn pointer to a s      if ( BUFR2TAC_DEBUG_LEVEL > 0 )
        bufr2tac_set_error ( s, 0, "syn_parse_x08()", "Descriptor not parsed" );
truct \ref synop_chunks where to set the results
  \param s pointer to a struct \ref bufr2tac_subset_state where is stored needed information in sequential analysis

  It returns 0 if success, 1 if problems when processing. If a descriptor is not processed returns 0 anyway
*/
int syn_parse_x10 ( struct synop_chunks *syn, struct bufr2tac_subset_state *s )
{
  char aux[16];

  if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
    {
      return 0;
    }

  switch ( s->a->desc.y )
    {
    case 4: // 0 10 004 . Pressure
      pascal_to_PPPP ( aux, s->val );
      strcpy ( syn->s1.PoPoPoPo, aux );
      syn->mask |= SYNOP_SEC1;
      break;

    case 9: // 0 10 009. Geopotential at standard level
      sprintf ( syn->s1.hhh, "%03d", abs(s->ival) % 1000 );
      syn->mask |= SYNOP_SEC1;
      break;

    case 51: // 0 10 051 . Pressure reduced to mean sea level
      pascal_to_PPPP ( aux, s->val );
      memcpy ( syn->s1.PPPP, aux, 4 );
      syn->s1.PPPP[4] = 0;
      syn->mask |= SYNOP_SEC1;
      break;

    case 61: // 0 10 061 . 3-hour pressure change
      pascal_to_ppp ( aux, s->val );
      memcpy(syn->s1.ppp, aux, 3);
      syn->s1.ppp[3] = 0;
      syn->mask |= SYNOP_SEC1;
      break;

    case 62: // 0 10 062 . 24-hour pressure change
      pascal_to_ppp ( aux, s->val );
      memcpy ( syn->s3.ppp24, aux, 3 );
      syn->s3.ppp24[3] = 0;
      if ( s->val >= 0 )
        {
          strcpy ( syn->s3.snp24, "8" );
        }
      else
        {
          strcpy ( syn->s3.snp24, "9" );
        }
      syn->mask |= SYNOP_SEC3;
      break;

    case 63: // 0 10 063 . Characteristic of pressure tendency
      if ( s->ival > 9 || s->ival < 0)
        return 1;
      sprintf ( syn->s1.a, "%1d",s->ival );
      syn->mask |= SYNOP_SEC1;
      break;

    default:
      if ( BUFR2TAC_DEBUG_LEVEL > 1 && (s->a->mask & DESCRIPTOR_VALUE_MISSING) == 0 ) 
        bufr2tac_set_error ( s, 0, "syn_parse_x10()", "Descriptor not parsed" );
      break;
    }
  return 0;
}

/*!
  \fn int buoy_parse_x10 ( struct buoy_chunks *b, struct bufr2tac_subset_state *s )
  \brief Parse a expanded descriptor with X = 10
  \param b pointer to a struct \ref buoy_chunks where to set the results
  \param s pointer to a struct \ref bufr2tac_subset_state where is stored needed information in sequential analysis

  It returns 0 if success, 1 if problems when processing. If a descriptor is not processed returns 0 anyway
*/
int buoy_parse_x10 ( struct buoy_chunks *b, struct bufr2tac_subset_state *s )
{
  char aux[16];

  if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
    {
      return 0;
    }

  switch ( s->a->desc.y )
    {

    case 4: // 0 10 004 . Pressure
      pascal_to_PPPP ( aux, s->val );
      strcpy ( b->s1.PoPoPoPo, aux );
      b->mask |= BUOY_SEC1;
      break;

    case 51: // 0 10 051 . Pressure reduced to mean sea level
      pascal_to_PPPP ( aux, s->val );
      strcpy ( b->s1.PPPP, aux );
      b->mask |= BUOY_SEC1;
      break;

    case 63: // 0 10 063 . Characteristic of pressure tendency
      sprintf ( b->s1.a, "%1d",s->ival );
      b->mask |= BUOY_SEC1;
      break;

    case 61: // 0 10 061 . 3-hour pressure change
      pascal_to_ppp ( aux, s->val );
      memcpy( b->s1.ppp, aux, 3 );
      b->s1.ppp[3] = 0;
      b->mask |= BUOY_SEC1;
      break;

    default:
      if ( BUFR2TAC_DEBUG_LEVEL > 1 && (s->a->mask & DESCRIPTOR_VALUE_MISSING) == 0 ) 
        bufr2tac_set_error ( s, 0, "buoy_parse_x10()", "Descriptor not parsed" );
      break;
    }
  return 0;
}


/*!
  \fn int climat_parse_x10 ( struct climat_chunks *c, struct bufr2tac_subset_state *s )
  \brief Parse a expanded descriptor with X = 10
  \param c pointer to a struct \ref climat_chunks where to set the results
  \param s pointer to a struct \ref bufr2tac_subset_state where is stored needed information in sequential analysis

  It returns 0 if success, 1 if problems when processing. If a descriptor is not processed returns 0 anyway
*/
int climat_parse_x10 ( struct climat_chunks *c, struct bufr2tac_subset_state *s )
{
  char aux[16];

  if ( c == NULL || s == NULL )
    {
      return 1;
    }

  if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
    {
      return 0;
    }

  switch ( s->a->desc.y )
    {
    case 4: // 0 10 004 . Pressure
      if ( s->isq_val == 4 )
        {
          pascal_to_PPPP ( aux, s->val );
          if ( s->is_normal == 0 )
            {
              strcpy ( c->s1.PoPoPoPo, aux );
              c->mask |= CLIMAT_SEC1;
            }
          else
            {
              strcpy ( c->s2.PoPoPoPo, aux );
              c->mask |= CLIMAT_SEC2;
            }
        }
      break;

    case 9: // 0 10 009 . Heigh of geopotential
      if ( s->isq_val == 4 )
        {
          if ( s->is_normal == 0 )
            {
              sprintf ( c->s1.PPPP, "%04d", s->ival );
              c->mask |= CLIMAT_SEC1;
            }
          else
            {
              sprintf ( c->s1.PPPP, "%04d", s->ival );
              c->mask |= CLIMAT_SEC2;
            }
        }
      break;

    case 51: // 0 10 051 . Pressure reduced to mean sea level
      if ( s->isq_val == 4 )
        {
          pascal_to_PPPP ( aux, s->val );
          if ( s->is_normal == 0 )
            {
              strcpy ( c->s1.PPPP, aux );
              c->mask |= CLIMAT_SEC1;
            }
          else
            {
              strcpy ( c->s2.PPPP, aux );
              c->mask |= CLIMAT_SEC2;
            }
        }
      break;

    default:
      if ( BUFR2TAC_DEBUG_LEVEL > 1 && (s->a->mask & DESCRIPTOR_VALUE_MISSING) == 0 ) 
        bufr2tac_set_error ( s, 0, "climat_parse_x10()", "Descriptor not parsed" );
      break;

    }
  return 0;
}

/*!
  \fn int temp_parse_x10 ( struct temp_chunks *t, struct bufr2tac_subset_state *s )
  \brief Parse a expanded descriptor with X = 10
  \param t pointer to a struct \ref temp_chunks where to set the results
  \param s pointer to a struct \ref bufr2tac_subset_state where is stored needed information in sequential analysis

  It returns 0 if success, 1 if problems when processing. If a descriptor is not processed returns 0 anyway
*/
int temp_parse_x10 ( const struct temp_chunks *t, struct bufr2tac_subset_state *s )
{
  if ( t == NULL || s == NULL )
    {
      return 1;
    }

  if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
    {
      return 0;
    }

  switch ( s->a->desc.y )
    {
    case 9: // 0 10 009. geopotential
      if ( s->rep > 0 && s->r->n > 0 )
        {
          s->r->raw[s->r->n - 1].h = s->val;
        }
      break;

    default:
      if ( BUFR2TAC_DEBUG_LEVEL > 1 && (s->a->mask & DESCRIPTOR_VALUE_MISSING) == 0 ) 
        bufr2tac_set_error ( s, 0, "temp_parse_x10()", "Descriptor not parsed" );
      break;
    }
  return 0;
}
