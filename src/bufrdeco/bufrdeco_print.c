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
 \file bufrdeco_print.c
 \brief file with the code to print data or any output from library decobufr
 */
#include "bufrdeco.h"

/*!
  \fn int sprint_sec0_info( char *target, size_t lmax, struct bufrdeco *b )
  \brief Prints info from sec0
  \param target string target
  \param lmax available size in target
  \param b pointer to the source struct \ref bufrdeco
*/
int sprint_sec0_info ( char *target, size_t lmax, struct bufrdeco *b )
{
  size_t used = 0;

  bufrdeco_assert ( b != NULL );

  if ( lmax == 0 || target == NULL )
    {
      snprintf ( b->error, sizeof ( b->error ), "%s(): Unspected NULL argument(s)\n", __func__ );
      return 1;
    }
    
  if ( b->mask & BUFRDECO_OUTPUT_HTML )
    return sprint_sec0_info_html ( target, lmax, b );

  used += snprintf ( target + used, lmax - used, "#### SEC 0 INFO ###\n" );
  used += snprintf ( target + used, lmax - used, "Bufr length:           %5u\n", b->sec0.bufr_length );
  used += snprintf ( target + used, lmax - used, "Bufr edition:          %5u\n", b->sec0.edition );
  return 0;
}

/*!
  \fn int print_sec0_info(struct bufrdeco *b)
  \brief Prints info from sec0
  \param b pointer to the source struct \ref bufrdeco
*/
int print_sec0_info ( struct bufrdeco *b )
{
  char caux[512];

  bufrdeco_assert ( b != NULL );

  caux[0] = 0;
  sprint_sec0_info ( caux, 512, b );
  printf ( "%s", caux );
  return 0;
}


/*!
  \fn int_bits sprint_sec1_info( char *target, size_t lmax, struct bufrdeco *b )
  \brief Prints info from sec1
  \param target string target
  \param lmax available size in target
  \param b pointer to the source struct \ref bufrdeco
*/
int sprint_sec1_info ( char *target, size_t lmax, struct bufrdeco *b )
{
  size_t used = 0;
  
  bufrdeco_assert ( b != NULL );

  if ( lmax == 0 || target == NULL )
    {
      snprintf ( b->error, sizeof ( b->error ), "%s(): Unspected NULL argument(s)\n", __func__ );
      return 1;
    }

  if ( b->mask & BUFRDECO_OUTPUT_HTML )
    return sprint_sec1_info_html ( target, lmax, b );

  used += snprintf ( target + used, lmax - used, "\n#### SEC 1 INFO ###\n" );
  used += snprintf ( target + used, lmax - used, "Sec1 length:           %5u\n", b->sec1.length );
  used += snprintf ( target + used, lmax - used, "Bufr master table:     %5u\n", b->sec1.master );
  used += snprintf ( target + used, lmax - used, "Centre:                %5u\n", b->sec1.centre );
  used += snprintf ( target + used, lmax - used, "Sub-Centre:            %5u\n", b->sec1.subcentre );
  used += snprintf ( target + used, lmax - used, "Update sequence:       %5u\n", b->sec1.update );
  used += snprintf ( target + used, lmax - used, "Options:               %5x\n", b->sec1.options );
  used += snprintf ( target + used, lmax - used, "Category:              %5u\n", b->sec1.category );
  used += snprintf ( target + used, lmax - used, "Subcategory:           %5u\n", b->sec1.subcategory );
  used += snprintf ( target + used, lmax - used, "Sub-category local:    %5u\n", b->sec1.subcategory_local );
  used += snprintf ( target + used, lmax - used, "Master table version:  %5u\n", b->sec1.master_version );
  used += snprintf ( target + used, lmax - used, "Master table local:    %5u\n", b->sec1.master_local );
  used += snprintf ( target + used, lmax - used, "Year:                  %5u\n", b->sec1.year );
  used += snprintf ( target + used, lmax - used, "Month:                 %5u\n", b->sec1.month );
  used += snprintf ( target + used, lmax - used, "Day:                   %5u\n", b->sec1.day );
  used += snprintf ( target + used, lmax - used, "Hour:                  %5u\n", b->sec1.hour );
  used += snprintf ( target + used, lmax - used, "Minute:                %5u\n", b->sec1.minute );
  used += snprintf ( target + used, lmax - used, "Second:                %5u\n", b->sec1.second );
  if ( b->sec0.edition == 3 )
    used += snprintf ( target + used, lmax - used, "Aditional space:       %5u\n", b->sec1.length - 17 );
  else
    used += snprintf ( target + used, lmax - used, "Aditional space:       %5u\n", b->sec1.length - 22 );
  if ( b->tables->b.path[0] )
    {
      used += snprintf ( target + used, lmax - used, "Tables used: '%s'\n", b->tables->b.path );
      used += snprintf ( target + used, lmax - used, "             '%s'\n", b->tables->c.path );
      used += snprintf ( target + used, lmax - used, "             '%s'\n", b->tables->d.path );
    }
  return 0;
}

