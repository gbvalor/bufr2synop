/***************************************************************************
 *   Copyright (C) 2013-2017 by Guillermo Ballester Valor                  *
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
 \file bufr2tac_x22.c
 \brief decodes the descriptors with X = 22 (oceanographic data)
*/
#include "bufr2tac.h"


/*!
  \fn int syn_parse_x22 ( struct synop_chunks *syn, struct bufr2tac_subset_state *s )
  \brief Parse a expanded descriptor with X = 01
  \param syn pointer to a struct \ref synop_chunks where to set the results
  \param s pointer to a struct \ref bufr2tac_subset_state where is stored needed information in sequential analysis

  It returns 0 if success, 1 if problems when processing. If a descriptor is not processed returns 0 anyway
*/
int syn_parse_x22 ( struct synop_chunks *syn, struct bufr2tac_subset_state *s )
{
  char aux[16];

  if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
    return 0;

  switch ( s->a->desc.y )
    {
    case 1: // 0 22 001 Direction of waves
    case 3: // 0 22 003 Direction of swell waves
      if ( syn->s2.dw1dw1[0] == 0 )
        {
          sprintf ( syn->s2.dw1dw1, "%02d", ( s->ival + 5 ) /10 );
        }
      else if ( syn->s2.dw2dw2[0] == 0 )
        {
          sprintf ( syn->s2.dw2dw2, "%02d", ( s->ival + 5 ) /10 );
        }
      syn->mask |= SYNOP_SEC2; // have sec2 data
      break;
    case 11: // 0 22 011 wind period in seconds
      if ( syn->s2.PwPw[0] == 0 )
        {
          sprintf ( syn->s2.PwPw, "%02d", ( int ) ( s->val ) );
          syn->mask |= SYNOP_SEC2; // have sec2 data
        }
      break;
    case 12: // 0 22 012 wind wave period in seconds
      if ( syn->s2.PwaPwa[0] == 0 )
        {
          sprintf ( syn->s2.PwaPwa, "%02d", ( int ) ( s->val ) );
          syn->mask |= SYNOP_SEC2; // have sec2 data
        }
      break;
    case 13: // 0 22 013 swell wave period in seconds
      if ( syn->s2.Pw1Pw1[0] == 0 )
        {
          sprintf ( syn->s2.Pw1Pw1, "%02d", ( int ) ( s->val ) );
        }
      else if ( syn->s2.Pw2Pw2[0] == 0 )
        {
          sprintf ( syn->s2.Pw2Pw2, "%02d", ( int ) ( s->val ) );
        }
      syn->mask |= SYNOP_SEC2; // have sec2 data
      break;
    case 21: // 0 22 021  wind wave heigh in m
      if ( syn->s2.HwHw[0] == 0 )
        {
          sprintf ( syn->s2.HwHw, "%02d", ( int ) ( s->val * 2.0 + 0.01 ) );
          syn->mask |= SYNOP_SEC2; // have sec2 data
        }
      break;
    case 22: // 0 22 022 wind wave heigh in meters
      if ( syn->s2.HwaHwa[0] == 0 )
        {
          sprintf ( syn->s2.HwaHwa, "%02d", ( int ) ( s->val * 2.0 + 0.01 ) ); // 0.5 m units
          syn->mask |= SYNOP_SEC2; // have sec2 data
        }
      if ( syn->s2.HwaHwaHwa[0] == 0 )
        {
          sprintf ( syn->s2.HwaHwaHwa, "%03d", ( int ) ( s->val * 10.0 + 0.5 ) ); // 0.1 m units
          syn->mask |= SYNOP_SEC2; // have sec2 data
        }
      break;
    case 23: // 0 22 023 swell wave heigh in meters
      if ( syn->s2.Hw1Hw1[0] == 0 )
        {
          sprintf ( syn->s2.Hw1Hw1, "%02d", ( int ) ( s->val * 2.0 + 0.01 ) );
        }
      else if ( syn->s2.Hw2Hw2[0] == 0 )
        {
          sprintf ( syn->s2.Hw2Hw2, "%02d", ( int ) ( s->val * 2.0 + 0.01 ) );
        }
      syn->mask |= SYNOP_SEC2; // have sec2 data
      break;
    case 42:
    case 43:
    case 45:
    case 49: // 0 22 049 Sea surface temperature
      if ( syn->s2.TwTwTw[0] == 0 )
        {
          if ( kelvin_to_snTTT ( aux, s->val ) )
            {
              syn->s2.ss[0] = aux[0];
              strcpy ( syn->s2.TwTwTw, aux + 1 );
              syn->mask |= SYNOP_SEC2; // have sec2 data
            }
        }
      break;
    default:
      break;
    }
  return 0;
}

