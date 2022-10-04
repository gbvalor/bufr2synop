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
 \file bufrdeco_json.c
 \brief This file has the code which functions to print BUFR data in json format
*/
#include "bufrdeco.h"

/*!
 * \fn buf_t bufrdeco_print_json_scape_string_cvals ( FILE *out, char *source )
 * \brief prints a descriptor string value scaping the '"' for a json format
 * \param out output stream
 * \param source source string
 * \return The amount of bytes sent to out
 */
buf_t bufrdeco_print_json_scape_string_cvals ( FILE *out, char *source )
{
  buf_t i = 0, used = 0;
  while ( source[i] )
    {
      switch ( source[i] )
        {
        case '"':
          used += fprintf ( out, "%c%c", '\\', source[i++] );
          break;
        default:
          used += fprintf ( out, "%c", source[i++] );
          break;
        }
    }
  return used;
}

/*!
 * \fn buf_t bufrdeco_print_subset_data_prologue (FILE *out,  struct bufrdeco *b )
 * \brief Prints the prologue of object with a subset data in json format
 * \param out output stream opened by caller
 * \param b pointer to active struct \ref bufrdeco
 * \return The amount of bytes sent to out
 */
buf_t bufrdeco_print_json_subset_data_prologue ( FILE *out,  struct bufrdeco *b )
{
  buf_t used = 0;

  bufrdeco_assert ( b != NULL );

  // print prologue
  used += fprintf ( out, "{\"BUFR File\":\"%s\",\"Subset\":%u,\"Decoded data\":",b->header.filename, b->seq.ss );

  return used;
}

/*!
 * \fn buf_t bufrdeco_print_subset_data_epilogue ( FILE *out )
 * \brief Prints the prologue of object with a subset data in json format
 * \param out output stream opened by caller
 *
 * \return The amount of bytes sent to out
 */
buf_t bufrdeco_print_json_subset_data_epilogue ( FILE *out )
{
  buf_t used = 0;

  // print prologue
  used += fprintf ( out, "}\n" );
  return used;
}

/*!
 *  \fn buf_t bufrdeco_print_json_sequence_descriptor_header (FILE *out,  struct bufr_sequence *seq )
 *  \brief Print the header of a sequence descriptor (f == 3)
 *  \param out output stream opened by caller
 *  \param seq pointer to a bufr_sequence \ref bufr_sequence
 *
 * \return The amount of bytes sent to out
 */
buf_t bufrdeco_print_json_sequence_descriptor_header ( FILE *out,  struct bufr_sequence *seq )
{
  buf_t used = 0;

  used += fprintf ( out, "{\"Descriptor\":\"%c %c%c %c%c%c\",", seq->key[0], seq->key[1], seq->key[2], seq->key[3], seq->key[4], seq->key[5] );
  used += fprintf ( out, "\"Sequence description\":\"%s\",\"Expanded sequence\":[", seq->name );
  return used;
}

/*!
 *  \fn buf_t bufrdeco_print_json_sequence_descriptor_header (FILE *out )
 *  \brief Print the final of a sequence descriptor (f == 3)
 *  \param out output stream opened by caller
 *
 * \return The amount of bytes sent to out
 */
buf_t bufrdeco_print_json_sequence_descriptor_final ( FILE *out )
{
  buf_t used = 0;
  // print final
  used += fprintf ( out,  "]}" );
  return used;
}


/*!
 *  \fn buf_t bufrdeco_print_json_object_atom_data (FILE *out, struct bufr_atom_data *a, char *add )
 *  \brief Print an json object with a descriptor data
 *  \param out output stream opened by caller
 *  \param a pointer to target struct \ref bufr_atom_data
 *  \param add adtional optional info
 *
 * \return The amount of bytes sent to out
 *
 * There are four cases of objects, depending of data type
 * { "descriptor":"f xx yyy", "name":"name_of_descriptor" , "unit":"name_of_unit", "value":"string_value"}
 * { "descriptor":"f xx yyy", "name":"name_of_descriptor" , "unit":"Code table", "value":"numeric_value", "meaning":"explanation_string}
 * { "descriptor":"f xx yyy", "name":"name_of_descriptor" , "unit":"Flag value", "value":"numeric_value", "meaning":"explanation_string}
 * { "descriptor":"f xx yyy", "name":"name_of_descriptor" , "unit":"name_of_unit", "value":"numeric_value"}
 */
