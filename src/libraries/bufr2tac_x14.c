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
 \file bufr2tac_x14.c
 \brief decodes the descriptors with X = 14 (Radiation)
 */
#include "bufr2tac.h"

/*!
  \fn int check_kj_m2(double val)
  \brief Check if a radiation value can be wrote in Kj/m2 using up to 4 chars
  \param val radiation value in J/m2

  Returns 1 if success, 0 otherwise
*/
int check_kj_m2 ( double val )
{
  return ( fabs ( val ) < 1.0e7 ) ? 1 : 0;
}

/*!
  \fn int check_j_cm2(double val)
  \brief Check if a radiation value can be wrote in J/cm2 using up to 4 chars
  \param val radiation value in J/m2

  Returns 1 if success, 0 otherwise
*/
int check_j_cm2 ( double val )
{
  return ( fabs ( val ) < 1.0e8 ) ? 1 : 0;
}


/*!
  \fn int syn_parse_x14 ( struct synop_chunks *syn, struct bufr2tac_subset_state *s )
  \brief Parse a expanded descriptor with X = 14
  \param syn pointer to a struct \ref synop_chunks where to set the results
  \param s pointer to a struct \ref bufr2tac_subset_state where is stored needed information in sequential analysis

  It returns 0 if success, 1 if problems when processing. If a descriptor is not processed returns 0 anyway
*/
int syn_parse_x14 ( struct synop_chunks *syn, struct bufr2tac_subset_state *s )
{
  int tpd;

  switch ( s->a->desc.y )
    {
    case 1: // 0 14 001 long-wave radiation 24 hours
    case 11:
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          return 0;
        }
      strcpy ( syn->s3.j524[4], "4" );
      sprintf ( syn->s3.FFFF24[4], "%04d", ( int ) ( s->val / 10000.0 + 0.5 ) );
      syn->mask |= SYNOP_SEC3;
      break;

    case 2: // 0 14 002 Long-wave radiation
    case 12:
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          return 0;
        }
      tpd = time_period_duration ( s );
      if ( tpd == ( 24 * 3600 ) )
        {
          if ( check_j_cm2 ( s->val ) == 0 )
            return 0;
          if ( strcmp ( "07", syn->s0.II ) == 0 ) // France
            {
              if ( strcmp ( "06", syn->e.HH ) == 0 )
                {
                  if ( s->ival >= 0 )
                    {
                      strcpy ( syn->s3.j524[4], "4" );
                      sprintf ( syn->s3.FFFF24[4], "%04d", ( int ) ( s->val / 10000.0 + 0.5 ) );
                    }
                  else
                    {
                      strcpy ( syn->s3.j524[5], "5" );
                      sprintf ( syn->s3.FFFF24[5], "%04d", ( int ) ( - ( s->val ) / 10000.0 + 0.5 ) );
                    }
                  syn->mask |= SYNOP_SEC3;
                }
            }
          else
            {
              if ( s->ival >= 0 )
                {
                  strcpy ( syn->s3.j524[4], "4" );
                  sprintf ( syn->s3.FFFF24[4], "%04d", ( int ) ( s->val / 10000.0 + 0.5 ) );
                }
              else
                {
                  strcpy ( syn->s3.j524[5], "5" );
                  sprintf ( syn->s3.FFFF24[5], "%04d", ( int ) ( - ( s->val ) / 10000.0 + 0.5 ) );
                }
              syn->mask |= SYNOP_SEC3;
            }
        }
      else if ( tpd == 3600 )
        {
          if ( check_kj_m2 ( s->val ) == 0 )
            return 0;
          if ( s->ival >= 0 )
            {
              strcpy ( syn->s3.j5[4], "4" );
              sprintf ( syn->s3.FFFF[4], "%04d", ( int ) ( s->val / 1000.0 + 0.5 ) );
            }
          else
            {
              strcpy ( syn->s3.j5[4], "5" );
              sprintf ( syn->s3.FFFF[4], "%04d", ( int ) ( - ( s->val ) / 1000.0 + 0.5 ) );
            }
        }
      syn->mask |= SYNOP_SEC3;
      break;

    case 3: // 0 14 003 short-wave radiation 24 hours
    case 13:
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          return 0;
        }
      if ( check_j_cm2 ( s->val ) == 0 )
        return 0;
      strcpy ( syn->s3.j524[6], "6" );
      sprintf ( syn->s3.FFFF24[6], "%04d", ( int ) ( s->val / 10000.0 + 0.5 ) );
      syn->mask |= SYNOP_SEC3;
      break;

    case 4: // 0 14 002 short-wave radiation
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          return 0;
        }
      tpd = time_period_duration ( s );
      if ( tpd == ( 24 * 3600 ) )
        {
          if ( check_j_cm2 ( s->val ) == 0 )
            return 0;
          if ( strcmp ( "07", syn->s0.II ) == 0 ) // France
            {
              if ( strcmp ( "06", syn->e.HH ) == 0 )
                {
                  strcpy ( syn->s3.j524[6], "6" );
                  sprintf ( syn->s3.FFFF24[6], "%04d", ( int ) ( s->val / 10000.0 + 0.5 ) );
                }
            }
          else
            {
              strcpy ( syn->s3.j524[6], "6" );
              sprintf ( syn->s3.FFFF24[6], "%04d", ( int ) ( s->val / 10000.0 + 0.5 ) );
            }
        }
      else if ( tpd == 3600 )
        {
          if ( check_kj_m2 ( s->val ) == 0 )
            return 0;
          strcpy ( syn->s3.j5[6], "6" );
          sprintf ( syn->s3.FFFF[6], "%04d", ( int ) ( s->val / 1000.0 + 0.5 ) );
        }
      syn->mask |= SYNOP_SEC3;
      break;

    case 14: // 0 14 014 Net short wave radiation integrated over period specified
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          return 0;
        }
      tpd = time_period_duration ( s );
      if ( tpd == ( 24 * 3600 ) )
        {
          if ( check_j_cm2 ( s->val ) == 0 )
            return 0;
          if ( strcmp ( "07", syn->s0.II ) == 0 ) // France
            {
              if ( strcmp ( "06", syn->e.HH ) == 0 )
                {
                  sprintf ( syn->s3.FFFF507, "%04d", ( int ) ( s->val / 10000.0 + 0.5 ) );
                  syn->mask |= SYNOP_SEC3;
                }
            }
          else
            {
              sprintf ( syn->s3.FFFF507, "%04d", ( int ) ( s->val / 10000.0 + 0.5 ) );
              syn->mask |= SYNOP_SEC3;
            }
        }
      else if ( tpd == 3600 )
        {
          if ( check_kj_m2 ( s->val ) == 0 )
            return 0;
          sprintf ( syn->s3.FFFF407, "%04d", ( int ) ( s->val / 1000.0 + 0.5 ) );
        }
      syn->mask |= SYNOP_SEC3;
      break;


    case 16: // 0 14 016 Short wave radiation integrated over period specified
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          return 0;
        }
      tpd = time_period_duration ( s );
      if ( tpd == ( 24 * 3600 ) )
        {
          if ( check_j_cm2 ( s->val ) == 0 )
            return 0;
          if ( strcmp ( "07", syn->s0.II ) == 0 ) // France
            {
              if ( strcmp ( "06", syn->e.HH ) == 0 )
                {
                  if ( s->ival >= 0 )
                    {
                      strcpy ( syn->s3.j524[0], "0" );
                      sprintf ( syn->s3.FFFF24[0], "%04d", ( int ) ( s->val / 10000.0 + 0.5 ) );
                    }
                  else
                    {
                      strcpy ( syn->s3.j524[1], "1" );
                      sprintf ( syn->s3.FFFF24[1], "%04d", ( int ) ( - ( s->val ) / 10000.0 + 0.5 ) );
                    }
                  syn->mask |= SYNOP_SEC3;
                }
            }
          else
            {
              if ( s->ival >= 0 )
                {
                  strcpy ( syn->s3.j524[0], "0" );
                  sprintf ( syn->s3.FFFF24[0], "%04d", ( int ) ( s->val / 10000.0 + 0.5 ) );
                }
              else
                {
                  strcpy ( syn->s3.j524[1], "1" );
                  sprintf ( syn->s3.FFFF24[1], "%04d", ( int ) ( - ( s->val ) / 10000.0 + 0.5 ) );
                }
              syn->mask |= SYNOP_SEC3;
            }
        }
      else if ( tpd == 3600 )
        {
          if ( check_kj_m2 ( s->val ) == 0 )
            return 0;
          if ( s->ival >= 0 )
            {
              strcpy ( syn->s3.j5[0], "0" );
              sprintf ( syn->s3.FFFF[0], "%04d", ( int ) ( s->val / 1000.0 + 0.5 ) );
            }
          else
            {
              strcpy ( syn->s3.j5[1], "1" );
              sprintf ( syn->s3.FFFF[1], "%04d", ( int ) ( - ( s->val ) / 1000.0 + 0.5 ) );
            }
        }
      syn->mask |= SYNOP_SEC3;
      break;

    case 28: // 0 14 028 Global solar radiation integrated over period specified
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          return 0;
        }
      tpd = time_period_duration ( s );
      if ( tpd == ( 24 * 3600 ) )
        {
          if ( check_j_cm2 ( s->val ) == 0 )
            return 0;
          if ( strcmp ( "07", syn->s0.II ) == 0 ) // France
            {
              if ( strcmp ( "06", syn->e.HH ) == 0 )
                {
                  strcpy ( syn->s3.j524[2], "2" );
                  sprintf ( syn->s3.FFFF24[2], "%04d", ( int ) ( s->val / 10000.0 + 0.5 ) );
                  syn->mask |= SYNOP_SEC3;
                }
            }
          else
            {
              strcpy ( syn->s3.j524[2], "2" );
              sprintf ( syn->s3.FFFF24[2], "%04d", ( int ) ( s->val / 10000.0 + 0.5 ) );
              syn->mask |= SYNOP_SEC3;
            }
        }
      else if ( tpd == 3600 )
        {
          if ( check_kj_m2 ( s->val ) == 0 )
            return 0;
          strcpy ( syn->s3.j5[2], "2" );
          sprintf ( syn->s3.FFFF[2], "%04d", ( int ) ( s->val / 1000.0 + 0.5 ) );
        }
      syn->mask |= SYNOP_SEC3;
      break;

    case 29: // 0 14 029 Diffuse solar radiation integrated over period specified
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          return 0;
        }
      tpd = time_period_duration ( s );
      if ( tpd == ( 24 * 3600 ) )
        {
          if ( check_j_cm2 ( s->val ) == 0 )
            return 0;
          if ( strcmp ( "07", syn->s0.II ) == 0 ) // France
            {
              if ( strcmp ( "06", syn->e.HH ) == 0 )
                {
                  strcpy ( syn->s3.j524[3], "3" );
                  sprintf ( syn->s3.FFFF24[3], "%04d", ( int ) ( s->val / 10000.0 + 0.5 ) );
                  syn->mask |= SYNOP_SEC3;
                }
            }
          else
            {
              strcpy ( syn->s3.j524[3], "3" );
              sprintf ( syn->s3.FFFF24[3], "%04d", ( int ) ( s->val / 10000.0 + 0.5 ) );
              syn->mask |= SYNOP_SEC3;
            }
        }
      else if ( tpd == 3600 )
        {
          if ( check_kj_m2 ( s->val ) == 0 )
            return 0;
          strcpy ( syn->s3.j5[3], "3" );
          sprintf ( syn->s3.FFFF[3], "%04d", ( int ) ( s->val / 1000.0 + 0.5 ) );
        }
      syn->mask |= SYNOP_SEC3;
      break;


    case 30: // 0 14 030 Direct solar radiation integrated over period specified
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          return 0;
        }
      tpd = time_period_duration ( s );
      if ( tpd == ( 24 * 3600 ) )
        {
          if ( check_j_cm2 ( s->val ) == 0 )
            return 0;
          if ( strcmp ( "07", syn->s0.II ) == 0 ) // France
            {
              if ( strcmp ( "06", syn->e.HH ) == 0 )
                {
                  sprintf ( syn->s3.FFFF508, "%04d", ( int ) ( s->val / 10000.0 + 0.5 ) );
                  syn->mask |= SYNOP_SEC3;
                }
            }
          else
            {
              sprintf ( syn->s3.FFFF508, "%04d", ( int ) ( s->val / 10000.0 + 0.5 ) );
              syn->mask |= SYNOP_SEC3;
            }
        }
      else if ( tpd == 3600 )
        {
          if ( check_kj_m2 ( s->val ) == 0 )
            return 0;
          sprintf ( syn->s3.FFFF408, "%04d", ( int ) ( s->val / 1000.0 + 0.5 ) );
        }
      syn->mask |= SYNOP_SEC3;
      break;

    case 31: // 0 14 031 Total sunshine
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          return 0;
        }
      tpd = time_period_duration ( s );
      if ( tpd == ( 24 * 3600 ) )
        {
          if ( check_j_cm2 ( s->val ) == 0 )
            return 0;
          if ( strcmp ( "07", syn->s0.II ) == 0 ) // France
            {
              if ( strcmp ( "06", syn->e.HH ) == 0 )
                {
                  sprintf ( syn->s3.SSS, "%03d", (abs(s->ival) / 6) % 1000 );
                  syn->mask |= SYNOP_SEC3;
                }
            }
          else
            {
              sprintf ( syn->s3.SSS, "%03d", (abs(s->ival) / 6) % 1000 );
              syn->mask |= SYNOP_SEC3;
            }
        }
      else if ( tpd == 3600 )
        {
          if ( check_kj_m2 ( s->val ) == 0 )
            return 0;
          sprintf ( syn->s3.SS, "%02d", abs(s->ival / 6) % 100 );
          syn->mask |= SYNOP_SEC3;
        }
      break;

    case 32: // 0 14 032 Total sunshine (hours)
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          return 0;
        }
      tpd = time_period_duration ( s );
      if ( tpd == ( 24 * 3600 ) )
        {
          if ( check_j_cm2 ( s->val ) == 0 )
            return 0;
          if ( strcmp ( "07", syn->s0.II ) == 0 ) // France
            {
              if ( strcmp ( "06", syn->e.HH ) == 0 )
                {
                  sprintf ( syn->s3.SSS, "%03d", s->ival * 10 );
                  syn->mask |= SYNOP_SEC3;
                }
            }
          else
            {
              sprintf ( syn->s3.SSS, "%03d", s->ival * 10 );
              syn->mask |= SYNOP_SEC3;
            }
        }
      else if ( tpd == 3600 )
        {
          if ( check_kj_m2 ( s->val ) == 0 )
            return 0;
          sprintf ( syn->s3.SS, "%02d", s->ival * 10 );
          syn->mask |= SYNOP_SEC3;
        }
      break;

    default:
      if ( BUFR2TAC_DEBUG_LEVEL > 1 && (s->a->mask & DESCRIPTOR_VALUE_MISSING) == 0 ) 
        bufr2tac_set_error ( s, 0, "syn_parse_x14()", "Descriptor not parsed" );
      break;
    }

  return 0;
}

