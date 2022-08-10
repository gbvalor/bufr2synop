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
 \file bufrdeco_compressed.c
 \brief This file has the code to deal with compressed bufr reports
*/
#include "bufrdeco.h"

/*!
  \fn int bufrdeco_parse_compressed ( struct bufrdeco_compressed_data_references *r, struct bufrdeco *b )
  \brief Parse a compressed data bufr
  \param r pointer to a struct \ref bufrdeco_compressed_data_references where to set the results
  \param b basic container struct \ref bufrdeco

  When a bufr report has compressed data, it is needed to do a first parse step to get references about
  where to find the data for every descriptor in subsets. This is what this function does. If succeeded
  the struct \a r will have all needed data to decode individual subsets.

  Also be mind that the descriptors tree have to be already parsed when calling this function

  Returns 0 if succeeded, 1 otherwise
*/
int bufrdeco_parse_compressed ( struct bufrdeco_compressed_data_references *r, struct bufrdeco *b )
{
  // Check about parsed tree
  if ( b->tree == NULL || b->tree->nseq == 0 )
    {
      sprintf ( b->error, "bufrdeco_parse_compressed(): Try to parse compressed data without parsed tree\n" );
      return 1;
    }

  // first we assure that needed memory is allocated and array of references initizalized
  if ( bufrdeco_init_compressed_data_references ( r ) )
    {
      return 1;
    }

  // Then we make the parsing task in a recursive way. NULL pointer says it is the begining.
  if ( bufrdeco_parse_compressed_recursive ( r, NULL, b ) )
    {
      return 1;
    }

  // all is OK
  return 0;
}