buf_t bufrdeco_print_json_object_atom_data ( FILE *out,  struct bufr_atom_data *a, char *add )
{
  char aux[256];
  buf_t used = 0;

  used += fprintf ( out,  "{\"Descriptor\":\"%u %02u %03u\",\"Name\":\"%s\",", a->desc.f, a->desc.x, a->desc.y, bufr_adjust_string ( a->name ) );

  // add aditional info keys
  if ( add != NULL && add[0] )
    used += fprintf ( out, "%s,", add );

  if ( a->mask & DESCRIPTOR_HAVE_STRING_VALUE )
    {
      // string data case
      if ( a->mask & DESCRIPTOR_VALUE_MISSING )
        used += fprintf ( out, "\"Unit\":\"CCITT5\",\"Value\":\"MISSING\"}" ) ;
      else
        {
          used += fprintf ( out, "\"Unit\":\"CCITT5\",\"Value\":\"" ) ;
          used += bufrdeco_print_json_scape_string_cvals ( out, a->cval );
          used += fprintf ( out, "\"}" );
        }
    }
  else if ( a->mask & DESCRIPTOR_IS_CODE_TABLE )
    {
      // code table case
      if ( a->mask & DESCRIPTOR_VALUE_MISSING )
        used += fprintf ( out, "\"Unit\":\"Code table\",\"Value\":\"MISSING\"}" ) ;
      else
        {
          used += fprintf ( out, "\"Unit\":\"Code table\",\"Value\":%u,", ( uint32_t ) ( a->val + 0.5 ) ) ;
          used += fprintf ( out, "\"Meaning\":\"%s\"}", bufr_adjust_string ( a->ctable ) );
        }
    }
  else if ( a->mask & DESCRIPTOR_IS_FLAG_TABLE )
    {
      // flag table case
      if ( a->mask & DESCRIPTOR_VALUE_MISSING )
        used += fprintf ( out, "\"Unit\":\"Flag table\",\"Value\":\"MISSING\"}" ) ;
      else
        {
          used += fprintf ( out, "\"Unit\":\"Flag table\",\"Value\":\"0x%08X\",", ( uint32_t ) ( a->val ) ) ;
          used += fprintf ( out, "\"Meaning\":\"%s\"}", bufr_adjust_string ( a->ctable ) );
        }
    }
  else
    {
      // numeric data case
      if ( a->mask & DESCRIPTOR_VALUE_MISSING )
        used += fprintf ( out, "\"Unit\":\"%s\",\"Value\":\"MISSING\"}", a->unit ) ;
      else
        {
          used += fprintf ( out, "\"Unit\":\"%s\",\"Value\":%s}", a->unit,
                            get_formatted_value_from_escale2 ( aux, sizeof ( aux ), a->escale, a->val ) );

        }
    }
  return used;
}

/*!
 *  \fn buf_t bufrdeco_print_json_object_operator_descriptor (FILE *out, struct bufr_descriptor *d, char *add )
 *  \brief print an operator desciptor as a json object
 *  \param out output stream opened by caller
 *  \param d pointer to operator descriptor
 *  \param add adtional optional info
 *
 * \return The amount of bytes sent to out
 */
buf_t bufrdeco_print_json_object_operator_descriptor ( FILE *out,  struct bufr_descriptor *d, char *add )
{
  buf_t used = 0;
  char explanation[256];

  if ( d->f == 2 )
    {
      // only prints if f == 2
      used += fprintf ( out, "{\"Descriptor\":\"%u %02u %03u\",", d->f, d->x, d->y );
      // add aditional info keys
      if ( add != NULL && add[0] )
        used += fprintf ( out, "%s,", add );

      used += fprintf ( out, "\"Operator\": \"%s\"}", bufrdeco_get_f2_descriptor_explanation ( explanation, sizeof ( explanation ), d ) );
    }
  return used;
}

