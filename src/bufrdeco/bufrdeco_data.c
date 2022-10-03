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
 \file bufrdeco_data.c
 \brief This file has the code which extract data from a bufr
*/
#include "bufrdeco.h"


/*!
  \fn struct bufrdeco_subset_sequence_data * bufrdeco_get_subset_sequence_data(struct bufrdeco *b)
  \brief Parse and get a struct \ref bufrdeco_subset_sequence_data
  \param b basic container struct \ref bufrdeco
  \return If succeeded returns a pointer to the struct \ref bufrdeco_subset_sequence_data with the results for a
  subset,  otherwise returns NULL

  Note that if succeeded the counter to current subset index is increased. Remember that in case of non
  compressed data we must to decode all previous subsets to get the desired one. In case of compressed
  we can access directly to a the desired subset

*/
struct bufrdeco_subset_sequence_data * bufrdeco_get_subset_sequence_data ( struct bufrdeco *b )
{
  bufrdeco_assert ( b != NULL );

  if ( bufrdeco_decode_data_subset ( b ) )
    {
      return NULL;
    }
  return & ( b->seq );
}


/*!
  \fn  int bufrdeco_decode_data_subset ( struct bufrdeco *b )
  \brief  User interface to decode a BUFR subset
  \param b pointer to the base struct \ref bufrdeco
  \return Return 0 in case of success, 1 otherwise

  Note that if succeeded the counter to current subset index is increased. Remember that in case of non
  compressed data we must to decode all previous subsets to get the desired one. In case of compressed
  we can access directly to a the desired subset

*/
int bufrdeco_decode_data_subset ( struct bufrdeco *b )
{
  struct bufrdeco_subset_sequence_data *s;
  struct bufrdeco_compressed_data_references *r;

  // check arguments
  bufrdeco_assert ( b != NULL );

  s = & ( b->seq );
  r = & ( b->refs );

  if ( s == NULL || r == NULL )
    {
      snprintf ( b->error, sizeof ( b->error ), "%s(): Unspected NULL argument(s)\n", __func__ );
      return 1;
    }

  // Check about parsed tree
  if ( b->tree == NULL || b->tree->nseq == 0 )
    {
      snprintf ( b->error, sizeof ( b->error ), "%s(): Try to parse compressed data without parsed tree\n", __func__ );
      return 1;
    }

  // First we init the subset sequence data
  if ( bufrdeco_clean_subset_sequence_data ( s ) )
    {
      return 1;
    }

  // also we clean the possible defined bitmaps in prior subsets
  if ( bufrdeco_clean_bitmaps ( b ) )
    {
      return 1;
    }

  // Then we get the data from an already parsed descriptor tree
  if ( b->sec3.compressed )
    {
      if ( r->nd == 0 )
        {
          // case of compressed data and still not parsed
          if ( bufrdeco_parse_compressed ( r, b ) )
            {
              return 1;
            }
        }
      if ( bufr_decode_subset_data_compressed ( s, r, b ) )
        {
          return 1;
        }
    }
  else
    {
      if ( bufrdeco_decode_subset_data_recursive ( s, NULL, b ) )
        {
          return 1;
        }
    }

  // Finally we update the subset counter
  ( b->state.subset ) ++;
  return 0;
}


/*!
  \fn int bufrdeco_increase_data_array ( struct bufrdeco_subset_sequence_data *s )
  \brief doubles the allocated space for a struct \ref bufrdeco_subset_sequence_data whenever is posible
  \param s pointer to source struct \ref bufrdeco_subset_sequence_data
  \return 0 when success, otherwise return 1 and the struct is unmodified

  The amount of data in a bufr must be huge. In a first moment, the dimension of a sequence of structs
  \ref bufr_atom_data is \ref BUFR_NMAXSEQ but may be increased. This function task is try to double the
  allocated dimension and reallocate it.

*/
int bufrdeco_increase_data_array ( struct bufrdeco_subset_sequence_data *d )
{
  bufrdeco_assert ( d != NULL );

  if ( d->dim < ( BUFR_NMAXSEQ * 8 ) ) // check if reached the limit
    {
      if ( ( d->sequence = ( struct bufr_atom_data * ) realloc ( ( void * ) d->sequence,
                           d->dim * 2 * sizeof ( struct bufr_atom_data ) ) ) == NULL )
        {
          return 1;
        }
      else
        {
          d->dim *= 2;
          return 0;
        }
    }
  else
    {
      return 1;
    }
}

