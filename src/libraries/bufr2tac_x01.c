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
 \file bufr2tac_x01.c
 \brief decodes the descriptors with X = 01 (station index, name and country)
*/
#include "bufr2tac.h"

/*!
  \fn int syn_parse_x01 ( struct synop_chunks *syn, struct bufr2tac_subset_state *s )
  \brief Parse a expanded descriptor with X = 01
  \param syn pointer to a struct \ref synop_chunks where to set the results
  \param s pointer to a struct \ref bufr2tac_subset_state where is stored needed information in sequential analysis

  It returns 0 if success, 1 if problems when processing. If a descriptor is not processed returns 0 anyway
*/
int syn_parse_x01 ( struct synop_chunks *syn, struct bufr2tac_subset_state *s )
{
  char aux[80];

  if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
    return 0;

  switch ( s->a->desc.y )
    {
    case 1: // 0 01 001 . WMO block number
      sprintf ( syn->s0.II, "%02d", s->ival );
      break;

    case 2: // 0 01 002 . WMO station number
      sprintf ( syn->s0.iii, "%03d", s->ival );
      break;

    case 3: // 0 01 003 . WMO Region
      sprintf ( syn->s0.A1, "%d", s->ival );
      if ( strcmp ( syn->s0.A1, "1" ) == 0 )
        strcpy ( syn->s0.Reg, "I" );
      else if ( strcmp ( syn->s0.A1, "2" ) == 0 )
        strcpy ( syn->s0.Reg, "II" );
      else if ( strcmp ( syn->s0.A1, "3" ) == 0 )
        strcpy ( syn->s0.Reg, "III" );
      else if ( strcmp ( syn->s0.A1, "4" ) == 0 )
        strcpy ( syn->s0.Reg, "IV" );
      else if ( strcmp ( syn->s0.A1, "5" ) == 0 )
        strcpy ( syn->s0.Reg, "V" );
      else if ( strcmp ( syn->s0.A1, "6" ) == 0 )
        strcpy ( syn->s0.Reg, "VI" );
      break;

    case 4: // 0 01 004 . WMO Subarea
    case 20: // 0 01 020 . WMO region subarea
      sprintf ( syn->s0.bw, "%d", s->ival );
      break;

    case 10: // 0 01 010. Stationary buoy platform identifier
      if ( strlen ( s->a->cval ) < 16 )
        {
          strcpy ( aux, s->a->cval );
          adjust_string ( aux );
          if ( aux[0] == '\xff' )
            break;
          if ( strlen ( aux ) < 10 )
            strcpy ( syn->s0.D_D, aux );
          else
            bufr2tac_set_error ( s, 1, "syn_parse_x01()", "Ship or mobile land station index length >= 10. Cannot set s0.D_D" );
        }
      else if ( BUFR2TAC_DEBUG_LEVEL > 0 )
        {
          bufr2tac_set_error ( s, 1, "syn_parse_x01()", "Ship or mobile land station index length >= 16" );
        }
      break;

    case 11: // 0 01 011. Ship or mobile land station index
      if ( strlen ( s->a->cval ) < 16 )
        {
          strcpy ( aux, s->a->cval );
          adjust_string ( aux );
          if ( aux[0] == '\xff' )
            break;
          if ( strlen ( aux ) < 10 )
            strcpy ( syn->s0.D_D, aux );
          else
            bufr2tac_set_error ( s, 1, "syn_parse_x01()", "Ship or mobile land station index length >= 10. Cannot set s0.D_D" );
        }
      else if ( BUFR2TAC_DEBUG_LEVEL > 0 )
        {
          bufr2tac_set_error ( s, 1, "syn_parse_x01()", "Ship or mobile land station index length >= 16" );
        }
      break;

    case 15: // 0 01 015 . Station or site name
    case 18: // 0 01 018 . Short station or site name
    case 19: // 0 01 019 . Long station or site name
      if ( strlen ( s->a->cval ) <= 80 )
        {
          strcpy ( aux, s->a->cval );
          adjust_string ( aux );
          strcpy ( s->name, aux );
          s->mask |= SUBSET_MASK_HAVE_NAME;
        }
      else if ( BUFR2TAC_DEBUG_LEVEL > 0 )
        {
          bufr2tac_set_error ( s, 1, "syn_parse_x01()", "Station or site name length > 80. Cannot set s->name" );
        }
      break;

    case 101: // 0 01 101 . State identifier
      if ( strlen ( s->a->ctable ) <= 256 )
        {
          strcpy ( aux, s->a->ctable );
          adjust_string ( aux );
          strcpy ( s->country, aux );
          s->mask |= SUBSET_MASK_HAVE_COUNTRY;
        }
      else if ( BUFR2TAC_DEBUG_LEVEL > 0 )
        {
          bufr2tac_set_error ( s, 1, "syn_parse_x01()", "State identifier length > 256. Cannot set s->country" );
        }
      break;

    default:
      if ( BUFR2TAC_DEBUG_LEVEL > 1 && (s->a->mask & DESCRIPTOR_VALUE_MISSING) == 0 ) 
        bufr2tac_set_error ( s, 0, "syn_parse_x01()", "Descriptor not parsed" );
      break;
    }

  //set WMO region if we know II and iii and still no set A1
  if ( ( syn->s0.A1[0] == 0 ) && syn->s0.II[0] && syn->s0.iii[0] )
    {
      guess_WMO_region ( syn->s0.A1, syn->s0.Reg, syn->s0.II, syn->s0.iii );
    }
  return 0;
}

