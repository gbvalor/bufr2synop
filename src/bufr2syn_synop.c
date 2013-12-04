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
 \file bufr2syn_synop.c
 \brief file with the code to parse a sequence as a SYNOP FM-12
 */
#include "bufr2synop.h"

/*!
  \fn int synop_YYYYMMDDHHmm_to_YYGG(struct synop_chunks *syn)
  \brief Sets YYGG from YYYYMMDDHHmm extended group
  \param syn pointer to the target struct \ref synop_chunks
*/
int synop_YYYYMMDDHHmm_to_YYGG(struct synop_chunks *syn)
{
   char aux[20];
   time_t t;
   struct tm tim;

   if (strlen(syn->e.YYYY) && 
       strlen(syn->e.MM) &&
       strlen(syn->e.DD) &&
       strlen(syn->e.HH) &&
       strlen(syn->e.MM))
    sprintf(aux,"%s%s%s%s%s", syn->e.YYYY, syn->e.MM, syn->e.DD, syn->e.HH, syn->e.mm);

    if (strlen(aux) != 12)
       return 1;

    memset(&tim, 0, sizeof(struct tm));
    strptime(aux, "%Y%m%d%H%M", &tim);

    t = mktime(&tim) + 1799 ; // rounding trick
    gmtime_r(&t, &tim);
    sprintf(syn->s0.YY, "%02d", tim.tm_mday); 
    sprintf(syn->s0.GG, "%02d", tim.tm_hour); 
    return 0;
}

/*!
  \fn int parse_subset_as_aaxx(struct synop_chunks *syn, struct bufr_subset_sequence_data *sq, int *kdtlst, size_t nlst, int *ksec1, char *err)
  \brief parses a subset sequence as an Land fixed SYNOP-12 FM report
  \param syn pointer to a struct \ref synop_chunks where set the results
  \param sq pointer to a struct \ref bufr_subset_sequence_data with the parsed sequence on input
  \param kdtlst array of descriptors (as integers) from the non-expanded bufr list
  \param nlst size of kdtlst array
  \param ksec1 array of integers set by ECMWF bufrdc library
  \param err string with detected errors, if any

  It return 0 if all is OK. Otherwise it also fills the \a err string
*/
int parse_subset_as_aaxx(struct synop_chunks *syn, struct bufr_subset_sequence_data *sq, int *kdtlst, size_t nlst, int *ksec1, char *err)
{
    size_t is;
    time_t tt;
    struct tm tim;
    char aux[16];

    // clean data
    clean_synop_chunks(syn);

    sprintf(syn->s0.MiMi, "AA");
    sprintf(syn->s0.MjMj, "XX");

    for (is = 0; is < sq->nd; is++)
    {
       if (sq->sequence[is].mask & DESCRIPTOR_VALUE_MISSING)
         continue;
       switch (sq->sequence[is].desc.x)
       {
          case 1: //localization descriptors
            switch(sq->sequence[is].desc.y)
            {
              case 1: // 0 01 001
                 sprintf(syn->s0.II, "%02d", (int) sq->sequence[is].val);
              break;
              case 2: // 0 01 002
                 sprintf(syn->s0.iii, "%03d", (int) sq->sequence[is].val);
              break;
              default:
              break;
            }
          break;
          case 4: //Date time descriptors
            switch(sq->sequence[is].desc.y)
            {
              case 1: // 0 04 001
                 sprintf(syn->e.YYYY, "%04d", (int) sq->sequence[is].val);
              break;
              case 2: // 0 04 002
                 sprintf(syn->e.MM, "%02d", (int) sq->sequence[is].val);
              break;
              case 3: // 0 04 003
                 sprintf(syn->e.DD, "%02d", (int) sq->sequence[is].val);
                 //sprintf(syn->s0.YY, "%02d", (int) sq->sequence[is].val);
              break;
              case 4: // 0 04 004
                 sprintf(syn->e.HH, "%02d", (int) sq->sequence[is].val);
                 //sprintf(syn->s0.GG, "%02d", (int) sq->sequence[is].val);
              break;
              case 5: // 0 04 005
                 sprintf(syn->e.mm, "%02d", (int) sq->sequence[is].val);
              break;
            }
          break;
          case 2: // Intrumentation
            {
              switch(sq->sequence[is].desc.y)
              {
              case 2: // 0 02 002
                 //sprintf(syn->e.iw, "%02d", (int) sq->sequence[is].val);
              break;
              }
            }
          default:
          break;
       }

    }

    // fix YYGG according with YYYYMMDDHHmm
    synop_YYYYMMDDHHmm_to_YYGG(syn);
    return 0;
}

/*!
 \fn int print_synop(char *report, size_t lmax, struct synop_chunks *syn)
 \brief prints a synop into a string
 \param report target string
 \param lmax max size of string
 \param syn pointer to a struct \ref synop_chuncks with the result of parse tasks 

 returns 0 if all went right
*/
int print_synop(char *report, size_t lmax, struct synop_chunks *syn)
{
   char *c;

   c = report;
   // print time extension
   c += sprintf(c, "%s%s%s%s%s", syn->e.YYYY, syn->e.MM, syn->e.DD, syn->e.HH, syn->e.mm);

   // Print type
   c += sprintf(c, " %s%s", syn->s0.MiMi, syn->s0.MjMj);

   // print IIiii
   c += sprintf(c, " %s%s", syn->s0.II, syn->s0.iii);

   // print YYGGiw
   c += sprintf(c, " %s%s/", syn->s0.YY, syn->s0.GG);

   c += sprintf(c, "=");

   return 0;
}