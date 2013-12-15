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
 \file bufr2syn_buoy.c
 \brief file with the code to parse buoy squences
 */
#include "bufr2synop.h"

/*!
  \fn int buoy_YYYYMMDDHHmm_to_YYGG(struct synop_chunks *syn)
  \brief Sets YYGG from YYYYMMDDHHmm extended group
  \param syn pointer to the target struct \ref synop_chunks
*/
int buoy_YYYYMMDDHHmm_to_JMMYYGGgg ( struct buoy_chunks *b )
{
  char aux[20];
  time_t t;
  struct tm tim;

  if ( strlen ( b->e.YYYY ) &&
       strlen ( b->e.MM ) &&
       strlen ( b->e.DD ) &&
       strlen ( b->e.HH ) &&
       strlen ( b->e.MM ) )
    sprintf ( aux,"%s%s%s%s%s", b->e.YYYY, b->e.MM, b->e.DD, b->e.HH, b->e.mm );

  if ( strlen ( aux ) != 12 )
    return 1;

  memset ( &tim, 0, sizeof ( struct tm ) );
  strptime ( aux, "%Y%m%d%H%M", &tim );

  t = mktime ( &tim );
  gmtime_r ( &t, &tim );
  sprintf ( b->s0.YY, "%02d", tim.tm_mday );
  sprintf ( b->s0.GG, "%02d", tim.tm_hour );
  sprintf ( b->s0.MM, "%02d", tim.tm_mon + 1);
  sprintf ( b->s0.J, "%d", tim.tm_year % 10);
  sprintf ( b->s0.gg, "%02d", tim.tm_min);

  return 0;
}



/*!
  \fn int parse_subset_as_buoy(struct buoy_chunks *b, struct bufr_subset_sequence_data *sq, int *kdtlst, size_t nlst, int *ksec1, char *err)
  \brief parses a subset sequence as an Buoy SYNOP FM-18 report
  \param b pointer to a struct \ref buoy_chunks where set the results
  \param sq pointer to a struct \ref bufr_subset_sequence_data with the parsed sequence on input
  \param kdtlst array of descriptors (as integers) from the non-expanded bufr list
  \param nlst size of kdtlst array
  \param ksec1 array of integers set by ECMWF bufrdc library
  \param err string with detected errors, if any

  It return 0 if all is OK. Otherwise it also fills the \a err string
*/
int parse_subset_as_buoy( struct buoy_chunks *b, struct bufr_subset_sequence_data *sq, int *kdtlst, size_t nlst, int *ksec1, char *err )
{
  int ival; // integer value for a descriptor
  double val; // double value for a descriptor
  size_t is; 
  int disp; // time displacement in seconds
  time_t tt, itval;
  struct tm tim;
  struct bufr_subset_state s; // stores util data when parsing sequence
  char aux[16];

  // clean data
  clean_buoy_chunks ( b );
  memset(&s, 0, sizeof(struct bufr_subset_state));

  // reject if still not coded type
  if (strcmp(TYPE,"ZZYY"))
  {
    sprintf(err,"%: '%s' report still not decoded in this software", SELF, TYPE);
    return 1;
  }

  strcpy(b->s0.MiMi, "ZZ");
  strcpy(b->s0.MjMj, "YY");

  b->mask = BUOY_SEC0;

  /**** First pass, sequential analysis *****/
  for ( is = 0, itval = 0; is < sq->nd; is++ )
    {
      if ( sq->sequence[is].mask & DESCRIPTOR_VALUE_MISSING )
        continue;
      s.i = is;
      s.ival = ( int ) sq->sequence[is].val;
      ival = ival;
      s.val = sq->sequence[is].val;
      val = s.val;
      s.a = &sq->sequence[is];
      switch ( sq->sequence[is].desc.x )
        {

        case 1: //localization descriptors
          buoy_parse_x01 ( b, &s, err );
          break;

        case 2: //Date time descriptors
          buoy_parse_x02 ( b, &s, err );
          break;

        case 4: //Date time descriptors
          buoy_parse_x04 ( b, &s, err );
          break;

        case 5: //Position
          buoy_parse_x05 ( b, &s, err );
          break;

        case 6: // Horizontal Position -2
          buoy_parse_x06 ( b, &s, err );
          break;

        case 10: // Air Pressure descriptors
          buoy_parse_x10 ( b, &s, err );
          break;

        case 11: // wind  data
          buoy_parse_x11 ( b, &s, err );
          break;

        case 12: //Temperature descriptors
          buoy_parse_x12 ( b, &s, err );
          break;

        case 13: // Humidity and precipitation data
          buoy_parse_x13 ( b, &s, err );
          break;

        case 14: // Radiation
          buoy_parse_x14 ( b, &s, err );
          break;

        case 20: // Cloud data
          buoy_parse_x20 ( b, &s, err );
          break;

        case 22: // Oceanographic data
          buoy_parse_x22 ( b, &s, err );
          break;

        case 31: // Replicators
          buoy_parse_x31 ( b, &s, err );
          break;


        case 33: // Quality data
          buoy_parse_x33 ( b, &s, err );
          break;


        default:
          break;
        }

    }

  /****** Second pass. Global results and consistence analysis ************/

  // adjust iw
  if ( b->s0.iw[0] == '/' && b->s1.ff[0] != '/' )
    b->s0.iw[0] = '1';

  // fill date fields YYYYMMDDHHmm
  buoy_YYYYMMDDHHmm_to_JMMYYGGgg ( b );
  b->mask |= BUOY_EXT;

  // check if set both LaLaLa and LoLoLoLo to set Qc 
  if ((b->s0.Qc[0] == 0) && b->s0.LaLaLaLaLa[0] && b->s0.LoLoLoLoLoLo[0])
  {
    if (s.mask & SUBSET_MASK_LATITUDE_SOUTH)
    {
       if (s.mask & SUBSET_MASK_LONGITUDE_WEST)
         strcpy(b->s0.Qc, "5");
       else
         strcpy(b->s0.Qc, "3");
    }
    else
    {
       if (s.mask & SUBSET_MASK_LONGITUDE_WEST)
         strcpy(b->s0.Qc, "7");
       else
        strcpy(b->s0.Qc, "1");
    }
  }

  // check Qx
  if (b->s1.Qx[0] == 0 && b->s1.Qd[0])
     b->s1.Qx[0] = '9';
  if (b->s2.Qx[0] == 0 && b->s2.Qd[0])
     b->s2.Qx[0] = '9';

  if (b->s3.Qd1[0] ==  0)
    b->s3.Qd1[0] == '0'; 
  if (b->s3.Qd2[0] ==  0)
    b->s3.Qd2[0] == '0'; 

  return 0;
}