/*!
  \fn int bufrdeco_decode_subset_data_recursive ( struct bufrdeco_subset_sequence_data *s, struct bufr_sequence *l, struct bufrdeco *b )
  \brief decode the data from a subset in a recursive way
  \param s pointer to the target struct \ref bufrdeco_subset_sequence_data
  \param l pointer to the source struct \ref bufr_sequence
  \param b pointer to the base struct \ref bufrdeco

  \return 0 in case of success, 1 otherwise
*/
int bufrdeco_decode_subset_data_recursive ( struct bufrdeco_subset_sequence_data *d, struct bufr_sequence *l, struct bufrdeco *b )
{
  size_t i, j, k;
  struct bufr_sequence *seq;
  struct bufr_replicator replicator;

  bufrdeco_assert ( b != NULL );

  if ( d == NULL )
    {
      snprintf ( b->error, sizeof ( b->error ), "%s(): Unspected NULL argument(s)\n", __func__ );
      return 1;
    }


  // clean subset data, if l == NULL we are at the begining of a subset, level 0
  if ( l == NULL )
    {

      //memset ( s, 0, sizeof ( struct bufr_subset_sequence_data ) );
      d->nd = 0;
      d->ss = b->state.subset;
      seq = & ( b->tree->seq[0] );
      if ( b->state.subset == 0 )
        {
          b->state.bit_offset = 0;
        }

      // Manage bit offset of the subset
      if ( b->state.subset < BUFR_MAX_SUBSETS )
        {
          // No bit offset still set for this subset
          if ( b->offsets.ofs[b->state.subset] == 0 )
            {
              if ( b->offsets.nr <= b->state.subset )
                b->offsets.nr = b->state.subset + 1;
              b->offsets.ofs[b->state.subset] = b->state.bit_offset;
            }
          else
            {
              // offset is already set, so we can go directly to the data
              b->state.bit_offset = b->offsets.ofs[b->state.subset];
            }
        }
      else
        {
          snprintf ( b->error, sizeof ( b->error ), "%s(): Cannot manage subset bit offset for subset %u. Consider increase BUFR_MAX_SUBSETS\n",
                     __func__, b->state.subset );
          return 1;
        }

      // also reset reference and bits inc
      b->state.added_bit_length = 0;
      b->state.added_scale = 0;
      b->state.added_reference = 0;
      b->state.assoc_bits = 0;
      b->state.changing_reference = 255;
      b->state.fixed_ccitt = 0;
      b->state.local_bit_reserved = 0;
      b->state.factor_reference = 1;
      b->state.quality_active = 0;
      b->state.subs_active = 0;
      b->state.retained_active = 0;
      b->state.stat1_active = 0;
      b->state.dstat_active = 0;
      b->state.bitmaping = 0;
      b->state.data_repetition_factor = 0;
      b->state.bitmap = NULL;

      // print prologue if needed
      if ( b->mask & BUFRDECO_OUTPUT_JSON_SUBSET_DATA )
        bufrdeco_print_json_subset_data_prologue ( b->out,  b );

    }
  else
    {
      seq = l;
    }

  // loop for a sequence
  if ( b->mask & BUFRDECO_OUTPUT_JSON_SUBSET_DATA )
    {
      //if (seq->iseq)
      //  bufrdeco_print_json_separator ( b->out ); // print json array element separator
      bufrdeco_print_json_sequence_descriptor_header ( b->out, seq );
    }

  for ( i = 0, j = 0 ; i < seq->ndesc ; i++ )
    {
      switch ( seq->lseq[i].f )
        {
        case 0:

          // Checks if no_data_present is active for this descriptor in this sequence
          if ( seq->no_data_present.active &&
               i >= seq->no_data_present.first &&
               i <= seq->no_data_present.last )
            {
              // If here then no_data_present has been active in this sequence
              if ( seq->lseq[i].x > 9 &&
                   seq->lseq[i].x != 31 )
                {
                  snprintf ( b->error, sizeof ( b->error ), "%s(): Getting data from table b with class other than 1-9,31 and no data present activated\n",
                             __func__ );
                  return 1;
                }

            }


          // Repeat the extraction of data if b->state.data_repetition_factor != 0
          // It get data al least a time
          // It is supossed that prior parsed descriptor was 0 31 011 or 0 31 012 which set de value of repetition data
          do
            {
              // decrement counter if not zero
              if ( b->state.data_repetition_factor )
                ( b->state.data_repetition_factor )--;

              // Get data from table B
              if ( bufrdeco_tableB_val ( & ( d->sequence[d->nd] ), b, & ( seq->lseq[i] ) ) )
                {
                  return 1;
                }
              if ( b->mask & BUFRDECO_OUTPUT_JSON_SUBSET_DATA )
              {
                if (j)
                  bufrdeco_print_json_separator ( b->out );
                bufrdeco_print_json_object_atom_data ( b->out, & ( d->sequence[d->nd] ), NULL );
                j++;
              }
            }
          while ( b->state.data_repetition_factor != 0 );

          // case of delayed replicator description and data
          if ( seq->lseq[i].x == 31 && ( seq->lseq[i].y == 11 || seq->lseq[i].y == 12 ) )
            {
              b->state.data_repetition_factor = ( buf_t ) d->sequence[d->nd].val;
            }

          //case of first order statistics
          if ( b->state.stat1_active &&
               seq->lseq[i].x == 8 && seq->lseq[i].y == 23 )
            {
              k = b->bitmap.bmap[b->bitmap.nba - 1]->ns1; // index un stqts
              b->bitmap.bmap[b->bitmap.nba - 1]->stat1_desc[k] = d->nd; // Set the value of statistical parameter
              // update the number of quality variables for the bitmap
              if ( k < BUFR_MAX_QUALITY_DATA )
                ( b->bitmap.bmap[b->bitmap.nba - 1]->ns1 )++;
              else
                {
                  snprintf ( b->error, sizeof ( b->error ), "%s(): No more space for first order statistic vars in bitmap. Check BUFR_MAX_QUALITY_DATA\n",
                             __func__ );
                  return 1;
                }
            }

          //case of difference statistics
          if ( b->state.dstat_active &&
               seq->lseq[i].x == 8 && seq->lseq[i].y == 24 )
            {
              k = b->bitmap.bmap[b->bitmap.nba - 1]->nds; // index in stats
              b->bitmap.bmap[b->bitmap.nba - 1]->dstat_desc[k] = d->nd; // Set the value of statistical parameter
              // update the number of quality variables for the bitmap
              if ( k < BUFR_MAX_QUALITY_DATA )
                ( b->bitmap.bmap[b->bitmap.nba - 1]->nds )++;
              else
                {
                  snprintf ( b->error, sizeof ( b->error ), "%s(): No more space for difference statistic vars in bitmap. Check BUFR_MAX_QUALITY_DATA\n",
                             __func__ );
                  return 1;
                }
            }



          // Add info about common sequence to which bufr_atom_data belongs
          d->sequence[d->nd].seq = seq;
          d->sequence[d->nd].ns = i;

          //bufr_print_atom_data_stdout(& ( d->sequence[d->nd] ));
          if ( d->nd < ( d->dim - 1 ) )
            {
              ( d->nd ) ++;
            }
          else if ( bufrdeco_increase_data_array ( d ) == 0 )
            {
              ( d->nd ) ++;
            }
          else
            {
              snprintf ( b->error, sizeof ( b->error ), "%s(): No more bufr_atom_data available. Check BUFR_NMAXSEQ\n", __func__ );
              return 1;
            }
          break;

        case 1:
          // Case of replicator descriptor
          replicator.s = seq;
          replicator.ixrep = i;
          if ( b->mask & BUFRDECO_OUTPUT_JSON_SUBSET_DATA )
            {
              if (j)
                bufrdeco_print_json_separator ( b->out );
              bufrdeco_print_json_object_replicator_descriptor ( b->out, & ( seq->lseq[i] ), NULL );
              j++;
            }
          
          if ( seq->lseq[i].y != 0 )
            {
              // no delayed
              replicator.ixdel = i;
              replicator.ndesc = seq->lseq[i].x;
              replicator.nloops = seq->lseq[i].y;

              // Check if this replicator is for a bit-map defining
              if ( b->state.bitmaping )
                {
                  b->state.bitmaping = replicator.nloops; // set it properly
                }

              bufrdeco_decode_replicated_subsequence ( d, &replicator, b );

              // and then set again bitamping to 0, because it is finished
              b->state.bitmaping = 0;

              i += replicator.ndesc;
            }
          else
            {
              // case of delayed;
              replicator.ixdel = i + 1;
              replicator.ndesc = seq->lseq[i].x;
              // here we read ndesc from delayed replicator descriptor
              if ( bufrdeco_tableB_val ( & ( d->sequence[d->nd] ), b, & ( seq->lseq[i + 1] ) ) )
                {
                  return 1;
                }
              if ( b->mask & BUFRDECO_OUTPUT_JSON_SUBSET_DATA )
                {
                  if (j)
                    bufrdeco_print_json_separator ( b->out );
                  bufrdeco_print_json_object_atom_data ( b->out, & ( d->sequence[d->nd] ), NULL );
                  j++;
                }
              // Add info about common sequence to which bufr_atom_data belongs
              d->sequence[d->nd].seq = seq;
              d->sequence[d->nd].ns = i + 1;

              replicator.nloops = ( size_t ) ( d->sequence[d->nd].val );

              // Check if this replicator is for a bit-map defining
              if ( b->state.bitmaping )
                {
                  b->state.bitmaping = replicator.nloops; // set it properly
                }

              if ( d->nd < ( d->dim - 1 ) )
                {
                  ( d->nd ) ++;
                }
              else if ( bufrdeco_increase_data_array ( d ) == 0 )
                {
                  ( d->nd ) ++;
                }
              else
                {

                  snprintf ( b->error, sizeof ( b->error ), "%s(): No more bufr_atom_data available. Check BUFR_NMAXSEQ\n", __func__ );
                  return 1;
                }
              bufrdeco_decode_replicated_subsequence ( d, &replicator, b );

              // and then set again bitamping to 0, because it is finished
              b->state.bitmaping = 0;

              i += replicator.ndesc + 1;
            }
          //i = replicator.ixdel + replicator.ndesc; // update i properly
          break;

        case 2:
          // Case of operator descriptor
          if ( bufrdeco_parse_f2_descriptor ( d, & ( seq->lseq[i] ), b ) )
            {
              return 1;
            }
          if ( b->mask & BUFRDECO_OUTPUT_JSON_SUBSET_DATA )
          {
            if (j)
              bufrdeco_print_json_separator ( b->out );
            bufrdeco_print_json_object_operator_descriptor ( b->out, & ( seq->lseq[i] ), NULL );
            j++;
          }
          break;

        case 3:
          if ( b->mask & BUFRDECO_OUTPUT_JSON_SUBSET_DATA )
          {
            if (j)
              bufrdeco_print_json_separator ( b->out ); // print json array element separator
            j++;
          }
          // Case of sequence descriptor
          if ( bufrdeco_decode_subset_data_recursive ( d, seq->sons[i], b ) )
            {
              return 1;
            }
          break;

        default:
          // this case is not possible
          snprintf ( b->error, sizeof ( b->error ), "%s(): Found bad 'f' in descriptor '%s' \n", seq->lseq[i].c, __func__ );
          return 1;
          break;
        }
    }

  // End of sequence
  if ( b->mask & BUFRDECO_OUTPUT_JSON_SUBSET_DATA )
    bufrdeco_print_json_sequence_descriptor_final ( b->out );

  // End of data subset
  if ( l == NULL && ( b->mask & BUFRDECO_OUTPUT_JSON_SUBSET_DATA ) )
    bufrdeco_print_json_subset_data_epilogue ( b->out );

  return 0;

};

