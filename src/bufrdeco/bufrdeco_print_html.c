/***************************************************************************
 *   Copyright (C) 2013-2022 by Guillermo Ballester Valor                  *
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

/*!
  \fn void sprint_sec0_info_html( char *target, size_t lmax, struct bufrdeco *b )
  \brief Prints info from sec0 in html format
  \param target string target
  \param lmax available size in target
  \param b pointer to the source struct \ref bufrdeco
  \return If succeeded return 0
*/ 
int sprint_sec0_info_html ( char *target, size_t lmax, struct bufrdeco *b )
{
  size_t used = 0;

  bufrdeco_assert ( b != NULL && lmax != 0 && target != NULL );
  
  used += snprintf ( target + used, lmax - used, "<table class='bufr_sec0'>\n<caption>SEC 0 INFO</caption>\n" );
  used += snprintf ( target + used, lmax - used, "<tr><td>Bufr length</td><td>%5u</td></tr>\n", b->sec0.bufr_length );
  used += snprintf ( target + used, lmax - used, "<tr><td>Bufr edition</td><td>%5u</td></tr>\n", b->sec0.edition );
  snprintf ( target + used, lmax - used, "</table>\n" );
  return 0;
}

/*!
  \fn int sprint_sec1_info_html( char *target, size_t lmax, struct bufrdeco *b )
  \brief Prints info from sec1
  \param target string target
  \param lmax available size in target
  \param b pointer to the source struct \ref bufrdeco
  \return If succeeded return 0
*/
int sprint_sec1_info_html ( char *target, size_t lmax, struct bufrdeco *b )
{
  size_t used = 0;

  bufrdeco_assert ( b != NULL && lmax != 0 && target != NULL );
 
  used += snprintf ( target + used , lmax - used,"<div class='bufr_sec1'>\n" );
  used += snprintf ( target + used , lmax - used,"<table>\n<caption>SEC 1 INFO</caption>\n" );
  used += snprintf ( target + used , lmax - used,"<tr><td>Sec1 length</td><td>%5u</td></tr>\n", b->sec1.length );
  used += snprintf ( target + used , lmax - used,"<tr><td>Bufr master table</td><td>%5u</td></tr>\n", b->sec1.master );
  used += snprintf ( target + used , lmax - used,"<tr><td>Centre</td><td>%5u</td></tr>\n", b->sec1.centre );
  used += snprintf ( target + used , lmax - used,"<tr><td>Sub-Centre</td><td>%5u</td></tr>\n", b->sec1.subcentre );
  used += snprintf ( target + used , lmax - used,"<tr><td>Update sequence</td><td>%5u</td></tr>\n", b->sec1.update );
  used += snprintf ( target + used , lmax - used,"<tr><td>Options</td><td>%5x</td></tr>\n", b->sec1.options );
  used += snprintf ( target + used , lmax - used,"<tr><td>Category</td><td>%5u</td></tr>\n", b->sec1.category );
  used += snprintf ( target + used , lmax - used,"<tr><td>Subcategory</td><td>%5u</td></tr>\n", b->sec1.subcategory );
  used += snprintf ( target + used , lmax - used,"<tr><td>Sub-category local</td><td>%5u</td></tr>\n", b->sec1.subcategory_local );
  used += snprintf ( target + used , lmax - used,"<tr><td>Master table version</td><td>%5u</td></tr>\n", b->sec1.master_version );
  used += snprintf ( target + used , lmax - used,"<tr><td>Master table local</td><td>%5u</td></tr>\n", b->sec1.master_local );
  used += snprintf ( target + used , lmax - used,"<tr><td>Year</td><td>%5u</td></tr>\n", b->sec1.year );
  used += snprintf ( target + used , lmax - used,"<tr><td>Month</td><td>%5u</td></tr>\n", b->sec1.month );
  used += snprintf ( target + used , lmax - used,"<tr><td>Day</td><td>%5u</td></tr>\n", b->sec1.day );
  used += snprintf ( target + used , lmax - used,"<tr><td>Hour</td><td>%5u</td></tr>\n", b->sec1.hour );
  used += snprintf ( target + used , lmax - used,"<tr><td>Minute</td><td>%5u</td></tr>\n", b->sec1.minute );
  used += snprintf ( target + used , lmax - used,"<tr><td>Second</td><td>%5u</td></tr>\n", b->sec1.second );
  if ( b->sec0.edition == 3 )
    used += snprintf ( target + used , lmax - used,"<tr><td>Aditional space</td><td>%5u</td></tr>\n", b->sec1.length - 17 );
  else
    used += snprintf ( target + used , lmax - used,"<tr><td>Aditional space</td><td>%5u</td></tr>\n", b->sec1.length - 22 );

  if ( b->tables->b.path[0] )
    {
      used += snprintf ( target + used , lmax - used,"<tr><td>Tables used</td><td>'%s'<br>\n", b->tables->b.path );
      used += snprintf ( target + used , lmax - used,"'%s'<br>\n", b->tables->c.path );
      used += snprintf ( target + used , lmax - used,"'%s'</td></tr>\n", b->tables->d.path );
    }
  used += snprintf ( target + used , lmax - used,"</table>\n" );
  snprintf ( target + used , lmax - used,"</div>\n" );
  return 0;
}

