/***************************************************************************
 *   Copyright (C) 2013-2017 by Guillermo Ballester Valor                  *
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
 \file bufrdeco_print_html.c
 \brief file with the code to print data or any output from library decobufr in html format
 */
#include "bufrdeco.h"

#define strcat_protected(_target_,_source_,_limit_) \
 if ((strlen(_target_) + strlen(_source_)) < _limit_) \
 { \
    strcat(_target_,_source_); \
 } \
 else \
 { \
    return 1; \
 }

/*!
  \fn void sprint_sec0_info_html( char *target, size_t lmax, struct bufrdeco *b )
  \brief Prints info from sec0 in html format
  \param target string target
  \param lmax available size in target
  \param b pointer to the source struct \ref bufrdeco
*/
int sprint_sec0_info_html ( char *target, size_t lmax, struct bufrdeco *b )
{
  char caux[512], *c;
  c = caux;
  c += sprintf ( c, "<table class='bufr_sec0'>\n<caption>SEC 0 INFO</caption>\n" );
  c += sprintf ( c, "<tr><td>Bufr length</td><td>%5u</td></tr>\n", b->sec0.bufr_length );
  c += sprintf ( c, "<tr><td>Bufr edition</td><td>%5u</td></tr>\n", b->sec0.edition );
  c += sprintf ( c, "</table>\n" );
  strcat_protected ( target, caux, lmax );
  return 0;
}

/*!
  \fn void sprint_sec1_info_html( char *target, size_t lmax, struct bufrdeco *b )
  \brief Prints info from sec1
  \param target string target
  \param lmax available size in target
  \param b pointer to the source struct \ref bufrdeco
*/
int sprint_sec1_info_html ( char *target, size_t lmax, struct bufrdeco *b )
{
  char caux[2048], *c;
  c = caux;

  c += sprintf ( c, "<div class='bufr_sec1'>\n");
  c += sprintf ( c, "<table>\n<caption>SEC 1 INFO</caption>\n" );
  c += sprintf ( c, "<tr><td>Sec1 length</td><td>%5u</td></tr>\n", b->sec1.length );
  c += sprintf ( c, "<tr><td>Bufr master table</td><td>%5u</td></tr>\n", b->sec1.master );
  c += sprintf ( c, "<tr><td>Centre</td><td>%5u</td></tr>\n", b->sec1.centre );
  c += sprintf ( c, "<tr><td>Sub-Centre</td><td>%5u</td></tr>\n", b->sec1.subcentre );
  c += sprintf ( c, "<tr><td>Update sequence</td><td>%5u</td></tr>\n", b->sec1.update );
  c += sprintf ( c, "<tr><td>Options</td><td>%5x</td></tr>\n", b->sec1.options );
  c += sprintf ( c, "<tr><td>Category</td><td>%5u</td></tr>\n", b->sec1.category );
  c += sprintf ( c, "<tr><td>Subcategory</td><td>%5u</td></tr>\n", b->sec1.subcategory );
  c += sprintf ( c, "<tr><td>Sub-category local</td><td>%5u</td></tr>\n", b->sec1.subcategory_local );
  c += sprintf ( c, "<tr><td>Master table version</td><td>%5u</td></tr>\n", b->sec1.master_version );
  c += sprintf ( c, "<tr><td>Master table local</td><td>%5u</td></tr>\n", b->sec1.master_local );
  c += sprintf ( c, "<tr><td>Year</td><td>%5u</td></tr>\n", b->sec1.year );
  c += sprintf ( c, "<tr><td>Month</td><td>%5u</td></tr>\n", b->sec1.month );
  c += sprintf ( c, "<tr><td>Day</td><td>%5u</td></tr>\n", b->sec1.day );
  c += sprintf ( c, "<tr><td>Hour</td><td>%5u</td></tr>\n", b->sec1.hour );
  c += sprintf ( c, "<tr><td>Minute</td><td>%5u</td></tr>\n", b->sec1.minute );
  c += sprintf ( c, "<tr><td>Second</td><td>%5u</td></tr>\n", b->sec1.second );
  c += sprintf ( c, "<tr><td>Aditional space</td><td>%5u</td></tr>\n", b->sec1.length - 22 );

  if ( b->tables->b.path[0] )
    {
      c += sprintf ( c, "<tr><td>Tables used</td><td>'%s'<br>\n", b->tables->b.path );
      c += sprintf ( c, "'%s'<br>\n", b->tables->c.path );
      c += sprintf ( c, "'%s'</td></tr>\n", b->tables->d.path );
    }
  c += sprintf ( c, "</table>\n" );
  c += sprintf ( c, "</div>\n" );
  strcat_protected ( target, caux, lmax );
  return 0;
}