/*!
  \fn int buoy_parse_x01 ( struct buoy_chunks *b, struct bufr2tac_subset_state *s )
  \brief Parse a expanded descriptor with X = 01
  \param b pointer to a struct \ref synop_chunks where to set the results
  \param s pointer to a struct \ref bufr2tac_subset_state where is stored needed information in sequential analysis

  It returns 0 if success, 1 if problems when processing. If a descriptor is not processed returns 0 anyway
*/
int buoy_parse_x01 ( struct buoy_chunks *b, struct bufr2tac_subset_state *s )
{
  char aux[80];

  if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
    return 0;

  switch ( s->a->desc.y )
    {
    case 3: // 0 01 003
      if ( s->ival > 0 )
        sprintf ( b->s0.A1, "%d", s->ival % 10 );
      else if ( s->ival == 0 )
        sprintf ( b->s0.A1, "7" );
      break;

    case 4: // 0 01 004
    case 20: // 0 01 004
      sprintf ( b->s0.bw, "%d", s->ival );
      break;
      
    case 5: // 0 01 005
      if ( s->ival < 1000 )
        sprintf ( b->s0.nbnbnb, "%03d", s->ival % 1000 );
      else
        sprintf ( b->s0.nbnbnb, "%05d", s->ival % 100000 ); // note this is a nnnnn extension
      break;
      
    case 15: // 0 01 015. Station or site name
      if ( strlen ( s->a->cval ) <= 80 )
        {
          strcpy ( aux, s->a->cval );
          adjust_string ( aux );
          strcpy ( s->name, aux );
          s->mask |= SUBSET_MASK_HAVE_NAME;
        }
      else if (BUFR2TAC_DEBUG_LEVEL > 0)
        {
          bufr2tac_set_error ( s, 1, "buoy_parse_x01()", "Station or site name length > 80. Cannot set s->name" );
        }
      break;
      
    case 87: // 0 01 087. WMO marine observing platform extended identifier
      // See https://community.wmo.int/rules-allocating-wmo-numbers
      // A1bwnnn is equivalent to A1Bwnnnnn when nnnnn < 1000
      if ( s->a->val < 100000000 )
        {
          sprintf ( aux, "%07.0lf", s->a->val );
          b->s0.A1[0] = aux[0];
          b->s0.A1[1] = 0;
          b->s0.bw[0] = aux[1];
          b->s0.bw[1] = 0;
          if ( ( ( int ) ( s->a->val ) % 100000 ) < 1000 )
            strcpy ( b->s0.nbnbnb, & aux[4] );
          else
            strcpy ( b->s0.nbnbnb, & aux[2] );
        }
      else if (BUFR2TAC_DEBUG_LEVEL > 0)
        {
          bufr2tac_set_error ( s, 1, "buoy_parse_x01()", 
                               "WMO marine observing platform extended identifier >= 100000000. Cannot set s0.A1, s0.bw, s0.nbnbnb" );
        }
      break;
      
    case 101: // 0 01 101 . State identifier
      if ( strlen ( s->a->ctable ) <= 256 )
        {
          strcpy ( aux, s->a->ctable );
          adjust_string ( aux );
          strcpy ( s->country, aux );
          s->mask |= SUBSET_MASK_HAVE_COUNTRY;
        }
      else if ( BUFR2TAC_DEBUG_LEVEL > 0 )
        {
          bufr2tac_set_error ( s, 1, "buoy_parse_x01()", "State identifier length > 256. Cannot set s->country" );
        }
      break;

    default:
      if ( BUFR2TAC_DEBUG_LEVEL > 1 && (s->a->mask & DESCRIPTOR_VALUE_MISSING) == 0 ) 
        bufr2tac_set_error ( s, 0, "buoy_parse_x01()", "Descriptor not parsed" );
      break;
    }
  return 0;
}

