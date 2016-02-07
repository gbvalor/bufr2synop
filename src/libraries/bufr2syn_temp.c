/***************************************************************************
 *   Copyright (C) 2013-2016 by Guillermo Ballester Valor                  *
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
 \file bufr2syn_temp.c
 \brief file with the code to parse a sequence as a FM-35 TEMP, FM-36 TEMP SHIP, FM-37 TEMP DROP, FM-38 TEMP MOBIL
 */
#include "bufr2syn.h"


/*!
  \fn char *guess_WMO_region_temp(struct synop_chunks *syn)
  \brief Try to find WMO region if it is not already set and WMO Block and number index are known
  \param syn pointer to the struct \ref synop_chunks with all known data for a synop

  It returns a pointer to the string with WMO region
*/
char *guess_WMO_region_temp ( struct temp_chunks *t )
{
  if ( t->a.s1.A1[0] )
    {
      return t->a.s1.A1;
    }

  if ( t->a.s1.II[0] == 0  || t->a.s1.iii[0] == 0 )
    return t->a.s1.A1;

  if ( guess_WMO_region ( t->a.s1.A1, t->a.s1.Reg, t->a.s1.II, t->a.s1.iii ) != NULL )
    {
      // copy the results in part A to parts A,B and C
      strcpy ( t->b.s1.A1, t->a.s1.A1 );
      strcpy ( t->c.s1.A1, t->a.s1.A1 );
      strcpy ( t->d.s1.A1, t->a.s1.A1 );
      strcpy ( t->b.s1.Reg, t->a.s1.Reg );
      strcpy ( t->c.s1.Reg, t->a.s1.Reg );
      strcpy ( t->d.s1.Reg, t->a.s1.Reg );
      return t->a.s1.A1;
    }
  else
    return NULL;
}



/*!
  \fn int parse_subset_as_temp(struct metreport *m, char *type, struct bufr_subset_sequence_data *sq, char *err)
  \brief parses a subset sequence as an Land fixed TEMP FM-35, TEMP SHIP FM-36, TEMP DROP FM-37 or TEMP MOBIL FM-38 report
  \param m pointer to a struct \ref metreport where set some results
  \param type strint with MiMiMjMj to choose the type of temp, temp ship, temp drop or temp mobil)
  \param sq pointer to a struct \ref bufr_subset_sequence_data with the parsed sequence on input
  \param err string with detected errors, if any

  It return 0 if all is OK. Otherwise returns 1 and it also fills the \a err string
*/
int parse_subset_as_temp ( struct metreport *m, struct bufr_subset_state *s, struct bufr_subset_sequence_data *sq, char *err )
{
  size_t is;
  char aux[16];
  struct temp_chunks *t;

  if (sq == NULL)
    return 1;
  
  t = &m->temp;

  // clean data
  clean_temp_chunks ( t );

  // reject if still not coded type
  if ( strcmp ( s->type_report,"TTXX" ) == 0)
    {
      // FIXME
      sprintf ( err,"bufr2syn: parse_subset_as_temp(): '%s' reports still not decoded in this software", s->type_report );
      return 1;
    }

  // Set the part. We are sure of this
  strcpy(t->a.s1.MjMj, "AA");
  strcpy(t->b.s1.MjMj, "BB");
  strcpy(t->c.s1.MjMj, "CC");
  strcpy(t->d.s1.MjMj, "DD");
  
  /**** First pass, sequential analysis *****/
  for ( is = 0; is < sq->nd; is++ )
    {
      s->i = is;
      s->ival = ( int ) sq->sequence[is].val;
      s->val = sq->sequence[is].val;
      s->a = &sq->sequence[is];
      if ( is > 0 )
        {
          s->a1 = &sq->sequence[is - 1];
        }

      // check if is a significance qualifier
      if ( sq->sequence[is].desc.x == 8 )
        {
          temp_parse_x08 ( t, s );
        }      
      
       switch ( sq->sequence[is].desc.x )
        {
        case 1: //localization descriptors
          temp_parse_x01 ( t, s );
          break;

        case 2: //Type of station descriptors
          temp_parse_x02 ( t, s );
          break;

        case 4: //Date and time descriptors
          temp_parse_x04 ( t, s );
          break;

	default:
          break;
        }
    } 
    
   /* Check about needed descriptors */
  if ( ( ( s->mask & SUBSET_MASK_HAVE_LATITUDE ) == 0 ) ||
       ( ( s->mask & SUBSET_MASK_HAVE_LONGITUDE ) == 0 ) ||
       ( ( s->mask & SUBSET_MASK_HAVE_YEAR ) == 0 ) ||
       ( ( s->mask & SUBSET_MASK_HAVE_MONTH ) == 0 ) ||
       ( ( s->mask & SUBSET_MASK_HAVE_DAY ) == 0 ) ||
       ( ( s->mask & SUBSET_MASK_HAVE_HOUR ) == 0 ) ||
       ( ( s->mask & SUBSET_MASK_HAVE_SECOND ) == 0 ) 
      )
    {
      sprintf ( err,"bufr2syn: parse_subset_as_temp(): lack of mandatory descriptor in sequence" );
      return 1;
    }

  /* Guess the type of TEMP report according with data parsed */  
  if (t->a.s1.II[0])
  {
    // it seems this is a TEMP report
    strcpy( t->a.s1.MiMi, "TT");
    strcpy( t->b.s1.MiMi, "TT");
    strcpy( t->c.s1.MiMi, "TT");
    strcpy( t->d.s1.MiMi, "TT");
  }
  else if (t->a.s1.h0h0h0h0[0])
  {
    // it seems this is a TEMP MOBIL report
    strcpy( t->a.s1.MiMi, "II");
    strcpy( t->b.s1.MiMi, "II");
    strcpy( t->c.s1.MiMi, "II");
    strcpy( t->d.s1.MiMi, "II");
  }
  else if (t->a.s1.D_D[0])
  {
    // it seems this is a TEMP SHIP report
    strcpy( t->a.s1.MiMi, "UU");
    strcpy( t->b.s1.MiMi, "UU");
    strcpy( t->c.s1.MiMi, "UU");
    strcpy( t->d.s1.MiMi, "UU");
  }
  else if (t->a.s1.Ula[0])
  {
    // it seems this is a TEMP DROP report
    strcpy( t->a.s1.MiMi, "XX");
    strcpy( t->b.s1.MiMi, "XX");
    strcpy( t->c.s1.MiMi, "XX");
    strcpy( t->d.s1.MiMi, "XX");
  }
  else
  {
    sprintf ( err,"bufr2syn: parse_subset_as_temp(): Unknown type TEMP report" );
    return 1;
  }
  sprintf(m->type, "%s%s" , t->a.s1.MiMi, t->a.s1.MjMj);
  sprintf(m->type2, "%s%s" , t->b.s1.MiMi, t->b.s1.MjMj);
  sprintf(m->type3, "%s%s" , t->c.s1.MiMi, t->c.s1.MjMj);
  sprintf(m->type4, "%s%s" , t->d.s1.MiMi, t->d.s1.MjMj);
  
  /****** Second pass. Global results and consistence analysis ************/
  sprintf ( aux,"%s%s%s%s%s%s", t->a.e.YYYY, t->a.e.MM, t->a.e.DD, t->a.e.HH, t->a.e.mm, t->a.e.ss );
  YYYYMMDDHHmm_to_met_datetime ( &m->t, aux );
  
  return 0;
}
