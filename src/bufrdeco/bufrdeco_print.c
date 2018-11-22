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
 \file bufrdeco_print.c
 \brief file with the code to print data or any output from library decobufr
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
  \fn void sprint_sec0_info( char *target, size_t lmax, struct bufrdeco *b )
  \brief Prints info from sec0
  \param target string target
  \param lmax available size in target
  \param b pointer to the source struct \ref bufrdeco
*/
int sprint_sec0_info ( char *target, size_t lmax, struct bufrdeco *b )
{
  char caux[512], *c;

  if ( b->mask & BUFRDECO_OUTPUT_HTML )
    return sprint_sec0_info_html ( target, lmax, b );

  c = caux;
  c += sprintf ( c, "#### SEC 0 INFO ###\n" );
  c += sprintf ( c, "Bufr length:           %5u\n", b->sec0.bufr_length );
  c += sprintf ( c, "Bufr edition:          %5u\n", b->sec0.edition );
  strcat_protected ( target, caux, lmax );
  return 0;
}

/*!
  \fn void print_sec0_info(struct bufrdeco *b)
  \brief Prints info from sec0
  \param b pointer to the source struct \ref bufrdeco
*/
void print_sec0_info ( struct bufrdeco *b )
{
  char caux[512];
  caux[0] = 0;
  sprint_sec0_info ( caux, 512, b );
  printf ( "%s", caux );
}


/*!
  \fn void sprint_sec1_info( char *target, size_t lmax, struct bufrdeco *b )
  \brief Prints info from sec1
  \param target string target
  \param lmax available size in target
  \param b pointer to the source struct \ref bufrdeco
*/
int sprint_sec1_info ( char *target, size_t lmax, struct bufrdeco *b )
{
  char caux[2048], *c;

  if ( b->mask & BUFRDECO_OUTPUT_HTML )
    return sprint_sec1_info_html ( target, lmax, b );

  c = caux;
  c += sprintf ( c, "\n#### SEC 1 INFO ###\n" );
  c += sprintf ( c, "Sec1 length:           %5u\n", b->sec1.length );
  c += sprintf ( c, "Bufr master table:     %5u\n", b->sec1.master );
  c += sprintf ( c, "Centre:                %5u\n", b->sec1.centre );
  c += sprintf ( c, "Sub-Centre:            %5u\n", b->sec1.subcentre );
  c += sprintf ( c, "Update sequence:       %5u\n", b->sec1.update );
  c += sprintf ( c, "Options:               %5x\n", b->sec1.options );
  c += sprintf ( c, "Category:              %5u\n", b->sec1.category );
  c += sprintf ( c, "Subcategory:           %5u\n", b->sec1.subcategory );
  c += sprintf ( c, "Sub-category local:    %5u\n", b->sec1.subcategory_local );
  c += sprintf ( c, "Master table version:  %5u\n", b->sec1.master_version );
  c += sprintf ( c, "Master table local:    %5u\n", b->sec1.master_local );
  c += sprintf ( c, "Year:                  %5u\n", b->sec1.year );
  c += sprintf ( c, "Month:                 %5u\n", b->sec1.month );
  c += sprintf ( c, "Day:                   %5u\n", b->sec1.day );
  c += sprintf ( c, "Hour:                  %5u\n", b->sec1.hour );
  c += sprintf ( c, "Minute:                %5u\n", b->sec1.minute );
  c += sprintf ( c, "Second:                %5u\n", b->sec1.second );
  if (b->sec0.edition == 3)
    c += sprintf ( c, "Aditional space:       %5u\n", b->sec1.length - 17 );
  else    
    c += sprintf ( c, "Aditional space:       %5u\n", b->sec1.length - 22 );
  if ( b->tables->b.path[0] )
    {
      c += sprintf ( c, "Tables used: '%s'\n", b->tables->b.path );
      c += sprintf ( c, "             '%s'\n", b->tables->c.path );
      c += sprintf ( c, "             '%s'\n", b->tables->d.path );
    }
  strcat_protected ( target, caux, lmax );
  return 0;
}

/*!
  \fn void print_sec1_info(struct bufrdeco *b)
  \brief Prints info from sec1
  \param b pointer to the source struct \ref bufrdeco
*/
void print_sec1_info ( struct bufrdeco *b )
{
  char caux[2048];
  caux[0] = 0;
  sprint_sec1_info ( caux, 2048, b );
  printf ( "%s", caux );
}

