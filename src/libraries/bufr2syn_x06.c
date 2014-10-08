/***************************************************************************
 *   Copyright (C) 2013-2014 by Guillermo Ballester Valor                  *
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
 \file bufr2syn_x06.c
 \brief decodes the descriptors with X = 06 (Horizontal Position-2)
 */
#include "bufr2syn.h"


/*!
  \fn char * latlon_to_MMM(char *target, double lat, double lon)
  \brief convert latitude and longitude to a MMM string
  \param lat latitude (degree, N positive)
  \param lon longitude (degree, E positive)
  \param target resulting MMM string
*/
char * latlon_to_MMM(char *target, double lat, double lon)
{
   int col, row, ori = 0;

   if (lon < 0.0)
      col = (int)(-lat * 0.1) + 1;
   else
      col = 36 - (int)(lat * 0.1);

  
   if (lat >= 80.0)
      row = 25;
   else if (lat >=  0.0)
      row = (int)(lat * 0.1);
   else
   {
      ori = 299;
      row = (int)(-lat * 0.1);
   }

   sprintf(target,"%03d", col + ori + row * 36);

   return target;
}

/*!
  \fn int syn_parse_x06 ( struct synop_chunks *syn, struct bufr_subset_state *s )
  \brief Parse a expanded descriptor with X = 06
  \param syn pointer to a struct \ref synop_chunks where to set the results
  \param s pointer to a struct \ref bufr_subset_state where is stored needed information in sequential analysis
  \param err string with optional error 

  It returns 0 if success, 1 if problems when processing. If a descriptor is not processed returns 0 anyway
*/
int syn_parse_x06 ( struct synop_chunks *syn, struct bufr_subset_state *s )
{
  int ia;

  if ( s->a->mask & DESCRIPTOR_VALUE_MISSING)
    return 0;

  switch ( s->a->desc.y )
    {
    case 1: // 0 06 001
    case 2: // 0 06 002
      if (s->val < 0.0)
        s->mask |= SUBSET_MASK_LONGITUDE_WEST; // Sign for longitude
      s->mask |= SUBSET_MASK_HAVE_LONGITUDE;
      ia = (int) (fabs(s->val) * 10.0 + 0.5);
      sprintf(syn->s0.LoLoLoLo, "%04d",ia);
      syn->s0.Ulo[0] = syn->s0.LoLoLoLo[2];
      s->lon = s->val;
      break;
    default:
      break;
    }

  // check if set both LaLaLa and LoLoLoLo to set Qc
  if ((syn->s0.Qc[0] == 0) && syn->s0.LaLaLa[0] && syn->s0.LoLoLoLo[0])
    {
      if (s->mask & SUBSET_MASK_LATITUDE_SOUTH)
        {
          if (s->mask & SUBSET_MASK_LONGITUDE_WEST)
            strcpy(syn->s0.Qc, "5");
          else
            strcpy(syn->s0.Qc, "3");
        }
      else
        {
          if (s->mask & SUBSET_MASK_LONGITUDE_WEST)
            strcpy(syn->s0.Qc, "7");
          else
            strcpy(syn->s0.Qc, "1");
        }
    }

  // check if about MMM
  if ((syn->s0.MMM[0] == 0) && syn->s0.LaLaLa[0] && syn->s0.LoLoLoLo[0])
  {
     latlon_to_MMM(syn->s0.MMM, s->lat, s->lon);
  }

  return 0;
}

/*!
  \fn int buoy_parse_x06 ( struct buoy_chunks *b, struct bufr_subset_state *s )
  \brief Parse a expanded descriptor with X = 06
  \param b pointer to a struct \ref buoy_chunks where to set the results
  \param s pointer to a struct \ref bufr_subset_state where is stored needed information in sequential analysis
  \param err string with optional error 

  It returns 0 if success, 1 if problems when processing. If a descriptor is not processed returns 0 anyway
*/
int buoy_parse_x06 ( struct buoy_chunks *b, struct bufr_subset_state *s )
{
  int ia;

  if ( s->a->mask & DESCRIPTOR_VALUE_MISSING)
    return 0;

  switch ( s->a->desc.y )
    {
    case 1: // 0 06 001
    case 2: // 0 06 002
      if (s->val < 0.0)
        s->mask |= SUBSET_MASK_LONGITUDE_WEST; // Sign for longitude
      s->mask |= SUBSET_MASK_HAVE_LONGITUDE;
      s->lon = s->val;
      ia = (int) (fabs(s->val) * 1000.0 + 0.5);
      sprintf(b->s0.LoLoLoLoLoLo, "%06d",ia);
      break;
      default:
      break;
    }
  return 0;
}
