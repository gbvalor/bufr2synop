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
 \file bufr2tac_x05.c
 \brief decodes the descriptors with X = 05 (Position-1)
 */
#include "bufr2tac.h"

/*!
  \fn char * grad_to_D ( char *D, double grad )
  \brief Converts true direction in grads to D (code table 0700)
  \param grad the true direction (degrees)
  \param D the resulting code
*/
char * grad_to_D ( char *D, double grad )
{
  if ( grad >= 22.5 && grad < 67.5 )
    strcpy ( D, "1" );
  else if ( grad >= 67.5 && grad < 112.5 )
    strcpy ( D, "2" );
  else if ( grad >= 112.5 && grad < 157.5 )
    strcpy ( D, "3" );
  else if ( grad >= 157.5 && grad < 202.5 )
    strcpy ( D, "4" );
  else if ( grad >= 202.5 && grad < 247.5 )
    strcpy ( D, "5" );
  else if ( grad >= 247.5 && grad < 292.5 )
    strcpy ( D, "6" );
  else if ( grad >= 292.5 && grad < 337.5 )
    strcpy ( D, "7" );
  else if ( ( grad >= 337.5 && grad <= 360.0 ) ||
            ( grad >0.0 && grad < 22.5 ) )
    strcpy ( D, "8" );
  else if ( grad == 0.0 )
    strcpy ( D, "0" );
  else
    strcpy ( D, "9" );
  return D;
}

/*!
  \fn int syn_parse_x05 ( struct synop_chunks *syn, struct bufr2tac_subset_state *s )
  \brief Parse a expanded descriptor with X = 05
  \param syn pointer to a struct \ref synop_chunks where to set the results
  \param s pointer to a struct \ref bufr2tac_subset_state where is stored needed information in sequential analysis

  It returns 0 if success, 1 if problems when processing. If a descriptor is not processed returns 0 anyway
*/
int syn_parse_x05 ( struct synop_chunks *syn, struct bufr2tac_subset_state *s )
{
  int ia;

  if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
    return 0;

  switch ( s->a->desc.y )
    {
    case 1: // 0 05 001 . Latitude (High accuracy)
    case 2: // 0 05 002 . Latitude (Coarse accuracy)
      if ( s->val < 0.0 )
        s->mask |= SUBSET_MASK_LATITUDE_SOUTH; // Sign for latitude
      s->mask |= SUBSET_MASK_HAVE_LATITUDE;
      ia = ( int ) ( fabs ( s->val ) * 10.0 + 0.5 );
      if (ia > 999 || ia < 0)
        return 1;
      sprintf ( syn->s0.LaLaLa, "%03d", ia );
      syn->s0.Ula[0] = syn->s0.LaLaLa[1];
      s->lat = s->val;
      break;

    case 21: // 0 05 021 . Bearing or azimut
      grad_to_D ( syn->s3.Da, s->val );
      syn->mask |= SYNOP_SEC3;
      break
      ;
    default:
      break;
    }

  // check if set both LaLaLa and LoLoLoLo to set Qc
  if ( ( syn->s0.Qc[0] == 0 ) && syn->s0.LaLaLa[0] && syn->s0.LoLoLoLo[0] )
    {
      if ( s->mask & SUBSET_MASK_LATITUDE_SOUTH )
        {
          if ( s->mask & SUBSET_MASK_LONGITUDE_WEST )
            strcpy ( syn->s0.Qc, "5" );
          else
            strcpy ( syn->s0.Qc, "3" );
        }
      else
        {
          if ( s->mask & SUBSET_MASK_LONGITUDE_WEST )
            strcpy ( syn->s0.Qc, "7" );
          else
            strcpy ( syn->s0.Qc, "1" );
        }
    }

  // check about Mardsen square
  if ( ( syn->s0.MMM[0] == 0 ) && syn->s0.LaLaLa[0] && syn->s0.LoLoLoLo[0] )
    {
      latlon_to_MMM ( syn->s0.MMM, s->lat, s->lon ); // compute it
    }
  return 0;
}

/*!
  \fn int buoy_parse_x05 ( struct buoy_chunks *b, struct bufr2tac_subset_state *s )
  \brief Parse a expanded descriptor with X = 05
  \param b pointer to a struct \ref buoy_chunks where to set the results
  \param s pointer to a struct \ref bufr2tac_subset_state where is stored needed information in sequential analysis

  It returns 0 if success, 1 if problems when processing. If a descriptor is not processed returns 0 anyway
*/
int buoy_parse_x05 ( struct buoy_chunks *b, struct bufr2tac_subset_state *s )
{
  int ia;

  if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
    return 0;

  switch ( s->a->desc.y )
    {
    case 1: // 0 05 001 . Latitude (High accuracy)
    case 2: // 0 05 002 . Latitude (Coarse accuracy)
      if ( s->val < 0.0 )
        s->mask |= SUBSET_MASK_LATITUDE_SOUTH; // Sign for latitude
      s->mask |= SUBSET_MASK_HAVE_LATITUDE;
      ia = ( int ) ( fabs ( s->val ) * 1000.0 + 0.5 );
      sprintf ( b->s0.LaLaLaLaLa, "%05d",ia );
      s->lat = s->val;
      break;
    default:
      break;
    }

  return 0;
}