/*!
  \fn int bufrdeco_parse_compressed_recursive ( struct bufrdeco_compressed_data_references *r, struct bufr_sequence *l, struct bufrdeco *b )
  \brief Parse recursively the compressed data in a bufr report to get references where to get data for every descriptor in a subset
  \param r pointer to target struct \ref bufrdeco_compressed_data_references where to set results
  \param l pointer to a struct \ref bufr_sequence to parse in this call. If NULL then it is first root sequence
  \param b basic container struct \ref bufrdeco

  Returns 0 if succeeded, 1 otherwise
*/
int bufrdeco_parse_compressed_recursive ( struct bufrdeco_compressed_data_references *r, struct bufr_sequence *l, struct bufrdeco *b )
{
  int res;
  size_t i, k;
  struct bufr_sequence *seq; // auxiliar pointer
  struct bufrdeco_compressed_ref *rf;
  struct bufr_replicator replicator;

  if ( l == NULL )
    {
      // inits the array
      // At the moment there are no structs bufrdeco_compressed ref
      r->nd = 0;
      // set the auxiliar pointer at the begining
      seq = & ( b->tree->seq[0] );
      // Cleans the state of parsing
      memset ( & ( b->state ), 0, sizeof ( struct bufrdeco_decoding_data_state ) );
      // and set some other initial values
      b->state.bit_offset = 0;
      b->state.added_bit_length = 0;
      b->state.added_scale = 0;
      b->state.added_reference = 0;
      b->state.assoc_bits = 0;
      b->state.changing_reference = 255;
      b->state.fixed_ccitt = 0;
      b->state.local_bit_reserved = 0;
      b->state.factor_reference = 1;
      b->state.quality_active = 0;
      b->state.bitmaping = 0;
      b->state.bitmap = NULL;
    }
  else
    {
      // This is the case when layer of sequence is no 0, i.e. has been called recursively
      // We just set the auxiliar index
      seq = l;
    }

  // loop for a sequence
  for ( i = 0; i < seq->ndesc ; i++ )
    {
      // switch according to 'f' of the descriptor of sequence
      switch ( seq->lseq[i].f )
        {
        case 0:
          // Get data from table B
          rf = & ( r->refs[r->nd] ); // pointer to the target struct bufrdeco_compressed ref

          // First the the data itself
          if ( bufrdeco_tableb_compressed ( rf, b, & ( seq->lseq[i] ), 0 ) )
            {
              return 1;
            }

          //case of first order statistics
          if ( b->state.stat1_active &&
               seq->lseq[i].x == 8 && seq->lseq[i].y == 23 )
            {
              k = b->bitmap.bmap[b->bitmap.nba - 1]->ns1; // index un stqts
              b->bitmap.bmap[b->bitmap.nba - 1]->stat1_desc[k] = r->nd; // Set the value of statistical parameter
              // update the number of quality variables for the bitmap
              if ( k < BUFR_MAX_QUALITY_DATA )
                ( b->bitmap.bmap[b->bitmap.nba - 1]->ns1 )++;
              else
                {
                  sprintf ( b->error, "bufrdeco_decode_replicated_subsequence_compressed(): No more space for first order statistic vars in bitmap. Check BUFR_MAX_QUALITY_DATA\n" );
                  return 1;
                }
            }

          //case of difference statistics
          if ( b->state.dstat_active &&
               seq->lseq[i].x == 8 && seq->lseq[i].y == 24 )
            {
              k = b->bitmap.bmap[b->bitmap.nba - 1]->nds; // index un stqts
              b->bitmap.bmap[b->bitmap.nba - 1]->dstat_desc[k] = r->nd; // Set the value of statistical parameter
              // update the number of quality variables for the bitmap
              if ( k < BUFR_MAX_QUALITY_DATA )
                ( b->bitmap.bmap[b->bitmap.nba - 1]->nds )++;
              else
                {
                  sprintf ( b->error, "bufrdeco_decode_replicated_subsequence_compressed(): No more space for difference statistic vars in bitmap. Check BUFR_MAX_QUALITY_DATA\n" );
                  return 1;
                }
            }

          if ( r->nd < ( BUFR_NMAXSEQ - 1 ) )
            {
              r->nd += 1;
            }
          else
            {
              sprintf ( b->error, "bufr_parse_compressed_recursive(): Reached limit. Consider increas BUFR_NMAXSEQ\n" );
              return 1;
            }

          // Then we check for an associated field
          // If is not an associated field returned value is -1
          rf = & ( r->refs[r->nd] );
          res = bufrdeco_tableb_compressed ( rf, b, & ( seq->lseq[i] ), 1 );
          if ( res > 0 )
            {
              return 1; // case of error
            }
          else if ( res == 0 )
            {
              // print_bufrdeco_compressed_ref ( rf );
              // associated field read with success
              if ( r->nd < ( BUFR_NMAXSEQ - 1 ) )
                {
                  r->nd += 1;
                }
              else
                {
                  sprintf ( b->error, "bufr_parse_compressed_recursive(): Reached limit. Consider increas BUFR_NMAXSEQ\n" );
                  return 1;
                }
              // Update the pointer to the target struct bufrdeco_compressed ref
              i++; // Update main loop index
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

              // call to decode a replicated subsequence
              bufrdeco_decode_replicated_subsequence_compressed ( r, &replicator, b );

              // and then set again bitamping to 0, because it is finished
              b->state.bitmaping = 0;
            }
          else
            {
              // case of delayed;
              replicator.ixdel = i + 1;
              replicator.ndesc = seq->lseq[i].x;
              rf = & ( r->refs[r->nd] );

              // The the data itself to get the loops
              if ( bufrdeco_tableb_compressed ( rf, b, & ( seq->lseq[i + 1] ), 0 ) )
                {
                  return 1;
                }

              //print_bufrdeco_compressed_ref ( rf );
              if ( r->nd < ( BUFR_NMAXSEQ -1 ) )
                {
                  r->nd += 1;
                }
              else
                {
                  sprintf ( b->error, "bufrdeco_decode_replicated_subsequence_compressed(): Reached limit. Consider increas BUFR_NMAXSEQ\n" );
                  return 1;
                }

              replicator.nloops = ( size_t ) rf->ref0;

              // Check if this replicator is for a bit-map defining
              if ( b->state.bitmaping )
                {
                  b->state.bitmaping = replicator.nloops; // set it properly
                }

              // call to decode a replicated subsequence
              bufrdeco_decode_replicated_subsequence_compressed ( r, &replicator, b );

              // and then set again bitamping to 0, because it is finished
              b->state.bitmaping = 0;
            }
          i = replicator.ixdel + replicator.ndesc; // update i properly
          break;

        case 2:
          // Case of operator descriptor
          if ( bufrdeco_parse_f2_compressed ( r, & ( seq->lseq[i] ), b ) )
            {
              return 1;
            }
          break;

        case 3:
          // Case of sequence descriptor
          if ( bufrdeco_parse_compressed_recursive ( r, seq->sons[i], b ) )
            {
              return 1;
            }
          break;

        default:
          // this case is not possible
          sprintf ( b->error, "bufr_parse_compressed_recursive(): Found bad 'f' in descriptor\n" );
          return 1;
          break;
        }
    }
  return 0;
}