/*!
  \fn int climat_parse_x01 ( struct climat_chunks *c, struct bufr2tac_subset_state *s )
  \brief Parse a expanded descriptor with X = 01
  \param c pointer to a struct \ref climat_chunks where to set the results
  \param s pointer to a struct \ref bufr2tac_subset_state where is stored needed information in sequential analysis

  It returns 0 if success, 1 if problems when processing. If a descriptor is not processed returns 0 anyway
*/
int climat_parse_x01 ( struct climat_chunks *c, struct bufr2tac_subset_state *s )
{
  char aux[80];

  if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
    return 0;

  switch ( s->a->desc.y )
    {

    case 1: // 0 01 001 . WMO block number
      sprintf ( c->s0.II, "%02d", s->ival );
      break;

    case 2: // 0 01 002 . WMO station number
      sprintf ( c->s0.iii, "%03d", s->ival );
      break;

    case 3: // 0 01 003 . WMO Region
      if ( s->ival == 1 )
        strcpy ( c->s0.Reg, "I" );
      else if ( s->ival == 2 )
        strcpy ( c->s0.Reg, "II" );
      else if ( s->ival == 3 )
        strcpy ( c->s0.Reg, "III" );
      else if ( s->ival == 4 )
        strcpy ( c->s0.Reg, "IV" );
      else if ( s->ival == 5 )
        strcpy ( c->s0.Reg, "V" );
      else if ( s->ival == 6 )
        strcpy ( c->s0.Reg, "VI" );
      break;

    case 15: // 0 01 015 . Station or site name
    case 18: // 0 01 018 . Short station or site name
    case 19: // 0 01 019 . Long station or site name
      if ( strlen ( s->a->cval ) <= 80 )
        {
          strcpy ( aux, s->a->cval );
          adjust_string ( aux );
          strcpy ( s->name, aux );
          s->mask |= SUBSET_MASK_HAVE_NAME;
        }
      else if (BUFR2TAC_DEBUG_LEVEL > 0)
        {
          bufr2tac_set_error ( s, 1, "climat_parse_x01()", "Station or site name length > 80. Cannot set s->name" );
        }
      break;


    default:
      if ( BUFR2TAC_DEBUG_LEVEL > 1 && (s->a->mask & DESCRIPTOR_VALUE_MISSING) == 0 ) 
        bufr2tac_set_error ( s, 0, "climat_parse_x01()", "Descriptor not parsed" );
      break;
    }
  return 0;
}