/*!
  \fn void sprint_sec3_info( char *target, size_t lmax, struct bufrdeco *b )
  \brief Prints info from sec3
  \param target string target
  \param lmax available size in target
  \param b pointer to the source struct \ref bufrdeco
*/
int sprint_sec3_info ( char *target, size_t lmax, struct bufrdeco *b )
{
  size_t i;
  char caux[8192], *c;

  if ( b->mask & BUFRDECO_OUTPUT_HTML )
    return sprint_sec3_info_html ( target, lmax, b );

  c = caux;
  c += sprintf ( c, "\n#### SEC 3 INFO ###\n" );
  c += sprintf ( c, "Sec3 length:           %5u\n", b->sec3.length );
  c += sprintf ( c, "Subsets:               %5u\n", b->sec3.subsets );
  c += sprintf ( c, "Observed:              %5u\n", b->sec3.observed );
  c += sprintf ( c, "Compressed:            %5u\n", b->sec3.compressed );
  c += sprintf ( c, "Unexpanded descriptors %5u\n", b->sec3.ndesc );

  for ( i = 0; i < b->sec3.ndesc; i++ )
    {
      c += sprintf ( c, "  %3lu:                      %u %02u %03u\n", i, b->sec3.unexpanded[i].f,
                     b->sec3.unexpanded[i].x, b->sec3.unexpanded[i].y );
    }
  strcat_protected ( target, caux, lmax );
  return 0;
}

/*!
  \fn void print_sec3_info(struct bufrdeco *b)
  \brief Prints info from sec3
  \param b pointer to the source struct \ref bufrdeco
*/
void print_sec3_info ( struct bufrdeco *b )
{
  char caux[8192];
  caux[0] = 0;
  sprint_sec3_info ( caux, 8192, b );
  printf ( "%s", caux );
}


/*!
  \fn void sprint_sec4_info( char *target, size_t lmax, struct bufrdeco *b )
  \brief Prints info from sec4
  \param target string target
  \param lmax available size in target
  \param b pointer to the source struct \ref bufrdeco
*/
int sprint_sec4_info ( char *target, size_t lmax, struct bufrdeco *b )
{
  char caux[8192], *c;

  if ( b->mask & BUFRDECO_OUTPUT_HTML )
    return sprint_sec4_info_html ( target, lmax, b );

  c = caux;
  c += sprintf ( c, "\n#### SEC 4 INFO ###\n" );
  c += sprintf ( c, "Sec4 length:           %5u\n", b->sec4.length );
  strcat_protected ( target, caux, lmax );
  return 0;
}

/*!
  \fn void print_sec4_info(struct bufrdeco *b)
  \brief Prints info from sec3
  \param b pointer to the source struct \ref bufrdeco
*/
void print_sec4_info ( struct bufrdeco *b )
{
  char caux[8192];
  caux[0] = 0;
  sprint_sec4_info ( caux, 8192, b );
  printf ( "%s", caux );
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
  size_t i, j, k;
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
          fprintf ( f, "        |" );
        }
      fprintf ( f,  "%u %02u %03u", l->lseq[i].f, l->lseq[i].x,l->lseq[i].y );

      if ( l->lseq[i].f != 3 )
        {
          if ( l->lseq[i].f == 0 )
            {
              if ( bufr_find_tableb_index ( &k, & ( b->tables->b ), l->lseq[i].c ) )
                fprintf ( f , "\n" );
              else if ( is_a_delayed_descriptor ( & l->lseq[i] ) ||
                        is_a_short_delayed_descriptor ( & l->lseq[i] ) )
                fprintf ( f , ":* %s\n", b->tables->b.item[k].name );
              else
                fprintf ( f , ": %s\n", b->tables->b.item[k].name );
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
                    fprintf ( f, ":* Replicator for next %d descriptor %d times\n", l->lseq[i].x, l->lseq[i].y );
                  else
                    fprintf ( f, ":* Replicator for next %d descriptors %d times\n", l->lseq[i].x, l->lseq[i].y );
                }
            }
          else
            fprintf ( f, "\n" );
          continue;
        }

      // we then recursively parse the son
      fprintf ( f, ":-> %s\n", l->sons[i]->name );
      if ( bufrdeco_fprint_tree_recursive ( f, b, l->sons[i] ) )
        {
          return 1;
        }

    }

  return 0;
}