/*!
  \fn int print_sec1_info(struct bufrdeco *b)
  \brief Prints info from sec1
  \param b pointer to the source struct \ref bufrdeco
*/
int print_sec1_info ( struct bufrdeco *b )
{
  char caux[2048];

  bufrdeco_assert ( b != NULL );

  caux[0] = 0;
  sprint_sec1_info ( caux, 2048, b );
  printf ( "%s", caux );
  return 0;
}

/*!
  \fn int sprint_sec3_info( char *target, size_t lmax, struct bufrdeco *b )
  \brief Prints info from sec3
  \param target string target
  \param lmax available size in target
  \param b pointer to the source struct \ref bufrdeco
*/
int sprint_sec3_info ( char *target, size_t lmax, struct bufrdeco *b )
{
  size_t i;
  size_t used = 0;

  bufrdeco_assert ( b != NULL );
  
  if ( lmax == 0 || target == NULL)
    {
      snprintf ( b->error, sizeof ( b->error ), "%s(): Unspected NULL argument(s)\n", __func__ );
      return 1;
    }

  if ( b->mask & BUFRDECO_OUTPUT_HTML )
    return sprint_sec3_info_html ( target, lmax, b );

  used += snprintf ( target + used, lmax - used, "\n#### SEC 3 INFO ###\n" );
  used += snprintf ( target + used, lmax - used, "Sec3 length:           %5u\n", b->sec3.length );
  used += snprintf ( target + used, lmax - used, "Subsets:               %5u\n", b->sec3.subsets );
  used += snprintf ( target + used, lmax - used, "Observed:              %5u\n", b->sec3.observed );
  used += snprintf ( target + used, lmax - used, "Compressed:            %5u\n", b->sec3.compressed );
  used += snprintf ( target + used, lmax - used, "Unexpanded descriptors %5u\n", b->sec3.ndesc );

  for ( i = 0; i < b->sec3.ndesc; i++ )
    {
      used += snprintf ( target + used, lmax - used, "  %3lu:                      %u %02u %03u\n", i, b->sec3.unexpanded[i].f,
                         b->sec3.unexpanded[i].x, b->sec3.unexpanded[i].y );
    }
  return 0;
}

/*!
  \fn int print_sec3_info(struct bufrdeco *b)
  \brief Prints info from sec3
  \param b pointer to the source struct \ref bufrdeco
*/
int print_sec3_info ( struct bufrdeco *b )
{
  char caux[8192];

  bufrdeco_assert ( b != NULL );

  caux[0] = 0;
  sprint_sec3_info ( caux, 8192, b );
  printf ( "%s", caux );
  return 0;
}


