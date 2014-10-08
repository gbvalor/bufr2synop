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
 \file bufr2syn_x11.c
 \brief decodes the descriptors with X = 11
 */
#include "bufr2syn.h"

char * secs_to_tt(char *tt, int secs)
{
  int i;

  i = (-secs) / 360;
  if (i <= 60)
    sprintf(tt, "%02d", i);
  else if (i < 700)
    sprintf(tt, "61");
  else if (i < 800)
    sprintf(tt, "62");
  else if (i < 900)
    sprintf(tt, "63");
  else if (i < 1000)
    sprintf(tt, "64");
  else if (i < 1100)
    sprintf(tt, "65");
  else if (i < 1200)
    sprintf(tt, "66");
  else if (i < 1800)
    sprintf(tt, "67");
  else
    sprintf(tt, "68");
  return tt;
}

/*!
  \fn int syn_parse_x11 ( struct synop_chunks *syn, struct bufr_subset_state *s )
  \brief Parse a expanded descriptor with X = 11
  \param syn pointer to a struct \ref synop_chunks where to set the results
  \param s pointer to a struct \ref bufr_subset_state where is stored needed information in sequential analysis
  \param err string with optional error

  It returns 0 if success, 1 if problems when processing. If a descriptor is not processed returns 0 anyway
*/
int syn_parse_x11 ( struct synop_chunks *syn, struct bufr_subset_state *s )
{

  if ( s->a->mask & DESCRIPTOR_VALUE_MISSING)
    return 0;

  switch ( s->a->desc.y )
    {
    case 1: // 0 11 001
    case 11: // 0 11 011
      sprintf ( syn->s1.dd, "%02d", ( s->ival + 5 ) / 10 );
      syn->mask |= SYNOP_SEC1;
      break;
    case 2: // 0 11 002
    case 12: // 0 11 012
      if ( syn->s0.iw[0] == '4' )
        s->val *= 1.94384449;
      if ( s->val < 100.0 )
        sprintf ( syn->s1.ff, "%02d", ( int ) ( s->val + 0.5 ) );
      else
        {
          sprintf ( syn->s1.ff,"99" );
          sprintf ( syn->s1.fff, "%03d", ( int ) ( s->val + 0.5 ) );
        }
      syn->mask |= SYNOP_SEC1;
      break;
    case 41: // max wind gust speed
      if (s->itval == -600)
        {
          sprintf( syn->s3.d9.misc[syn->s3.d9.n].SpSp, "910");
          if ( syn->s0.iw[0] == '4' )
            s->val *= 1.94384449;
          if ( s->val < 100.0 )
            sprintf ( syn->s3.d9.misc[syn->s3.d9.n].spsp, "%02d", ( int ) ( s->val + 0.5 ) );
          else
            {
              sprintf ( syn->s3.d9.misc[syn->s3.d9.n].spsp,"99" );
              syn->s3.d9.n++;
              sprintf( syn->s3.d9.misc[syn->s3.d9.n].SpSp, "00");
              sprintf ( syn->s3.d9.misc[syn->s3.d9.n].spsp, "%03d", ( int ) ( s->val + 0.5 ) );
            }
          syn->s3.d9.n++;
        }
      else if (s->itval)
        {
          if (s->mask & SUBSET_MASK_HAVE_GUST)
            {
              sprintf( syn->s5.d9.misc[syn->s5.d9.n].SpSp, "907");
              secs_to_tt(syn->s5.d9.misc[syn->s5.d9.n].spsp, s->itval);
              syn->s5.d9.n++;
              sprintf( syn->s5.d9.misc[syn->s5.d9.n].SpSp, "911");
              if ( syn->s0.iw[0] == '4' )
                s->val *= 1.94384449;
              if ( s->val < 100.0 )
                sprintf ( syn->s5.d9.misc[syn->s5.d9.n].spsp, "%02d", ( int ) ( s->val + 0.5 ) );
              else
                {
                  sprintf ( syn->s5.d9.misc[syn->s5.d9.n].spsp,"99" );
                  syn->s5.d9.n++;
                  sprintf( syn->s5.d9.misc[syn->s5.d9.n].SpSp, "00");
                  sprintf ( syn->s5.d9.misc[syn->s5.d9.n].spsp, "%03d", ( int ) ( s->val + 0.5 ) );
                }
              syn->s5.d9.n++;
              syn->mask |= SYNOP_SEC5;
            }
          else
            {
              sprintf( syn->s3.d9.misc[syn->s3.d9.n].SpSp, "907");
              secs_to_tt(syn->s3.d9.misc[syn->s3.d9.n].spsp, s->itval);
              syn->s3.d9.n++;
              sprintf( syn->s3.d9.misc[syn->s3.d9.n].SpSp, "911");
              if ( syn->s0.iw[0] == '4' )
                s->val *= 1.94384449;
              if ( s->val < 100.0 )
                sprintf ( syn->s3.d9.misc[syn->s3.d9.n].spsp, "%02d", ( int ) ( s->val + 0.5 ) );
              else
                {
                  sprintf ( syn->s3.d9.misc[syn->s3.d9.n].spsp,"99" );
                  syn->s3.d9.n++;
                  sprintf( syn->s3.d9.misc[syn->s3.d9.n].SpSp, "00");
                  sprintf ( syn->s3.d9.misc[syn->s3.d9.n].spsp, "%03d", ( int ) ( s->val + 0.5 ) );
                }
              syn->s3.d9.n++;
              syn->mask |= SYNOP_SEC3;
              s->mask |= SUBSET_MASK_HAVE_GUST;
            }
        }
      break;
    case 43: // max wind gust direction
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
  \param err string with optional error

  It returns 0 if success, 1 if problems when processing. If a descriptor is not processed returns 0 anyway
*/
int buoy_parse_x11 ( struct buoy_chunks *b, struct bufr_subset_state *s )
{

  if ( s->a->mask & DESCRIPTOR_VALUE_MISSING)
    return 0;

  switch ( s->a->desc.y )
    {
    case 1: // 0 11 001
    case 11: // 0 11 011
      sprintf ( b->s1.dd, "%02d", ( s->ival + 5 ) / 10 );
      b->mask |= BUOY_SEC1;
      break;
    case 2: // 0 11 002
    case 12: // 0 11 012
      if ( b->s0.iw[0] == '4' )
        s->val *= 1.94384449;
      if ( s->val < 100.0 )
        sprintf ( b->s1.ff, "%02d", ( int ) ( s->val + 0.5 ) );
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