/*!
  \fn void bufrdeco_fprint_tree ( FILE *f, struct bufrdeco *b )
  \brief Print a tree of descriptors
  \param f Pointer to file opened by caller
  \param b pointer to a basic container struct \ref bufrdeco
*/
void bufrdeco_fprint_tree ( FILE *f, struct bufrdeco *b )
{
  if ( b->mask & BUFRDECO_OUTPUT_HTML )
    fprintf ( f, "<pre>\n" );
  bufrdeco_fprint_tree_recursive ( f, b, NULL );
  if ( b->mask & BUFRDECO_OUTPUT_HTML )
    fprintf ( f, "</pre>\n" );
};


/*!
  \fn void bufrdeco_print_tree ( struct bufrdeco *b )
  \brief Print a tree of descriptors
  \param b pointer to a basic container struct \ref bufrdeco
*/
void bufrdeco_print_tree ( struct bufrdeco *b )
{
  if ( b->mask & BUFRDECO_OUTPUT_HTML )
    printf ( "<pre>\n" );
  bufrdeco_fprint_tree_recursive ( stdout, b, NULL );
  if ( b->mask & BUFRDECO_OUTPUT_HTML )
    printf ( "</pre>\n" );
};


/*!
  \fn void bufrdeco_print_atom_data_file ( FILE *f, struct bufr_atom_data *a )
  \brief print the data in a struct \ref bufr_atom_data to a file already open by caller
  \param f Pointer to file opened by caller
  \param a pointer to struct ref \ref bufr_atom_data with data to print
*/
void bufrdeco_print_atom_data_file ( FILE *f, struct bufr_atom_data *a )
{
  char aux[1024];
  bufrdeco_print_atom_data ( aux, a );
  fprintf ( f,  "%s\n",aux );
}

/*!
  \fn void bufrdeco_print_atom_data_stdout ( struct bufr_atom_data *a )
  \brief print the data in a struct \ref bufr_atom_data to stdout
  \param a pointer to struct ref \ref bufr_atom_data with data to print
*/
void bufrdeco_print_atom_data_stdout ( struct bufr_atom_data *a )
{
  bufrdeco_print_atom_data_file ( stdout, a );
}