/*!
  \fn int sprint_sec4_info( char *target, size_t lmax, struct bufrdeco *b )
  \brief Prints info from sec4
  \param target string target
  \param lmax available size in target
  \param b pointer to the source struct \ref bufrdeco
*/
int sprint_sec4_info ( char *target, size_t lmax, struct bufrdeco *b )
{
  size_t used = 0;

  bufrdeco_assert ( b != NULL );
  
  if ( lmax == 0 || target == NULL )
    {
      snprintf ( b->error, sizeof ( b->error ), "%s(): Unspected NULL argument(s)\n", __func__ );
      return 1;
    }

  if ( b->mask & BUFRDECO_OUTPUT_HTML )
    return sprint_sec4_info_html ( target, lmax, b );

  used += snprintf ( target + used, lmax - used, "\n#### SEC 4 INFO ###\n" );
  used += snprintf ( target + used, lmax - used, "Sec4 length:           %5u\n\n", b->sec4.length );
  return 0;
}

/*!
  \fn int print_sec4_info(struct bufrdeco *b)
  \brief Prints info from sec3
  \param b pointer to the source struct \ref bufrdeco
*/
int print_sec4_info ( struct bufrdeco *b )
{
  char caux[8192];

  bufrdeco_assert ( b != NULL );

  caux[0] = 0;
  sprint_sec4_info ( caux, 8192, b );
  printf ( "%s", caux );
  return 0;
}

/*!
  \fn int bufrdeco_fprint_tree_recursive ( FILE *f, struct bufrdeco *b, struct bufr_sequence *seq )
  \brief  Print a tree of descriptors to a file in a recursive way
  \param f Pointer to file opened by caller
  \param b pointer to the basic container struct \ref bufrdeco
  \param seq pointer to the struct \ref bufr_sequence  to print

  If succeded return 0, else return 1
*/
int bufrdeco_fprint_tree_recursive ( FILE *f, struct bufrdeco *b, struct bufr_sequence *seq )
{
  buf_t i, j, k;
  struct bufr_sequence *l;
  char explanation[256];

  bufrdeco_assert ( f != NULL && b != NULL );

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
      // we search for descriptors with f == 3

      for ( j = 0; j < l->level; j++ )
        {
          fprintf ( f, "        |" );
        }
      fprintf ( f,  "%u %02u %03u", l->lseq[i].f, l->lseq[i].x,l->lseq[i].y );

      if ( l->lseq[i].f != 3 )
        {
          if ( l->lseq[i].f == 0 )
            {
              if ( bufr_find_tableB_index ( &k, & ( b->tables->b ), l->lseq[i].c ) )
                fprintf ( f, "\n" );
              else
                {
                  fprintf ( f, ":" );
                  if ( l->replicated[i] )
                    {
                      //fprintf ( f, " " );
                      for ( j = 0; j < l->replicated[i] ; j++ )
                        fprintf ( f, "*" );
                    }
                  if ( is_a_delayed_descriptor ( & l->lseq[i] ) ||
                       is_a_short_delayed_descriptor ( & l->lseq[i] ) )
                    fprintf ( f, "* %s\n", b->tables->b.item[k].name );
                  else
                    fprintf ( f, " %s\n", b->tables->b.item[k].name );
                }
            }
          else if ( l->lseq[i].f == 2 )
            {
              fprintf ( f, ": %s\n", bufrdeco_get_f2_descriptor_explanation ( explanation, sizeof ( explanation ), & ( l->lseq[i] ) ) );
            }
          else if ( l->lseq[i].f == 1 )
            {
              if ( l->lseq[i].y == 0 )
                {
                  if ( l->lseq[i].x == 1 )
                    fprintf ( f, ":* Replicator for %d descriptor after next delayed descriptor which set the number of replications.\n", l->lseq[i].x );
                  else
                    fprintf ( f, ":* Replicator for %d descriptors after next delayed descriptor which set the number of replications.\n", l->lseq[i].x );
                }
              else
                {
                  if ( l->lseq[i].x == 1 )
                    fprintf ( f, ":* Replicator for next descriptor %d times\n", l->lseq[i].y );
                  else
                    fprintf ( f, ":* Replicator for next %d descriptors %d times\n", l->lseq[i].x, l->lseq[i].y );
                }
            }
          else
            fprintf ( f, "\n" );
          continue;
        }

      // we then recursively parse the son
      if ( l->replicated[i] )
        {
          fprintf ( f, ":" );
          for ( j = 0; j < l->replicated[i] ; j++ )
            fprintf ( f, "*" );
        }

      fprintf ( f, "-> %s\n", l->sons[i]->name );
      if ( bufrdeco_fprint_tree_recursive ( f, b, l->sons[i] ) )
        {
          return 1;
        }

    }

  return 0;
}

