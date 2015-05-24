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
 \file bufr2syn_temp.c
 \brief file with the code to parse a sequence as a FM-35 TEMP, FM-36 TEMP SHIP, FM-37 TEMP DROP, FM-38 TEMP MOBIL
 */
#include "bufr2syn.h"


/*!
  \fn char *guess_WMO_region_synop(struct synop_chunks *syn)
  \brief Try to find WMO region if it is not already set and WMO Block and number index are known
  \param syn pointer to the struct \ref synop_chunks with all known data for a synop

  It returns a pointer to the string with WMO region
*/
char *guess_WMO_region_temp ( struct temp_chunks *temp )
{
  if ( temp->a.s1.A1[0] )
    {
      return temp->a.s1.A1;
    }

  if ( temp->a.s1.II[0] == 0  || temp->a.s1.iii[0] == 0 )
    return temp->a.s1.A1;

  if ( guess_WMO_region ( temp->a.s1.A1, temp->a.s1.Reg, temp->a.s1.II, temp->a.s1.iii ) != NULL )
    {
      // copy the results in part A to parts A,B and C
      strcpy ( temp->b.s1.A1, temp->a.s1.A1 );
      strcpy ( temp->c.s1.A1, temp->a.s1.A1 );
      strcpy ( temp->d.s1.A1, temp->a.s1.A1 );
      strcpy ( temp->b.s1.Reg, temp->a.s1.Reg );
      strcpy ( temp->c.s1.Reg, temp->a.s1.Reg );
      strcpy ( temp->d.s1.Reg, temp->a.s1.A1 );
      return temp->a.s1.A1;
    }
  else
    return NULL;
}



/*!
  \fn int parse_subset_as_temp(struct metreport *m, char *type, struct bufr_subset_sequence_data *sq, char *err)
  \brief parses a subset sequence as an Land fixed SYNOP FM-12, SHIP FM-13 or SYNOP-mobil FM-14 report
  \param m pointer to a struct \ref metreport where set some results
  \param type strint with MiMiMjMj to choose the type of temp, temp ship, temp drop or temp mobil)
  \param sq pointer to a struct \ref bufr_subset_sequence_data with the parsed sequence on input
  \param err string with detected errors, if any

  It return 0 if all is OK. Otherwise returns 1 and it also fills the \a err string
*/
int parse_subset_as_temp ( struct metreport *m, struct bufr_subset_state *s, struct bufr_subset_sequence_data *sq, char *err )
{
  struct temp_chunks *t;

  t = &m->temp;

  // clean data
  clean_temp_chunks ( t );

  // reject if still not coded type
  if ( strcmp ( s->type_report,"TTXX" ) )
    {
      sprintf ( err,"bufr2syn: parse_subset_as_temp(): '%s' reports still not decoded in this software", s->type_report );
      return 1;
    }


  return 0;
}
