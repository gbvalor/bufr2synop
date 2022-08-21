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

  Note that if succeeded the counter to current subset index is increased. Remember that in case of non
  compressed data we must to decode all previous subsets to get the desired one. In case of compressed
  we can access directly to a the desired subset

  If succeeded returns a pointer to the struct \ref bufrdeco_subset_sequence_data with the results for a
  subset,  otherwise returns NULL
*/
struct bufrdeco_subset_sequence_data * bufrdeco_get_subset_sequence_data ( struct bufrdeco *b )
{
  if ( bufrdeco_decode_data_subset ( & ( b->seq ), & ( b->refs ), b ) )
    {
      return NULL;
    }
  return & ( b->seq );
}


/*!
  \fn  int bufrdeco_decode_data_subset ( struct bufrdeco_subset_sequence_data *s, struct bufrdeco_compressed_data_references *r, struct bufrdeco *b )
  \brief  User interface to decode a BUFR subset
  \param s pointer to the target struct \ref bufrdeco_subset_sequence_data
  \param r pointer to the struct \ref bufrdeco_compressed_data_references
  \param b pointer to the base struct \ref bufrdeco

  Note that if succeeded the counter to current subset index is increased. Remember that in case of non
  compressed data we must to decode all previous subsets to get the desired one. In case of compressed
  we can access directly to a the desired subset

  Return 0 in case of success, 1 otherwise
*/
int bufrdeco_decode_data_subset ( struct bufrdeco_subset_sequence_data *s, struct bufrdeco_compressed_data_references *r,
                                  struct bufrdeco *b )
{
  // check arguments
  if ( b == NULL)
    return 1;
  
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
      if ( r->nd == 0 /*r->refs == NULL || r->dim == 0 */ )
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

  The amount of data in a bufr must be huge. In a first moment, the dimension of a sequence of structs
  \ref bufr_atom_data is \ref BUFR_NMAXSEQ but may be increased. This function task is try to double the
  allocated dimension and reallocate it.