/*!
  \fn int bufrdeco_fprint_tree ( FILE *f, struct bufrdeco *b )
  \brief Print a tree of descriptors
  \param f Pointer to file opened by caller
  \param b pointer to a basic container struct \ref bufrdeco
*/
int bufrdeco_fprint_tree ( FILE *f, struct bufrdeco *b )
{
  bufrdeco_assert ( f != NULL && b != NULL );

  if ( b->mask & BUFRDECO_OUTPUT_HTML )
    fprintf ( f, "<pre>\n" );
  bufrdeco_fprint_tree_recursive ( f, b, NULL );
  if ( b->mask & BUFRDECO_OUTPUT_HTML )
    fprintf ( f, "</pre>\n" );
  else
    fprintf ( f, "\n" );
  return 0;
};


/*!
  \fn int bufrdeco_print_tree ( struct bufrdeco *b )
  \brief Print a tree of descriptors
  \param b pointer to a basic container struct \ref bufrdeco
*/
int bufrdeco_print_tree ( struct bufrdeco *b )
{
  bufrdeco_assert ( b != NULL );

  if ( b->mask & BUFRDECO_OUTPUT_HTML )
    printf ( "<pre>\n" );
  bufrdeco_fprint_tree_recursive ( stdout, b, NULL );
  if ( b->mask & BUFRDECO_OUTPUT_HTML )
    printf ( "</pre>\n" );
  else
    printf ( "\n" );
  return 0;
};


/*!
  \fn int bufrdeco_print_atom_data_file ( FILE *f, struct bufr_atom_data *a )
  \brief print the data in a struct \ref bufr_atom_data to a file already open by caller
  \param f Pointer to file opened by caller
  \param a pointer to struct ref \ref bufr_atom_data with data to print
*/
int bufrdeco_print_atom_data_file ( FILE *f, struct bufr_atom_data *a )
{
  char aux[1024];
  bufrdeco_assert ( f != NULL && a != NULL );

  bufrdeco_print_atom_data ( aux, sizeof ( aux ), a );
  fprintf ( f,  "%s\n",aux );
  return 0;
}

/*!
  \fn int bufrdeco_print_atom_data_stdout ( struct bufr_atom_data *a )
  \brief print the data in a struct \ref bufr_atom_data to stdout
  \param a pointer to struct ref \ref bufr_atom_data with data to print
*/
int bufrdeco_print_atom_data_stdout ( struct bufr_atom_data *a )
{
  return bufrdeco_print_atom_data_file ( stdout, a );
}