/*!
 *  \fn buf_t bufrdeco_print_json_object_replicator_descriptor (FILE *out, struct bufr_descriptor *d, char *add )
 *  \brief print an operator desciptor as a json object
 *  \param out string where to print
 *  \param d pointer to operator descriptor
 *  \param add additional info
 *
 * \return The amount of bytes sent to out
 */
buf_t bufrdeco_print_json_object_replicator_descriptor ( FILE *out,  struct bufr_descriptor *d, char *add )
{
  buf_t used = 0;

  if ( d->f == 1 )
    {
      used += fprintf ( out, "{\"Descriptor\":\"%u %02u %03u\",", d->f, d->x, d->y );
      // add aditional info keys
      if ( add != NULL && add[0] )
        used += fprintf ( out, "%s,", add );

      if ( d->y == 0 )
        {
          if ( d->x == 1 )
            used += fprintf ( out, "\"Replicator\":\"Replicator for %d descriptor after next delayed descriptor which set the number of replications.\"}", d->x );
          else
            used += fprintf ( out, "\"Replicator\":\"Replicator for %d descriptors after next delayed descriptor which set the number of replications.\"}", d->x );
        }
      else
        {
          if ( d->x == 1 )
            used += fprintf ( out, "\"Replicator\":\"Replicator for next descriptor %d times.\"}", d->y );
          else
            used += fprintf ( out, "\"Replicator\":\"Replicator for next %d descriptors %d times\"}", d->x, d->y );
        }

    }
  return used;
}

/*!
 *  \fn buf_t bufrdeco_print_json_separator( FILE *out)
 *  \brief Print the comma ',' separator in an output
 *  \param out string where to print
 *
 *  \return The amount of bytes sent to out
 */
buf_t bufrdeco_print_json_separator ( FILE *out )
{
  return fprintf ( out, "," );
}

/*!
  \fn int sprint_sec0_info( FILE *out, struct bufrdeco *b )
* \brief Print info form sec 0 in json format
 * \param out string where to print
 * \param b actuve struct \ref bufrdeco
 *
 * \return The amount of bytes sent to out
*/
buf_t bufrdeco_print_json_sec0 ( FILE *out, struct bufrdeco *b )
{
  size_t used = 0;

  bufrdeco_assert ( b != NULL );
  used += fprintf ( out, "{\"Sec 0\":{" );
  used += fprintf ( out, "\"Bufr length\":%u,", b->sec0.bufr_length );
  used += fprintf ( out, "\"Bufr edition\":%u}}", b->sec0.edition );
  return used;
}


/*!
 * \fn bufrdeco_print_json_sec1 (FILE *out, struct bufrdeco *b)
 * \brief Print info form sec 1 in json format
 * \param out string where to print
 * \param b actuve struct \ref bufrdeco
 *
 * \return The amount of bytes sent to out
*/
buf_t bufrdeco_print_json_sec1 ( FILE *out, struct bufrdeco *b )
{
  size_t used = 0;

  bufrdeco_assert ( b != NULL );
  used += fprintf ( out, "{\"Sec 1\":{" );
  used += fprintf ( out, "\"Length\":%u", b->sec1.length );
  used += fprintf ( out, ",\"Bufr master table\":%u", b->sec1.master );
  used += fprintf ( out, ",\"Centre\":%u", b->sec1.centre );
  used += fprintf ( out, ",\"Sub-Centre\":%u", b->sec1.subcentre );
  used += fprintf ( out, ",\"Update sequence\":%u", b->sec1.update );
  used += fprintf ( out, ",\"Options\":\"0x%x\"", b->sec1.options );
  used += fprintf ( out, ",\"Category\":%u", b->sec1.category );
  used += fprintf ( out, ",\"Subcategory\":%u", b->sec1.subcategory );
  used += fprintf ( out, ",\"Sub-category local\":%u", b->sec1.subcategory_local );
  used += fprintf ( out, ",\"Master table version\":%u", b->sec1.master_version );
  used += fprintf ( out, ",\"Master table local\":%u", b->sec1.master_local );
  used += fprintf ( out, ",\"Year\":%u", b->sec1.year );
  used += fprintf ( out, ",\"Month\":%u", b->sec1.month );
  used += fprintf ( out, ",\"Day\":%u", b->sec1.day );
  used += fprintf ( out, ",\"Hour\":%u", b->sec1.hour );
  used += fprintf ( out, ",\"Minute\":%u", b->sec1.minute );
  used += fprintf ( out, ",\"Second\":%u", b->sec1.second );
  if ( b->sec0.edition == 3 )
    used += fprintf ( out, ",\"Aditional space\":%u", b->sec1.length - 17 );
  else
    used += fprintf ( out, ",\"Aditional space\":%u", b->sec1.length - 22 );
  used += fprintf ( out, "}" );

  if ( b->tables->b.path[0] )
    {
      used += fprintf ( out, ",{\"Used tables\":{" );
      used += fprintf ( out, ",\"TableB file\":\"%s\"", b->tables->b.path );
      used += fprintf ( out, ",\"tableC file\":\"%s\"", b->tables->c.path );
      used += fprintf ( out, ",\"tableD file\":\"%s\"", b->tables->d.path );
      used += fprintf ( out, "}" );
    }
  used += fprintf ( out, "}" );

  return used;
}