/*!
  \fn char * bufrdeco_print_atom_data ( char *target, struct bufr_atom_data *a )
  \brief print the data in a struct \ref bufr_atom_data to a string
  \param target string where to print the result
  \param a pointer to struct ref \ref bufr_atom_data with data to print

  Returns a pointer to result string
*/
char * bufrdeco_print_atom_data ( char *target, struct bufr_atom_data *a )
{
  char aux[256], *c;
  size_t nlimit, climit;

  c = target;
  c += sprintf ( c, "%u %02u %03u ", a->desc.f, a->desc.x, a->desc.y );
  strcpy ( aux, a->name );
  aux[64] = '\0';
  c += sprintf ( c, "%-64s ", aux );
  strcpy ( aux, a->unit );
  aux[20] = '\0';
  c += sprintf ( c, "%-20s", aux );
  if ( a->mask & DESCRIPTOR_VALUE_MISSING )
    {
      c += sprintf ( c, "%17s", "MISSING" );
    }
  else
    {
      if ( a->mask & DESCRIPTOR_HAVE_STRING_VALUE )
        {
          strcpy ( aux, a->cval );
          aux[64] = '\0';
          c += sprintf ( c, "                  " );
          c += sprintf ( c, "%s", aux );
        }
      else if ( a->mask & DESCRIPTOR_HAVE_CODE_TABLE_STRING
                || strstr ( a->unit, "CODE TABLE" ) == a->unit
                || strstr ( a->unit, "Code table" ) == a->unit )
        {
          strcpy ( aux, a->ctable );
          aux[64] = '\0';
          c += sprintf ( c, "%17u ", ( uint32_t ) a->val );
          c += sprintf ( c, "%s", aux );
        }
      else if ( a->mask & DESCRIPTOR_HAVE_FLAG_TABLE_STRING )
        {
          strcpy ( aux, a->ctable );
          aux[64] = '\0';
          c += sprintf ( c, "       0x%08X ", ( uint32_t ) a->val );
          c += sprintf ( c, "%s", aux );
        }
      else
        {
          c += sprintf ( c, "%s ", get_formatted_value_from_escale ( aux, a->escale, a->val ) );
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
        strcpy ( aux, a->name + nlimit );
      if ( strlen ( aux ) > 64 )
        aux[64] = 0;
      c += sprintf ( c, "\n                 %-64s", aux );
      aux[0] = 0;
      if ( strlen ( a->ctable ) > climit && climit < BUFR_EXPLAINED_LENGTH )
        strcpy ( aux, a->ctable + climit );
      if ( strlen ( aux ) > 64 )
        aux[64] = 0;
      c += sprintf ( c, "                                       %s", aux );
      nlimit += 64;
      climit += 64;
    }

  if (a->is_bitmaped_by != 0)
      c += sprintf(c, " *IS BITMAPED BY #%u*", a->is_bitmaped_by);

  if (a->bitmap_to != 0)
      c += sprintf(c, " *BITMAP TO #%u*", a->bitmap_to);

  return target;
}

/*!
  \fn void bufrdeco_fprint_subset_sequence_data ( struct bufrdeco_subset_sequence_data *s )
  \brief Prints a struct \ref bufrdeco_subset_sequence_data
  \param s pointer to the struct to print
*/
void bufrdeco_fprint_subset_sequence_data ( FILE *f, struct bufrdeco_subset_sequence_data *s )
{
  size_t i;
  char aux[1024];
  for ( i = 0; i < s->nd ; i++ )
    {
      if ( i && s->sequence[i].seq != s->sequence[i - 1].seq )
        fprintf ( f, "\n" );

      fprintf ( f, "%5lu:  %s\n", i, bufrdeco_print_atom_data ( aux, &s->sequence[i] ) );
    }
}



/*!
  \fn void bufrdeco_print_subset_sequence_data ( struct bufrdeco_subset_sequence_data *s )
  \brief Prints a struct \ref bufrdeco_subset_sequence_data
  \param s pointer to the struct to print
*/
void bufrdeco_print_subset_sequence_data ( struct bufrdeco_subset_sequence_data *s )
{
  bufrdeco_fprint_subset_sequence_data ( stdout, s );
}

/*!
  \fn void fprint_bufrdeco_compressed_ref ( FILE *f, struct bufrdeco_compressed_ref *r )
  \brief prints a struct bufrdeco_compressed_ref
  \param r pointer to the struct to print
*/
void fprint_bufrdeco_compressed_ref ( FILE *f, struct bufrdeco_compressed_ref *r )
{
  fprintf ( f, "%s -> A=%u, D=%u, ",r->desc.c,r->is_associated,r->has_data );
  if ( r->cref0[0] == '\0' )
    {
      fprintf ( f, "bits=%2u, ref=%10d, escale=%3d,",  r->bits, r->ref, r->escale );
      fprintf ( f, " bit0=%10lu, ref0=%10u, inc_bits=%2u ", r->bit0, r->ref0, r->inc_bits );
      fprintf ( f, "%s %s\n",r->name, r->unit );
    }
  else
    {
      fprintf ( f, "'%s', chars=%3u\n", r->cref0, r->inc_bits );
    }
}

/*!
  \fn void print_bufrdeco_compressed_ref ( struct bufrdeco_compressed_ref *r )
  \brief prints a struct bufrdeco_compressed_ref
  \param r pointer to the struct to print
*/
void print_bufrdeco_compressed_ref ( struct bufrdeco_compressed_ref *r )
{
  fprint_bufrdeco_compressed_ref ( stdout, r );
}

/*!
  \fn void print_bufrdeco_compressed_data_references ( struct bufrdeco_compressed_data_references *r )
  \brief prints a struct bufrdeco_compressed_references
  \param r pointer to the struct to print

  It is used mainly in debug stage
*/
void print_bufrdeco_compressed_data_references ( struct bufrdeco_compressed_data_references *r )
{
  size_t  i;
  for ( i = 0; i < r->nd; i++ )
    print_bufrdeco_compressed_ref ( & r->refs[i] );
}

/*!
  \fn void fprint_bufrdeco_compressed_data_references ( struct bufrdeco_compressed_data_references *r )
  \brief prints a struct bufrdeco_compressed_references
  \param r pointer to the struct to print

  It is used mainly in debug stage
*/
void fprint_bufrdeco_compressed_data_references ( FILE *f, struct bufrdeco_compressed_data_references *r )
{
  size_t  i;
  for ( i = 0; i < r->nd; i++ )
    fprint_bufrdeco_compressed_ref ( f, & r->refs[i] );
}
