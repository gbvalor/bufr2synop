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
 \file bufrdeco_data_tree.c
 \brief This file has the code which functions to print the subset data in a expanded tree
*/
#include "bufrdeco.h"

/*!
 * \fn buf_t bufrdeco_print_subset_data_prologue ( char *o, buf_t sz, struct bufrdeco *b )
 * \brief Prints the prologue of object with a subset data in json format
 * \param o string where to print
 * \param sz size available to print (bytes)
 * \param b pointer to active struct \ref bufrdeco
 * 
 * \return The used bytes 
 */
buf_t bufrdeco_print_subset_data_prologue ( struct bufrdeco *b )
{
  buf_t used = 0;

  bufrdeco_assert ( b != NULL );

  // print prologue
  used += printf ( "{\"BUFR File\":\"%s\",\"Subset\":%u,\"Decoded data\":",b->header.filename, b->seq.ss );

  return used;
}

/*!
 * \fn buf_t bufrdeco_print_subset_data_epilogue ( void )
 * \brief Prints the prologue of object with a subset data in json format
 * 
 * \return The used bytes 
 */
buf_t bufrdeco_print_subset_data_epilogue ( void )
{
  buf_t used = 0;

  // print prologue
  used += printf ( "}\n");
  return used;
}

buf_t bufrdeco_print_json_sequence_descriptor_header (struct bufr_sequence *seq)
{
  buf_t used = 0;
  used += printf ("{\"Descriptor\":%c %c%c %c%c%c,", seq->key[0], seq->key[1], seq->key[2], seq->key[3], seq->key[4], seq->key[5]);
  used += printf ("\"Description\":\"%s\",\"Expanded\":[", seq->name); 
  return used;
}

buf_t bufrdeco_print_json_sequence_descriptor_final (void)
{
  buf_t used = 0;
  // print final
  used += printf ( "]}");
  return used;
}


/*!
 *  \fn buf_t bufrdeco_print_json_object_atom_data ( struct bufr_atom_data *a, char *out, buf_t lmax )
 *  \brief Print an json object with a descriptor data
 *  \param a pointer to target struct \ref bufr_atom_data
 *  \param mode if 1 then do not print descriptors with MISSING data
 *
 * There are four cases of objects, depending of data type
 * { "descriptor":"f xx yyy", "name":"name_of_descriptor" , "unit":"name_of_unit", "value":"string_value"}
 * { "descriptor":"f xx yyy", "name":"name_of_descriptor" , "unit":"Code table", "value":"numeric_value", "meaning":"explanation_string}
 * { "descriptor":"f xx yyy", "name":"name_of_descriptor" , "unit":"Flag value", "value":"numeric_value", "meaning":"explanation_string}
 * { "descriptor":"f xx yyy", "name":"name_of_descriptor" , "unit":"name_of_unit", "value":"numeric_value"}
 *
 *   Return the bytes used
 */