/*!
  \fn int sprint_sec3_info_html( char *target, size_t lmax, struct bufrdeco *b )
  \brief Prints info from sec3 formatted as html
  \param target string target
  \param lmax available size in target
  \param b pointer to the source struct \ref bufrdeco
  \return If succeeded return 0
*/
int sprint_sec3_info_html ( char *target, size_t lmax, struct bufrdeco *b )
{
  size_t i;
  size_t used = 0; 

  bufrdeco_assert ( b != NULL && lmax != 0 && target != NULL );
 
  used += snprintf ( target + used , lmax - used,"<div class='bufr_sec3'>\n" );
  used += snprintf ( target + used , lmax - used,"<table>\n<caption>SEC 3 INFO</caption>\n" );
  used += snprintf ( target + used , lmax - used,"<tr><td>Sec3 length</td><td>%5u\n", b->sec3.length );
  used += snprintf ( target + used , lmax - used,"<tr><td>Subsets</td><td>%5u\n", b->sec3.subsets );
  used += snprintf ( target + used , lmax - used,"<tr><td>Observed</td><td>%5u\n", b->sec3.observed );
  used += snprintf ( target + used , lmax - used,"<tr><td>Compressed</td><td>%5u\n", b->sec3.compressed );
  used += snprintf ( target + used , lmax - used,"<tr><td>Unexpanded descriptors</td><td>%5u</td></tr>\n", b->sec3.ndesc );
  for ( i = 0; i < b->sec3.ndesc; i++ )
    {
      used += snprintf ( target + used , lmax - used,"<tr><td>%3lu:</td><td>%u %02u %03u</td></tr>\n", i, b->sec3.unexpanded[i].f,
                     b->sec3.unexpanded[i].x, b->sec3.unexpanded[i].y );
    }
  used += snprintf ( target + used , lmax - used,"</table>\n" );
  snprintf ( target + used , lmax - used,"</div>\n" );
  return 0;
}

/*!
  \fn int sprint_sec4_info_html( char *target, size_t lmax, struct bufrdeco *b )
  \brief Prints info from sec4
  \param target string target
  \param lmax available size in target
  \param b pointer to the source struct \ref bufrdeco
  \return If succeeded return 0, 1 otherwise
*/
int sprint_sec4_info_html ( char *target, size_t lmax, struct bufrdeco *b )
{
  size_t used = 0;

  bufrdeco_assert ( b != NULL && lmax != 0 && target != NULL );
  
  used += snprintf ( target + used , lmax - used,"<div class='bufr_sec4'>\n" );
  used += snprintf ( target + used , lmax - used,"<table>\n<caption>SEC 4 INFO</caption>\n" );
  used += snprintf ( target + used , lmax - used,"<tr><td>Sec4 length</td><td>%5u</td></tr>\n", b->sec4.length );
  used += snprintf ( target + used , lmax - used,"</table>\n" );
  snprintf ( target + used , lmax - used,"</div>\n" );
  return 0;
}

