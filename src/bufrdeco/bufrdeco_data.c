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
 \file bufrdeco_data.c
 \brief This file has the code which extract data from a bufr
*/
#include "bufrdeco.h"

/*!
  \fn  int bufrdeco_decode_data_subset ( struct bufrdeco_subset_sequence_data *s, struct bufrdeco_compressed_data_references *r, struct bufr *b )
  \brief  User interface to decode a BUFR subset
  \param s pointer to the target struct \ref bufrdeco_subset_sequence_data
  \param r pointer to the struct \ref bufrdeco_compressed_data_references
  \param b pointer to the base struct \ref bufr

  Return 0 in case of success, 1 otherwise
*/
int bufrdeco_decode_data_subset ( struct bufrdeco_subset_sequence_data *s, struct bufrdeco_compressed_data_references *r,
                                  struct bufr *b )
{
  // First we init the subset sequence data
  if ( bufrdeco_clean_subset_sequence_data ( s ) )
    {
      return 1;
    }

  // Then we get the data from an already parsed descriptor tree
  if ( b->sec3.compressed )
    {
      if ( r->refs == NULL || r->dim == 0 )
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

  Return 0 when success, otherwise return 1 and the struct is unmodified
*/
int bufrdeco_increase_data_array ( struct bufrdeco_subset_sequence_data *s )
{
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
  \fn int bufrdeco_decode_subset_data_recursive ( struct bufrdeco_subset_sequence_data *s, struct bufr_sequence *l, struct bufr *b )
  \brief decode the data from a subset in a recursive way
  \param s pointer to the target struct \ref bufrdeco_subset_sequence_data
  \param l pointer to the source struct \ref bufr_sequence
  \param b pointer to the base struct \ref bufr

  Return 0 in case of success, 1 otherwise
*/
int bufrdeco_decode_subset_data_recursive ( struct bufrdeco_subset_sequence_data *s, struct bufr_sequence *l, struct bufr *b )
{
  size_t i;
  struct bufr_sequence *seq;
  struct bufr_replicator replicator;

  // clean subset data
  if ( l == NULL )
    {
      //memset ( s, 0, sizeof ( struct bufr_subset_sequence_data ) );
      s->nd = 0;
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
          // Get data from table B
          if ( bufrdeco_tableb_val ( & ( s->sequence[s->nd] ), b, & ( seq->lseq[i] ) ) )
            {
              return 1;
            }

          //bufr_print_atom_data_stdout(& ( s->sequence[s->nd] ));
          if ( s->nd < s->dim )
            {
              ( s->nd ) ++;
            }
          else if ( bufrdeco_increase_data_array ( s ) == 0 )
            {
              ( s->nd ) ++;
            }
          else
            {
              sprintf ( b->error, "bufr_decode_data_subset_recursive(): No more bufr_atom_data available. Check BUFR_NMAXSEQ\n" );
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
              bufrdeco_decode_replicated_subsequence ( s, &replicator, b );
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
              replicator.nloops = ( size_t ) ( s->sequence[s->nd].val );
              if ( s->nd < s->dim )
                {
                  ( s->nd ) ++;
                }
              else if ( bufrdeco_increase_data_array ( s ) == 0 )
                {
                  ( s->nd ) ++;
                }
              else
                {

                  sprintf ( b->error, "bufr_decode_data_subset_recursive(): No more bufr_atom_data available. Check BUFR_NMAXSEQ\n" );
                  return 1;
                }
              bufrdeco_decode_replicated_subsequence ( s, &replicator, b );
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
          /*if ( s->nd < s->dim )
            ( s->nd ) ++;
          else if ( bufrdeco_increase_data_array ( s ) == 0 )
            ( s->nd ) ++;
          else
            {
              sprintf ( b->error, "bufr_decode_data_subset_recursive(): No more bufr_atom_data available. Check BUFR_NMAXSEQ\n" );
              return 1;
            }*/
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
          sprintf ( b->error, "bufr_decode_data_subset_recursive(): Found bad 'f' in descriptor\n" );
          return 1;
          break;
        }
    }
  return 0;
};

/*!
  \fn int bufrdeco_decode_replicated_subsequence ( struct bufrdeco_subset_sequence_data *s, struct bufr_replicator *r, struct bufr *b )
  \brief Decodes a replicated sequence
  \param s target struct \ref bufrdeco_subset_sequence_data
  \param r pointer to a struct \ref bufr_replicator which manage the replication task
  \param b pointer to the base struct \ref bufr

  If succeeded return 0, 1 otherwise
*/
int bufrdeco_decode_replicated_subsequence ( struct bufrdeco_subset_sequence_data *s, struct bufr_replicator *r, struct bufr *b )
{
  size_t i;
  size_t ixloop; // Index for loop
  size_t ixd; // Index for descriptor
  struct bufr_sequence *l = r->s; // sequence
  struct bufr_replicator replicator;

  //printf("nloops=%lu, ndesc=%lu\n", r->nloops, r->ndesc);
  for ( ixloop = 0; ixloop < r->nloops; ixloop++ )
    {
      for ( ixd = 0; ixd < r->ndesc ; ixd ++ )
        {
          i = ixd + r->ixdel + 1;
          switch ( l->lseq[i].f )
            {
            case 0:
              // Get data from table B
              if ( bufrdeco_tableb_val ( & ( s->sequence[s->nd] ), b, & ( l->lseq[i] ) ) )
                {
                  return 1;
                }
              //bufrdeco_print_atom_data_stdout(& ( s->sequence[s->nd] ));
              if ( s->nd < s->dim )
                {
                  ( s->nd ) ++;
                }
              else if ( bufrdeco_increase_data_array ( s ) == 0 )
                {
                  ( s->nd ) ++;
                }
              else
                {
                  sprintf ( b->error, "bufr_decode_data_subset_recursive(): No more bufr_atom_data available. Check BUFR_NMAXSEQ\n" );
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
                  if ( s->nd < s->dim )
                    {
                      ( s->nd ) ++;
                    }
                  else if ( bufrdeco_increase_data_array ( s ) == 0 )
                    {
                      ( s->nd ) ++;
                    }
                  else
                    {
                      sprintf ( b->error, "bufr_decode_data_subset_recursive(): No more bufr_atom_data available. Check BUFR_NMAXSEQ\n" );
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
              /*
                  if ( s->nd < s->dim )
                    ( s->nd ) ++;
                  else if ( bufrdeco_increase_data_array ( s ) == 0 )
                    ( s->nd ) ++;
                  else
                    {
                      sprintf ( b->error, "bufr_decode_data_subset_recursive(): No more bufr_atom_data available. Check NMAXSEQ\n" );
                      return 1;
                    }*/
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
              sprintf ( b->error, "bufr_decode_subset_data_recursive(): Found bad 'f' in descriptor\n" );
              return 1;
              break;
            }
        }
    }
  return 0;
}