buf_t bufrdeco_print_json_object_atom_data ( struct bufr_atom_data *a, ibuf_t mode )
{
  char aux[256];
  buf_t used = 0;

  // Case of MISSING date and do not want MISSING (mode != 0)
  if ( a->mask & DESCRIPTOR_VALUE_MISSING && mode )
    return 0;

  used += printf ( "{\"descriptor\":\"%u %02u %03u\",", a->desc.f, a->desc.x, a->desc.y );
  if ( a->mask & DESCRIPTOR_HAVE_STRING_VALUE )
    {
      // string data case
      if ( a->mask & DESCRIPTOR_VALUE_MISSING )
        used += printf ( "\"unit\":\"CCITT5\",\"value\":\"MISSING\"}" ) ;
      else
        {
          used += printf ( "\"unit\":\"CCITT5\",\"value\":\"%s\"}",  a->cval ) ;
        }
    }
  else if ( a->mask & DESCRIPTOR_IS_CODE_TABLE )
    {
      // code table case
      if ( a->mask & DESCRIPTOR_VALUE_MISSING )
        used += printf ( "\"unit\":\"Code table\",\"value\":\"MISSING\"}" ) ;
      else
        {
          used += printf ( "\"unit\":\"Code table\",\"value\":%u,", ( uint32_t ) ( a->val + 0.5 ) ) ;
          used += printf ( "\"meaning\":\"%s\"}", a->ctable );
        }
    }
  else if ( a->mask & DESCRIPTOR_IS_FLAG_TABLE )
    {
      // flag table case
      if ( a->mask & DESCRIPTOR_VALUE_MISSING )
        used += printf ( "\"unit\":\"Flag table\",\"value\":\"MISSING\"}" ) ;
      else
        {
          used += printf ( "\"unit\":\"Flag table\",\"value\":\"0x%08X\"}", ( uint32_t ) ( a->val ) ) ;
          used += printf ( "\"meaning\":\"%s\"}", a->ctable );
        }
    }
  else
    {
      // numeric data case
      if ( a->mask & DESCRIPTOR_VALUE_MISSING )
        used += printf ( "\"unit\":\"%s\",\"value\":\"MISSING\"}", a->unit ) ;
      else
        {
          used += printf ( "\"unit\":\"%s\",\"value\":%s}", a->unit,
                             get_formatted_value_from_escale ( aux, sizeof ( aux ), a->escale, a->val ) );

        }
    }
  return used;
}

/*!
 *  \fn buf_t bufrdeco_print_json_object_operator_descriptor ( struct bufr_descriptor *d )
 *  \brief print an operator desciptor as a json object
 *  \param d pointer to operator descriptor
 *
 *  Return the bytes used
 */
buf_t bufrdeco_print_json_object_operator_descriptor ( struct bufr_descriptor *d )
{
  buf_t used = 0;
  char explanation[256];

  if ( d->f == 2 )
    {
      // only prints if f == 2
      used += printf ( "{\"descriptor\":\"%u %02u %03u\",", d->f, d->x, d->y );
      used += printf ( "\"Operator\": \"%s\"}", bufrdeco_get_f2_descriptor_explanation ( explanation, sizeof ( explanation ), d ) );
    }
  return used;
}

/*!
 *  \fn buf_t bufrdeco_print_json_object_replicator_descriptor ( struct bufr_descriptor *d, char *out, buf_t lmax )
 *  \brief print an operator desciptor as a json object
 *  \param d pointer to operator descriptor
 *  \param out string where to print
 *  \param lmax size available to print (bytes)
 *
 *  Return the bytes used
 */
buf_t bufrdeco_print_json_object_replicator_descriptor ( struct bufr_descriptor *d )
{
  buf_t used = 0;

  if ( d->f == 1 )
    {
      used += printf ( "{\"descriptor\":\"%u %02u %03u\",", d->f, d->x, d->y );
      if ( d->y == 0 )
        {
          if ( d->x == 1 )
            used += printf ( "\"Replicator\":\"Replicator for %d descriptor after next delayed descriptor which set the number of replications.\"}", d->x );
          else
            used += printf ( "\"Replicator\":\"Replicator for %d descriptors after next delayed descriptor which set the number of replications.\"}", d->x );
        }
      else
        {
          if ( d->x == 1 )
            used += printf ( "\"Replicator\":\"Replicator for next descriptor %d times.\"}", d->y );
          else
            used += printf ( "\"Replicator\":\"Replicator for next %d descriptors %d times\n}", d->x, d->y );
        }

    }
  return used;
}

/*! \fn buf_t bufrdeco_print_sequence_data_tree_recursive (char *out, buf_t lmax, struct bufrdeco_subset_sequence_data *s, struct bufr_sequence *l, struct bufrdeco *b, ibuf_t mode )
 *  \brief Print the data of a \ref bufr_sequence in json format
 *
 *
 *  /if mode == 1 then also print missing data
 */
buf_t bufrdeco_print_sequence_data_tree_recursive ( char *out, buf_t lmax,  struct bufrdeco_subset_sequence_data *d,
    struct bufr_sequence *lx, struct bufrdeco *b, ibuf_t mode )
{
  ibuf_t has_data = 0, ix;
  struct bufr_sequence *l;
  char explanation[256];

