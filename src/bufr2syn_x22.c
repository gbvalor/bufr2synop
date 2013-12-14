/***************************************************************************
 *   Copyright (C) 2013 by Guillermo Ballester Valor                       *
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
 \file bufr2syn_x22.c
 \brief decodes the descriptors with X = 22 (oceanographic data)
*/
#include "bufr2synop.h"

/*!
  \fn int syn_parse_x22 ( struct synop_chunks *syn, struct bufr_subset_state *s, char *err )
  \brief Parse a expanded descriptor with X = 01
  \param syn pointer to a struct \ref synop_chunks where to set the results
  \param s pointer to a struct \ref bufr_subset_state where is stored needed information in sequential analysis
  \param err string with optional error 

  It returns 0 if success, 1 if problems when processing. If a descriptor is not processed returns 0 anyway
*/
int syn_parse_x22 ( struct synop_chunks *syn, struct bufr_subset_state *s, char *err )
{
  char aux[16];
  switch ( s->a->desc.y )
    {
    case 1: // 0 22 001
    case 3: // 0 22 003 wind direction 
      if (syn->s2.dw1dw1[0] == 0)
      {
         sprintf(syn->s2.dw1dw1, "%02d", (s->ival + 5)/10);
      }
      else if (syn->s2.dw2dw2[0] == 0)
      {
         sprintf(syn->s2.dw2dw2, "%02d", (s->ival + 5)/10);
      }
      syn->mask |= SYNOP_SEC2; // have sec2 data
     break;
    case 11: // 0 22 011 wind period in seconds 
      if (syn->s2.PwPw[0] == 0)
      {
         sprintf(syn->s2.PwPw, "%02d", (int)(s->val));
         syn->mask |= SYNOP_SEC2; // have sec2 data
      }
    break;
    case 12: // 0 22 012 wind wave period in seconds 
      if (syn->s2.PwaPwa[0] == 0)
      {
         sprintf(syn->s2.PwaPwa, "%02d", (int)(s->val));
         syn->mask |= SYNOP_SEC2; // have sec2 data
      }
    break;
    case 13: // 0 22 013 swell wave period in seconds 
      if (syn->s2.Pw1Pw1[0] == 0)
      {
         sprintf(syn->s2.Pw1Pw1, "%02d", (int)(s->val));
      }
      else if (syn->s2.Pw2Pw2[0] == 0)
      {
         sprintf(syn->s2.Pw2Pw2, "%02d", (int)(s->val));
      }
      syn->mask |= SYNOP_SEC2; // have sec2 data
    break;
    case 21: // 0 22 021  wind wave heigh in m 
      if (syn->s2.HwHw[0] == 0)
      {
         sprintf(syn->s2.HwHw, "%02d", (int)(s->val * 2.0 + 0.01));
         syn->mask |= SYNOP_SEC2; // have sec2 data
      }
    break;
    case 22: // 0 22 022 wind wave heigh in meters 
      if (syn->s2.HwaHwa[0] == 0)
      {
         sprintf(syn->s2.HwaHwa, "%02d", (int)(s->val * 2.0 + 0.01));  // 0.5 m units
         syn->mask |= SYNOP_SEC2; // have sec2 data
      }
      if (syn->s2.HwaHwaHwa[0] == 0)
      {
         sprintf(syn->s2.HwaHwaHwa, "%03d", (int)(s->val * 10.0 + 0.01));  // 0.1 m units
         syn->mask |= SYNOP_SEC2; // have sec2 data
      }
    break;
    case 23: // 0 22 023 swell wave heigh in meters 
      if (syn->s2.Hw1Hw1[0] == 0)
      {
         sprintf(syn->s2.Hw1Hw1, "%02d", (int)(s->val * 2.0 + 0.01));
      }
      else if (syn->s2.Hw2Hw2[0] == 0)
      {
         sprintf(syn->s2.Hw2Hw2, "%02d",(int)(s->val * 2.0 + 0.01));
      }
    syn->mask |= SYNOP_SEC2; // have sec2 data
    break;
    case 42:
    case 43:
    case 45:
    case 49: // 0 22 049 Sea surface temperature
      if (syn->s2.TwTwTw[0] == 0)
        {
          if (kelvin_to_snTTT ( aux, s->val ))
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
