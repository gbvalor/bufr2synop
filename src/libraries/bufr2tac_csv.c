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
 \file bufr2tac_csv.c
 \brief Output a struct \ref metreport in csv labeled form
*/
#include "bufr2tac.h"

/*!
  \fn int print_csv(FILE *f, struct metreport *m)
  \brief prints a struct \ref metreport in labeled csv format
  \param f pointer to a file already open by caller routine
  \param m pointer to a struct \ref metreport containing the data to print
*/
int print_csv ( FILE *f, struct metreport *m )
{
  // prints header
  fprintf ( f, "\"%s\",", m->type );
  fprintf ( f, "\"%s\",",m->h->filename );
  // print GTS_HEADER
  if ( m->h != NULL )
    fprintf ( f, "\"%s %s %s %s\",",m->h->bname, m->h->center, m->h->dtrel, m->h->order );
  else
    fprintf ( f, "," );

  // print DATE AND TIME
  fprintf ( f, "\"%s\",",m->t.datime );

  // Geo data
  if ( strlen ( m->g.index ) )
    fprintf ( f, "\"%s\",", m->g.index );
  else
    fprintf ( f, "," );

  if ( strlen ( m->g.name ) )
    fprintf ( f, "\"%s\",", m->g.name );
  else
    fprintf ( f, "," );


  if ( strlen ( m->g.country ) )
    fprintf ( f, "\"%s\",", m->g.country );
  else
    fprintf ( f, "," );

  fprintf ( f, "%.6lf,", m->g.lat );
  fprintf ( f, "%.6lf,", m->g.lon );
  fprintf ( f, "%.1lf,", m->g.alt );
  fprintf ( f, "\"%s=\"\n", m->alphanum );
  return 0;
}