/*!
  \fn void sprint_sec3_info_html( char *target, size_t lmax, struct bufrdeco *b )
  \brief Prints info from sec3 formatted as html
  \param target string target
  \param lmax available size in target
  \param b pointer to the source struct \ref bufrdeco
*/
int sprint_sec3_info_html ( char *target, size_t lmax, struct bufrdeco *b )
{
  size_t i;
  char caux[4096], *c;


  c = caux;
  c += sprintf ( c, "<div class='bufr_sec3'>\n");
  c += sprintf ( c, "<table>\n<caption>SEC 3 INFO</caption>\n" );
  c += sprintf ( c, "<tr><td>Sec3 length</td><td>%5u\n", b->sec3.length );
  c += sprintf ( c, "<tr><td>Subsets</td><td>%5u\n", b->sec3.subsets );
  c += sprintf ( c, "<tr><td>Observed</td><td>%5u\n", b->sec3.observed );
  c += sprintf ( c, "<tr><td>Compressed</td><td>%5u\n", b->sec3.compressed );
  c += sprintf ( c, "<tr><td>Unexpanded descriptors</td><td>%5u</td></tr>\n", b->sec3.ndesc );
  for ( i = 0; i < b->sec3.ndesc; i++ )
    {
      c += sprintf ( c, "<tr><td>%3lu:</td><td>%u %02u %03u</td></tr>\n", i, b->sec3.unexpanded[i].f,
                     b->sec3.unexpanded[i].x, b->sec3.unexpanded[i].y );
    }
  c += sprintf ( c, "</table>\n" );
  c += sprintf ( c, "</div>\n" );
  strcat_protected ( target, caux, lmax );
  return 0;
}

/*!
  \fn void sprint_sec4_info_html( char *target, size_t lmax, struct bufrdeco *b )
  \brief Prints info from sec4
  \param target string target
  \param lmax available size in target
  \param b pointer to the source struct \ref bufrdeco
*/
int sprint_sec4_info_html ( char *target, size_t lmax, struct bufrdeco *b )
{
  char caux[4096], *c;
  c = caux;

  c += sprintf ( c, "<div class='bufr_sec4'>\n");
  c += sprintf ( c, "<table>\n<caption>SEC 4 INFO</caption>\n" );
  c += sprintf ( c, "<tr><td>Sec4 length</td><td>%5u</td></tr>\n", b->sec4.length );
  c += sprintf ( c, "</table>\n" );
  c += sprintf ( c, "</div>\n" );
  strcat_protected ( target, caux, lmax );
  return 0;
}

