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
 \file bufrdeco_tree.c
 \brief This file has the code which parse sequences in a bufr
*/
#include "bufrdeco.h"

/*!
  \fn int get_unexpanded_descriptor_array_from_sec3 ( struct bufr_sequence *s, struct bufr *b )
  \brief Set to a struct \ref bufr_sequence an unexpanded descriptor array from sec3 in a BUFR report
  \param s Pointer to the target struct \ref bufr_sequence
  \param b Pointer to the base struct \ref bufr
*/ 
int get_unexpanded_descriptor_array_from_sec3 ( struct bufr_sequence *s, struct bufr *b )
{
  size_t i;

  // First we copy the array descritors in sec3 as level0
  for ( i = 0; i < b->sec3.ndesc ; i++ )
    {
      memcpy ( & ( s->lseq[i] ), & ( b->sec3.unexpanded[i] ), sizeof ( struct bufr_descriptor ) );
      /*if ( b->sec3.compressed && is_a_delayed_descriptor ( & ( s->lseq[i] ) ) )
        {
          sprintf ( b->error, "get_unexpanded_descriptor_array_from_sec3(): Found a delayed descriptor in a compressed bufr\n" );
          return 1;
        }
      */
    }
  s->ndesc = b->sec3.ndesc;
  return 0;
}


/*!
  \fn int bufrdeco_parse_tree_recursive ( struct bufr *b, struct bufr_sequence *father,  const char *key )
  \brief Parse the descriptor tree in a recursive way
  \param key string with descriptor in form 'FXXYYY'
  \param father pointer to the father struct \ref bufr_sequence
  \param b pointer to the base struct \ref bufr
 
  Returns 0 if success, 1 otherwise
 */
int bufrdeco_parse_tree_recursive ( struct bufr *b, struct bufr_sequence *father,  const char *key )
{
  size_t i, nl;
  struct bufr_sequence *l;

  if ( key == NULL )
    {
      // case first layer
      b->tree->nseq = 1;
      l = & ( b->tree->seq[0] );
      strcpy ( l->key, "000000" );
      l->level = 0;
      l->father = NULL; // This layer is God, it has not father
      // here we get ndesc and lsec array
      if ( get_unexpanded_descriptor_array_from_sec3 ( l, b ) )
        {
          return 1;
        }
      printf ( "level=%lu ndesc=%lu\n", l->level, l->ndesc );
    }
  else
    {
      if ( b->tree->nseq < NMAXSEQ_DESCRIPTORS )
        {
          ( b->tree->nseq ) ++;
        }
      else
        {
          sprintf ( b->error,"bufr_parse_tree_deep(): Reached max number of bufr_sequence. "
                    "Use bigger NMAXSEQ_LAYER \n" );
          return 1;
        }
      nl = b->tree->nseq;
      l = & ( b->tree->seq[nl - 1] );
      strcpy ( l->key, key );
      l->level = father->level + 1;
      l->father = father;
      //printf ("level=%lu ", l->level);
      // here we get ndesc and lsec array from table d
      if ( bufrdeco_tabled_get_descritors_array ( l, b, key ) )
        {
          //printf("\n%s\n",b->error);
          return 1; // something went wrong
        }
      //printf ("ndesc=%lu\n",l->ndesc);
    }

  // now we detect sons and go to parse them
  for ( i = 0; i < l->ndesc ; i++ )
    {
      // we search for descriptors woth f == 3
      if ( l->lseq[i].f != 3 )
        {
          l->sons[i] = NULL;
          continue;
        }
      l->sons[i] = & ( b->tree->seq[b->tree->nseq] );
      // we then recursively parse the son
      if ( bufrdeco_parse_tree_recursive ( b, l, l->lseq[i].c ) )
        {
          return 1;
        }
    }

  // if we are here all gone well
  return 0;
}

/*!
  \fn int bufrdeco_parse_tree ( struct bufr *b )
  \brief Parse the tree of descriptors without expand the replicators
  \param b Pointer to the source struct \ref bufr
*/
int bufrdeco_parse_tree ( struct bufr *b )
{
  // here we start the parse
  return  bufrdeco_parse_tree_recursive ( b, NULL, NULL );
}