/*!
  \fn int bufrdeco_decode_replicated_subsequence_compressed ( struct bufrdeco_compressed_data_references *r, struct bufr_replicator *rep, struct bufrdeco *b )
  \brief decodes a repicated subsequence
  \param r pointer to target struct \ref bufrdeco_compressed_data_references where to set results
  \param rep pointer to a replicator which contains the data for replication
  \param b basic container struct \ref bufrdeco

  Returns 0 if succeeded, 1 otherwise
*/
int bufrdeco_decode_replicated_subsequence_compressed ( struct bufrdeco_compressed_data_references *r, struct bufr_replicator *rep, struct bufrdeco *b )
{
  int res;
  size_t i, k;
  size_t ixloop; // Index for loop
  size_t ixd; // Index for descriptor
  struct bufrdeco_compressed_ref *rf;
  struct bufr_replicator replicator;
  struct bufr_sequence *l = rep->s; // sequence

  // if no loops the we save process time and space in data
  if ( rep->nloops == 0 )
    return 0;


  // The big loop
  for ( ixloop = 0; ixloop < rep->nloops; ixloop++ )
    {
      for ( ixd = 0; ixd < rep->ndesc ; ixd ++ )
        {
          i = ixd + rep->ixdel + 1;
          switch ( l->lseq[i].f )
            {
            case 0:
              // Get data from table B
              rf = & ( r->refs[r->nd] );

              // First then the data itself
              if ( bufrdeco_tableb_compressed ( rf, b, & ( l->lseq[i] ), 0 ) )
                {
                  return 1;
                }

              // Case of defining bitmap 0 31 031
              if ( l->lseq[i].x == 31 && l->lseq[i].y == 31 && b->state.bitmaping )
                {
                  if ( rf->ref0 == 0 ) // Assume if ref0 == 0 then all subsets are present in same bits
                    {
                      r->refs[r->nd - b->state.bitmaping].is_bitmaped_by = ( uint32_t ) r->nd ;
                      rf->bitmap_to = r->nd - b->state.bitmaping;
                      bufrdeco_add_to_bitmap ( b->bitmap.bmap[b->bitmap.nba - 1], r->nd - b->state.bitmaping, r->nd );
                    }
                }

              // Process quality data
              if ( b->state.quality_active &&
                   l->lseq[i].x == 33 )
                {
                  if ( ixloop == 0 )
                    {
                      k = b->bitmap.bmap[b->bitmap.nba - 1]->nq; // index un quality_given_by array to set
                      b->bitmap.bmap[b->bitmap.nba - 1]->quality[k] = r->nd; // Set the value
                      // update the number of quality variables for the bitmap
                      if ( k < BUFR_MAX_QUALITY_DATA )
                        ( b->bitmap.bmap[b->bitmap.nba - 1]->nq )++;
                      else
                        {
                          sprintf ( b->error, "bufrdeco_decode_replicated_subsequence_compressed(): No more space for quality vars in bitmap. Check BUFR_MAX_QUALITY_DATA\n" );
                          return 1;
                        }
                    }
                  rf->related_to = b->bitmap.bmap[b->bitmap.nba - 1]->bitmap_to[ixloop];

                }

              //case of first order statistics
              if ( b->state.stat1_active &&
                   l->lseq[i].x == 8 && l->lseq[i].y == 23 )
                {
                  if ( ixloop == 0 )
                    {
                      k = b->bitmap.bmap[b->bitmap.nba - 1]->ns1; // index un stqts
                      b->bitmap.bmap[b->bitmap.nba - 1]->stat1_desc[k] = r->nd; // Set the value of statistical parameter
                      // update the number of quality variables for the bitmap
                      if ( k < BUFR_MAX_QUALITY_DATA )
                        ( b->bitmap.bmap[b->bitmap.nba - 1]->ns1 )++;
                      else
                        {
                          sprintf ( b->error, "bufrdeco_decode_replicated_subsequence_compressed(): No more space for first order statistic vars in bitmap. Check BUFR_MAX_QUALITY_DATA\n" );
                          return 1;
                        }
                    }
                  rf->related_to = b->bitmap.bmap[b->bitmap.nba - 1]->bitmap_to[ixloop];
                }

              //case of difference statistics
              if ( b->state.dstat_active &&
                   l->lseq[i].x == 8 && l->lseq[i].y == 24 )
                {
                  if ( ixloop == 0 )
                    {
                      k = b->bitmap.bmap[b->bitmap.nba - 1]->nds; // index un stqts
                      b->bitmap.bmap[b->bitmap.nba - 1]->dstat_desc[k] = r->nd; // Set the value of statistical parameter
                      // update the number of quality variables for the bitmap
                      if ( k < BUFR_MAX_QUALITY_DATA )
                        ( b->bitmap.bmap[b->bitmap.nba - 1]->nds )++;
                      else
                        {
                          sprintf ( b->error, "bufrdeco_decode_replicated_subsequence_compressed(): No more space for difference statistic vars in bitmap. Check BUFR_MAX_QUALITY_DATA\n" );
                          return 1;
                        }
                    }
                  rf->related_to = b->bitmap.bmap[b->bitmap.nba - 1]->bitmap_to[ixloop];
                }

              //print_bufrdeco_compressed_ref ( rf );
              if ( r->nd < ( BUFR_NMAXSEQ -1 ) )
                {
                  r->nd += 1;
                }
              else
                {
                  sprintf ( b->error, "bufrdeco_decode_replicated_subsequence_compressed(): Reached limit. Consider increas BUFR_NMAXSEQ\n" );
                  return 1;
                }

              // Then associated field
              rf = & ( r->refs[r->nd] );
              res = bufrdeco_tableb_compressed ( rf, b, & ( l->lseq[i] ), 1 );
              if ( res > 0 )
                {
                  return 1; // case of error
                }
              else if ( res == 0 )
                {
                  //print_bufrdeco_compressed_ref ( rf );
                  // associated field read with success
                  if ( r->nd < ( BUFR_NMAXSEQ - 1 ) )
                    {
                      r->nd += 1;
                    }
                  else
                    {
                      sprintf ( b->error, "bufrdeco_decode_replicated_subsequence_compressed(): Reached limit. Consider increas BUFR_NMAXSEQ\n" );
                      return 1;
                    }
                  b->state.assoc_bits = 0; // Set again the assoc_bits to 0
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
                  bufrdeco_decode_replicated_subsequence_compressed ( r, & replicator, b );
                  ixd += replicator.ndesc; // update ixd properly
                }
              else
                {
                  // case of delayed;
                  replicator.ixdel = i + 1;
                  replicator.ndesc = l->lseq[i].x;
                  rf = & ( r->refs[r->nd] );

                  // The data itself to get the loops
                  if ( bufrdeco_tableb_compressed ( rf, b, & ( l->lseq[i + 1] ), 0 ) )
                    {
                      return 1;
                    }
                  //print_bufrdeco_compressed_ref ( rf );
                  if ( r->nd < ( BUFR_NMAXSEQ - 1 ) )
                    {
                      r->nd += 1;
                    }
                  else
                    {
                      sprintf ( b->error, "bufrdeco_decode_replicated_subsequence_compressed(): Reached limit. Consider increas BUFR_NMAXSEQ\n" );
                      return 1;
                    }
                  replicator.nloops = ( size_t ) rf->ref0;
                }

              bufrdeco_decode_replicated_subsequence_compressed ( r, &replicator, b );
              ixd += replicator.ndesc + 1; // update ixd properly

              //i = rep->ixdel + rep->ndesc; // update i properly
              break;

            case 2:
              // Case of operator descriptor
              if ( bufrdeco_parse_f2_compressed ( r, & ( l->lseq[i] ), b ) )
                {
                  return 1;
                }

              // Case of subsituted values
              if ( b->state.subs_active && l->lseq[i].x == 23 && l->lseq[i].y == 255 )
                {
                  k = b->bitmap.bmap[b->bitmap.nba - 1]->bitmap_to[ixloop]; // ref which is bitmaped_to
                  // Get the bitmaped descriptor k
                  rf = & ( r->refs[r->nd] );
                  if ( ixloop == 0 )
                    {
                      b->bitmap.bmap[b->bitmap.nba - 1]->subs = r->nd;
                    }
                  if ( bufrdeco_tableb_compressed ( rf, b, & ( l->lseq[k] ), 0 ) )
                    {
                      return 1;
                    }
                  rf->related_to = b->bitmap.bmap[b->bitmap.nba - 1]->bitmap_to[ixloop];

                  if ( r->nd < ( BUFR_NMAXSEQ - 1 ) )
                    {
                      r->nd += 1;
                    }
                  else
                    {
                      sprintf ( b->error, "bufrdeco_decode_replicated_subsequence_compressed(): Reached limit. Consider increas BUFR_NMAXSEQ\n" );
                      return 1;
                    }
                }

              // Case of repaced/retained values
              if ( b->state.retained_active && l->lseq[i].x == 32 && l->lseq[i].y == 255 )
                {
                  k = b->bitmap.bmap[b->bitmap.nba - 1]->bitmap_to[ixloop];
                  // Get the bitmaped descriptor k
                  rf = & ( r->refs[r->nd] );
                  if ( ixloop == 0 )
                    {
                      b->bitmap.bmap[b->bitmap.nba - 1]->retain = r->nd;
                    }
                  if ( bufrdeco_tableb_compressed ( rf, b, & ( l->lseq[k] ), 0 ) )
                    {
                      return 1;
                    }

                  rf->related_to = b->bitmap.bmap[b->bitmap.nba - 1]->bitmap_to[ixloop];
                  if ( r->nd < ( BUFR_NMAXSEQ - 1 ) )
                    {
                      r->nd += 1;
                    }
                  else
                    {
                      sprintf ( b->error, "bufrdeco_decode_replicated_subsequence_compressed(): Reached limit. Consider increas BUFR_NMAXSEQ\n" );
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
                  rf = & ( r->refs[r->nd] );
                  if ( ixloop == 0 )
                    {
                      b->bitmap.bmap[b->bitmap.nba - 1]->stat1[b->bitmap.bmap[b->bitmap.nba - 1]->ns1 -1] = r->nd;
                    }

                  if ( bufrdeco_tableb_compressed ( rf, b, & ( r->refs[k].desc ), 0 ) )
                    {
                      return 1;
                    }
                  rf->related_to = b->bitmap.bmap[b->bitmap.nba - 1]->bitmap_to[ixloop];
                  if ( r->nd < ( BUFR_NMAXSEQ - 1 ) )
                    {
                      r->nd += 1;
                    }
                  else
                    {
                      sprintf ( b->error, "bufrdeco_decode_replicated_subsequence_compressed(): Reached limit. Consider increas BUFR_NMAXSEQ\n" );
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
                  rf = & ( r->refs[r->nd] );
                  if ( ixloop == 0 )
                    {
                      b->bitmap.bmap[b->bitmap.nba - 1]->stat1[b->bitmap.bmap[b->bitmap.nba - 1]->nds -1] = r->nd;
                    }

                  if ( bufrdeco_tableb_compressed ( rf, b, & ( r->refs[k].desc ), 0 ) )
                    {
                      return 1;
                    }

                  // here is where we change ref and bits
                  rf->ref = - ( ( int32_t ) 1 << rf->bits );
                  rf->bits++;
                  rf->related_to = b->bitmap.bmap[b->bitmap.nba - 1]->bitmap_to[ixloop];
                  if ( r->nd < ( BUFR_NMAXSEQ - 1 ) )
                    {
                      r->nd += 1;
                    }
                  else
                    {
                      sprintf ( b->error, "bufrdeco_decode_replicated_subsequence_compressed(): Reached limit. Consider increas BUFR_NMAXSEQ\n" );
                      return 1;
                    }
                }

              if ( l->lseq[i].x == 5 ) // cases wich produces a new ref
                {
                  if ( r->nd < ( BUFR_NMAXSEQ - 1 ) )
                    {
                      r->nd += 1;
                    }
                  else
                    {
                      sprintf ( b->error, "bufrdeco_decode_replicated_subsequence_compressed(): Reached limit. Consider increas BUFR_NMAXSEQ\n" );
                      return 1;
                    }
                }
              break;

            case 3:
              // Case of sequence descriptor
              if ( bufrdeco_parse_compressed_recursive ( r, l->sons[i], b ) )
                return 1;
              break;

            default:
              // this case is not possible
              sprintf ( b->error, "bufrdeco_decode_replicated_subsequence_compressed(): Found bad 'f' in descriptor\n" );
              return 1;
              break;
            }
        }
    }
  return 0;
}

/*!

  \fn int bufrdeco_get_atom_data_from_compressed_data_ref ( struct bufr_atom_data *a, struct bufrdeco_compressed_ref *r, size_t subset, struct bufrdeco *b )
  \brief Get atom data from a descriptor for a given subset
  \param a pointer to the target struct \ref bufr_atom_data where to set the results
  \param r pointer to the struct \ref bufrdeco_compressed_ref with the info to know how and where get the data
  \param subset index for solicited subset. First subset has index 0
  \param b basic container struct \ref bufrdeco

  Returns 0 if succeeded, 1 otherwise
*/
int bufrdeco_get_atom_data_from_compressed_data_ref ( struct bufr_atom_data *a, struct bufrdeco_compressed_ref *r,
    size_t subset, struct bufrdeco *b )
{
  size_t i, bit_offset;
  uint8_t has_data;
  uint32_t ival, ival0;
  int32_t ivals;
  struct bufr_tableb *tb;

  if ( is_a_local_descriptor ( & ( r->desc ) ) )
    {
      a->mask = DESCRIPTOR_IS_LOCAL;
      memcpy ( & ( a->desc ), & ( r->desc ), sizeof ( struct bufr_descriptor ) );
      strcpy ( a->name, "LOCAL DESCRIPTOR" );
      strcpy ( a->unit, "UNKNOWN" );
      if ( r->inc_bits )
        {
          bit_offset = r->bit0 + r->bits + 6 + r->inc_bits * subset;
          // extract inc_bits data
          if ( get_bits_as_uint32_t ( &ival, &has_data, &b->sec4.raw[4], & bit_offset, r->inc_bits ) == 0 )
            {
              sprintf ( b->error, "get_bufr_atom_data_from_compressed_data_ref(): Cannot get associated bits from '%s'\n", r->desc.c );
              return 1;
            }
          a->val = ival + r->ref0;
        }
      else
        a->val = r->ref0;
      return 0;
    }

  // some utils pointers
  tb = & ( b->tables->b );
  i = tb->x_start[r->desc.x] + tb->y_ref[r->desc.x][r->desc.y];

  a->mask = 0;

  // descriptor
  memcpy ( & ( a->desc ), & ( r->desc ), sizeof ( struct bufr_descriptor ) );
  // name
  strcpy ( a->name, r->name );
  //unit
  strcpy ( a->unit, r->unit );
  //scale
  a->escale = r->escale;

  // possible bitmaps
  if ( r->is_bitmaped_by )
    {
      a->is_bitmaped_by = r->is_bitmaped_by;
    }

  if ( r->bitmap_to )
    {
      a->bitmap_to = r->bitmap_to;
    }

  // Possible quality
  if ( r->related_to )
    {
      a->related_to = r->related_to;
    }


  // First we check about string fields
  if ( strstr ( a->unit, "CCITT" ) != NULL )
    {
      if ( r->has_data == 0 )
        {
          a->mask |= DESCRIPTOR_VALUE_MISSING;
          return 0;
        }

      if ( r->inc_bits == 0 )
        {
          // case of all data same, so copy the local ref
          strcpy ( a->cval, r->cref0 );
          a->mask |= DESCRIPTOR_HAVE_STRING_VALUE;
        }
      else
        {
          // we have to extract chars from section data
          // compute the bit_offset
          bit_offset = r->bit0 + r->bits + 6 + r->inc_bits * 8 * subset;
          if ( get_bits_as_char_array ( a->cval, &has_data, &b->sec4.raw[4], & bit_offset, r->inc_bits * 8 ) == 0 )
            {
              sprintf ( b->error, "get_bufr_atom_data_from_compressed_data_ref(): Cannot get uchars from '%s'\n", r->desc.c );
              return 1;
            }
          if ( has_data == 0 )
            {
              a->mask |= DESCRIPTOR_VALUE_MISSING;
            }
          else
            {
              a->mask |= DESCRIPTOR_HAVE_STRING_VALUE;
            }
        }
      return 0;
    }

  // now we check for associated data
  if ( r->is_associated )
    {
      strcpy ( a->name, "Associated value" );
      strcpy ( a->unit, "Associated unit" );
      if ( r->has_data == 0 )
        {
          a->associated = MISSING_INTEGER;
        }
      else
        {
          if ( r->inc_bits == 0 )
            {
              // case of all data same, so copy the local ref
              a->associated = r->ref0;
            }
          else
            {
              // compute the bit_offset
              bit_offset = r->bit0 + r->bits + 6 + r->inc_bits * subset;
              // extract inc_bits data
              if ( get_bits_as_uint32_t ( &ival, &has_data, &b->sec4.raw[4], & bit_offset, r->inc_bits ) == 0 )
                {
                  sprintf ( b->error, "get_bufr_atom_data_from_compressed_data_ref(): Cannot get associated bits from '%s'\n", r->desc.c );
                  return 1;
                }
              // finally get the associated data
              if ( has_data )
                {
                  a->associated = r->ref + ( int32_t ) ( r->ref0 +  ival );
                }
              else
                {
                  a->associated = MISSING_INTEGER;
                }
            }
        }
      return 0;
    }

  // numeric data
  if ( r->has_data == 0 || 
    r->inc_bits > 31 ) // ¿no more than 31 inc_bits for numeric data?
    {
      a->val = MISSING_REAL;
      a->mask |= DESCRIPTOR_VALUE_MISSING;
      return 0;
    }

  if ( r->inc_bits == 0 )
    {
      ivals = r->ref + ( int32_t ) r->ref0;
    }
  else
    {
      // read inc_bits data
      // compute the bit_offset
      bit_offset = r->bit0 + r->bits + 6 + r->inc_bits * subset;
      // extract inc_bits data
      if ( get_bits_as_uint32_t ( &ival0, &has_data, &b->sec4.raw[4], & bit_offset, r->inc_bits ) == 0 )
        {
          sprintf ( b->error, "get_bufr_atom_data_from_compressed_data_ref(): Cannot get %d inc_bits from '%s'\n", r->inc_bits , r->desc.c );
          return 1;
        }

      if ( has_data )
        {
          ivals = r->ref + ( int32_t ) ( r->ref0 + ival0 );
        }
      else
        {
          a->val = MISSING_REAL;
          a->mask |= DESCRIPTOR_VALUE_MISSING;
          return 0;
        }
    }

  // Get a numeric number
  a->val = ( double ) ( ivals ) * exp10 ( ( double ) ( - r->escale ) );

  //printf("ival = %lf\n", a->val);
  if ( strstr ( a->unit, "CODE TABLE" ) == a->unit  || strstr ( a->unit, "Code table" ) == a->unit )
    {
      ival = ( uint32_t ) ( a->val + 0.5 );
      a->mask |= DESCRIPTOR_IS_CODE_TABLE;
      if ( bufrdeco_explained_table_val ( a->ctable, 256, & ( b->tables->c ), & ( tb->item[i].tablec_ref ), & ( a->desc ), ival ) != NULL )
        {
          a->mask |= DESCRIPTOR_HAVE_CODE_TABLE_STRING;
        }
    }
  else if ( strstr ( a->unit,"FLAG" ) == a->unit || strstr ( a->unit,"Flag" ) == a->unit )
    {
      ival = ( uint32_t ) ( a->val + 0.5 );
      a->mask |= DESCRIPTOR_IS_FLAG_TABLE;

      if ( bufrdeco_explained_flag_val ( a->ctable, 256, & ( b->tables->c ), & ( a->desc ), ival, r->bits ) != NULL )
        {
          a->mask |= DESCRIPTOR_HAVE_FLAG_TABLE_STRING;
        }
    }

  return 0;
}

/*!
  \fn int bufr_decode_subset_data_compressed ( struct bufrdeco_subset_sequence_data *s, struct bufrdeco_compressed_data_references *r, struct bufrdeco *b )
  \brief Get data for a given subset in a compressed data bufr
  \param s pointer to a struct \ref bufrdeco_subset_sequence_data where to set the results
  \param r pointer to the struct \ref bufrdeco_compressed_data_references with the info about how and where to get the data
  \param b basic container struct \ref bufrdeco

  Returns 0 if succeeded, 1 otherwise
*/
int bufr_decode_subset_data_compressed ( struct bufrdeco_subset_sequence_data *s, struct bufrdeco_compressed_data_references *r, struct bufrdeco *b )
{
  size_t i; // references index

  // Previous check
  if ( r->refs == NULL || r->nd == 0 )
    {
      sprintf ( b->error, "bufr_decode_subset_data_compressed(): Try to get subset data without previous references\n" );
      return 1;
    }

  // first some clean
  if ( s->nd )
    {
      s->nd = 0;
    }

  // The subset index
  s->ss = b->state.subset;
    
  // then get sequence
  for ( i = 0; i < r->nd; i++ )
    {
      if ( bufrdeco_get_atom_data_from_compressed_data_ref ( & ( s->sequence[s->nd] ) , & ( r->refs[i] ), b->state.subset, b ) )
        return 1;

      if ( r->refs[i].is_associated == 0 )
        {
          if ( s->nd < ( s->dim - 1 ) )
            ( s->nd ) ++;
          else if ( bufrdeco_increase_data_array ( s ) == 0 )
            ( s->nd ) ++;
          else
            {
              sprintf ( b->error, "bufr_decode_subset_data_compressed(): No more bufr_atom_data available. Check BUFR_NMAXSEQ\n" );
              return 1;
            }
        }
      else
        {
          if ( s->nd < ( s->dim - 1 ) )
            ( s->nd ) ++;
          else if ( bufrdeco_increase_data_array ( s ) == 0 )
            ( s->nd ) ++;
          else
            {
              sprintf ( b->error, "bufr_decode_subset_data_compressed(): No more bufr_atom_data available. Check BUFR_NMAXSEQ\n" );
              return 1;
            }
        }
    }
  return 0;
}
