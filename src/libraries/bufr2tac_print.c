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
int print_plain ( FILE *f, struct metreport *m )
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
int print_html ( FILE *f, struct metreport *m )
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