/*!
  \fn char * bufrdeco_print_atom_data_html ( char *target, struct bufr_atom_data *a )
  \brief print the data in a struct \ref bufr_atom_data to a string as cells of table rows
  \param target string where to print the result
  \param a pointer to struct ref \ref bufr_atom_data with data to print

  Returns a pointer to result string
*/
char * bufrdeco_print_atom_data_html ( char *target, struct bufr_atom_data *a )
{
  char aux[256], *c;

  c = target;
  c += sprintf ( c, "<td>%u %02u %03u</td>", a->desc.f, a->desc.x, a->desc.y );
  c += sprintf ( c, "<td>%s</td>", a->name );
  c += sprintf ( c, "<td>%s</td>", a->unit );
  if ( a->mask & DESCRIPTOR_VALUE_MISSING )
    {
      c += sprintf ( c, "<td>MISSING</td><td></td>" );
    }
  else
    {
      if ( a->mask & DESCRIPTOR_HAVE_STRING_VALUE )
        {
          c += sprintf ( c, "<td></td><td>%s</td>\n", a->cval );
        }
      else if ( a->mask & DESCRIPTOR_HAVE_CODE_TABLE_STRING
                || strstr ( a->unit, "CODE TABLE" ) == a->unit
                || strstr ( a->unit, "Code table" ) == a->unit )
        {
          c += sprintf ( c, "<td align='right'>%17u</td>", ( uint32_t ) a->val );
          c += sprintf ( c, "<td>%s</td>\n", a->ctable );
        }
      else if ( a->mask & DESCRIPTOR_HAVE_FLAG_TABLE_STRING )
        {
          c += sprintf ( c, "<td align='right'>0x%08X</td>", ( uint32_t ) a->val );
          c += sprintf ( c, "<td>%s</td>\n", a->ctable );
        }
      else
        {
          c += sprintf ( c, "<td align='right'>%s</td><td></td>" , get_formatted_value_from_escale ( aux, a->escale, a->val ) );
        }
    }
  return target;
}

/*!
  \fn void bufrdeco_fprint_subset_sequence_data_html ( struct bufrdeco_subset_sequence_data *s )
  \brief Prints a struct \ref bufrdeco_subset_sequence_data as an html table
  \param s pointer to the struct to print
*/
void bufrdeco_fprint_subset_sequence_data_html ( FILE *f, struct bufrdeco_subset_sequence_data *s )
{
  size_t i;
  char aux[1024];

  fprintf ( f, "<div class='bufr_subset'>\n");
  fprintf ( f, "<table>\n" );
  for ( i = 0; i < s->nd ; i++ )
    {
      fprintf ( f, "<tr><td>%5lu:</td>%s</tr>\n", i, bufrdeco_print_atom_data_html ( aux, &s->sequence[i] ) );
    }
  fprintf ( f, "</table>\n" );
  fprintf ( f, "</div>\n" );
}

/*!
  \fn void bufrdeco_fprint_subset_sequence_data_html ( struct bufrdeco_subset_sequence_data *s )
  \brief Prints a struct \ref bufrdeco_subset_sequence_data as an html table
  \param s pointer to the struct to print
*/
void bufrdeco_print_subset_sequence_data_html ( struct bufrdeco_subset_sequence_data *s )
{
  return bufrdeco_fprint_subset_sequence_data_html ( stdout, s );
}

/*!
  \fn void bufrdeco_fprint_subset_sequence_data_tagged_html ( struct bufrdeco_subset_sequence_data *s, char *id )
  \brief Prints a struct \ref bufrdeco_subset_sequence_data as an html table
  \param s pointer to the struct to print
  \param id string with id for the subset 
*/
void bufrdeco_fprint_subset_sequence_data_tagged_html ( FILE *f, struct bufrdeco_subset_sequence_data *s, char *id)
{
  size_t i;
  char aux[1024];

  fprintf ( f, "\n<div class='bufr_subset' id='%s'>\n", id);
  fprintf ( f, "<table>\n" );
  for ( i = 0; i < s->nd ; i++ )
    {
      fprintf ( f, "<tr><td>%5lu:</td>%s</tr>\n", i, bufrdeco_print_atom_data_html ( aux, &s->sequence[i] ) );
    }
  fprintf ( f, "</table>\n" );
  fprintf ( f, "</div><br>\n" );
}

/*!
  \fn void bufrdeco_fprint_subset_sequence_data_tagged_html ( struct bufrdeco_subset_sequence_data *s, char *id )
  \brief Prints a struct \ref bufrdeco_subset_sequence_data as an html table
  \param s pointer to the struct to print
  \param id string with id for the subset 
*/
void bufrdeco_print_subset_sequence_data_tagged_html ( struct bufrdeco_subset_sequence_data *s, char *id )
{
  return bufrdeco_fprint_subset_sequence_data_tagged_html ( stdout, s, id );
}
