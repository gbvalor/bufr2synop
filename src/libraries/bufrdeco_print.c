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
 \file bufrdeco_print.c
 \brief file with the code to print data or any output from library decobufr
 */
#include "bufrdeco.h"

/*!
  \fn void print_sec0_info(struct bufr *b)
  \brief Prints info from sec0
  \param b pointer to the source struct \ref bufr
*/
void print_sec0_info ( struct bufr *b )
{
  printf ( "#### SEC 0 INFO ###\n" );
  printf ( "Bufr length:           %5u\n", b->sec0.bufr_length );
  printf ( "Bufr edition:          %5u\n", b->sec0.edition );
}

/*!
  \fn void print_sec1_info(struct bufr *b)
  \brief Prints info from sec1
  \param b pointer to the source struct \ref bufr
*/
void print_sec1_info ( struct bufr *b )
{
  printf ( "\n#### SEC 1 INFO ###\n" );
  printf ( "Sec1 length:           %5u\n", b->sec1.length );
  printf ( "Bufr master table:     %5u\n", b->sec1.master );
  printf ( "Centre:                %5u\n", b->sec1.centre );
  printf ( "Sub-Centre:            %5u\n", b->sec1.subcentre );
  printf ( "Update sequence:       %5u\n", b->sec1.update );
  printf ( "Options:               %5x\n", b->sec1.options );
  printf ( "Category:              %5u\n", b->sec1.category );
  printf ( "Subcategory:           %5u\n", b->sec1.subcategory );
  printf ( "Sub-category local:    %5u\n", b->sec1.subcategory_local );
  printf ( "Master table version:  %5u\n", b->sec1.master_version );
  printf ( "Master table local:    %5u\n", b->sec1.master_local );
  printf ( "Year:                  %5u\n", b->sec1.year );
  printf ( "Month:                 %5u\n", b->sec1.month );
  printf ( "Day:                   %5u\n", b->sec1.day );
  printf ( "Hour:                  %5u\n", b->sec1.hour );
  printf ( "Minute:                %5u\n", b->sec1.minute );
  printf ( "Second:                %5u\n", b->sec1.second );
  printf ( "Aditional space:       %5u\n", b->sec1.length - 22 );
}

/*!
  \fn void print_sec3_info(struct bufr *b)
  \brief Prints info from sec3
  \param b pointer to the source struct \ref bufr
*/
void print_sec3_info ( struct bufr *b )
{
  size_t i;
  printf ( "\n#### SEC 3 INFO ###\n" );
  printf ( "Sec3 length:           %5u\n", b->sec3.length );
  printf ( "Subsets:               %5u\n", b->sec3.subsets );
  printf ( "Observed:              %5u\n", b->sec3.observed );
  printf ( "Compressed:            %5u\n", b->sec3.compressed );
  printf ( "Unexpanded descriptors %5u\n", b->sec3.ndesc );
  for ( i = 0; i < b->sec3.ndesc; i++ )
    {
      printf ( "  %3lu:                      %u %02u %03u\n", i, b->sec3.unexpanded[i].f,
               b->sec3.unexpanded[i].x, b->sec3.unexpanded[i].y );
    }
}

/*!
  \fn void print_sec4_info(struct bufr *b)
  \brief Prints info from sec3
  \param b pointer to the source struct \ref bufr
*/
void print_sec4_info ( struct bufr *b )
{
  printf ( "\n#### SEC 4 INFO ###\n" );
  printf ( "Sec4 length:           %5u\n", b->sec4.length );
}

/*!
  \fn int bufr_print_tree_recursive ( struct bufr *b, struct bufr_sequence *seq )
*/ 
int bufr_print_tree_recursive ( struct bufr *b, struct bufr_sequence *seq )
{
  size_t i, j;
  struct bufr_sequence *l;

  if ( seq == NULL )
    {
      l = & ( b->tree->seq[0] );
    }
  else
    {
      l = seq;
    }

  for ( i = 0; i < l->ndesc; i++ )
    {
      // we search for descriptors woth f == 3

      for ( j = 0; j < l->level; j++ )
        {
          printf ( "        " );
        }
      printf ( "%u %02u %03u\n", l->lseq[i].f, l->lseq[i].x,l->lseq[i].y );

      if ( l->lseq[i].f != 3 )
        {
          continue;
        }

      // we then recursively parse the son
      if ( bufr_print_tree_recursive ( b, l->sons[i] ) )
        {
          return 1;
        }

    }

  return 0;
}

void bufr_print_tree ( struct bufr *b )
{
  bufr_print_tree_recursive ( b, NULL );
};


void bufr_print_atom_data_stdout ( struct bufr_atom_data *a )
{
  char aux[256];
  bufr_print_atom_data ( aux, a );
  printf ( "%s\n",aux );
}

char * bufr_print_atom_data ( char *target, struct bufr_atom_data *a )
{
  char aux[256], *c;
  c = target;
  c += sprintf ( c, "%u %02u %03u ", a->desc.f, a->desc.x, a->desc.y );
  strcpy ( aux, a->name );
  aux[40] = '\0';
  c += sprintf ( c, "%-40s ", aux );
  strcpy ( aux, a->unit );
  aux[20] = '\0';
  c += sprintf ( c, "%-20s", aux );
  if ( a->mask & DESCRIPTOR_VALUE_MISSING )
    c += sprintf ( c, "%+17s", "MISSING VALUE" );
  else
    {
      if ( a->mask & DESCRIPTOR_HAVE_STRING_VALUE )
        {
          strcpy ( aux, a->cval );
          aux[56] = '\0';
          c += sprintf ( c, "                  " );
          c += sprintf ( c, "%s", aux );
        }
      else if ( a->mask & DESCRIPTOR_HAVE_CODE_TABLE_STRING )
        {
          strcpy ( aux, a->ctable );
          aux[56] = '\0';
          c += sprintf ( c, "%17u ", ( uint32_t ) a->val );
          c += sprintf ( c, "%s", aux );
        }
      else if ( a->mask & DESCRIPTOR_HAVE_FLAG_TABLE_STRING )
        {
          strcpy ( aux, a->ctable );
          aux[56] = '\0';
          c += sprintf ( c, "       0x%08X ", ( uint32_t ) a->val );
          c += sprintf ( c, "%s", aux );
        }
      else
        {
          c += sprintf ( c, "%17.10e ", a->val );
        }
    }
  return target;
}

void bufr_print_subset_sequence_data ( struct bufr_subset_sequence_data *s )
{
  size_t i;
  char aux[1024];
  for ( i = 0; i < s->nd ; i++ )
    {
      printf ( "%5lu:  %s\n", i, bufr_print_atom_data ( aux, &s->sequence[i] ) );
    }
}
