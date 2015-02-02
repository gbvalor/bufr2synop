/***************************************************************************
 *   Copyright (C) 2013-2015 by Guillermo Ballester Valor                  *
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
#include "bufr2syn.h"

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
       strlen ( syn->e.mm ) )
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
  \fn char *guess_WMO_region_synop(struct synop_chunks *syn)
  \brief Try to find WMO region if it is not already set and WMO Block and number index are known
  \param syn pointer to the struct \ref synop_chunks with all known data for a synop

  It returns a pointer to the string with WMO region
*/
char *guess_WMO_region_synop(struct synop_chunks *syn)
{
  if (syn->s0.A1[0])
    {
      return syn->s0.A1;
    }

  if (syn->s0.II[0] == 0  || syn->s0.iii[0] == 0)
    return syn->s0.A1;

  return guess_WMO_region(syn->s0.A1, syn->s0.Reg, syn->s0.II, syn->s0.iii);
}


/*!
  \fn int parse_subset_as_synop (struct metreport *m, struct bufr_subset_state *s, struct bufr_subset_sequence_data *sq, char *err )
  \brief parses a subset sequence as an Land fixed SYNOP FM-12, SHIP FM-13 or SYNOP-mobil FM-14 report
  \param m pointer to a struct \ref metreport where set some results
  \param s pointer to a struct \ref bufr_subset_state 
  \param sq pointer to a struct \ref bufr_subset_sequence_data with the parsed sequence on input
  \param err string with detected errors, if any

  It return 0 if all is OK. Otherwise returns 1 and it also fills the \a err string
*/
int parse_subset_as_synop (struct metreport *m, struct bufr_subset_state *s, struct bufr_subset_sequence_data *sq, char *err )
{
  int ival; // integer value for a descriptor
  size_t is; 
  char aux[16];
  struct synop_chunks *syn;

  // An auxiliar pointer
  syn = &m->synop;

  // clean data
  clean_synop_chunks ( syn );

  // reject if still not coded type
  if (strcmp(s->type_report,"AAXX") && strcmp(s->type_report,"BBXX") && strcmp(s->type_report,"OOXX"))
  {
    sprintf(err,"bufr2syn: parse_subset_as_synop(): '%s' reports still not decoded in this software", s->type_report);
    return 1;
  }

  syn->s0.MiMi[0] = s->type_report[0];
  syn->s0.MiMi[1] = s->type_report[1];
  syn->s0.MjMj[0] = s->type_report[2];
  syn->s0.MjMj[1] = s->type_report[3];

  strcpy(m->type, s->type_report);

  /**** First pass, sequential analysis *****/
  for ( is = 0; is < sq->nd; is++ )
    {
      // check if is a significance qualifier
      if (sq->sequence[is].desc.x == 8)
      {
        s->i = is;
        s->a = &sq->sequence[is];
        s->ival = ( int ) sq->sequence[is].val;
        s->val = sq->sequence[is].val;
        syn_parse_x08 ( syn, s);
      }

      if ( s->isq)   // case of an significance qualifier
        continue;

      s->i = is;
      s->ival = ( int ) sq->sequence[is].val;
      ival = ival;
      s->val = sq->sequence[is].val;
      s->a = &sq->sequence[is];
      switch ( sq->sequence[is].desc.x )
        {
        case 1: //localization descriptors
          syn_parse_x01 ( syn, s);
          break;

        case 2: //Type of station descriptors
          syn_parse_x02 ( syn, s);
          break;

        case 4: //Date and time descriptors
          syn_parse_x04 ( syn, s);
          break;

        case 5: // Horizontal position. Latitude 
          syn_parse_x05 ( syn, s);
          break;

        case 6: // Horizontal position. Longitude
          syn_parse_x06 ( syn, s);
          break;

        case 7: // Vertical position
          syn_parse_x07 ( syn, s);
          break;

        case 10: // Air Pressure descriptors
          syn_parse_x10 ( syn, s);
          break;

        case 11: // wind  data
          syn_parse_x11 ( syn, s);
          break;

        case 12: //Temperature descriptors
          syn_parse_x12 ( syn, s);
          break;

        case 13: // Humidity and precipitation data
          syn_parse_x13 ( syn, s);
          break;

        case 14: // Radiation
          syn_parse_x14 ( syn, s);
          break;

        case 20: // Cloud data
          syn_parse_x20 ( syn, s);
          break;

        case 22: // Oceanographic data
          syn_parse_x22 ( syn, s);
          break;

        case 31: // Replicators
          syn_parse_x31 ( syn, s);
          break;


        default:
          break;
        }

    }

  /* Check about needed descriptors */
  if (((s->mask & SUBSET_MASK_HAVE_LATITUDE) == 0) ||
      ((s->mask & SUBSET_MASK_HAVE_LONGITUDE) == 0) ||
      ((s->mask & SUBSET_MASK_HAVE_YEAR) == 0) ||
      ((s->mask & SUBSET_MASK_HAVE_MONTH) == 0) ||
      ((s->mask & SUBSET_MASK_HAVE_DAY) == 0) ||
      ((s->mask & SUBSET_MASK_HAVE_HOUR) == 0) 
      )
      {
        sprintf(err,"bufr2syn: parse_subset_as_synop(): lack of mandatory descriptor in sequence");
        return 1;
      }

  if (strcmp(s->type_report, "AAXX") == 0 && (syn->s0.II[0] == 0 || syn->s0.iii[0] == 0))
    {
       sprintf(err,"bufr2syn: parse_subset_as_synop(): lack of mandatory index station for AAXX report");
       return 1;
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
  if (syn->s1.ix[0] == '/' /*&& syn->s1.ww[0] == 0*/)
    {
      if ((s->mask & SUBSET_MASK_HAVE_NO_SIGNIFICANT_WW) &&
          (s->mask & SUBSET_MASK_HAVE_NO_SIGNIFICANT_W1) &&
          (s->mask & SUBSET_MASK_HAVE_NO_SIGNIFICANT_W2)
         )
        {
          if (s->mask & SUBSET_MASK_HAVE_TYPE_STATION)
            {
              if (s->type == 1 || s->type == 2)
                strcpy(syn->s1.ix,"2");
              else if (s->type == 0)
                strcpy(syn->s1.ix,"5");
            }
          else
            strcpy(syn->s1.ix,"6"); //NOTE: here we assume an automatic station without W data
        }
      else if (s->mask & SUBSET_MASK_HAVE_TYPE_STATION)
        {
          if (s->type == 1)
            strcpy(syn->s1.ix,"3");
          else if (s->type == 0)
            strcpy(syn->s1.ix,"6");
        }
      else
        strcpy(syn->s1.ix,"6"); //NOTE: here we assume an automatic station without W data
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
  if (strlen(syn->s0.II))
  {
    strcpy(m->g.index, syn->s0.II);
    strcat(m->g.index, syn->s0.iii);
  }
  else if (strlen(syn->s0.D_D))
  {
    strcpy(m->g.index, syn->s0.D_D);
  }
  else if (strlen(syn->s0.IIIII))
  {
    strcpy(m->g.index, syn->s0.IIIII);
  }

  if (s->mask & SUBSET_MASK_HAVE_LATITUDE)
  {
    if (fabs(s->lat) <= 90.0)
      m->g.lat = s->lat;
    else
      return 1; // Bad latitude. Fatal error
  }
  if (s->mask & SUBSET_MASK_HAVE_LONGITUDE)
  {
    if (fabs(s->lon) <= 180.0)
      m->g.lon = s->lon;
    else
      return 1; // bad longitude. Fatal error
  }
  if (s->mask & SUBSET_MASK_HAVE_ALTITUDE)
    m->g.alt = s->alt;
  if (s->mask & SUBSET_MASK_HAVE_NAME)
    strcpy(m->g.name, s->name);
  if (s->mask & SUBSET_MASK_HAVE_COUNTRY)
    strcpy(m->g.country, s->country);

  sprintf ( aux,"%s%s%s%s%s", syn->e.YYYY, syn->e.MM, syn->e.DD, syn->e.HH, syn->e.mm );

  YYYYMMDDHHmm_to_met_datetime(&m->t, aux);

  if (check_date_from_future(m))
     return 1; // Bad date/time . Is a report from future!

  // If finally we arrive here, It succeded
  return 0;
}

