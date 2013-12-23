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
 \brief file with the code to parse a sequence as a SYNOP FM-12, FM-13 and FM-14
 */
#include "bufr2synop.h"

/*!
  \fn int synop_YYYYMMDDHHmm_to_YYGG(struct synop_chunks *syn)
  \brief Sets YYGG from YYYYMMDDHHmm extended group
  \param syn pointer to the target struct \ref synop_chunks
*/
int synop_YYYYMMDDHHmm_to_YYGG ( struct synop_chunks *syn )
{
  char aux[20];
  time_t t;
  struct tm tim;

  if ( strlen ( syn->e.YYYY ) &&
       strlen ( syn->e.MM ) &&
       strlen ( syn->e.DD ) &&
       strlen ( syn->e.HH ) &&
       strlen ( syn->e.MM ) )
    sprintf ( aux,"%s%s%s%s%s", syn->e.YYYY, syn->e.MM, syn->e.DD, syn->e.HH, syn->e.mm );

  if ( strlen ( aux ) != 12 )
    return 1;

  memset ( &tim, 0, sizeof ( struct tm ) );
  strptime ( aux, "%Y%m%d%H%M", &tim );

  t = mktime ( &tim ) + 1799 ; // rounding trick
  gmtime_r ( &t, &tim );
  sprintf ( syn->s0.YY, "%02d", tim.tm_mday );
  sprintf ( syn->s0.GG, "%02d", tim.tm_hour );
  return 0;
}

/*!
  \fn char *guess_WMO_region(struct synop_chunks *syn)
  \brief Try to find WMO region if it is not already set and WMO Block and number index are known
  \param syn pointer to the struct \ref synop_chunks with all known data for a synop

  It returns a pointer to the string with WMO region
*/
char *guess_WMO_region(struct synop_chunks *syn)
{
  char aux[8];

  if (syn->s0.A1[0])
    {
      return syn->s0.A1;
    }

  if (syn->s0.II[0] == 0  || syn->s0.iii[0] == 0)
    return syn->s0.A1;

  sprintf(aux,"%s%s",syn->s0.II, syn->s0.iii);

  if ((syn->s0.II[0] == '0' && (strstr(aux,"042") != aux) && (strstr(aux,"043") != aux)  &&
       (strstr(aux,"044") != aux)  && (strstr(aux,"0858") != aux) &&  (strstr(aux,"0859") != aux)) ||
      syn->s0.II[0] == '1' || (strstr(aux,"201") == aux) ||
      strcmp(syn->s0.II,"22") == 0 || strcmp(syn->s0.II,"26") == 0 || strcmp(syn->s0.II,"27") == 0 ||
      strcmp(syn->s0.II,"33") == 0 || strcmp(syn->s0.II,"34") == 0 || strcmp(syn->s0.II,"22") == 0 ||
      (strstr(aux,"201") == aux))
    {
      // Reg 6. Europe
      syn->s0.A1[0] = '6';
      strcpy(syn->s0.Reg,"VI");
    }
  else if (syn->s0.II[0] == '6' || (strstr(aux,"0858") == aux) ||  (strstr(aux,"0859") == aux))
    {
      // Reg 1. Africa
      syn->s0.A1[0] = '1';
      strcpy(syn->s0.Reg,"I");
    }
  else if (syn->s0.II[0] == '5' || (strcmp(syn->s0.II,"49") == 0) || (strcmp(syn->s0.II,"21") == 0) ||
           (strcmp(syn->s0.II,"23") == 0) || (strcmp(syn->s0.II,"24") == 0) || (strcmp(syn->s0.II,"25") == 0) ||
           (strcmp(syn->s0.II,"28") == 0) || (strcmp(syn->s0.II,"29") == 0) || (strcmp(syn->s0.II,"30") == 0) ||
           (strcmp(syn->s0.II,"31") == 0) || (strcmp(syn->s0.II,"32") == 0) || (strcmp(syn->s0.II,"38") == 0) ||
           (strcmp(syn->s0.II,"35") == 0) || (strcmp(syn->s0.II,"36") == 0) || (strcmp(syn->s0.II,"39") == 0) ||
           (strcmp(aux, "20200") >= 0 && strcmp(aux, "20999") <= 0) ||
           (strcmp(aux, "40000") >= 0 && strcmp(aux, "48599") <= 0) ||
           (strcmp(aux, "48800") >= 0 && strcmp(aux, "49999") <= 0))
    {
      // Reg 2. Asia
      syn->s0.A1[0] = '2';
      strcpy(syn->s0.Reg,"II");
    }
  else if (strcmp(aux, "80000") >= 0 && strcmp(aux, "88999") <= 0)
    {
      // Reg 3. South america
      syn->s0.A1[0] = '3';
      strcpy(syn->s0.Reg,"III");
    }
  else if (syn->s0.II[0] == '7' || strstr(aux,"042") == aux ||
           strstr(aux,"043") == aux  || strstr(aux,"044") == aux)
    {
      // Reg 4. North and central america
      syn->s0.A1[0] = '4';
      strcpy(syn->s0.Reg,"IV");
    }
  else if ((strcmp(aux, "48600") >= 0 && strcmp(aux, "48799") <= 0) ||
           (strcmp(aux, "90000") >= 0 && strcmp(aux, "98999") <= 0))
    {
      // Reg 5. Pacific South
      syn->s0.A1[0] = '4';
      strcpy(syn->s0.Reg,"IV");
    }
  else if (strcmp(syn->s0.II,"89") == 0)
    {
      // Reg 0. Antarctica
      syn->s0.A1[0] = '0';
      strcpy(syn->s0.Reg,"0");
    }
  return syn->s0.A1;
}