/*!
  \fn int climat_parse_x05 ( struct climat_chunks *c, struct bufr2tac_subset_state *s )
  \brief Parse a expanded descriptor with X = 05
  \param c pointer to a struct \ref climat_chunks where to set the results
  \param s pointer to a struct \ref bufr2tac_subset_state where is stored needed information in sequential analysis

  It returns 0 if success, 1 if problems when processing. If a descriptor is not processed returns 0 anyway
*/
int climat_parse_x05 ( struct climat_chunks *c, struct bufr2tac_subset_state *s )
{
  if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
    return 0;

  // this is to avoid warning
  if ( c == NULL )
    return 1;

  switch ( s->a->desc.y )
    {
    case 1: // 0 05 001 . Latitude (High accuracy)
    case 2: // 0 05 002 . Latitude (Coarse accuracy)
      if ( s->val < 0.0 )
        s->mask |= SUBSET_MASK_LATITUDE_SOUTH; // Sign for latitude
      s->mask |= SUBSET_MASK_HAVE_LATITUDE;
      s->lat = s->val;
      break;
    default:
      break;
    }
  return 0;
}

/*!
  \fn int temp_parse_x05 ( struct climat_chunks *syn, struct bufr2tac_subset_state *s )
  \brief Parse a expanded descriptor with X = 05
  \param t pointer to a struct \ref climat_chunks where to set the results
  \param s pointer to a struct \ref bufr2tac_subset_state where is stored needed information in sequential analysis

  It returns 0 if success, 1 if problems when processing. If a descriptor is not processed returns 0 anyway
*/
int temp_parse_x05 ( struct temp_chunks *t, struct bufr2tac_subset_state *s )
{
  int ia;

  if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
    return 0;

  // this is to avoid warning
  if ( t == NULL )
    return 1;

  switch ( s->a->desc.y )
    {
    case 1: // 0 05 001 . Latitude (High accuracy)
    case 2: // 0 05 002 . Latitude (Coarse accuracy)
      if ( s->val < 0.0 )
        s->mask |= SUBSET_MASK_LATITUDE_SOUTH; // Sign for latitude
      s->mask |= SUBSET_MASK_HAVE_LATITUDE;
      s->lat = s->val;
      ia = ( int ) ( fabs ( s->val ) * 10.0 + 0.5 );
      sprintf ( t->a.s1.LaLaLa, "%03d",ia );
      sprintf ( t->b.s1.LaLaLa, "%03d",ia );
      sprintf ( t->c.s1.LaLaLa, "%03d",ia );
      sprintf ( t->d.s1.LaLaLa, "%03d",ia );
      t->a.s1.Ula[0] = t->a.s1.LaLaLa[1];
      t->b.s1.Ula[0] = t->b.s1.LaLaLa[1];
      t->c.s1.Ula[0] = t->c.s1.LaLaLa[1];
      t->d.s1.Ula[0] = t->d.s1.LaLaLa[1];

      // check if set both LaLaLa and LoLoLoLo to set Qc
      if ( ( t->a.s1.Qc[0] == 0 ) && t->a.s1.LaLaLa[0] && t->a.s1.LoLoLoLo[0] )
        {
          if ( s->mask & SUBSET_MASK_LATITUDE_SOUTH )
            {
              if ( s->mask & SUBSET_MASK_LONGITUDE_WEST )
                strcpy ( t->a.s1.Qc, "5" );
              else
                strcpy ( t->a.s1.Qc, "3" );
            }
          else
            {
              if ( s->mask & SUBSET_MASK_LONGITUDE_WEST )
                strcpy ( t->a.s1.Qc, "7" );
              else
                strcpy ( t->a.s1.Qc, "1" );
            }
          strcpy ( t->b.s1.Qc, t->a.s1.Qc );
          strcpy ( t->c.s1.Qc, t->a.s1.Qc );
          strcpy ( t->d.s1.Qc, t->a.s1.Qc );
        }

      // check if about MMM
      if ( ( t->a.s1.MMM[0] == 0 ) && t->a.s1.LaLaLa[0] && t->a.s1.LoLoLoLo[0] )
        {
          latlon_to_MMM ( t->a.s1.MMM, s->lat, s->lon );
          strcpy ( t->b.s1.MMM, t->a.s1.MMM );
          strcpy ( t->c.s1.MMM, t->a.s1.MMM );
          strcpy ( t->d.s1.MMM, t->a.s1.MMM );
        }
      break;

    case 15: // 0 05 015. Latitude displacement since launch site (high accuracy)
      if ( s->rep > 0 && s->r->n > 0 )
        {
          s->r->raw[s->r->n - 1].dlat = s->val;
        }
      else if ( s->w->n > 0 )
        {
          s->w->raw[s->w->n - 1].dlat = s->val;
        }
      break;

    default:
      break;
    }


  return 0;
}
