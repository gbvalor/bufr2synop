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
 \file bufr2tac_json.c
 \brief Output a struct \ref metreport in json form
*/
#include "bufr2tac.h"

int print_json_alphanum ( FILE *f, char *type, char *alphanum, struct metreport *m )
{
  fprintf ( f, " { \n  \"type\": \"%s\",\n", type );
  if ( m->h != NULL )
    {
      fprintf ( f, "  \"bufrfile\": \"%s\",\n", m->h->filename );
      fprintf ( f, "  \"gts_header\": \"%s %s %s %s\",\n", m->h->bname, m->h->center, m->h->dtrel, m->h->order );
    }
  fprintf ( f, "  \"observation_datetime\": \"%s\",\n", m->t.datime );
  fprintf ( f, "  \"geo\": { \n" );
  if ( strlen ( m->g.index ) )
    {
      fprintf ( f, "    \"index\": \"%s\",\n", m->g.index );
    }
  if ( strlen ( m->g.name ) )
    {
      fprintf ( f, "    \"name\": \"%s\",\n", m->g.name );
    }
  if ( strlen ( m->g.country ) )
    {
      fprintf ( f, "    \"country\": \"%s\",\n", m->g.country );
    }
  fprintf ( f, "    \"latitude\": %.6lf,\n", m->g.lat );
  fprintf ( f, "    \"longitude\": %.6lf,\n", m->g.lon );
  fprintf ( f, "    \"altitude\": %.1lf\n", m->g.alt );
  fprintf ( f, "    },\n" );
  fprintf ( f, "  \"report\": \"%s\"\n", alphanum );
  fprintf ( f, "  }" );
  return 0;
}
/*!
  \fn int print_json(FILE *f, struct metreport *m)
  \brief prints a struct \ref metreport in json format
  \param f pointer to a file already open by caller routine
  \param m pointer to a struct \ref metreport containing the data to print
*/
int print_json ( FILE *f, struct metreport *m )
{
  fprintf ( f, "{\"metreport\" :" );
  if ( m->alphanum[0] )
    {
      print_json_alphanum ( f, m->type, m->alphanum, m );
    }

  if ( m->alphanum2[0] ) //TTBB
    {
      fprintf ( f, "," );
      print_json_alphanum ( f, m->type2, m->alphanum2, m );
    }

  if ( m->alphanum3[0] ) //TTCC
    {
      fprintf ( f, "," );
      print_json_alphanum ( f, m->type3, m->alphanum3, m );
    }
  if ( m->alphanum4[0] ) //TTDD
    {
      fprintf ( f, "," );
      print_json_alphanum ( f, m->type4, m->alphanum4, m );
    }

  fprintf ( f, "\n}\n" );
  return 0;
}
