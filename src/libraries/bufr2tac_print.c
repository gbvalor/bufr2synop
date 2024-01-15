/***************************************************************************
 *   Copyright (C) 2013-2024 by Guillermo Ballester Valor                  *
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
 \file bufr2tac_print.c
 \brief file with the code to print the results
 */
#include "bufr2tac.h"

/*!
  \fn  int print_plain ( FILE *f, struct metreport *m )
  \brief Print in a file the report decoded to Traditional Alphanumeric Code in plain text format. A line per report
  \param f pointer to file where to write to
  \param m pointer to struct \ref metreport where the decoded report is stored
*/
int print_plain ( FILE *f, const struct metreport *m )
{
  if ( m->alphanum[0] )
    fprintf ( f, "%s\n", m->alphanum );
  if ( m->alphanum2[0] )
    fprintf ( f, "%s\n", m->alphanum2 );
  if ( m->alphanum3[0] )
    fprintf ( f, "%s\n", m->alphanum3 );
  if ( m->alphanum4[0] )
    fprintf ( f, "%s\n", m->alphanum4 );
  return 0;
}

/*!
  \fn  int print_html ( FILE *f, struct metreport *m )
  \brief Print in a file the report decoded to Traditional Alphanumeric Code in plain html format. A line per report
  \param f pointer to file where to write to
  \param m pointer to struct \ref metreport where the decoded report is stored
*/
int print_html ( FILE *f, const struct metreport *m )
{
  fprintf(f, "<pre>");
  if ( m->alphanum[0] )
    fprintf ( f, "%s\n", m->alphanum );
  if ( m->alphanum2[0] )
    fprintf ( f, "%s\n", m->alphanum2 );
  if ( m->alphanum3[0] )
    fprintf ( f, "%s\n", m->alphanum3 );
  if ( m->alphanum4[0] )
    fprintf ( f, "%s\n", m->alphanum4 );
  fprintf(f, "</pre>");
  return 0;
}

/*!
 *  \fn size_t print_geo ( char **geo,  size_t lmax, const struct metrepor *m )
 *  \brief Prints a WIGOS identifier in a TAC output report
 *  \param m pointer to struct \ref metreport where are both target and source
 */
size_t print_geo ( char **geo,  size_t lmax, const struct metreport *m )
{
  size_t used;
  char sep = '|';
  
  used = snprintf(*geo, lmax, "%8.4lf %9.4lf %6.1lf%c", m->g.lat, m->g.lon, m->g.alt, sep);
  *geo += used;
  return used;
}

/*!
 *  \fn size_t print_wigos_id ( char **wid,  size_t lmax, const struct metrepor *m )
 *  \brief Prints a WIGOS identifier in a TAC output report
 *  \param m pointer to struct \ref metreport where are both target and source
 */
  size_t print_wigos_id ( char **wid,  size_t lmax, const struct metreport *m )
{
  char aux[40];
  size_t used;
  char sep = '|';
  
  if (m->g.wid.series == 0 && m->g.wid.issuer == 0 && m->g.wid.issue == 0 && m->g.wid.local_id[0] == '\0')
    sprintf (aux,"0-0-0-MISSING");
  else
    sprintf (aux,"%d-%d-%d-%s", m->g.wid.series, m->g.wid.issuer, m->g.wid.issue, m->g.wid.local_id );
 
  used = snprintf (*wid, lmax, "%-32s%c", aux, sep);
  *wid += used;
  return used;
}