/*!
  \fn buf_t bufrdeco_print_json_sec2( FILE *out, struct bufrdeco *b )
* \brief Print info form optional sec 2 in json format
 * \param out string where to print
 * \param b actuve struct \ref bufrdeco
 *
 * \return The amount of bytes sent to out
*/
buf_t bufrdeco_print_json_sec2 ( FILE *out, struct bufrdeco *b )
{
  size_t used = 0;

  bufrdeco_assert ( b != NULL );
  used += fprintf ( out, "{\"Sec 2\":{" );
  if ( b->sec1.options & 0x80 )
    used += fprintf ( out, "\"Length\":%u}}", b->sec2.length );
  else
    used += fprintf ( out, "\"Length\":0}}" );

  return used;
}

/*!
 * \fn bufrdeco_print_json_sec3 (FILE *out, struct bufrdeco *b)
 * \brief Print info form sec 3 in json format
 * \param out string where to print
 * \param b actuve struct \ref bufrdeco
 *
 * \return The amount of bytes sent to out
*/
buf_t bufrdeco_print_json_sec3 ( FILE *out, struct bufrdeco *b )
{
  buf_t used = 0, i;

  bufrdeco_assert ( b != NULL );
  used += fprintf ( out, "{\"Sec 3\":{" );
  used += fprintf ( out, "\"Sec3 length\":%u", b->sec3.length );
  used += fprintf ( out, ",\"Subsets\":%u", b->sec3.subsets );
  used += fprintf ( out, ",\"Observed\":%u", b->sec3.observed );
  used += fprintf ( out, ",\"Compressed\":%u", b->sec3.compressed );
  used += fprintf ( out, ",\"Unexpanded descriptors\":%u", b->sec3.ndesc );
  used += fprintf ( out, ",\"Unexpanded array\":" );

  used += fprintf ( out,"[" );
  for ( i = 0; i < b->sec3.ndesc; i++ )
    {
      if ( i )
        used += fprintf ( out,"," );
      used += fprintf ( out, "{\"%u\":\"%u %02u %03u\"}", i, b->sec3.unexpanded[i].f, b->sec3.unexpanded[i].x, b->sec3.unexpanded[i].y );
    }
  used += fprintf ( out,"]}}" );

  return used;
}

