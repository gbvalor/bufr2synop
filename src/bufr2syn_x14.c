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
 \file bufr2syn_x14.c
 \brief decodes the descriptors with X = 14 (Radiation)
 */
#include "bufr2syn.h"

/*!
  \fn int syn_parse_x14 ( struct synop_chunks *syn, struct bufr_subset_state *s, char *err )
  \brief Parse a expanded descriptor with X = 14
  \param syn pointer to a struct \ref synop_chunks where to set the results
  \param s pointer to a struct \ref bufr_subset_state where is stored needed information in sequential analysis
  \param err string with optional error 

  It returns 0 if success, 1 if problems when processing. If a descriptor is not processed returns 0 anyway
*/
int syn_parse_x14 ( struct synop_chunks *syn, struct bufr_subset_state *s, char *err )
{
  switch ( s->a->desc.y )
    {
    case 1: // 0 14 001 long-wave radiation 24 hours
    case 11:
      if (syn->s3.SSS[0])
        {
          strcpy(syn->s3.j524[4], "4");
          sprintf(syn->s3.FFFF24[4], "%04d", s->ival / 10000);
          syn->mask |= SYNOP_SEC3;
        }
      break;
    case 2: // 0 14 002 Long-wave radiation
    case 12:
      if (s->itval == (-24 * 3600))
        {
          if (syn->s3.SSS[0])
            {
              strcpy(syn->s3.j524[4], "4");
              sprintf(syn->s3.FFFF24[4], "%04d", s->ival / 10000);
              syn->mask |= SYNOP_SEC3;
            }
        }
      else if (s->itval == -3600)
        {
          if (syn->s3.SS[0])
            {
              strcpy(syn->s3.j5[4], "4");
              sprintf(syn->s3.FFFF[4], "%04d", s->ival / 1000);
            }
        }
      syn->mask |= SYNOP_SEC3;
      break;
    case 3: // 0 14 001 short-wave radiation 24 hours
    case 13:
      if (syn->s3.SSS[0])
        {
          strcpy(syn->s3.j524[6], "6");
          sprintf(syn->s3.FFFF24[6], "%04d", s->ival / 10000);
          syn->mask |= SYNOP_SEC3;
        }
      break;
    case 4: // 0 14 002 short-wave radiation
    case 14:
      if (s->itval == (-24 * 3600))
        {
          if (syn->s3.SSS[0])
            {
              strcpy(syn->s3.j524[6], "6");
              sprintf(syn->s3.FFFF24[6], "%04d", s->ival / 10000);
            }
        }
      else if (s->itval == -3600)
        {
          if (syn->s3.SS[0])
            {
              strcpy(syn->s3.j5[6], "6");
              sprintf(syn->s3.FFFF[6], "%04d", s->ival / 1000);
            }
        }
      syn->mask |= SYNOP_SEC3;
      break;
    case 31: // 0 14 031 Total sunshine
      if (s->itval == (-24 * 3600))
        {
          sprintf(syn->s3.SSS, "%03d", s->ival / 6);
        }
      else if (s->itval == -3600)
        {
          sprintf(syn->s3.SS, "%02d", s->ival / 6);
        }
      syn->mask |= SYNOP_SEC3;
      break;
    case 32: // 0 14 032 Total sunshine (hours)
      if (s->itval == (-24 * 3600))
        {
          sprintf(syn->s3.SSS, "%03d", s->ival * 10);
        }
      else if (s->itval == -3600)
        {
          sprintf(syn->s3.SS, "%02d", s->ival * 10);
        }
      syn->mask |= SYNOP_SEC3;
      break;
    default:
      break;
    }

  return 0;
}

/*!
  \fn int buoy_parse_x14 ( struct buoy_chunks *b, struct bufr_subset_state *s, char *err )
  \brief Parse a expanded descriptor with X = 14
  \param b pointer to a struct \ref buoy_chunks where to set the results
  \param s pointer to a struct \ref bufr_subset_state where is stored needed information in sequential analysis
  \param err string with optional error 

  It returns 0 if success, 1 if problems when processing. If a descriptor is not processed returns 0 anyway
*/
int buoy_parse_x14 ( struct buoy_chunks *b, struct bufr_subset_state *s, char *err )
{
  char aux[16];

  switch ( s->a->desc.y )
    {
      default:
      break;
    }
  return 0;
}