/*!
  \fn int bufrdeco_decode_replicated_subsequence ( struct bufrdeco_subset_sequence_data *s, struct bufr_replicator *r, struct bufrdeco *b )
  \brief Decodes a replicated sequence
  \param s target struct \ref bufrdeco_subset_sequence_data
  \param r pointer to a struct \ref bufr_replicator which manage the replication task
  \param b pointer to the base struct \ref bufrdeco
  \return If succeeded return 0, 1 otherwise
*/
int bufrdeco_decode_replicated_subsequence ( struct bufrdeco_subset_sequence_data *d, struct bufr_replicator *r, struct bufrdeco *b )
{
  buf_t i, k;
  buf_t ixloop; // Index for loop
  buf_t ixd; // Index for descriptor
  struct bufr_sequence *l = r->s; // sequence
  struct bufr_replicator replicator;
  char aux[80];

  bufrdeco_assert ( b != NULL );

  if ( d == NULL || r == NULL )
    {
      {
        snprintf ( b->error, sizeof ( b->error ), "%s(): Unspected NULL argument(s)\n", __func__ );
        return 1;
      }
    }

  for ( ixloop = 0; ixloop < r->nloops; ixloop++ )
    {
      for ( ixd = 0; ixd < r->ndesc ; ixd ++ )
        {
          if ( b->mask & BUFRDECO_OUTPUT_JSON_SUBSET_DATA )
            snprintf(aux, sizeof (aux), "\"Replicated\":\"Descriptor %u/%u of loop %u/%u\"", ixd + 1, ixloop + 1, r->ndesc, r->nloops);
  
          i = ixd + r->ixdel + 1;
          switch ( l->lseq[i].f )
            {
            case 0:
              // Checks if no_data_present is active for this descriptor in this sequence
              if ( l->no_data_present.active &&
                   i >= l->no_data_present.first &&
                   i <= l->no_data_present.last )
                {
                  // If here then no_data_present has been active in this sequence
                  if ( l->lseq[i].x > 9 &&
                       l->lseq[i].x != 31 )
                    {
                      snprintf ( b->error, sizeof ( b->error ), "%s(): Getting data from table b with class other than 1-9,31 and no data present activated\n", __func__ );
                      return 1;
                    }
                }

              // Repeat the extraction of data if b->state.data_repetition_factor != 0
              // It get data al least a time
              do
                {
                  // decrement counter if not zero
                  if ( b->state.data_repetition_factor )
                    ( b->state.data_repetition_factor )++;

                  // Get data from table B
                  if ( bufrdeco_tableB_val ( & ( d->sequence[d->nd] ), b, & ( l->lseq[i] ) ) )
                    {
                      return 1;
                    }
                  if ( b->mask & BUFRDECO_OUTPUT_JSON_SUBSET_DATA )
                  {
                    bufrdeco_print_json_separator ( b->out );
                    bufrdeco_print_json_object_atom_data ( b->out, & ( d->sequence[d->nd] ), aux );
                  }
                }
              while ( b->state.data_repetition_factor != 0 );

              // case of delayed replicator description and data
              if ( l->lseq[i].x == 31 && ( l->lseq[i].y == 11 || l->lseq[i].y == 12 ) )
                b->state.data_repetition_factor = ( buf_t ) d->sequence[d->nd].val;


              // Case of defining bitmap 0 31 031
              if ( l->lseq[i].x == 31 && l->lseq[i].y == 31 && b->state.bitmaping )
                {
                  if ( d->sequence[d->nd].val == 0.0 ) // Check if it is meaning present data
                    {
                      d->sequence[d->nd - b->state.bitmaping].is_bitmaped_by =  d->nd;
                      d->sequence[d->nd].bitmap_to =  d->nd - b->state.bitmaping;
                      // Add reference to bitmap
                      bufrdeco_add_to_bitmap ( b->bitmap.bmap[b->bitmap.nba - 1], d->nd - b->state.bitmaping, d->nd );
                    }
                }

              // Process quality data
              if ( b->state.quality_active &&
                   l->lseq[i].x == 33 )
                {
                  if ( ixloop == 0 )
                    {
                      k = b->bitmap.bmap[b->bitmap.nba - 1]->nq;
                      b->bitmap.bmap[b->bitmap.nba - 1]->quality[k] = d->nd;
                      if ( k < BUFR_MAX_QUALITY_DATA )
                        ( b->bitmap.bmap[b->bitmap.nba - 1]->nq )++;
                      else
                        {
                          snprintf ( b->error, sizeof ( b->error ), "%s(): No more space for quality vars in bitmap. Check BUFR_MAX_QUALITY_DATA\n", __func__ );
                          return 1;
                        }
                    }
                  d->sequence[d->nd].related_to = b->bitmap.bmap[b->bitmap.nba - 1]->bitmap_to[ixloop];
                }

              //case of first order statistics
              if ( b->state.stat1_active &&
                   l->lseq[i].x == 8 && l->lseq[i].y == 23 )
                {
                  if ( ixloop == 0 )
                    {
                      k = b->bitmap.bmap[b->bitmap.nba - 1]->ns1; // index un stqts
                      b->bitmap.bmap[b->bitmap.nba - 1]->stat1_desc[k] = d->nd; // Set the value of statistical parameter
                      // update the number of quality variables for the bitmap
                      if ( k < BUFR_MAX_QUALITY_DATA )
                        ( b->bitmap.bmap[b->bitmap.nba - 1]->ns1 )++;
                      else
                        {
                          snprintf ( b->error, sizeof ( b->error ), "%s(): No more space for first order statistic vars in bitmap. Check BUFR_MAX_QUALITY_DATA\n", __func__ );
                          return 1;
                        }
                    }
                  d->sequence[d->nd].related_to = b->bitmap.bmap[b->bitmap.nba - 1]->bitmap_to[ixloop];
                }

              //case of difference statistics
              if ( b->state.dstat_active &&
                   l->lseq[i].x == 8 && l->lseq[i].y == 24 )
                {
                  if ( ixloop == 0 )
                    {
                      k = b->bitmap.bmap[b->bitmap.nba - 1]->nds; // index in stats
                      b->bitmap.bmap[b->bitmap.nba - 1]->dstat_desc[k] = d->nd; // Set the value of statistical parameter
                      // update the number of quality variables for the bitmap
                      if ( k < BUFR_MAX_QUALITY_DATA )
                        ( b->bitmap.bmap[b->bitmap.nba - 1]->nds )++;
                      else
                        {
                          snprintf ( b->error, sizeof ( b->error ), "%s(): No more space for difference statistic vars in bitmap. Check BUFR_MAX_QUALITY_DATA\n", __func__ );
                          return 1;
                        }
                    }
                  d->sequence[d->nd].related_to = b->bitmap.bmap[b->bitmap.nba - 1]->bitmap_to[ixloop];
                }

              if ( d->nd < ( d->dim - 1 ) )
                {
                  ( d->nd ) ++;
                }
              else if ( bufrdeco_increase_data_array ( d ) == 0 )
                {
                  ( d->nd ) ++;
                }
              else
                {
                  snprintf ( b->error, sizeof ( b->error ), "%s(): No more bufr_atom_data available. Check BUFR_NMAXSEQ\n", __func__ );
                  return 1;
                }
              break;

            case 1:
              // Case of replicator descriptor
              replicator.s = l;
              replicator.ixrep = i;
              if ( b->mask & BUFRDECO_OUTPUT_JSON_SUBSET_DATA )
                {
                  bufrdeco_print_json_separator ( b->out );
                  bufrdeco_print_json_object_replicator_descriptor ( b->out, & ( l->lseq[i] ), aux );
                }

              if ( l->lseq[i].y != 0 )
                {
                  // no delayed
                  replicator.ixdel = i;
                  replicator.ndesc = l->lseq[i].x;
                  replicator.nloops = l->lseq[i].y;
                  bufrdeco_decode_replicated_subsequence ( d, &replicator, b );
                  ixd += replicator.ndesc; // update ixd properly
                }
              else
                {
                  // case of delayed;
                  replicator.ixdel = i + 1;
                  replicator.ndesc = l->lseq[i].x;
                  // here we read ndesc from delayed replicator descriptor
                  if ( bufrdeco_tableB_val ( & ( d->sequence[d->nd] ), b, & ( l->lseq[i + 1] ) ) )
                    {
                      return 1;
                    }
                  if ( b->mask & BUFRDECO_OUTPUT_JSON_SUBSET_DATA )
                    {
                      bufrdeco_print_json_separator ( b->out );
                      bufrdeco_print_json_object_atom_data ( b->out, & ( d->sequence[d->nd] ), aux );
                    }
                  replicator.nloops = ( size_t ) d->sequence[d->nd].val;
                  if ( d->nd < ( d->dim - 1 ) )
                    {
                      ( d->nd ) ++;
                    }
                  else if ( bufrdeco_increase_data_array ( d ) == 0 )
                    {
                      ( d->nd ) ++;
                    }
                  else
                    {
                      snprintf ( b->error, sizeof ( b->error ), "%s(): No more bufr_atom_data available. Check BUFR_NMAXSEQ\n", __func__ );
                      return 1;
                    }
                  bufrdeco_decode_replicated_subsequence ( d, &replicator, b );
                  ixd += replicator.ndesc + 1; // update ixd properly
                }
              //i = r->ixdel + r->ndesc; // update i properly
              break;

            case 2:
              // Case of operator descriptor
              if ( bufrdeco_parse_f2_descriptor ( d, & ( l->lseq[i] ), b ) )
                {
                  return 1;
                }

              if ( b->mask & BUFRDECO_OUTPUT_JSON_SUBSET_DATA )
              {
                bufrdeco_print_json_separator ( b->out );
                bufrdeco_print_json_object_operator_descriptor ( b->out, & ( l->lseq[i] ), aux );
              }
              // Case of subsituted values
              if ( b->state.subs_active && l->lseq[i].x == 23 && l->lseq[i].y == 255 )
                {
                  k = b->bitmap.bmap[b->bitmap.nba - 1]->bitmap_to[ixloop]; // ref which is bitmaped_to
                  // Get the bitmaped descriptor k
                  if ( ixloop == 0 )
                    {
                      b->bitmap.bmap[b->bitmap.nba - 1]->subs = d->nd;
                    }
                  if ( bufrdeco_tableB_val ( & ( d->sequence[d->nd] ), b, & ( l->lseq[k] ) ) )
                    {
                      return 1;
                    }
                  d->sequence[d->nd].related_to = b->bitmap.bmap[b->bitmap.nba - 1]->bitmap_to[ixloop];

                  if ( d->nd < ( d->dim - 1 ) )
                    {
                      d->nd += 1;
                    }
                  else
                    {
                      snprintf ( b->error, sizeof ( b->error ), "%s(): Reached limit. Consider increas BUFR_NMAXSEQ\n", __func__ );
                      return 1;
                    }
                }

              // Case of replaced/retained values
              if ( b->state.retained_active && l->lseq[i].x == 32 && l->lseq[i].y == 255 )
                {
                  k = b->bitmap.bmap[b->bitmap.nba - 1]->bitmap_to[ixloop];
                  // Get the bitmaped descriptor k
                  if ( ixloop == 0 )
                    {
                      b->bitmap.bmap[b->bitmap.nba - 1]->retain = d->nd;
                    }
                  if ( bufrdeco_tableB_val ( & ( d->sequence[d->nd] ), b, & ( l->lseq[k] ) ) )
                    {
                      return 1;
                    }

                  d->sequence[d->nd].related_to = b->bitmap.bmap[b->bitmap.nba - 1]->bitmap_to[ixloop];
                  if ( d->nd < ( d->dim - 1 ) )
                    {
                      d->nd += 1;
                    }
                  else
                    {
                      snprintf ( b->error, sizeof ( b->error ), "%s(): Reached limit. Consider increas BUFR_NMAXSEQ\n", __func__ );
                      return 1;
                    }
                }

              // Case of first order statistics
              //
              // This operator shall signify a data item containing a
              // first-order statistical value of the type indicated by
              // the preceding 0 08 023 element descriptor; the
              // element descriptor to which the first-order statistic
              // relates is obtained by the application of the data
              // present bit-map associated with the first-order
              // statistical values follow operator; first-order statistical
              // values shall be represented as defined by this element
              // descriptor.
              if ( b->state.stat1_active && l->lseq[i].x == 24 && l->lseq[i].y == 255 )
                {
                  k = b->bitmap.bmap[b->bitmap.nba - 1]->bitmap_to[ixloop];

                  // Get the bitmaped descriptor k
                  if ( ixloop == 0 )
                    {
                      b->bitmap.bmap[b->bitmap.nba - 1]->stat1[b->bitmap.bmap[b->bitmap.nba - 1]->ns1 -1] = d->nd;
                    }

                  if ( bufrdeco_tableB_val ( & ( d->sequence[d->nd] ), b, & ( l->lseq[k] ) ) )
                    {
                      return 1;
                    }

                  d->sequence[d->nd].related_to = b->bitmap.bmap[b->bitmap.nba - 1]->bitmap_to[ixloop];
                  if ( d->nd < ( d->dim - 1 ) )
                    {
                      d->nd += 1;
                    }
                  else
                    {
                      snprintf ( b->error, sizeof ( b->error ), "%s(): Reached limit. Consider increas BUFR_NMAXSEQ\n", __func__ );
                      return 1;
                    }
                }

              // Case of difference statistics
              //
              // This operator shall signify a data item containing a
              // difference statistical value of the type indicated by
              // the preceding 0 08 024 element descriptor; the
              // element descriptor to which the difference statistical
              // value relates is obtained by the application of the
              // data present bit-map associated with the difference
              // statistical values follow operator; difference statistical
              // values shall be represented as defined by this
              // element descriptor, but with a reference value of –2**n
              // and a data width of (n+1), where n is the data width
              // given by the original descriptor. This special reference
              // value allows the statistical difference values to
              // be centred around zero.
              if ( b->state.dstat_active && l->lseq[i].x == 25 && l->lseq[i].y == 255 )
                {
                  k = b->bitmap.bmap[b->bitmap.nba - 1]->bitmap_to[ixloop];

                  // Get the bitmaped descriptor k
                  if ( ixloop == 0 )
                    {
                      b->bitmap.bmap[b->bitmap.nba - 1]->stat1[b->bitmap.bmap[b->bitmap.nba - 1]->nds -1] = d->nd;
                    }

                  // in bufrdeco_tableB_val() is taken into acount when is difference statistics active
                  if ( bufrdeco_tableB_val ( & ( d->sequence[d->nd] ), b, & ( l->lseq[k] ) ) )
                    {
                      return 1;
                    }

                  /*d->sequence[d->nd].ref = - ( ( int32_t ) 1 << d->sequence[d->nd].bits );
                  d->sequence[d->nd].bits++;*/
                  d->sequence[d->nd].related_to = b->bitmap.bmap[b->bitmap.nba - 1]->bitmap_to[ixloop];
                  if ( d->nd < ( d->dim - 1 ) )
                    {
                      d->nd += 1;
                    }
                  else
                    {
                      snprintf ( b->error, sizeof ( b->error ), "%s(): Reached limit. Consider increas BUFR_NMAXSEQ\n", __func__ );
                      return 1;
                    }
                }

              break;

            case 3:
              if ( b->mask & BUFRDECO_OUTPUT_JSON_SUBSET_DATA )
                bufrdeco_print_json_separator ( b->out );
              if ( bufrdeco_decode_subset_data_recursive ( d, l->sons[i], b ) )
                {
                  return 1;
                }
              break;

            default:
              // this case is not possible
              snprintf ( b->error, sizeof ( b->error ), "%s(): Found bad 'f' in descriptor\n", __func__ );
              return 1;
              break;
            }
        }
    }
  return 0;
}