  Return 0 when success, otherwise return 1 and the struct is unmodified
*/
int bufrdeco_increase_data_array ( struct bufrdeco_subset_sequence_data *s )
{
  if ( s == NULL )
    return 1;

  if ( s->dim < ( BUFR_NMAXSEQ * 8 ) ) // check if reached the limit
    {
      if ( ( s->sequence = ( struct bufr_atom_data * ) realloc ( ( void * ) s->sequence,
                           s->dim * 2 * sizeof ( struct bufr_atom_data ) ) ) == NULL )
        {
          return 1;
        }
      else
        {
          s->dim *= 2;
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

  Return 0 in case of success, 1 otherwise
*/
int bufrdeco_decode_subset_data_recursive ( struct bufrdeco_subset_sequence_data *s, struct bufr_sequence *l, struct bufrdeco *b )
{
  size_t i, k;
  struct bufr_sequence *seq;
  struct bufr_replicator replicator;

  if (b == NULL)
    return 1;
  
  if ( s == NULL )
    {
      snprintf ( b->error, sizeof ( b->error ), "%s(): Unspected NULL argument(s)\n", __func__ );
      return 1;
    }

  // clean subset data
  if ( l == NULL )
    {
      //memset ( s, 0, sizeof ( struct bufr_subset_sequence_data ) );
      s->nd = 0;
      s->ss = b->state.subset;
      seq = & ( b->tree->seq[0] );
      if ( b->state.subset == 0 )
        {
          b->state.bit_offset = 0;
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
      b->state.bitmap = NULL;
    }
  else
    {
      seq = l;
    }

  // loop for a sequence
  for ( i = 0; i < seq->ndesc ; i++ )
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
                  snprintf ( b->error, sizeof ( b->error ), "%s(): Getting data from table b with class other than 1-9,31 and no data present activated\n", __func__ );
                  return 1;
                }

            }

          // Get data from table B
          if ( bufrdeco_tableb_val ( & ( s->sequence[s->nd] ), b, & ( seq->lseq[i] ) ) )
            {
              return 1;
            }

          //case of first order statistics
          if ( b->state.stat1_active &&
               seq->lseq[i].x == 8 && seq->lseq[i].y == 23 )
            {
              k = b->bitmap.bmap[b->bitmap.nba - 1]->ns1; // index un stqts
              b->bitmap.bmap[b->bitmap.nba - 1]->stat1_desc[k] = s->nd; // Set the value of statistical parameter
              // update the number of quality variables for the bitmap
              if ( k < BUFR_MAX_QUALITY_DATA )
                ( b->bitmap.bmap[b->bitmap.nba - 1]->ns1 )++;
              else
                {
                  snprintf ( b->error, sizeof ( b->error ), "%s(): No more space for first order statistic vars in bitmap. Check BUFR_MAX_QUALITY_DATA\n", __func__ );
                  return 1;
                }
            }

          //case of difference statistics
          if ( b->state.dstat_active &&
               seq->lseq[i].x == 8 && seq->lseq[i].y == 24 )
            {
              k = b->bitmap.bmap[b->bitmap.nba - 1]->nds; // index in stats
              b->bitmap.bmap[b->bitmap.nba - 1]->dstat_desc[k] = s->nd; // Set the value of statistical parameter
              // update the number of quality variables for the bitmap
              if ( k < BUFR_MAX_QUALITY_DATA )
                ( b->bitmap.bmap[b->bitmap.nba - 1]->nds )++;
              else
                {
                  snprintf ( b->error, sizeof ( b->error ), "%s(): No more space for difference statistic vars in bitmap. Check BUFR_MAX_QUALITY_DATA\n", __func__ );
                  return 1;
                }
            }



          // Add info about common sequence to which bufr_atom_data belongs
          s->sequence[s->nd].seq = seq;
          s->sequence[s->nd].ns = i;

          //bufr_print_atom_data_stdout(& ( s->sequence[s->nd] ));
          if ( s->nd < ( s->dim - 1 ) )
            {
              ( s->nd ) ++;
            }
          else if ( bufrdeco_increase_data_array ( s ) == 0 )
            {
              ( s->nd ) ++;
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


              bufrdeco_decode_replicated_subsequence ( s, &replicator, b );

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
              if ( bufrdeco_tableb_val ( & ( s->sequence[s->nd] ), b, & ( seq->lseq[i + 1] ) ) )
                {
                  return 1;
                }
              // Add info about common sequence to which bufr_atom_data belongs
              s->sequence[s->nd].seq = seq;
              s->sequence[s->nd].ns = i + 1;

              replicator.nloops = ( size_t ) ( s->sequence[s->nd].val );

              // Check if this replicator is for a bit-map defining
              if ( b->state.bitmaping )
                {
                  b->state.bitmaping = replicator.nloops; // set it properly
                }

              if ( s->nd < ( s->dim - 1 ) )
                {
                  ( s->nd ) ++;
                }
              else if ( bufrdeco_increase_data_array ( s ) == 0 )
                {
                  ( s->nd ) ++;
                }
              else
                {

                  snprintf ( b->error, sizeof ( b->error ), "%s(): No more bufr_atom_data available. Check BUFR_NMAXSEQ\n", __func__ );
                  return 1;
                }
              bufrdeco_decode_replicated_subsequence ( s, &replicator, b );

              // and then set again bitamping to 0, because it is finished
              b->state.bitmaping = 0;

              i += replicator.ndesc + 1;
            }
          //i = replicator.ixdel + replicator.ndesc; // update i properly
          break;

        case 2:
          // Case of operator descriptor
          if ( bufrdeco_parse_f2_descriptor ( s, & ( seq->lseq[i] ), b ) )
            {
              return 1;
            }
          break;

        case 3:
          // Case of sequence descriptor
          if ( bufrdeco_decode_subset_data_recursive ( s, seq->sons[i], b ) )
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
  return 0;
};

/*!
  \fn int bufrdeco_decode_replicated_subsequence ( struct bufrdeco_subset_sequence_data *s, struct bufr_replicator *r, struct bufrdeco *b )
  \brief Decodes a replicated sequence
  \param s target struct \ref bufrdeco_subset_sequence_data
  \param r pointer to a struct \ref bufr_replicator which manage the replication task
  \param b pointer to the base struct \ref bufrdeco

  If succeeded return 0, 1 otherwise
*/
int bufrdeco_decode_replicated_subsequence ( struct bufrdeco_subset_sequence_data *s, struct bufr_replicator *r, struct bufrdeco *b )
{
  size_t i, k;
  size_t ixloop; // Index for loop
  size_t ixd; // Index for descriptor
  struct bufr_sequence *l = r->s; // sequence
  struct bufr_replicator replicator;

  if (b == NULL)
    return 1;
  
  if (s == NULL || r == NULL )
  {
    {
      snprintf ( b->error, sizeof ( b->error ), "%s(): Unspected NULL argument(s)\n", __func__ );
      return 1;
    }
  }
  //printf("nloops=%lu, ndesc=%lu\n", r->nloops, r->ndesc);
  for ( ixloop = 0; ixloop < r->nloops; ixloop++ )
    {
      for ( ixd = 0; ixd < r->ndesc ; ixd ++ )
        {
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

              // Get data from table B
              if ( bufrdeco_tableb_val ( & ( s->sequence[s->nd] ), b, & ( l->lseq[i] ) ) )
                {
                  return 1;
                }

              // Case of defining bitmap 0 31 031
              if ( l->lseq[i].x == 31 && l->lseq[i].y == 31 && b->state.bitmaping )
                {
                  if ( s->sequence[s->nd].val == 0.0 ) // Check if it is meaning present data
                    {
                      s->sequence[s->nd - b->state.bitmaping].is_bitmaped_by =  s->nd;
                      s->sequence[s->nd].bitmap_to =  s->nd - b->state.bitmaping;
                      // Add reference to bitmap
                      bufrdeco_add_to_bitmap ( b->bitmap.bmap[b->bitmap.nba - 1], s->nd - b->state.bitmaping, s->nd );
                    }
                }

              // Process quality data
              if ( b->state.quality_active &&
                   l->lseq[i].x == 33 )
                {
                  if ( ixloop == 0 )
                    {
                      k = b->bitmap.bmap[b->bitmap.nba - 1]->nq;
                      b->bitmap.bmap[b->bitmap.nba - 1]->quality[k] = s->nd;
                      if ( k < BUFR_MAX_QUALITY_DATA )
                        ( b->bitmap.bmap[b->bitmap.nba - 1]->nq )++;
                      else
                        {
                          snprintf ( b->error, sizeof ( b->error ), "%s(): No more space for quality vars in bitmap. Check BUFR_MAX_QUALITY_DATA\n", __func__ );
                          return 1;
                        }
                    }
                  s->sequence[s->nd].related_to = b->bitmap.bmap[b->bitmap.nba - 1]->bitmap_to[ixloop];
                }

              //case of first order statistics
              if ( b->state.stat1_active &&
                   l->lseq[i].x == 8 && l->lseq[i].y == 23 )
                {
                  if ( ixloop == 0 )
                    {
                      k = b->bitmap.bmap[b->bitmap.nba - 1]->ns1; // index un stqts
                      b->bitmap.bmap[b->bitmap.nba - 1]->stat1_desc[k] = s->nd; // Set the value of statistical parameter
                      // update the number of quality variables for the bitmap
                      if ( k < BUFR_MAX_QUALITY_DATA )
                        ( b->bitmap.bmap[b->bitmap.nba - 1]->ns1 )++;
                      else
                        {
                          snprintf ( b->error, sizeof ( b->error ), "%s(): No more space for first order statistic vars in bitmap. Check BUFR_MAX_QUALITY_DATA\n", __func__ );
                          return 1;
                        }
                    }
                  s->sequence[s->nd].related_to = b->bitmap.bmap[b->bitmap.nba - 1]->bitmap_to[ixloop];
                }

              //case of difference statistics
              if ( b->state.dstat_active &&
                   l->lseq[i].x == 8 && l->lseq[i].y == 24 )
                {
                  if ( ixloop == 0 )
                    {
                      k = b->bitmap.bmap[b->bitmap.nba - 1]->nds; // index in stats
                      b->bitmap.bmap[b->bitmap.nba - 1]->dstat_desc[k] = s->nd; // Set the value of statistical parameter
                      // update the number of quality variables for the bitmap
                      if ( k < BUFR_MAX_QUALITY_DATA )
                        ( b->bitmap.bmap[b->bitmap.nba - 1]->nds )++;
                      else
                        {
                          snprintf ( b->error, sizeof ( b->error ), "%s(): No more space for difference statistic vars in bitmap. Check BUFR_MAX_QUALITY_DATA\n", __func__ );
                          return 1;
                        }
                    }
                  s->sequence[s->nd].related_to = b->bitmap.bmap[b->bitmap.nba - 1]->bitmap_to[ixloop];
                }

              if ( s->nd < ( s->dim - 1 ) )
                {
                  ( s->nd ) ++;
                }
              else if ( bufrdeco_increase_data_array ( s ) == 0 )
                {
                  ( s->nd ) ++;
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
              if ( l->lseq[i].y != 0 )
                {
                  // no delayed
                  replicator.ixdel = i;
                  replicator.ndesc = l->lseq[i].x;
                  replicator.nloops = l->lseq[i].y;
                  bufrdeco_decode_replicated_subsequence ( s, &replicator, b );
                  ixd += replicator.ndesc; // update ixd properly
                }
              else
                {
                  // case of delayed;
                  replicator.ixdel = i + 1;
                  replicator.ndesc = l->lseq[i].x;
                  // here we read ndesc from delayed replicator descriptor
                  if ( bufrdeco_tableb_val ( & ( s->sequence[s->nd] ), b, & ( l->lseq[i + 1] ) ) )
                    {
                      return 1;
                    }
                  replicator.nloops = ( size_t ) s->sequence[s->nd].val;
                  if ( s->nd < ( s->dim - 1 ) )
                    {
                      ( s->nd ) ++;
                    }
                  else if ( bufrdeco_increase_data_array ( s ) == 0 )
                    {
                      ( s->nd ) ++;
                    }
                  else
                    {
                      snprintf ( b->error, sizeof ( b->error ), "%s(): No more bufr_atom_data available. Check BUFR_NMAXSEQ\n", __func__ );
                      return 1;
                    }
                  bufrdeco_decode_replicated_subsequence ( s, &replicator, b );
                  ixd += replicator.ndesc + 1; // update ixd properly
                }
              //i = r->ixdel + r->ndesc; // update i properly
              break;

            case 2:
              // Case of operator descriptor
              if ( bufrdeco_parse_f2_descriptor ( s, & ( l->lseq[i] ), b ) )
                {
                  return 1;
                }

              // Case of subsituted values
              if ( b->state.subs_active && l->lseq[i].x == 23 && l->lseq[i].y == 255 )
                {
                  k = b->bitmap.bmap[b->bitmap.nba - 1]->bitmap_to[ixloop]; // ref which is bitmaped_to
                  // Get the bitmaped descriptor k
                  if ( ixloop == 0 )
                    {
                      b->bitmap.bmap[b->bitmap.nba - 1]->subs = s->nd;
                    }
                  if ( bufrdeco_tableb_val ( & ( s->sequence[s->nd] ), b, & ( l->lseq[k] ) ) )
                    {
                      return 1;
                    }
                  s->sequence[s->nd].related_to = b->bitmap.bmap[b->bitmap.nba - 1]->bitmap_to[ixloop];

                  if ( s->nd < ( s->dim - 1 ) )
                    {
                      s->nd += 1;
                    }
                  else
                    {
                      snprintf ( b->error, sizeof ( b->error ), "%s(): Reached limit. Consider increas BUFR_NMAXSEQ\n", __func__ );
                      return 1;
                    }
                }

              // Case of repaced/retained values
              if ( b->state.retained_active && l->lseq[i].x == 32 && l->lseq[i].y == 255 )
                {
                  k = b->bitmap.bmap[b->bitmap.nba - 1]->bitmap_to[ixloop];
                  // Get the bitmaped descriptor k
                  if ( ixloop == 0 )
                    {
                      b->bitmap.bmap[b->bitmap.nba - 1]->retain = s->nd;
                    }
                  if ( bufrdeco_tableb_val ( & ( s->sequence[s->nd] ), b, & ( l->lseq[k] ) ) )
                    {
                      return 1;
                    }

                  s->sequence[s->nd].related_to = b->bitmap.bmap[b->bitmap.nba - 1]->bitmap_to[ixloop];
                  if ( s->nd < ( s->dim - 1 ) )
                    {
                      s->nd += 1;
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
                      b->bitmap.bmap[b->bitmap.nba - 1]->stat1[b->bitmap.bmap[b->bitmap.nba - 1]->ns1 -1] = s->nd;
                    }

                  if ( bufrdeco_tableb_val ( & ( s->sequence[s->nd] ), b, & ( l->lseq[k] ) ) )
                    {
                      return 1;
                    }

                  s->sequence[s->nd].related_to = b->bitmap.bmap[b->bitmap.nba - 1]->bitmap_to[ixloop];
                  if ( s->nd < ( s->dim - 1 ) )
                    {
                      s->nd += 1;
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
                      b->bitmap.bmap[b->bitmap.nba - 1]->stat1[b->bitmap.bmap[b->bitmap.nba - 1]->nds -1] = s->nd;
                    }

                  // in bufrdeco_tableb_val() is taken into acount when is difference statistics active
                  if ( bufrdeco_tableb_val ( & ( s->sequence[s->nd] ), b, & ( l->lseq[k] ) ) )
                    {
                      return 1;
                    }

                  /*s->sequence[s->nd].ref = - ( ( int32_t ) 1 << s->sequence[s->nd].bits );
                  s->sequence[s->nd].bits++;*/
                  s->sequence[s->nd].related_to = b->bitmap.bmap[b->bitmap.nba - 1]->bitmap_to[ixloop];
                  if ( s->nd < ( s->dim - 1 ) )
                    {
                      s->nd += 1;
                    }
                  else
                    {
                      snprintf ( b->error, sizeof ( b->error ), "%s(): Reached limit. Consider increas BUFR_NMAXSEQ\n", __func__ );
                      return 1;
                    }
                }

              break;

            case 3:
              // Case of sequence descriptor
              if ( bufrdeco_decode_subset_data_recursive ( s, l->sons[i], b ) )
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