/*!
  \fn char * bufrdeco_print_atom_data_html ( char *target, size_t lmax, struct bufr_atom_data *a, uint32_t ss )
  \brief print the data in a struct \ref bufr_atom_data to a string as cells of table rows
  \param target string where to print the result
  \param lmax dimension of target
  \param a pointer to struct ref \ref bufr_atom_data with data to print
  \param ss index of subset in bufr

  \return a pointer to result string
*/
char * bufrdeco_print_atom_data_html ( char *target, size_t lmax, struct bufr_atom_data *a, uint32_t ss )
{
  char aux[256];
  size_t used = 0;

  bufrdeco_assert ( a != NULL && lmax != 0 && target != NULL );
   
  used += snprintf ( target + used , lmax - used,"<td class='desc'>%u %02u %03u</td>", a->desc.f, a->desc.x, a->desc.y );
  used += snprintf ( target + used , lmax - used,"<td class='name'>%s</td>", a->name );
  used += snprintf ( target + used , lmax - used,"<td class='unit'>%s</td>", a->unit );
  if ( a->mask & DESCRIPTOR_VALUE_MISSING )
    {
      if ( a->is_bitmaped_by != 0 )
        snprintf ( target + used , lmax - used,"<td class='missing'>MISSING</td><td>NOTE: Bitmaped by <a href='#d%u_%u'>#%u</a></td>", ss, a->is_bitmaped_by, a->is_bitmaped_by );
      else if ( a->bitmap_to != 0 )
        snprintf ( target + used , lmax - used,"<td class='missing'>MISSING</td><td>NOTE: Bitmap to <a href='#d%u_%u'>#%u</a></td>", ss, a->bitmap_to, a->bitmap_to );
      else if ( a->related_to != 0 )
        snprintf ( target + used , lmax - used,"<td class='missing'>MISSING</td><td>NOTE: Related to <a href='#d%u_%u'>#%u</a></td>", ss, a->related_to, a->related_to );
      else
        snprintf ( target + used , lmax - used,"<td class='missing'>MISSING</td><td></td>" );
    }
  else
    {
      if ( a->mask & DESCRIPTOR_HAVE_STRING_VALUE )
        {
          if ( a->is_bitmaped_by != 0 )
            snprintf ( target + used , lmax - used,"<td></td><td class='cval'>%s<br>NOTE: Bitmaped by <a href='#d%u_%u'>#%u</a></td>\n", a->cval, ss, a->is_bitmaped_by, a->is_bitmaped_by );
          else if ( a->bitmap_to != 0 )
            snprintf ( target + used , lmax - used,"<td></td><td class='cval'>%s<br>NOTE: Bitmap to <a href='#d%u_%u'>#%u</a></td>\n", a->cval, ss, a->bitmap_to, a->bitmap_to );
          else if ( a->related_to != 0 )
            snprintf ( target + used , lmax - used,"<td></td><td class='cval'>%s<br>NOTE: Related to <a href='#d%u_%u'>#%u</a></td>\n", a->cval, ss, a->related_to, a->related_to );
          else
            snprintf ( target + used , lmax - used,"<td></td><td class='cval'>%s</td>\n", a->cval );
        }
      else if ( a->mask & DESCRIPTOR_HAVE_CODE_TABLE_STRING
                || strstr ( a->unit, "CODE TABLE" ) == a->unit
                || strstr ( a->unit, "Code table" ) == a->unit )
        {
          used += snprintf ( target + used , lmax - used,"<td class='ival'>%17u</td>", ( uint32_t ) a->val );
          if ( a->is_bitmaped_by != 0 )
            snprintf ( target + used , lmax - used,"<td class='ctable'>%s<br>NOTE: Bitmaped by <a href='#d%u_%u'>#%u</a></td>\n", a->ctable, ss, a->is_bitmaped_by, a->is_bitmaped_by );
          else if ( a->bitmap_to != 0 )
            snprintf ( target + used , lmax - used,"<td class='ctable'>%s<br>NOTE: Bitmap to <a href='#d%u_%u'>#%u</a></td>\n", a->ctable, ss, a->bitmap_to, a->bitmap_to  );
          else if ( a->related_to != 0 )
            snprintf ( target + used , lmax - used,"<td class='ctable'>%s<br>NOTE: Related to <a href='#d%u_%u'>#%u</a></td>\n", a->ctable, ss, a->related_to, a->related_to  );
          else
            snprintf ( target + used , lmax - used,"<td class='ctable'>%s</td>\n", a->ctable );
        }
      else if ( a->mask & DESCRIPTOR_HAVE_FLAG_TABLE_STRING )
        {
          used += snprintf ( target + used , lmax - used,"<td class='hval'>0x%08X</td>", ( uint32_t ) a->val );
          if ( a->is_bitmaped_by != 0 )
            snprintf ( target + used , lmax - used,"<td class='ctable'>%s<br>NOTE: Bitmaped by <a href='#d%u_%u'>#%u</a></td>\n", a->ctable, ss, a->is_bitmaped_by, a->is_bitmaped_by );
          else if ( a->bitmap_to != 0 )
            snprintf ( target + used , lmax - used,"<td class='ctable'>%s<br>NOTE: Bitmap to <a href='#d%u_%u'>#%u</a></td>\n", a->ctable, ss, a->bitmap_to, a->bitmap_to  );
          else if ( a->related_to != 0 )
            snprintf ( target + used , lmax - used,"<td class='ctable'>%s<br>NOTE: Related to <a href='#d%u_%u'>#%u</a></td>\n", a->ctable, ss, a->related_to, a->related_to  );
          else
            snprintf ( target + used , lmax - used,"<td class='ctable'>%s</td>\n", a->ctable );
        }
      else
        {
          if ( a->is_bitmaped_by != 0 )
            snprintf ( target + used , lmax - used,"<td class='rval'>%s</td><td>NOTE: Bitmaped by <a href='#d%u_%u'>#%u</a></td>" , get_formatted_value_from_escale ( aux, sizeof (aux), a->escale, a->val ), ss, a->is_bitmaped_by, a->is_bitmaped_by );
          else if ( a->bitmap_to != 0 )
            snprintf ( target + used , lmax - used,"<td class='rval'>%s</td><td>NOTE: Bitmap to <a href='#d%u_%u'>#%u</a></td>" , get_formatted_value_from_escale ( aux, sizeof (aux), a->escale, a->val ) , ss, a->bitmap_to, a->bitmap_to );
          else if ( a->related_to != 0 )
            snprintf ( target + used , lmax - used,"<td class='rval'>%s</td><td>NOTE: Related to <a href='#d%u_%u'>#%u</a></td>" , get_formatted_value_from_escale ( aux, sizeof (aux), a->escale, a->val ), ss, a->related_to, a->related_to );
          else
            snprintf ( target + used , lmax - used,"<td class='rval'>%s</td><td></td>" , get_formatted_value_from_escale ( aux, sizeof (aux), a->escale, a->val ) );
        }
    }
  return target;
}

