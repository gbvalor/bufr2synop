/***************************************************************************
 *   Copyright (C) 2012 by Guillermo Ballester Valor                       *
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


int syn_parse_x22 ( struct synop_chunks *syn, struct bufr_subset_state *s, char *err )
{
  char aux[16];
  switch ( s->a->desc.y )
    {
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
    case 13: // 0 22 013 wave period in seconds 
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
    case 21: // 0 22 021 wind heigh in m 
      if (syn->s2.HwHw[0] == 0)
      {
         sprintf(syn->s2.HwHw, "%02d", (int)(s->val));
         syn->mask |= SYNOP_SEC2; // have sec2 data
      }
    break;
    case 22: // 0 22 022 wind wave heigh in meters 
      if (syn->s2.HwaHwa[0] == 0)
      {
         sprintf(syn->s2.HwaHwa, "%02d", (int)(s->val));
         syn->mask |= SYNOP_SEC2; // have sec2 data
      }
    break;
    case 23: // 0 22 023 wave heigh in meters 
      if (syn->s2.Hw1Hw1[0] == 0)
      {
         sprintf(syn->s2.Hw1Hw1, "%02d", (int)(s->val));
      }
      else if (syn->s2.Hw2Hw2[0] == 0)
      {
         sprintf(syn->s2.Hw2Hw2, "%02d",(int)(s->val));
      }
    syn->mask |= SYNOP_SEC2; // have sec2 data
    break;
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