/*!
  \fn int buoy_parse_x22 ( struct buoy_chunks *b, struct bufr2tac_subset_state *s )
  \brief Parse a expanded descriptor with X = 22
  \param b pointer to a struct \ref buoy_chunks where to set the results
  \param s pointer to a struct \ref bufr2tac_subset_state where is stored needed information in sequential analysis

  It returns 0 if success, 1 if problems when processing. If a descriptor is not processed returns 0 anyway
*/
int buoy_parse_x22 ( struct buoy_chunks *b, struct bufr2tac_subset_state *s )
{
  char aux[16];

  if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
    return 0;

  switch ( s->a->desc.y )
    {
    case 4: // 0 22 004 Direction of current
      if ( s->layer < 32 ) // only 32 max layers
        {
          if ( b->s3.l2[s->layer].dd[0] == 0 )
            {
              sprintf ( b->s3.l2[s->layer].dd, "%02d", ( int ) ( s->val + 5.0 ) / 10 ) ;
              b->mask |= BUOY_SEC3;
            }
          if ( b->s3.l2[s->layer].zzzz[0] == 0 ) // also stores pendent deep
            {
              sprintf ( b->s3.l2[s->layer].zzzz, "%04d", s->deep );
              b->mask |= BUOY_SEC3;
            }
        }
    case 12: // 0 22 012 wind wave period in seconds
      if ( b->s2.PwaPwa[0] == 0 )
        {
          sprintf ( b->s2.PwaPwa, "%02d", ( int ) ( s->val ) );
          b->mask |= BUOY_SEC2; // have sec2 data
          if ( b->s2.PwaPwaPwa[0] == 0 )
            sprintf ( b->s2.PwaPwaPwa, "%03d", ( int ) ( s->val * 10 + 0.5 ) );

        }
      break;
    case 22: // 0 22 022 wind wave heigh in meters
      if ( b->s2.HwaHwa[0] == 0 )
        {
          sprintf ( b->s2.HwaHwa, "%02d", ( int ) ( s->val * 2.0 + 0.01 ) ); // 0.5 m units
          b->mask |= BUOY_SEC2; // have sec2 data
        }
      if ( b->s2.HwaHwaHwa[0] == 0 )
        {
          sprintf ( b->s2.HwaHwaHwa, "%03d", ( int ) ( s->val * 10.0 + 0.01 ) ); // 0.1 m units
          b->mask |= BUOY_SEC2; // have sec2 data
        }
      break;
    case 31: // 0 22 033 current speed
      if ( s->layer < 32 ) // only 32 max layers
        {
          if ( b->s3.l2[s->layer].dd[0] == 0 )
            {
              sprintf ( b->s3.l2[s->layer].dd, "%03d", ( int ) ( s->val * 100.0 + 0.5 ) ) ;
              b->mask |= BUOY_SEC3;
            }
          if ( b->s3.l2[s->layer].zzzz[0] == 0 ) // also stores pendent deep
            {
              sprintf ( b->s3.l2[s->layer].zzzz, "%04d", s->deep );
              b->mask |= BUOY_SEC3;
            }
        }
      break;
    case 42:
    case 43:
      if ( s->deep == 0 )
        {
          if ( b->s2.TwTwTw[0] == 0 )
            {
              if ( kelvin_to_snTTT ( aux, s->val ) )
                {
                  b->s2.sn[0] = aux[0];
                  strcpy ( b->s2.TwTwTw, aux + 1 );
                  b->mask |= BUOY_SEC2; // have sec2 data
                }
            }
        }
      if ( s->layer < 32 ) // only 32 max layers
        {
          if ( b->s3.l1[s->layer].TTTT[0] == 0 )
            {
              kelvin_to_TTTT ( b->s3.l1[s->layer].TTTT, s->val );
              b->mask |= BUOY_SEC3;
            }
          if ( b->s3.l1[s->layer].zzzz[0] == 0 ) // also stores pendent deep
            {
              sprintf ( b->s3.l1[s->layer].zzzz, "%04d", s->deep );
              b->mask |= BUOY_SEC3;
            }
        }
      break;
    case 45:
    case 49: // 0 22 049 Sea surface temperature
      if ( b->s2.TwTwTw[0] == 0 )
        {
          if ( kelvin_to_snTTT ( aux, s->val ) )
            {
              b->s2.sn[0] = aux[0];
              strcpy ( b->s2.TwTwTw, aux + 1 );
              b->mask |= BUOY_SEC2; // have sec2 data
            }
        }
      break;
    case 62: // 0 22 062 Salinity
      if ( s->layer < 32 ) // only 32 max layers
        {
          if ( b->s3.l1[s->layer].SSSS[0] == 0 )
            {
              sprintf ( b->s3.l1[s->layer].SSSS, "%04d", ( int ) ( s->val * 100.0 + 0.5 ) );
              b->mask |= BUOY_SEC3;
            }
          if ( b->s3.l1[s->layer].zzzz[0] == 0 ) // also stores pendent deep
            {
              sprintf ( b->s3.l1[s->layer].zzzz, "%04d", s->deep );
              b->mask |= BUOY_SEC3;
            }
        }
      break;
    default:
      break;
    }
  return 0;
}

/*!
  \fn int temp_parse_x22 ( struct temp_chunks *t, struct bufr2tac_subset_state *s )
  \brief Parse a expanded descriptor with X = 22
  \param t pointer to a struct \ref temp_chunks where to set the results
  \param s pointer to a struct \ref bufr2tac_subset_state where is stored needed information in sequential analysis

  It returns 0 if success, 1 if problems when processing. If a descriptor is not processed returns 0 anyway
*/
int temp_parse_x22 ( struct temp_chunks *t, struct bufr2tac_subset_state *s )
{
  char aux[16];

  if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
    return 0;

  switch ( s->a->desc.y )
    {
    case 43:
      if ( kelvin_to_snTTT ( aux, s->val ) )
        {
          t->a.s7.sn[0] = aux[0];
          t->b.s7.sn[0] = aux[0];
          t->c.s7.sn[0] = aux[0];
          t->d.s7.sn[0] = aux[0];
          strcpy ( t->a.s7.TwTwTw, aux + 1 );
          strcpy ( t->b.s7.TwTwTw, aux + 1 );
          strcpy ( t->c.s7.TwTwTw, aux + 1 );
          strcpy ( t->d.s7.TwTwTw, aux + 1 );
          t->a.mask |= TEMP_SEC_7; // have sec7 data
        }
      break;
      
    default:
      break;
    }

  return 0;
}