/*!
  \fn char * bufrdeco_print_atom_data ( char *target, size_t lmax, struct bufr_atom_data *a )
  \brief print the data in a struct \ref bufr_atom_data to a string
  \param target string where to print the result
  \param lmax size of allocated string \a target 
  \param a pointer to struct ref \ref bufr_atom_data with data to print

  Returns a pointer to result string
*/
char * bufrdeco_print_atom_data ( char *target, size_t lmax, struct bufr_atom_data *a )
{
  char aux[256];
  size_t used = 0;
  size_t nlimit, climit;
  
  bufrdeco_assert ( a != NULL && target != NULL );

  used += snprintf ( target + used, lmax - used, "%u %02u %03u ", a->desc.f, a->desc.x, a->desc.y );
  strcpy_safe ( aux, a->name );
  aux[64] = '\0';
  used += snprintf ( target + used, lmax - used, "%-64s ", aux );
  strcpy_safe ( aux, a->unit );
  aux[20] = '\0';
  used += snprintf ( target + used, lmax - used, "%-20s", aux );
  if ( a->mask & DESCRIPTOR_VALUE_MISSING )
    {
      used += snprintf ( target + used, lmax - used, "%17s", "MISSING" );
    }
  else
    {
      if ( a->mask & DESCRIPTOR_HAVE_STRING_VALUE )
        {
          strcpy_safe ( aux, a->cval );
          aux[64] = '\0';
          used += snprintf ( target + used, lmax - used, "                  " );
          used += snprintf ( target + used, lmax - used, "%s", aux );
        }
      else if ( a->mask & DESCRIPTOR_HAVE_CODE_TABLE_STRING
                || strstr ( a->unit, "CODE TABLE" ) == a->unit
                || strstr ( a->unit, "Code table" ) == a->unit )
        {
          strcpy_safe ( aux, a->ctable );
          aux[64] = '\0';
          used += snprintf ( target + used, lmax - used, "%17u ", ( uint32_t ) a->val );
          used += snprintf ( target + used, lmax - used, "%s", aux );
        }
      else if ( a->mask & DESCRIPTOR_HAVE_FLAG_TABLE_STRING )
        {
          strcpy_safe ( aux, a->ctable );
          aux[64] = '\0';
          used += snprintf ( target + used, lmax - used, "       0x%08X ", ( uint32_t ) a->val );
          used += snprintf ( target + used, lmax - used, "%s", aux );
        }
      else
        {
          used += snprintf ( target + used, lmax - used, "%s ", get_formatted_value_from_escale ( aux, sizeof ( aux ), a->escale, a->val ) );
        }

    }

  // Now print remaining chars in a->name or a->ctable
  nlimit = 64;
  climit = 64;
  while ( ( strlen ( a->name ) > nlimit && nlimit < BUFR_TABLEB_NAME_LENGTH ) ||
          ( strlen ( a->ctable ) > climit && climit < BUFR_EXPLAINED_LENGTH ) )
    {
      aux[0] = 0;
      if ( strlen ( a->name ) > nlimit && nlimit < BUFR_TABLEB_NAME_LENGTH )
        strncpy ( aux, a->name + nlimit, 64 );
      used += snprintf ( target + used, lmax - used, "\n                 %-64s", aux );
      aux[0] = 0;
      if ( strlen ( a->ctable ) > climit && climit < BUFR_EXPLAINED_LENGTH )
        strncpy ( aux, a->ctable + climit, 64 );
      used += snprintf ( target + used, lmax - used, "                                       %s", aux );
      nlimit += 64;
      climit += 64;
    }

  if ( a->is_bitmaped_by != 0 )
    used += snprintf ( target + used, lmax - used, " *IS BITMAPED BY #%u*", a->is_bitmaped_by );

  if ( a->bitmap_to != 0 )
    used += snprintf ( target + used, lmax - used, " *BITMAP TO #%u*", a->bitmap_to );

  if ( a->related_to != 0 )
    used += snprintf ( target + used, lmax - used, " *RELATED TO #%u*", a->related_to );

  return target;
}