/*!
  \fn int buoy_parse_x14 ( struct buoy_chunks *b, struct bufr2tac_subset_state *s )
  \brief Parse a expanded descriptor with X = 14
  \param b pointer to a struct \ref buoy_chunks where to set the results
  \param s pointer to a struct \ref bufr2tac_subset_state where is stored needed information in sequential analysis

  It returns 0 if success, 1 if problems when processing. If a descriptor is not processed returns 0 anyway
*/
int buoy_parse_x14 ( struct buoy_chunks *b, struct bufr2tac_subset_state *s )
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
    default:
      if ( BUFR2TAC_DEBUG_LEVEL > 1 && (s->a->mask & DESCRIPTOR_VALUE_MISSING) == 0 ) 
        bufr2tac_set_error ( s, 0, "buoy_parse_x14()", "Descriptor not parsed" );
      break;
    }
  return 0;
}

/*!
  \fn int climat_parse_x14 ( struct climat_chunks *c, struct bufr2tac_subset_state *s )
  \brief Parse a expanded descriptor with X = 14
  \param c pointer to a struct \ref climat_chunks where to set the results
  \param s pointer to a struct \ref bufr2tac_subset_state where is stored needed information in sequential analysis

  It returns 0 if success, 1 if problems when processing. If a descriptor is not processed returns 0 anyway
*/
int climat_parse_x14 ( struct climat_chunks *c, struct bufr2tac_subset_state *s )
{
  if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
    {
      return 0;
    }

  if ( c == NULL )
    {
      return 1;
    }

  switch ( s->a->desc.y )
    {
    case 32: // 0 14 032 . Total sunshine
      if ( s->is_normal == 0 )
        {
          sprintf ( c->s1.S1S1S1, "%03d", s->ival );
          c->mask |= CLIMAT_SEC1;
        }
      else
        {
          sprintf ( c->s2.S1S1S1, "%03d", s->ival );
          c->mask |= CLIMAT_SEC2;
        }
      break;

    case 33: // 0 14 033 . total sunshine percentage
      sprintf ( c->s1.pspsps, "%03d", s->ival );
      c->mask |= CLIMAT_SEC1;
      break;

    default:
      if ( BUFR2TAC_DEBUG_LEVEL > 1 && (s->a->mask & DESCRIPTOR_VALUE_MISSING) == 0 ) 
        bufr2tac_set_error ( s, 0, "climat_parse_x14()", "Descriptor not parsed" );
      break;
    }
  return 0;
}