/*!
  \fn int parse_subset_as_synop(struct metreport *m, struct synop_chunks *syn, char *type, struct bufr_subset_sequence_data *sq, int *kdtlst, size_t nlst, int *ksec1, char *err)
  \brief parses a subset sequence as an Land fixed SYNOP FM-12, SHIP FM-13 or SYNOP-mobil FM-14 report
  \param m pointer to a struct \ref metreport where set some results
  \param syn pointer to a struct \ref synop_chunks where set the results
  \param type strint with MiMiMjMj to choose the type pf synop (AAXX, BBXX ...) (synop, ship or synop-mobil)
  \param sq pointer to a struct \ref bufr_subset_sequence_data with the parsed sequence on input
  \param kdtlst array of descriptors (as integers) from the non-expanded bufr list
  \param nlst size of kdtlst array
  \param ksec1 array of integers set by ECMWF bufrdc library
  \param err string with detected errors, if any

  It return 0 if all is OK. Otherwise it also fills the \a err string
*/
int parse_subset_as_synop (struct metreport *m, struct synop_chunks *syn, char *type, struct bufr_subset_sequence_data *sq, int *kdtlst, size_t nlst, int *ksec1, char *err )
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
  clean_synop_chunks ( syn );
  memset(&s, 0, sizeof(struct bufr_subset_state));

  // reject if still not coded type
  if (strcmp(TYPE,"AAXX") && strcmp(TYPE,"BBXX"))
  {
    sprintf(err,"%: '%s' reports still not decoded in this software", SELF, TYPE);
    return 1;
  }

  syn->s0.MiMi[0] = type[0];
  syn->s0.MiMi[1] = type[1];
  syn->s0.MjMj[0] = type[2];
  syn->s0.MjMj[1] = type[3];

  strcpy(m->type, type);

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
          syn_parse_x01 ( syn, &s, err );
          break;

        case 2: //Date time descriptors
          syn_parse_x02 ( syn, &s, err );
          break;

        case 4: //Date time descriptors
          syn_parse_x04 ( syn, &s, err );
          break;

        case 5: //Position
          syn_parse_x05 ( syn, &s, err );
          break;

        case 6: // Horizontal Position -2
          syn_parse_x06 ( syn, &s, err );
          break;

        case 7: // Vertical position
          syn_parse_x07 ( syn, &s, err );
          break;

        case 10: // Air Pressure descriptors
          syn_parse_x10 ( syn, &s, err );
          break;

        case 11: // wind  data
          syn_parse_x11 ( syn, &s, err );
          break;

        case 12: //Temperature descriptors
          syn_parse_x12 ( syn, &s, err );
          break;

        case 13: // Humidity and precipitation data
          syn_parse_x13 ( syn, &s, err );
          break;

        case 14: // Radiation
          syn_parse_x14 ( syn, &s, err );
          break;

        case 20: // Cloud data
          syn_parse_x20 ( syn, &s, err );
          break;

        case 22: // Oceanographic data
          syn_parse_x22 ( syn, &s, err );
          break;

        case 31: // Replicators
          syn_parse_x31 ( syn, &s, err );
          break;


        default:
          break;
        }

    }

  /****** Second pass. Global results and consistence analysis ************/
  // set ir
  if ( syn->s1.RRR[0] == 0 && syn->s3.RRR[0] == 0 && syn->s3.RRRR24[0] == 0 )
    {
      // no prec data case
      syn->s1.ir[0] = '4';
    }
  else if ( syn->s1.RRR[0] == 0 && syn->s3.RRR[0] != 0 )
    syn->s1.ir[0] = '2';
  else if ( syn->s1.RRR[0] != 0 && syn->s3.RRR[0] == 0 )
    syn->s1.ir[0] = '1';
  else
    syn->s1.ir[0] = '0';


  // adjust iw
  if ( syn->s0.iw[0] == '/' && syn->s1.ff[0] != '/' )
    syn->s0.iw[0] = '1';

  // adjust ix
  if (syn->s1.ix[0] == '/' && syn->s1.ww[0] == 0)
    {
      if ((s.mask & SUBSET_MASK_HAVE_NO_SIGNIFICANT_WW) &&
          (s.mask & SUBSET_MASK_HAVE_NO_SIGNIFICANT_W1) &&
          (s.mask & SUBSET_MASK_HAVE_NO_SIGNIFICANT_W2)
         )
        {
          if (s.mask & SUBSET_MASK_HAVE_TYPE_STATION)
            {
              if (s.type == 1 || s.type == 2)
                strcpy(syn->s1.ix,"2");
              else if (s.type == 0)
                strcpy(syn->s1.ix,"5");
            }
        }
      else if (s.mask & SUBSET_MASK_HAVE_TYPE_STATION)
        {
          if (s.type == 1)
            strcpy(syn->s1.ix,"3");
          else if (s.type == 0)
            strcpy(syn->s1.ix,"6");
        }
    }

  /****** Final Adjust ***********/
  // fix YYGG according with YYYYMMDDHHmm
  synop_YYYYMMDDHHmm_to_YYGG ( syn );
  if ( strcmp(syn->e.mm,"00"))
    {
      strcpy ( syn->s1.GG, syn->e.HH );
      strcpy ( syn->s1.gg, syn->e.mm );
    }
  syn->mask |= SYNOP_EXT;

  // Fill some metreport fields
  if (s.mask & SUBSET_MASK_HAVE_LATITUDE)
    m->g.lat = s.lat;
  if (s.mask & SUBSET_MASK_HAVE_LONGITUDE)
    m->g.lon = s.lon;
  if (s.mask & SUBSET_MASK_HAVE_ALTITUDE)
    m->g.alt = s.alt;
  if (s.mask & SUBSET_MASK_HAVE_NAME)
    strcpy(m->g.name, s.name);
  if (s.mask & SUBSET_MASK_HAVE_COUNTRY)
    strcpy(m->g.country, s.country);

  sprintf ( aux,"%s%s%s%s%s", syn->e.YYYY, syn->e.MM, syn->e.DD, syn->e.HH, syn->e.mm );
  YYYYMMDDHHmm_to_met_datetime(&m->t, aux);

  return 0;
}