  buf_t used = 0, i, start = 0;

  if ( lx == NULL )
    {
      // Case of first sequence of level 0 (the begining)
      l = & ( b->tree->seq[0] );
    }
  else
    {
      l = lx;
    }

  // Begin the json pair "name_of_sequence":{object with descriptors}
  used += snprintf ( out + used, lmax - used, ",\"%s\":", l->name );

  // Loop in descriptors of a bufr_sequence
  for ( i = 0; i < l->ndesc ; i++ )
    {
      if ( l->lseq[i].f != 3 )
        {
          if ( l->lseq[i].f == 0 )
            {

              // Find index of the struct bufr_atom_data in *d with data in the given sequence *l
              if ( ( ix = bufrdeco_find_bufr_atom_data ( l, i, d, start ) ) < 0 )
                {
                  snprintf ( b->error, sizeof ( b->error ), "%s(): Cannot find data in the sequence\n", __func__ );
                  return 1;
                }

              // Here prints the data of a descriptor as an object, depending of mask
              used += bufrdeco_print_json_object_atom_data ( & ( d->sequence[ix] ), mode );
            }
          else if ( l->lseq[i].f == 2 )
            {
              used += snprintf ( out + used, lmax - used, "{\"descriptor\":\"%u %02u %03u\",", l->lseq[i].f, l->lseq[i].x, l->lseq[i].y );
              used += snprintf ( out + used, lmax - used, "\"Operator\": \"%s\"}", bufrdeco_get_f2_descriptor_explanation ( explanation, sizeof ( explanation ), & ( l->lseq[i] ) ) );
            }
          else if ( l->lseq[i].f == 1 )
            {
              used += snprintf ( out + used, lmax - used, "{\"descriptor\":\"%u %02u %03u\",", l->lseq[i].f, l->lseq[i].x, l->lseq[i].y );
              if ( l->lseq[i].y == 0 )
                {
                  if ( l->lseq[i].x == 1 )
                    used += snprintf ( out + used, lmax - used, "\"Replicator\":\"Replicator for %d descriptor after next delayed descriptor which set the number of replications.\"}", l->lseq[i].x );
                  else
                    used += snprintf ( out + used, lmax - used, "\"Replicator\":\"Replicator for %d descriptors after next delayed descriptor which set the number of replications.\"}", l->lseq[i].x );
                }
              else
                {
                  if ( l->lseq[i].x == 1 )
                    used += snprintf ( out + used, lmax - used, "\"Replicator\":\"Replicator for next descriptor %d times.\"}", l->lseq[i].y );
                  else
                    used += snprintf ( out + used, lmax - used, "\"Replicator\":\"Replicator for next %d descriptors %d times\n}", l->lseq[i].x, l->lseq[i].y );
                }

            }
        }
    }

  // End the json object
  used += snprintf ( out + used, lmax - used, "}" );

  if ( mode || has_data )
    return used;
  else
    return 0;
}


/*!
 * \fn ibuf_t bufrdeco_find_bufr_atom_data ( struct bufr_sequence *s, buf_t is, struct bufrdeco_subset_sequence_data *d, buf_t start)
 * \brief Search first struct \ref bufr_atom_data in a struct \ref bufrdeco_subset_sequence_data which matches a descritor in a bufr_sequence
 * \param s pointer to the struct \ref bufr_sequence to match
 * \param is index of descriptor in \a s to match
 * \param d poniter to struct \ref bufrdeco_subset_sequence_data where to search
 * \param start index for element in \a d.sequence where to start the search
 *
 *  Return the index for struct \ref bufr_atom_data in the \a d.sequence if found. Returns -1 if not found
 */
ibuf_t bufrdeco_find_bufr_atom_data ( struct bufr_sequence *s, buf_t is, struct bufrdeco_subset_sequence_data *d, buf_t start )
{
  buf_t i = start;

  while ( i < d->nd )
    {
      if ( d->sequence[i].seq == s && d->sequence[i].ns == is )
        return ( ibuf_t ) i;
      i++;
    }
  return -1;
}
