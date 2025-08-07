/***************************************************************************
 *   Copyright (C) 2013-2024 by Guillermo Ballester Valor                  *
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
  \brief Preliminary parse of a compressed data bufr
  \param r pointer to a struct \ref bufrdeco_compressed_data_references where to set the results
  \param b basic container struct \ref bufrdeco
  \return Returns 0 if succeeded, 1 otherwise

  When a bufr report has compressed data, it is needed to do a first parse step to get references about
  where to find the data for every descriptor in subsets. This is what this function does. If succeeded
  the struct \a r will have all needed data to decode individual subsets.

  Also be mind that the descriptors tree have to be already parsed when calling this function

*/
int bufrdeco_parse_compressed ( struct bufrdeco_compressed_data_references *r, struct bufrdeco *b )
{

  // Check about parsed tree
  if ( b->tree == NULL || b->tree->nseq == 0 )
    {
      snprintf ( b->error, sizeof ( b->error ),"%s(): Try to parse compressed data without parsed tree\n", __func__ );
      return 1; //fatal error, no parsed tree
    }

  // first we assure that needed memory is allocated and array of bufr_compressed references initizalized
  if ( bufrdeco_init_compressed_data_references ( r ) )
    {
      return 1; // Something went wrong
    }

  // Then we make the parsing task in a recursive way. NULL pointer says it is the begining.
  // The NULL argument is because we begin the compresed Preliminary parse tasks
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
  \return Returns 0 if succeeded, 1 otherwise
*/
int bufrdeco_parse_compressed_recursive ( struct bufrdeco_compressed_data_references *r, struct bufr_sequence *l, struct bufrdeco *b )
{
  buf_t i, j, k;
  struct bufr_sequence *seq; // auxiliar pointer
  struct bufrdeco_compressed_ref *rf;
  struct bufr_replicator replicator;
  const struct bufrdeco_decode_subset_bitacora *dsb;
  struct bufrdeco_decode_subset_event event;
  struct bufr_tableB *tb;


  // Check arguments
  if ( b == NULL )
    return 1;

  if ( r == NULL )
    {
      snprintf ( b->error, sizeof ( b->error ), "%s(): Unspected NULL argument\n", __func__ );
      return 1;
    }

  if ( l == NULL )
    {
      // inits the array. This is the begining of preliminary parse
      // At the moment there are no structs bufrdeco_compressed_ref used
      r->nd = 0;
      // set the auxiliar pointer at the begining of array of sequences of bufrdeco_expanded_tree
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
      b->state.subs_active = 0;
      b->state.retained_active = 0;
      b->state.stat1_active = 0;
      b->state.dstat_active = 0;
      b->state.bitmaping = 0;
      b->state.bitmap = NULL;

    }
  else
    {
      // This is the case when layer of sequence is no 0, i.e. has been called recursively
      // We just set the auxiliar index
      seq = l;
    }

  // to write easily
  dsb = &b->bitacora;

  // Mark the init of a sequence.
  // Set the event to mark the init of a sequence
  memset ( &event, 0, sizeof ( struct bufrdeco_decode_subset_event ) );
  event.mask =  BUFRDECO_EVENT_SEQUENCE_INIT_BITMASK;
  event.ref_index = -1; // No data, just the init of a sequence
  event.pointer = seq; // The sequence
  event.iaux[0] = seq->iseq; // The bufr_sequence index in expanded tree
  if ( bufrdeco_add_event_to_bitacora ( b, &event ) )
    return 1;

  // to write easily
  rf = & ( r->refs[r->nd] );
  rf->seq = seq;

  // loop for a sequence
  for ( i = 0; i < seq->ndesc ; i++ )
    {
      // switch according to 'f' of the descriptor of sequence
      switch ( seq->lseq[i].f )
        {

        case 0:

          // clean de event
          memset ( &event, 0, sizeof ( struct bufrdeco_decode_subset_event ) );

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

          // Check about associated bit fields already defined
          //
          // Note that the associated fields are not prefixed onto the data described by 0 31 YYY
          // descriptor. This is a general rule: none of the Table C operators are applied to any of the
          // Table B, Class 31 Data description operator qualifiers.

          if ( b->state.associated.nd && seq->lseq[i].x != 31 )
            {
              //j = b->state.associated.nd;
              for ( j = 1; j <= b->state.associated.nd ; j++ )
                {
                  rf = & ( r->refs[r->nd] );
                  k = b->state.associated.afield[j - 1].index + 1; // index in array of associated fields which currently is position j in stack (plus one)
                  if ( bufrdeco_tableB_compressed ( rf, b, & ( seq->lseq[i] ), k ) )
                    {
                      return 1; // case of error
                    }
                  // Set the event to mark the data descriptor
                  event.mask |=  BUFRDECO_EVENT_DATA_DESCRIPTOR_BITMASK ;
                  event.mask |=  BUFRDECO_EVENT_DATA_ASSOCIATED_BITMASK ;
                  event.ref_index = r->nd;
                  event.pointer = & ( seq->lseq[i] ); // The descriptor with data
                  event.iaux[0] = seq->iseq; // The bufr_sequence index in expanded tree
                  event.iaux[6] = k; // The index of associated in array (plus one)
                  rf->bitac = dsb->nd;     // set the bitacora index in ref
                  rf->associated_to = r->nd + b->state.associated.nd - j + 1; // mark wich reference is associated
                  rf->seq = seq;

                  if ( bufrdeco_add_event_to_bitacora ( b, &event ) )
                    return 1;

                  // associated field read with success, set again the used elements of r
                  if ( bufrdeco_increase_compressed_data_references_count ( r, b ) )
                    return 1;
                }
            }

          // Get data from table B
          // Then the the data itself
          rf = & ( r->refs[r->nd] ); // pointer to the target struct bufrdeco_compressed ref. To read/write code easily
          if ( bufrdeco_tableB_compressed ( rf, b, & ( seq->lseq[i] ), 0 ) )
            {
              return 1;
            }

          //case of defining an associated field meaning (operator 0 31 021)
          if ( b->state.associated.nd &&
               seq->lseq[i].x == 31 && seq->lseq[i].y == 21 )
            {
              // Set the extracted val to
              b->state.associated.afield[b->state.associated.nd - 1].val = rf->ref0;
              b->assoc.afield[b->assoc.nd - 1].val = rf->ref0;
              // Get the meaning of associated field
              tb = & ( b->tables->b );
              bufrdeco_explained_table_val ( b->assoc.afield[b->assoc.nd - 1].cval, 256, & ( b->tables->c ),
                                             & ( tb->item[i].tableC_ref ), & ( seq->lseq[i] ), rf->ref0 );
              strcpy ( b->state.associated.afield[b->state.associated.nd - 1].cval, b->assoc.afield[b->assoc.nd - 1].cval );
            }

          //case of first order statistics
          if ( b->state.stat1_active &&
               seq->lseq[i].x == 8 && seq->lseq[i].y == 23 )
            {
              k = b->bitmap.bmap[b->bitmap.nba - 1]->ns1; // index of first statistical var
              b->bitmap.bmap[b->bitmap.nba - 1]->stat1_desc[k] = r->nd; // Set the index of statistical parameter
              // update the number of quality variables for the bitmap
              if ( k < BUFR_MAX_QUALITY_DATA )
                ( b->bitmap.bmap[b->bitmap.nba - 1]->ns1 )++;
              else
                {
                  snprintf ( b->error, sizeof ( b->error ), "%s(): No more space for first order statistic vars in bitmap. Check BUFR_MAX_QUALITY_DATA\n", __func__ );
                  return 1;
                }

              event.mask |= BUFRDECO_EVENT_DATA_FIRST_ORDER_STAT_BITMASK;
              event.iaux[1] = k; // k is the index of first statistical var assiciated to current bitmap
              event.pointer2 = ( b->bitmap.bmap[b->bitmap.nba - 1] ); // pointer2 is the pointer to current bitmap
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
                  snprintf ( b->error, sizeof ( b->error ), "%s(): No more space for difference statistic vars in bitmap. Check BUFR_MAX_QUALITY_DATA\n", __func__ );
                  return 1;
                }
              event.mask |= BUFRDECO_EVENT_DATA_DIFF_STAT_BITMASK;
              event.iaux[1] = k; // k is the index of diff statistical var assiciated to current bitmap
              event.pointer2 = ( b->bitmap.bmap[b->bitmap.nba - 1] ); // pointer2 is the pointer to current bitmap
            }

          // Complete the event to mark the data descriptor
          event.mask |=  BUFRDECO_EVENT_DATA_DESCRIPTOR_BITMASK ;
          event.ref_index = r->nd; // the subset ref index
          event.pointer = & ( seq->lseq[i] ); // The descriptor with data
          event.iaux[0] = seq->iseq; // The bufr_sequence index in expanded tree
          rf->bitac = dsb->nd; // set the bitacora index in ref
          if ( bufrdeco_add_event_to_bitacora ( b, &event ) )
            return 1;

          rf->seq = seq;
          if ( bufrdeco_increase_compressed_data_references_count ( r, b ) )
            return 1;

          break;

        case 1:

          // Set the event to mark the init of a replicator
          memset ( &event, 0, sizeof ( struct bufrdeco_decode_subset_event ) );
          event.mask =  BUFRDECO_EVENT_REPLICATOR_DESCRIPTOR_BITMASK;
          event.ref_index = -1; // No data, just the init of a replicator descriptor
          event.pointer = & ( seq->lseq[i] ); // The descritor sequence
          event.iaux[0] = seq->iseq; // The bufr_sequence index in expanded tree
          event.iaux[3] = seq->lseq[i].x;
          event.iaux[5] = seq->lseq[i].y;
          if ( bufrdeco_add_event_to_bitacora ( b, &event ) )
            return 1;


          rf = & ( r->refs[r->nd] );
          rf->seq = seq;
          rf->desc = & ( seq->lseq[i] );

          memset ( &replicator, 0, sizeof ( struct bufr_replicator ) ); // mr proper
          replicator.s = seq;
          replicator.ixrep = i; // the index of recplicator descriptor in the sequence
          if ( seq->lseq[i].y != 0 )
            {
              // no delayed
              replicator.ixdel = i; // The index of descriptor with info about actual replication factor
              replicator.ndesc = seq->lseq[i].x;
              replicator.nloops = seq->lseq[i].y;

              // Check if this replicator is for a bit-map defining
              if ( b->state.bitmaping )
                {
                  b->state.bitmaping = replicator.nloops; // set it properly
                  b->bitmap.bmap[b->bitmap.nba - 1]->dim = replicator.nloops; // set the dim of bitmap
                }

              // call to decode a replicated subsequence
              bufrdeco_decode_replicated_subsequence_compressed ( r, &replicator, b );

              // and then set again bitamping to 0, because it is finished
              b->state.bitmaping = 0;
            }
          else
            {
              // case of delayed;
              replicator.ixdel = i + 1; // the index of descriptor in sequence with information about the actual amonut of replications
              replicator.ndesc = seq->lseq[i].x;
              //rf = & ( r->refs[r->nd] );

              // The the data itself to get the loops
              if ( bufrdeco_tableB_compressed ( rf, b, & ( seq->lseq[ replicator.ixdel ] ), 0 ) )
                {
                  return 1;
                }

              // Set the event to mark the data descriptor
              event.mask =  BUFRDECO_EVENT_DATA_DESCRIPTOR_BITMASK ;
              event.ref_index = r->nd;
              event.pointer = & ( seq->lseq[i] ); // The descriptor with data
              event.iaux[0] = seq->iseq; // The bufr_sequence index in expanded tree
              event.iaux[5] = rf->ref0;
              rf->bitac = dsb->nd;     // set the bitacora index in ref
              if ( bufrdeco_add_event_to_bitacora ( b, &event ) )
                return 1;

              rf->seq = seq;
              // Set the used elements of bufrdeco_compressed_ref
              if ( bufrdeco_increase_compressed_data_references_count ( r, b ) )
                return 1;

              // Note that is supossed all subsets has the same nlopps,
              // so the inc_bits must be 0 and nloops = ref0
              replicator.nloops = ( size_t ) rf->ref0;

              // Check if this replicator is for a bit-map defining
              if ( b->state.bitmaping )
                {
                  b->state.bitmaping = replicator.nloops; // set it properly
                  b->bitmap.bmap[b->bitmap.nba - 1]->dim = replicator.nloops; // set the dim of bitmap
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

          // clean de event
          memset ( &event, 0, sizeof ( struct bufrdeco_decode_subset_event ) );

          // Set the event to mark the operator descriptor
          {
            event.mask =  BUFRDECO_EVENT_OPERATOR_DESCRIPTOR_BITMASK ;
            event.ref_index = -1;
            event.pointer = & ( seq->lseq[i] ); // The descriptor with data
            event.iaux[0] = seq->iseq; // The bufr_sequence index in expanded tree
            if ( bufrdeco_add_event_to_bitacora ( b, &event ) )
              return 1;
          }

          rf = & ( r->refs[r->nd] );
          rf->seq = seq;
          rf->desc = & ( seq->lseq[i] );
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
          snprintf ( b->error, sizeof ( b->error ), "%s(): Found bad 'f' in descriptor '%s' \n", __func__, seq->lseq[i].c );
          return 1;
          break;
        }
    }

  // Set the event to mark the end of a sequence
  event.mask =  BUFRDECO_EVENT_SEQUENCE_FINAL_BITMASK;
  event.ref_index = -1; // No data, just the final of a sequence
  event.pointer = seq; // The descritor sequence
  event.iaux[0] = seq->iseq; // The bufr_sequence index in expanded tree
  if ( bufrdeco_add_event_to_bitacora ( b, &event ) )
    return 1;

  return 0;
}

/*!
  \fn int bufrdeco_decode_replicated_subsequence_compressed ( struct bufrdeco_compressed_data_references *r, struct bufr_replicator *rep, struct bufrdeco *b )
  \brief decodes a repicated subsequence
  \param r pointer to target struct \ref bufrdeco_compressed_data_references where to set results
  \param rep pointer to a replicator which contains the data for replication
  \param b basic container struct \ref bufrdeco

  \return Returns 0 if succeeded, 1 otherwise
*/
int bufrdeco_decode_replicated_subsequence_compressed ( struct bufrdeco_compressed_data_references *r, struct bufr_replicator *rep, struct bufrdeco *b )
{
  buf_t i, j, k;
  buf_t ixloop; // Index for loop
  buf_t ixd; // Index for descriptor
  struct bufrdeco_compressed_ref *rf;
  struct bufrdeco_decode_subset_bitacora *dsb;
  struct bufrdeco_decode_subset_event event;
  struct bufr_replicator replicator;
  struct bufr_sequence *l; // sequence

  // check arguments
  if ( b == NULL )
    return 1;

  if ( r == NULL || rep == NULL )
    {
      snprintf ( b->error, sizeof ( b->error ), "%s(): Unspected NULL argument(s)\n", __func__ );
      return 1;
    }

   l = rep->s; 
  // if no loops the we save process time and space in data
  if ( rep->nloops == 0 )
    return 0;

  // the registry
  dsb = &b->bitacora;

  // The big loop
  for ( ixloop = 0; ixloop < rep->nloops; ixloop++ )
    {
      for ( ixd = 0; ixd < rep->ndesc ; ixd ++ )
        {
          i = ixd + rep->ixdel + 1;

          // init and common event members
          event.mask = 0;
          event.pointer = & ( l->lseq[i] ); // The descriptor with data
          event.iaux[0] = l->iseq; // The bufr_sequence index in expanded tree
          event.iaux[2] = ixd + 1;
          event.iaux[3] = rep->ndesc;
          event.iaux[4] = ixloop + 1;
          event.iaux[5] = rep->nloops;
          event.iaux[6] = 0;
          event.iaux[7] = 0;
          event.iaux[8] = 0;

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

              // Check about associated bit fields already defined
              //
              // Note that the associated fields are not prefixed onto the data described by 0 31 YYY
              // descriptor. This is a general rule: none of the Table C operators are applied to any of the
              // Table B, Class 31 Data description operator qualifiers.

              if ( b->state.associated.nd && l->lseq[i].x != 31 )
                {
                  //j = b->state.associated.nd;
                  for ( j = 1; j <= b->state.associated.nd ; j++ )
                    {
                      rf = & ( r->refs[r->nd] );
                      k = b->state.associated.afield[j - 1].index + 1; //index in array of associated fields which currently is position j in stack (plus one)
                      if ( bufrdeco_tableB_compressed ( rf, b, & ( l->lseq[i] ), j ) )
                        {
                          return 1; // case of error
                        }
                      // Set the event to mark the data descriptor
                      event.mask |=  BUFRDECO_EVENT_DATA_DESCRIPTOR_BITMASK ;
                      event.mask |=  BUFRDECO_EVENT_DATA_ASSOCIATED_BITMASK ;
                      event.ref_index = r->nd;
                      event.pointer = & ( l->lseq[i] ); // The descriptor with data
                      event.iaux[0] = l->iseq; // The bufr_sequence index in expanded tree
                      event.iaux[6] = k; // The index of associated in array (plus one)
                      rf->bitac = dsb->nd;     // set the bitacora index in ref
                      rf->associated_to = r->nd + b->state.associated.nd - ( j - 1 ); // mark wich reference is associated
                      rf->seq = l;

                      if ( bufrdeco_add_event_to_bitacora ( b, &event ) )
                        return 1;

                      // associated field read with success, set again the used elements of r
                      if ( bufrdeco_increase_compressed_data_references_count ( r, b ) )
                        return 1;
                    }
                }


              // Get data from table B
              rf = & ( r->refs[r->nd] );
              rf->replicated_desc = ixd + 1;
              rf->replicated_loop = ixloop + 1;
              rf->replicated_ndesc = rep->ndesc;
              rf->replicated_nloop = rep->nloops;

              // then the data itself
              if ( bufrdeco_tableB_compressed ( rf, b, & ( l->lseq[i] ), 0 ) )
                {
                  return 1;
                }

              //case of defining an associated field meaning (operator 0 31 021)
              if ( b->state.associated.nd &&
                   l->lseq[i].x == 31 && l->lseq[i].y == 21 )
                {
                  // Set the extracted val to
                  b->state.associated.afield[b->state.associated.nd - 1].val = rf->ref0;
                  b->assoc.afield[b->assoc.nd - 1].val = rf->ref0;
                }


              // Case of defining bitmap 0 31 031
              // Descriptor 0 31 031, used in conjunction with quality control or statistics operators 2 22 YYY through 2 32 YYY,
              // shall indicate the presence of quality control information when the indicator value is set to zero. It may be used
              // in conjunction with the replication operator 1 01 YYY to construct a table of data present/not present indicators,
              // forming a data present bit-map as defined in Regulation 94.5.5.3. This makes it possible to present
              // quality control information and statistical information for selected data corresponding to element descriptors which
              // precede the 2 22 YYY to 2 32 YYY operators.
              if ( l->lseq[i].x == 31 && l->lseq[i].y == 31 && b->state.bitmaping )
                {
                  // Mark the init of bitmap
                  if ( ixloop == 0 )
                    b->bitmap.bmap[b->bitmap.nba - 1]->nb0 = r->nd;

                  if ( rf->ref0 == 0 ) // Assume if ref0 == 0 then all subsets are present in same bits
                    {
                      r->refs[r->nd - b->state.bitmaping].is_bitmaped_by = ( uint32_t ) r->nd;
                      rf->bitmap_to = r->nd - b->state.bitmaping ;
                      bufrdeco_add_to_bitmap ( b->bitmap.bmap[b->bitmap.nba - 1], r->nd - b->state.bitmaping, r->nd );
                    }
                  event.mask |= BUFRDECO_EVENT_DATA_BITMAP_BITMASK;
                }

              // Process quality data
              if ( b->state.quality_active &&
                   l->lseq[i].x == 33 )
                {
                  if ( ixloop == 0 )   // here ixloop is refered to those bits = 0 in the bimap
                    {
                      k = b->bitmap.bmap[b->bitmap.nba - 1]->nq; // index un quality_given_by array to set
                      b->bitmap.bmap[b->bitmap.nba - 1]->quality[k] = r->nd; // Set the value
                      // update the number of quality variables for the bitmap
                      if ( k < BUFR_MAX_QUALITY_DATA )
                        ( b->bitmap.bmap[b->bitmap.nba - 1]->nq )++;
                      else
                        {
                          snprintf ( b->error, sizeof ( b->error ), "%s(): No more space for quality vars in bitmap. Check BUFR_MAX_QUALITY_DATA\n", __func__ );
                          return 1;
                        }
                    }
                  rf->related_to = b->bitmap.bmap[b->bitmap.nba - 1]->bitmap_to[ixloop];
                  event.mask |= BUFRDECO_EVENT_DATA_QUALIFIYER_BITMASK;
                  event.pointer2 = b->bitmap.bmap[b->bitmap.nba - 1];
                  event.iaux[1] = b->bitmap.bmap[b->bitmap.nba - 1]->nq - 1;
                }

              //case of first order statistics
              if ( b->state.stat1_active &&
                   l->lseq[i].x == 8 && l->lseq[i].y == 23 )
                {
                  if ( ixloop == 0 )
                    {
                      k = b->bitmap.bmap[b->bitmap.nba - 1]->ns1; // index un stqts
                      b->bitmap.bmap[b->bitmap.nba - 1]->stat1_desc[k] = r->nd; // Set the value of statistical parameter
                      // the member stat1_expl is the ctable value of descriptor 0 08 023
                      // Should be set when reading data
                      // update the number of quality variables for the bitmap
                      if ( k < BUFR_MAX_QUALITY_DATA )
                        ( b->bitmap.bmap[b->bitmap.nba - 1]->ns1 )++;
                      else
                        {
                          snprintf ( b->error, sizeof ( b->error ), "%s(): No more space for first order statistic vars in bitmap. Check BUFR_MAX_QUALITY_DATA\n", __func__ );
                          return 1;
                        }

                    }
                  rf->related_to = b->bitmap.bmap[b->bitmap.nba - 1]->bitmap_to[ixloop];
                  event.mask |= BUFRDECO_EVENT_DATA_FIRST_ORDER_STAT_BITMASK;
                  event.pointer2 = b->bitmap.bmap[b->bitmap.nba - 1];
                  event.iaux[1] = b->bitmap.bmap[b->bitmap.nba - 1]->ns1 - 1;
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
                          snprintf ( b->error, sizeof ( b->error ), "%s(): No more space for difference statistic vars in bitmap. Check BUFR_MAX_QUALITY_DATA\n", __func__ );
                          return 1;
                        }
                    }
                  rf->related_to = b->bitmap.bmap[b->bitmap.nba - 1]->bitmap_to[ixloop];
                  event.mask |= BUFRDECO_EVENT_DATA_DIFF_STAT_BITMASK;
                  event.pointer2 = b->bitmap.bmap[b->bitmap.nba - 1] ;
                  event.iaux[1] = b->bitmap.bmap[b->bitmap.nba - 1]->nds - 1;
                }

              // Complete the event to mark the data descriptor
              event.mask |=  BUFRDECO_EVENT_DATA_DESCRIPTOR_BITMASK ;
              event.ref_index = r->nd;
              event.pointer = & ( l->lseq[i] ); // The descriptor with data
              event.iaux[0] = l->iseq; // The bufr_sequence index in expanded tree
              rf->bitac = dsb->nd;     // set the bitacora index in ref
              if ( bufrdeco_add_event_to_bitacora ( b, &event ) )
                return 1;


              rf->seq = l;
              // Set the used elements of array in r
              if ( bufrdeco_increase_compressed_data_references_count ( r, b ) )
                return 1;


              break;

            case 1:
              // Case of replicator descriptor
              rf = & ( r->refs[r->nd] );
              rf->seq = l;
              rf->desc = & ( l->lseq[i] );
              rf->replicated_desc = ixd + 1;
              rf->replicated_loop = ixloop + 1;
              rf->replicated_ndesc = rep->ndesc;
              rf->replicated_nloop = rep->nloops;

              // Set the event to mark the init of a replicator
              memset ( &event, 0, sizeof ( struct bufrdeco_decode_subset_event ) );
              event.mask =  BUFRDECO_EVENT_REPLICATOR_DESCRIPTOR_BITMASK;
              event.ref_index = -1; // No data, just the init of a replicator descriptor
              event.pointer = & ( l->lseq[i] ); // The descritor sequence
              event.iaux[0] = l->iseq; // The bufr_sequence index in expanded tree
              event.iaux[3] = l->lseq[i].x;
              event.iaux[5] = l->lseq[i].y;
              if ( bufrdeco_add_event_to_bitacora ( b, &event ) )
                return 1;

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
                  //rf = & ( r->refs[r->nd] );

                  // The data itself to get the loops
                  if ( bufrdeco_tableB_compressed ( rf, b, & ( l->lseq[replicator.ixdel] ), 0 ) )
                    {
                      return 1;
                    }

                  // Set the event to mark the data descriptor
                  event.mask =  BUFRDECO_EVENT_DATA_DESCRIPTOR_BITMASK ;
                  event.ref_index = r->nd;
                  event.pointer = & ( l->lseq[i] ); // The descriptor with data
                  event.iaux[0] = l->iseq; // The bufr_sequence index in expanded tree
                  event.iaux[5] = rf->ref0;
                  rf->bitac = dsb->nd;     // set the bitacora index in ref
                  if ( bufrdeco_add_event_to_bitacora ( b, &event ) )
                    return 1;

                  rf->seq = l;

                  // Set the used elements of bufrdeco_compressed_ref
                  if ( bufrdeco_increase_compressed_data_references_count ( r, b ) )
                    return 1;
                  replicator.nloops = ( size_t ) rf->ref0;

                  bufrdeco_decode_replicated_subsequence_compressed ( r, &replicator, b );
                  ixd += replicator.ndesc + 1; // update ixd properly
                }

              break;

            case 2:
              if ( l->lseq[i].x != 5 &&
                   strcmp ( l->lseq[i].c,"223255" ) &&
                   strcmp ( l->lseq[i].c,"224255" ) &&
                   strcmp ( l->lseq[i].c,"225255" ) &&
                   strcmp ( l->lseq[i].c,"232255" )
                 )
                {
                  rf = & ( r->refs[r->nd] );
                  rf->seq = l;
                  rf->desc = & ( l->lseq[i] );
                  rf->replicated_desc = ixd + 1;
                  rf->replicated_loop = ixloop + 1;
                  rf->replicated_ndesc = rep->ndesc;
                  rf->replicated_nloop = rep->nloops;

                  event.mask =  BUFRDECO_EVENT_OPERATOR_DESCRIPTOR_BITMASK ;
                  event.ref_index = -1;
                  event.pointer = & ( l->lseq[i] ); // The descriptor with data
                  event.iaux[0] = l->iseq; // The bufr_sequence index in expanded tree
                  if ( bufrdeco_add_event_to_bitacora ( b, &event ) )
                    return 1;

                }

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
                  if ( bufrdeco_tableB_compressed ( rf, b, & ( l->lseq[k] ), 0 ) )
                    {
                      return 1;
                    }
                  rf->related_to = k;

                  // Set the event to mark the data descriptor
                  event.mask = ( BUFRDECO_EVENT_DATA_DESCRIPTOR_BITMASK | BUFRDECO_EVENT_OPERATOR_DESCRIPTOR_BITMASK );
                  event.mask |= BUFRDECO_EVENT_DATA_SUBSITUTE_BITMASK;
                  event.ref_index = r->nd;
                  event.pointer = & ( l->lseq[i] ); // The descriptor with data
                  event.pointer2 =  b->bitmap.bmap[b->bitmap.nba - 1];
                  event.iaux[0] = l->iseq; // The bufr_sequence index in expanded tree
                  rf->bitac = b->bitacora.nd;     // set the bitacora index in ref
                  rf->me = r->nd;
                  if ( bufrdeco_add_event_to_bitacora ( b, &event ) )
                    return 1;

                  rf->seq = l;
                  rf->desc = & ( l->lseq[i] );
                  if ( bufrdeco_increase_compressed_data_references_count ( r, b ) )
                    return 1;
                }

              // Case of replaced/retained values
              if ( b->state.retained_active && l->lseq[i].x == 32 && l->lseq[i].y == 255 )
                {
                  // Get the bitmaped descriptor k
                  k = b->bitmap.bmap[b->bitmap.nba - 1]->bitmap_to[ixloop];
                  rf = & ( r->refs[r->nd] );
                  if ( ixloop == 0 )
                    {
                      b->bitmap.bmap[b->bitmap.nba - 1]->retain = r->nd;
                    }
                  if ( bufrdeco_tableB_compressed ( rf, b, & ( l->lseq[k] ), 0 ) )
                    {
                      return 1;
                    }

                  rf->related_to = k;

                  // Set the event to mark the data descriptor
                  event.mask = ( BUFRDECO_EVENT_DATA_DESCRIPTOR_BITMASK | BUFRDECO_EVENT_OPERATOR_DESCRIPTOR_BITMASK );
                  event.mask |= BUFRDECO_EVENT_DATA_REPLACED_BITMASK;
                  event.ref_index = r->nd;
                  event.pointer = & ( l->lseq[i] ); // The descriptor with data
                  event.pointer2 =  b->bitmap.bmap[b->bitmap.nba - 1];
                  event.iaux[0] = l->iseq; // The bufr_sequence index in expanded tree
                  rf->bitac = b->bitacora.nd;     // set the bitacora index in ref
                  rf->me = r->nd;
                  if ( bufrdeco_add_event_to_bitacora ( b, &event ) )
                    return 1;

                  rf->seq = l;
                  rf->desc = & ( l->lseq[i] );
                  if ( bufrdeco_increase_compressed_data_references_count ( r, b ) )
                    return 1;
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
                  // Get the bitmaped descriptor k to which is related
                  k = b->bitmap.bmap[b->bitmap.nba - 1]->bitmap_to[ixloop];
                  rf = & ( r->refs[r->nd] );
                  if ( ixloop == 0 )
                    {
                      b->bitmap.bmap[b->bitmap.nba - 1]->stat1[b->bitmap.bmap[b->bitmap.nba - 1]->ns1 -1] = r->nd;
                    }

                  if ( bufrdeco_tableB_compressed ( rf, b, r->refs[k].desc, 0 ) )
                    {
                      return 1;
                    }

                  rf->related_to = k;

                  // Set the event to mark the data descriptor
                  event.mask = ( BUFRDECO_EVENT_DATA_DESCRIPTOR_BITMASK | BUFRDECO_EVENT_OPERATOR_DESCRIPTOR_BITMASK );
                  event.mask |= ( BUFRDECO_EVENT_DATA_FIRST_ORDER_STAT_BITMASK );
                  event.ref_index = r->nd;
                  event.pointer = & ( l->lseq[i] ); // The descriptor with data
                  event.pointer2 = b->bitmap.bmap[b->bitmap.nba - 1];
                  event.iaux[0] = l->iseq; // The bufr_sequence index in expanded tree
                  event.iaux[1] = b->bitmap.bmap[b->bitmap.nba - 1]->ns1 -1;
                  rf->bitac = b->bitacora.nd;     // set the bitacora index in ref
                  rf->me = r->nd;
                  if ( bufrdeco_add_event_to_bitacora ( b, &event ) )
                    return 1;

                  rf->seq = l;
                  rf->desc = & ( l->lseq[i] );

                  if ( bufrdeco_increase_compressed_data_references_count ( r, b ) )
                    return 1;
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

                  // Get the bitmaped descriptor k
                  k = b->bitmap.bmap[b->bitmap.nba - 1]->bitmap_to[ixloop];
                  rf = & ( r->refs[r->nd] );
                  if ( ixloop == 0 )
                    {
                      b->bitmap.bmap[b->bitmap.nba - 1]->stat1[b->bitmap.bmap[b->bitmap.nba - 1]->nds -1] = r->nd;
                    }

                  if ( bufrdeco_tableB_compressed ( rf, b, r->refs[k].desc, 0 ) )
                    {
                      return 1;
                    }

                  rf->related_to = k;

                  // here is where we change ref and bits
                  rf->ref = - ( ( int32_t ) 1 << rf->bits );
                  rf->bits++;

                  // Set the event to mark the data descriptor
                  event.mask = ( BUFRDECO_EVENT_DATA_DESCRIPTOR_BITMASK | BUFRDECO_EVENT_OPERATOR_DESCRIPTOR_BITMASK );
                  event.mask |= ( BUFRDECO_EVENT_DATA_DIFF_STAT_BITMASK );
                  event.ref_index = r->nd;
                  event.pointer = & ( l->lseq[i] ); // The descriptor with data
                  event.pointer2 = b->bitmap.bmap[b->bitmap.nba - 1];
                  event.iaux[0] = l->iseq; // The bufr_sequence index in expanded tree
                  event.iaux[1] = b->bitmap.bmap[b->bitmap.nba - 1]->nds -1;
                  rf->bitac = b->bitacora.nd;     // set the bitacora index in ref
                  rf->me = r->nd;
                  if ( bufrdeco_add_event_to_bitacora ( b, &event ) )
                    return 1;

                  rf->seq = l;
                  rf->desc = & ( l->lseq[i] );
                  if ( bufrdeco_increase_compressed_data_references_count ( r, b ) )
                    return 1;
                }

              break;

            case 3:
              // Case of sequence descriptor
              if ( bufrdeco_parse_compressed_recursive ( r, l->sons[i], b ) )
                return 1;
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

/*!

  \fn int bufrdeco_get_atom_data_from_compressed_data_ref ( struct bufr_atom_data *a, struct bufrdeco_compressed_ref *r, buf_t subset, struct bufrdeco *b )
  \brief Get atom data from a descriptor for a given subset
  \param a pointer to the target struct \ref bufr_atom_data where to set the results
  \param r pointer to the struct \ref bufrdeco_compressed_ref with the info to know how and where get the data
  \param subset index for solicited subset. First subset has index 0
  \param b basic container struct \ref bufrdeco

  \return Returns 0 if succeeded, 1 otherwise
*/
int bufrdeco_get_atom_data_from_compressed_data_ref ( struct bufr_atom_data *a, struct bufrdeco_compressed_ref *r,
    buf_t subset, struct bufrdeco *b )
{
  buf_t i, j, bit_offset, k;
  uint8_t has_data;
  uint32_t ival, ival0;
  int32_t ivals;
  char aux[8 * BUFR_TABLEB_NAME_LENGTH];
  struct bufr_tableB *tb;
  struct bufrdeco_bitmap *bitmap;

  if ( b == NULL )
    return 1;

  if ( a == NULL || r == NULL )
    {
      snprintf ( b->error, sizeof ( b->error ), "%s(): Unspected NULL argument(s)\n", __func__ );
      return 1;
    }

  // first we set the 'me' member
  a->me = ( b->seq.nd );

  if ( is_a_local_descriptor ( r->desc ) )
    {
      a->mask = DESCRIPTOR_IS_LOCAL;
      memcpy ( & ( a->desc ), r->desc, sizeof ( struct bufr_descriptor ) );
      strcpy ( a->name, "LOCAL DESCRIPTOR" );
      strcpy ( a->unit, "UNKNOWN" );
      if ( r->inc_bits )
        {
          bit_offset = r->bit0 + r->bits + 6 + r->inc_bits * subset;
          // extract inc_bits data
          if ( get_bits_as_uint32_t ( &ival, &has_data, &b->sec4.raw[4], & bit_offset, r->inc_bits ) == 0 )
            {
              snprintf ( b->error, sizeof ( b->error ), "%s(): Cannot get associated bits from '%s'\n", __func__, r->desc->c );
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
  i = tb->x_start[r->desc->x] + tb->y_ref[r->desc->x][r->desc->y];

  a->mask = 0;

  // descriptor
  memcpy ( & ( a->desc ), r->desc, sizeof ( struct bufr_descriptor ) );

  // name
  strcpy ( a->name, r->name );

  // In some cases of bitmap we need to change the name of parameters
  if ( strcmp ( a->desc.c, "224255" ) == 0 )
    {
      // case of first statistics
      bitmap = ( struct bufrdeco_bitmap * ) b->bitacora.event[r->bitac].pointer2;
      j = b->bitacora.event[r->bitac].iaux[1];
      k = bitmap->stat1_desc[j];// k is the data wich define the type or first statistical
      sprintf ( aux,"%s <- %s",bufr_adjust_string ( b->seq.sequence[k].ctable ), bufr_adjust_string ( r->name ) );
      memcpy ( a->name, aux, 127 );
      a->name[127] = '\0';
    }
  else if ( strcmp ( a->desc.c,"225255" ) == 0 )
    {
      // case of diff statistics
      bitmap = ( struct bufrdeco_bitmap * ) b->bitacora.event[r->bitac].pointer2;
      j = b->bitacora.event[r->bitac].iaux[1];
      k = bitmap->dstat_desc[j];// k is the data wich define the type or first statistical
      sprintf ( aux,"%s <- %s",bufr_adjust_string ( b->seq.sequence[k].ctable ), bufr_adjust_string ( r->name ) );
      memcpy ( a->name, aux, 127 );
      a->name[127] = '\0';
    }

  // Case to define what associated field is
  if ( r->is_associated && strcmp ( a->desc.c,"031021" ) )
    {

    }

  //unit
  //strcpy_safe ( a->unit, r->unit );
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
              snprintf ( b->error, sizeof ( b->error ), "%s(): Cannot get uchars from '%s'\n", __func__, r->desc->c );
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
      strcpy ( a->name, "Associated value to next data field" );
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
                  snprintf ( b->error, sizeof ( b->error ), "%s(): Cannot get associated bits from '%s'\n", __func__, r->desc->c );
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
       r->inc_bits > r->bits ) // Here we assume no more inc_bits than bits in reference value?
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
          snprintf ( b->error, sizeof ( b->error ), "%s(): Cannot get %d inc_bits from '%s'\n", __func__, r->inc_bits, r->desc->c );
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
  a->val = ( double ) ( ivals ) * Exp10 ( ( double ) ( - r->escale ) );

  if ( strstr ( a->unit, "CODE TABLE" ) == a->unit  || strstr ( a->unit, "Code table" ) == a->unit )
    {
      ival = ( uint32_t ) ( a->val + 0.5 );
      a->mask |= DESCRIPTOR_IS_CODE_TABLE;
      if ( bufrdeco_explained_table_val ( a->ctable, 256, & ( b->tables->c ), & ( tb->item[i].tableC_ref ), & ( a->desc ), ival ) != NULL )
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
  \return Returns 0 if succeeded, 1 otherwise
*/
int bufr_decode_subset_data_compressed ( struct bufrdeco_subset_sequence_data *s, struct bufrdeco_compressed_data_references *r, struct bufrdeco *b )
{
  size_t i = 0, /*j = 0,*/ k; // references index

  if ( b == NULL )
    return 1;

  // Previous check
  if ( r->refs == NULL || r->nd == 0 )
    {
      snprintf ( b->error, sizeof ( b->error ), "%s(): Try to get subset data without previous references\n", __func__ );
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
  for ( k = 0; k < b->bitacora.nd; k++ )
    {
      if ( b->bitacora.event[k].ref_index >= 0 )
        i = b->bitacora.event[k].ref_index;
      else
        continue;// index in compressed refs

      if ( bufrdeco_get_atom_data_from_compressed_data_ref ( & ( s->sequence[s->nd] ), & ( r->refs[i] ), b->state.subset, b ) )
        return 1;

      if ( r->refs[i].is_associated == 0 )
        {
          if ( s->nd < ( s->dim - 1 ) )
            ( s->nd ) ++;
          else if ( bufrdeco_increase_data_array ( s ) == 0 )
            ( s->nd ) ++;
          else
            {
              snprintf ( b->error, sizeof ( b->error ), "%s(): No more bufr_atom_data available. Check BUFR_NMAXSEQ\n", __func__ );
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
              snprintf ( b->error, sizeof ( b->error ), "%s(): No more bufr_atom_data available. Check BUFR_NMAXSEQ\n", __func__ );
              return 1;
            }
        }
    }

  if ( b->mask & BUFRDECO_OUTPUT_JSON_SUBSET_DATA )
    bufrdeco_print_json_subset_data ( b );
  return 0;
}