/*!
  \fn int temp_parse_x01 ( struct temp_chunks *t, struct bufr2tac_subset_state *s )
  \brief Parse a expanded descriptor with X = 01
  \param t pointer to a struct \ref temp_chunks where to set the results
  \param s pointer to a struct \ref bufr2tac_subset_state where is stored needed information in sequential analysis

  It returns 0 if success, 1 if problems when processing. If a descriptor is not processed returns 0 anyway
*/
int temp_parse_x01 ( struct temp_chunks *t, struct bufr2tac_subset_state *s )
{
  char aux[80];

  if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
    return 0;

  switch ( s->a->desc.y )
    {
    case 1: // 0 01 001 . WMO block number
      sprintf ( t->a.s1.II, "%02d", s->ival );
      sprintf ( t->b.s1.II, "%02d", s->ival );
      sprintf ( t->c.s1.II, "%02d", s->ival );
      sprintf ( t->d.s1.II, "%02d", s->ival );
      break;
    case 2: // 0 01 002 . WMO station number
      sprintf ( t->a.s1.iii, "%03d", s->ival );
      sprintf ( t->b.s1.iii, "%03d", s->ival );
      sprintf ( t->c.s1.iii, "%03d", s->ival );
      sprintf ( t->d.s1.iii, "%03d", s->ival );
      break;
    case 3: // 0 01 003 . WMO Region
      sprintf ( t->a.s1.A1, "%d", s->ival );
      sprintf ( t->b.s1.A1, "%d", s->ival );
      sprintf ( t->c.s1.A1, "%d", s->ival );
      sprintf ( t->d.s1.A1, "%d", s->ival );
      if ( strcmp ( t->a.s1.A1, "1" ) == 0 )
        {
          strcpy ( t->a.s1.Reg, "I" );
          strcpy ( t->b.s1.Reg, "I" );
          strcpy ( t->c.s1.Reg, "I" );
          strcpy ( t->d.s1.Reg, "I" );
        }
      else if ( strcmp ( t->a.s1.A1, "2" ) == 0 )
        {
          strcpy ( t->a.s1.Reg, "II" );
          strcpy ( t->b.s1.Reg, "II" );
          strcpy ( t->c.s1.Reg, "II" );
          strcpy ( t->d.s1.Reg, "II" );
        }
      else if ( strcmp ( t->a.s1.A1, "3" ) == 0 )
        {
          strcpy ( t->a.s1.Reg, "III" );
          strcpy ( t->b.s1.Reg, "III" );
          strcpy ( t->c.s1.Reg, "III" );
          strcpy ( t->d.s1.Reg, "III" );
        }
      else if ( strcmp ( t->a.s1.A1, "4" ) == 0 )
        {
          strcpy ( t->a.s1.Reg, "IV" );
          strcpy ( t->b.s1.Reg, "IV" );
          strcpy ( t->c.s1.Reg, "IV" );
          strcpy ( t->d.s1.Reg, "IV" );
        }
      else if ( strcmp ( t->a.s1.A1, "5" ) == 0 )
        {
          strcpy ( t->a.s1.Reg, "V" );
          strcpy ( t->b.s1.Reg, "V" );
          strcpy ( t->c.s1.Reg, "V" );
          strcpy ( t->d.s1.Reg, "V" );
        }
      else if ( strcmp ( t->a.s1.A1, "6" ) == 0 )
        {
          strcpy ( t->a.s1.Reg, "VI" );
          strcpy ( t->b.s1.Reg, "VI" );
          strcpy ( t->c.s1.Reg, "VI" );
          strcpy ( t->d.s1.Reg, "VI" );
        }
      break;
    case 4: // 0 01 004 . WMO Subarea
    case 20: // 0 01 020 . WMO region subarea
      sprintf ( t->a.s1.bw, "%d", s->ival );
      sprintf ( t->b.s1.bw, "%d", s->ival );
      sprintf ( t->c.s1.bw, "%d", s->ival );
      sprintf ( t->d.s1.bw, "%d", s->ival );
      break;
    case 11: // 0 01 011. Ship or mobile land station index
      if ( strlen ( s->a->cval ) < 16 )
        {
          strcpy ( aux, s->a->cval );
          adjust_string ( aux );
          if ( aux[0] == '\xff' )
            break;

          if ( strlen ( aux ) < 10 )
            {
              strcpy ( t->a.s1.D_D, aux );
              strcpy ( t->b.s1.D_D, aux );
              strcpy ( t->c.s1.D_D, aux );
              strcpy ( t->d.s1.D_D, aux );
            }
        }
      break;
    case 15: // 0 01 015 . Station or site name
    case 18: // 0 01 018 . Short station or site name
    case 19: // 0 01 019 . Long station or site name
      if ( strlen ( s->a->cval ) <= 80 )
        {
          strcpy ( aux, s->a->cval );
          adjust_string ( aux );
          strcpy ( s->name, aux );
          s->mask |= SUBSET_MASK_HAVE_NAME;
        }
      else if (BUFR2TAC_DEBUG_LEVEL > 0)
        {
          bufr2tac_set_error ( s, 1, "temp_parse_x01()", "Station or site name length > 80. Cannot set s->name" );
        }
      break;
    case 101: // 0 01 101 . State identifier
      if ( strlen ( s->a->ctable ) <= 256 )
        {
          strcpy ( aux, s->a->ctable );
          adjust_string ( aux );
          strcpy ( s->country, aux );
          s->mask |= SUBSET_MASK_HAVE_COUNTRY;
        }
      else if ( BUFR2TAC_DEBUG_LEVEL > 0 )
        {
          bufr2tac_set_error ( s, 1, "temp_parse_x01()", "State identifier length > 256. Cannot set s->country" );
        }
      break;
    default:
      if ( BUFR2TAC_DEBUG_LEVEL > 1 && (s->a->mask & DESCRIPTOR_VALUE_MISSING) == 0 ) 
        bufr2tac_set_error ( s, 0, "climat_parse_x01()", "Descriptor not parsed" );
      break;
    }
  return 0;
}