/*!
  \fn int bufrdeco_print_json_tree_recursive ( FILE *out, struct bufrdeco *b, struct bufr_sequence *seq )
  \brief  Print a tree of descriptors to a file in a recursive way in json format
  \param out stream opened by caller
  \param b pointer to the basic container struct \ref bufrdeco
  \param seq pointer to the struct \ref bufr_sequence  to print
  \return The amount of bytes sent to out
*/
buf_t bufrdeco_print_json_tree_recursive ( FILE *out, struct bufrdeco *b, struct bufr_sequence *seq )
{
  buf_t i, k, used = 0;
  struct bufr_sequence *l;
  char explanation[256];

  bufrdeco_assert ( out != NULL && b != NULL );

  if ( seq == NULL )
    {
      l = & ( b->tree->seq[0] );
    }
  else
    {
      l = seq;
    }

  // begins the array
  for ( i = 0; i < l->ndesc; i++ )
    {
      if ( i )
        used += fprintf ( out, "," );
      fprintf ( out,  "{\"%u %02u %03u\"", l->lseq[i].f, l->lseq[i].x,l->lseq[i].y );

      if ( l->lseq[i].f != 3 )
        {
          if ( l->lseq[i].f == 0 )
            {
              if ( bufr_find_tableB_index ( &k, & ( b->tables->b ), l->lseq[i].c ) )
                used += fprintf ( out, "\"Not found in tables\"}" );
              else
                {
                  used += fprintf ( out, ":\"" );
                  if ( l->replicated[i] )
                    {
                      if ( l->replicated[i] )
                        {
                          if ( l->replicated[i] == 1 )
                            used += fprintf ( out, "Replicated | " );
                          else
                            used += fprintf ( out, "Replicated depth %u | ", l->replicated[i] );
                        }
                    }
                  if ( is_a_delayed_descriptor ( & l->lseq[i] ) ||
                       is_a_short_delayed_descriptor ( & l->lseq[i] ) )
                    used += fprintf ( out, "* %s\"}", b->tables->b.item[k].name );
                  else
                    used += fprintf ( out, "%s\"}", b->tables->b.item[k].name );
                }
            }
          else if ( l->lseq[i].f == 2 )
            {
              used += fprintf ( out, ":\"%s\"}", bufrdeco_get_f2_descriptor_explanation ( explanation, sizeof ( explanation ), & ( l->lseq[i] ) ) );
            }
          else if ( l->lseq[i].f == 1 )
            {
              if ( l->lseq[i].y == 0 )
                {
                  if ( l->lseq[i].x == 1 )
                    used += fprintf ( out, ":\"* Replicator for %d descriptor after next delayed descriptor which set the number of replications.\"}", l->lseq[i].x );
                  else
                    used += fprintf ( out, ":\"* Replicator for %d descriptors after next delayed descriptor which set the number of replications.\"}", l->lseq[i].x );
                }
              else
                {
                  if ( l->lseq[i].x == 1 )
                    used += fprintf ( out, ":\"* Replicator for next descriptor %d times\"}", l->lseq[i].y );
                  else
                    used += fprintf ( out, ":\"* Replicator for next %d descriptors %d times\"}", l->lseq[i].x, l->lseq[i].y );
                }
            }
          else
            used += fprintf ( out, "\n" );
          continue;
        }
      else
        {
          // f == 3
          used += fprintf ( out, ":\"" );
          if ( l->replicated[i] )
            {
              if ( l->replicated[i] == 1 )
                used += fprintf ( out, "Replicated | " );
              else
                used += fprintf ( out, "Replicated depth %u | ", l->replicated[i] );
            }
          used += fprintf ( out, "%s\"", bufr_adjust_string ( l->name ) );
          used += fprintf ( out, ",\"Expanded\":[" );
          // parse de son
          used += bufrdeco_print_json_tree_recursive ( out, b, l->sons[i] );
          used += fprintf ( out, "]}" );
          // we then recursively parse the son
        }
    }
  return used;
}

/*!
  \fn int bufrdeco_print_tree ( struct bufrdeco *b )
  \brief Print a tree of descriptors
  \param b pointer to a basic container struct \ref bufrdeco
  \return The amount of bytes sent to out
*/
buf_t bufrdeco_print_json_tree ( struct bufrdeco *b )
{
  buf_t used = 0;
  bufrdeco_assert ( b != NULL );

  if ( b->mask & BUFRDECO_OUTPUT_JSON_EXPANDED_TREE )
    used += bufrdeco_print_json_tree_recursive ( b->out, b, NULL );
  return used;
};
