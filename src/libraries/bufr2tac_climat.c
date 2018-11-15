/***************************************************************************
 *   Copyright (C) 2013-2018 by Guillermo Ballester Valor                  *
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
 \file bufr2tac_climat.c
 \brief file with the code to parse a sequence as a FM-71 XII CLIMAT and FM-72 XII CLIMAT SHIP
 */
#include "bufr2tac.h"



/*!
  \fn int parse_subset_as_climat(struct metreport *m, char *type, struct bufr_subset_sequence_data *sq, char *err)
  \brief parses a subset sequence as an Land fixed CLIMAT FM-71 report
  \param m pointer to a struct \ref metreport where set some results
  \param type strint with MiMiMjMj to choose the type of temp, temp ship, temp drop or temp mobil)
  \param sq pointer to a struct \ref bufr_subset_sequence_data with the parsed sequence on input
  \param err string with detected errors, if any

  It return 0 if all is OK. Otherwise returns 1 and it also fills the \a err string
*/
int parse_subset_as_climat ( struct metreport *m, struct bufr2tac_subset_state *s, struct bufr_subset_sequence_data *sq, char *err )
{
  size_t is;
  char aux[32];
  struct climat_chunks *c;

  c = &m->climat;

  // clean data
  clean_climat_chunks ( c );

  // reject if still not coded type
  if ( strcmp ( s->type_report,"CLIMAT" ) )
    {
      sprintf ( err,"bufr2tac: parse_subset_as_climat(): '%s' reports still not decoded in this software", s->type_report );
      return 1;
    }

  strcpy ( m->type, s->type_report );

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

      switch ( sq->sequence[is].desc.x )
        {
        case 1: //localization descriptors
          climat_parse_x01 ( c, s );
          break;

        case 2: //Type of station descriptors
          climat_parse_x02 ( c, s );
          break;

        case 4: //Date and time descriptors
          climat_parse_x04 ( c, s );
          break;

        case 5: // Horizontal position. Latitude
          climat_parse_x05 ( c, s );
          break;

        case 6: // Horizontal position. Longitude
          climat_parse_x06 ( c, s );
          break;

        case 7: // Vertical position
          climat_parse_x07 ( c, s );
          break;

        case 8: // significance qualifier
          climat_parse_x08 ( c, s );
          break;

        case 10: // Air pressure
          climat_parse_x10 ( c, s );
          break;

        case 11: // Wind
          climat_parse_x11 ( c, s );
          break;

        case 12: // Temperature
          climat_parse_x12 ( c, s );
          break;

        case 13: // Humidity and precipitation data
          climat_parse_x13 ( c, s );
          break;

        case 14: // Radiation
          climat_parse_x14 ( c, s );
          break;

        default:
          break;
        }

    }

  // Fill some metreport fields
  if ( strlen ( c->s0.II ) )
    {
      strcpy ( m->g.index, c->s0.II );
      strcat ( m->g.index, c->s0.iii );
    }

  if ( s->mask & SUBSET_MASK_HAVE_LATITUDE )
    {
      m->g.lat = s->lat;
    }
  if ( s->mask & SUBSET_MASK_HAVE_LONGITUDE )
    {
      m->g.lon = s->lon;
    }
  if ( s->mask & SUBSET_MASK_HAVE_ALTITUDE )
    {
      m->g.alt = s->alt;
    }
  if ( s->mask & SUBSET_MASK_HAVE_NAME )
    {
      strcpy ( m->g.name, s->name );
    }
  if ( s->mask & SUBSET_MASK_HAVE_COUNTRY )
    {
      strcpy ( m->g.country, s->country );
    }

  sprintf ( aux,"%s%s%s%s%s", c->e.YYYY, c->e.MM, c->e.DD, c->e.HH, c->e.mm );
  YYYYMMDDHHmm_to_met_datetime ( &m->t, aux );

  if ( check_date_from_future ( m ) )
    {
      return 1;  // Bad date/time . Is a report from future!
    }


  return 0;
}