/*!
  \fn int bufrdeco_fprint_subset_sequence_data_html ( struct bufrdeco_subset_sequence_data *s )
  \brief Prints a struct \ref bufrdeco_subset_sequence_data as an html table
  \param s pointer to the struct to print
  \return If succeeded return 0
*/
int bufrdeco_fprint_subset_sequence_data_html ( FILE *f, struct bufrdeco_subset_sequence_data *s )
{
  size_t i;
  char aux[1024];

  bufrdeco_assert ( f != NULL && s != NULL );  

  fprintf ( f, "<div class='bufr_subset'>\n" );
  fprintf ( f, "<table>\n" );
  for ( i = 0; i < s->nd ; i++ )
    {
      fprintf ( f, "<tr><td>%5lu:</td>%s</tr>\n", i, bufrdeco_print_atom_data_html ( aux, sizeof (aux), &s->sequence[i], s->ss ) );
    }
  fprintf ( f, "</table>\n" );
  fprintf ( f, "</div>\n" );
  return 0;
}

/*!
  \fn int bufrdeco_fprint_subset_sequence_data_html ( struct bufrdeco_subset_sequence_data *s )
  \brief Prints a struct \ref bufrdeco_subset_sequence_data as an html table
  \param s pointer to the struct to print
  \return If succeeded return 0
*/
int bufrdeco_print_subset_sequence_data_html ( struct bufrdeco_subset_sequence_data *s )
{
  bufrdeco_assert ( s != NULL );
  
  return bufrdeco_fprint_subset_sequence_data_html ( stdout, s );
}

/*!
  \fn int bufrdeco_fprint_subset_sequence_data_tagged_html ( struct bufrdeco_subset_sequence_data *s, char *id )
  \brief Prints a struct \ref bufrdeco_subset_sequence_data as an html table
  \param s pointer to the struct to print
  \param id string with id for the subset
  \return If succeeded return 0
*/
int bufrdeco_fprint_subset_sequence_data_tagged_html ( FILE *f, struct bufrdeco_subset_sequence_data *s, char *id )
{
  size_t i;
  char aux[1024];
  
  bufrdeco_assert ( f != NULL && s != NULL && id != NULL );  

  fprintf ( f, "\n<div class='bufr_subset' id='%s'>\n", id );
  fprintf ( f, "<table>\n" );
  fprintf ( f, "<caption>%s</caption>\n", id );
  fprintf ( f, "<tr><th></th><th>Descriptor</th><th>Name</th><th>Unit</th><th>Value</th><th>Description</th></tr>\n" );
  for ( i = 0; i < s->nd ; i++ )
    {
      fprintf ( f, "<tr><td class='ndesc' id='d_%u_%lu' >#%lu</td>%s</tr>\n", s->ss, i, i, bufrdeco_print_atom_data_html ( aux, sizeof (aux), &s->sequence[i], s->ss ) );
    }
  fprintf ( f, "</table>\n" );
  fprintf ( f, "</div>\n" );
  return 0;
}

/*!
  \fn int bufrdeco_fprint_subset_sequence_data_tagged_html ( struct bufrdeco_subset_sequence_data *s, char *id )
  \brief Prints a struct \ref bufrdeco_subset_sequence_data as an html table
  \param s pointer to the struct to print
  \param id string with id for the subset
  \return If succeeded return 0
*/
int bufrdeco_print_subset_sequence_data_tagged_html ( struct bufrdeco_subset_sequence_data *s, char *id )
{
  bufrdeco_assert ( s != NULL );
  
  return bufrdeco_fprint_subset_sequence_data_tagged_html ( stdout, s, id );
}