/*!
  \fn int bufrdeco_fprint_subset_sequence_data ( struct bufrdeco_subset_sequence_data *s )
  \brief Prints a struct \ref bufrdeco_subset_sequence_data
  \param s pointer to the struct to print
*/
int bufrdeco_fprint_subset_sequence_data ( FILE *f, struct bufrdeco_subset_sequence_data *s )
{
  size_t i;
  char aux[1024];

  bufrdeco_assert ( f != NULL && s != NULL);

  for ( i = 0; i < s->nd ; i++ )
    {
      if ( i && s->sequence[i].seq != s->sequence[i - 1].seq )
        fprintf ( f, "\n" );

      fprintf ( f, "%5lu:  %s\n", i, bufrdeco_print_atom_data ( aux, sizeof ( aux ), &s->sequence[i] ) );
    }
  return 0;

}



/*!
  \fn int bufrdeco_print_subset_sequence_data ( struct bufrdeco_subset_sequence_data *s )
  \brief Prints a struct \ref bufrdeco_subset_sequence_data
  \param s pointer to the struct to print
*/
int bufrdeco_print_subset_sequence_data ( struct bufrdeco_subset_sequence_data *s )
{
  bufrdeco_assert ( s != NULL );
  return bufrdeco_fprint_subset_sequence_data ( stdout, s );
}

/*!
  \fn int fprint_bufrdeco_compressed_ref ( FILE *f, struct bufrdeco_compressed_ref *r )
  \brief prints a struct bufrdeco_compressed_ref
  \param f pointer to a file opened by caller
  \param r pointer to the struct to print
*/
int fprint_bufrdeco_compressed_ref ( FILE *f, struct bufrdeco_compressed_ref *r )
{
  bufrdeco_assert ( f != NULL && r != NULL );
  
  if ((r->mask & BUFRDECO_COMPRESSED_REF_DATA_DESCRIPTOR_BITMASK) == 0)
    return 0;
  
  fprintf ( f, "%s -> A=%u, D=%u, ",r->desc->c,r->is_associated,r->has_data );
  if ( r->cref0[0] == '\0' )
    {
      fprintf ( f, "bits=%2u, ref=%10d, escale=%3d,",  r->bits, r->ref, r->escale );
      fprintf ( f, " bit0=%10u, ref0=%10u, inc_bits=%2u ", r->bit0, r->ref0, r->inc_bits );
      fprintf ( f, "%s %s\n",r->name, r->unit );
    }
  else
    {
      fprintf ( f, "'%s', chars=%3u\n", r->cref0, r->inc_bits );
    }
  return 0;
}

/*!
  \fn int print_bufrdeco_compressed_ref ( struct bufrdeco_compressed_ref *r )
  \brief prints a struct bufrdeco_compressed_ref
  \param r pointer to the struct to print
*/
int print_bufrdeco_compressed_ref ( struct bufrdeco_compressed_ref *r )
{
  bufrdeco_assert ( r != NULL );
  
  return fprint_bufrdeco_compressed_ref ( stdout, r );
}

/*!
  \fn int print_bufrdeco_compressed_data_references ( struct bufrdeco_compressed_data_references *r )
  \brief prints a struct bufrdeco_compressed_references
  \param r pointer to the struct to print

  It is used mainly in debug stage
*/
int print_bufrdeco_compressed_data_references ( struct bufrdeco_compressed_data_references *r )
{
  size_t  i;
  bufrdeco_assert ( r != NULL );

  for ( i = 0; i < r->nd; i++ )
    print_bufrdeco_compressed_ref ( & r->refs[i] );

  return 0;
}

/*!
  \fn int fprint_bufrdeco_compressed_data_references ( struct bufrdeco_compressed_data_references *r )
  \brief prints a struct bufrdeco_compressed_references
  \param r pointer to the struct to print

  It is used mainly in debug stage
*/
int fprint_bufrdeco_compressed_data_references ( FILE *f, struct bufrdeco_compressed_data_references *r )
{
  size_t  i;
  
  bufrdeco_assert ( f != NULL && r != NULL);

  for ( i = 0; i < r->nd; i++ )
    fprint_bufrdeco_compressed_ref ( f, & r->refs[i] );

  return 0;
}