/*!
  \fn int bufrdeco_increase_data_array ( struct bufrdeco_subset_sequence_data *s )
  \brief doubles the allocated space for a struct \ref bufrdeco_subset_sequence_data whenever is posible
  \param s pointer to source struct \ref bufrdeco_subset_sequence_data
 
  Return 0 when success, otherwise return 1 and the struct is unmodified
*/
int bufrdeco_increase_data_array ( struct bufrdeco_subset_sequence_data *s )
{
  if ( s->dim < ( BUFR_NMAXSEQ * 8 ) )
    {
      if ( ( s->sequence = ( struct bufr_atom_data * ) realloc ( ( void * ) s->sequence,
                           s->dim * 2 * sizeof ( struct bufr_atom_data ) ) ) == NULL )
        return 1;
      else
        {
          s->dim *= 2;
          return 0;
        }
    }
  else
    return 1;
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
            ( s->nd ) ++;
          else if ( bufrdeco_increase_data_array ( s ) == 0 )
            ( s->nd ) ++;
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
          if ( seq->lseq[i].y != 0 )
            {
              // no delayed
              replicator.ixdel = i;
              replicator.ndesc = seq->lseq[i].x;
              replicator.nloops = seq->lseq[i].y;
              bufrdeco_decode_replicated_subsequence ( s, &replicator, b );
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
              replicator.nloops = ( size_t ) s->sequence[s->nd].val;
              if ( s->nd < s->dim )
                ( s->nd ) ++;
              else if ( bufrdeco_increase_data_array ( s ) == 0 )
                ( s->nd ) ++;
              else
                {

                  sprintf ( b->error, "bufr_decode_data_subset_recursive(): No more bufr_atom_data available. Check BUFR_NMAXSEQ\n" );
                  return 1;
                }
              bufrdeco_decode_replicated_subsequence ( s, &replicator, b );
            }
          i = replicator.ixdel + replicator.ndesc; // update i properly
          break;
        case 2:
          // Case of operator descriptor
          if ( bufrdeco_parse_f2_descriptor ( s, & ( seq->lseq[i] ), b ) )
            return 1;
          if ( s->nd < s->dim )
            ( s->nd ) ++;
          else if ( bufrdeco_increase_data_array ( s ) == 0 )
            ( s->nd ) ++;
          else
            {
              sprintf ( b->error, "bufr_decode_data_subset_recursive(): No more bufr_atom_data available. Check BUFR_NMAXSEQ\n" );
              return 1;
            }
          break;
        case 3:
          // Case of sequence descriptor
          if ( bufrdeco_decode_subset_data_recursive ( s, seq->sons[i], b ) )
            return 1;
          break;
        default:
          // this case is not possible
          sprintf ( b->error, "bufr_decode_data_subset(): Found bad 'f' in descriptor\n" );
          return 1;
          break;
        }
    }
  return 0;
};

/*!
  \fn  int bufrdeco_decode_data_subset ( struct bufrdeco_subset_sequence_data *s, struct bufr *b )
  \brief  User interface to decode a BUFR subset
  \param s pointer to the target struct \ref bufrdeco_subset_sequence_data
  \param b pointer to the base struct \ref bufr
  
  Return 0 in case of success, 1 otherwise
*/
int bufrdeco_decode_data_subset ( struct bufrdeco_subset_sequence_data *s, struct bufr *b )
{
  if ( bufrdeco_init_subset_sequence_data ( s ) )
    return 1;
  if ( bufrdeco_decode_subset_data_recursive ( s, NULL, b ) )
    return 1;

  ( b->state.subset ) ++;
  return 0;
}


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
              //bufr_print_atom_data_stdout(& ( s->sequence[s->nd] ));
              if ( s->nd < s->dim )
                ( s->nd ) ++;
              else if ( bufrdeco_increase_data_array ( s ) == 0 )
                ( s->nd ) ++;
              else
                {
                  sprintf ( b->error, "bufr_decode_data_subset_recursive(): No more bufr_atom_data available. Check BUFR_NMAXSEQ\n" );
                  return 1;
                }
              break;
            case 1:
              // Case of replicator descriptor
              r->s = l;
              r->ixrep = i;
              if ( l->lseq[i].y != 0 )
                {
                  // no delayed
                  r->ixdel = i;
                  r->ndesc = l->lseq[i].x;
                  r->nloops = l->lseq[i].y;
                  bufrdeco_decode_replicated_subsequence ( s, r, b );
                }
              else
                {
                  // case of delayed;
                  r->ixdel = i + 1;
                  r->ndesc = l->lseq[i].x;
                  // here we read ndesc from delayed replicator descriptor
                  if ( bufrdeco_tableb_val ( & ( s->sequence[s->nd] ), b, & ( l->lseq[i + 1] ) ) )
                    {
                      return 1;
                    }
                  r->nloops = ( size_t ) s->sequence[s->nd].val;
                  if ( s->nd < s->dim )
                    ( s->nd ) ++;
                  else if ( bufrdeco_increase_data_array ( s ) == 0 )
                    ( s->nd ) ++;
                  else
                    {
                      sprintf ( b->error, "bufr_decode_data_subset_recursive(): No more bufr_atom_data available. Check BUFR_NMAXSEQ\n" );
                      return 1;
                    }
                  bufrdeco_decode_replicated_subsequence ( s, r, b );
                }
              i = r->ixdel + r->ndesc; // update i properly
              break;
            case 2:
              // Case of operator descriptor
              if ( bufrdeco_parse_f2_descriptor ( s, & ( l->lseq[i] ), b ) )
                return 1;
              if ( s->nd < s->dim )
                ( s->nd ) ++;
              else if ( bufrdeco_increase_data_array ( s ) == 0 )
                ( s->nd ) ++;
              else
                {
                  sprintf ( b->error, "bufr_decode_data_subset_recursive(): No more bufr_atom_data available. Check NMAXSEQ\n" );
                  return 1;
                }
              break;
            case 3:
              // Case of sequence descriptor
              if ( bufrdeco_decode_subset_data_recursive ( s, l->sons[i], b ) )
                return 1;
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
