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

int get_unexpanded_descriptor_array_from_sec3 ( struct bufr_sequence *s, struct bufr *b )
{
  size_t i;

  // First we copy the array descritors in sec3 as level0
  for ( i = 0; i < b->sec3.ndesc ; i++ )
    {
      memcpy ( & ( s->lseq[i] ), & ( b->sec3.unexpanded[i] ), sizeof ( struct bufr_descriptor ) );
    }
  s->ndesc = b->sec3.ndesc;
  return 0;
}

int bufr_parse_tree_recursive ( struct bufr *b, struct bufr_sequence *father,  const char *key )
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
      // here we get ndesc and lsec array from table d
      if ( bufrdeco_tabled_get_descritors_array ( l, b, key ) )
        {
          return 1; // something went wrong
        }
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
      if ( bufr_parse_tree_recursive ( b, l, l->lseq[i].c ) )
        {
          return 1;
        }
    }

  // if we are here all gone well
  return 0;
}

int bufr_parse_tree ( struct bufr *b )
{
  // here we start the pars
  bufr_parse_tree_recursive ( b, NULL, NULL );
  return 0;
}

int bufr_print_tree_recursive ( struct bufr *b, struct bufr_sequence *seq )
{
  size_t i, j;
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
          printf ( "        " );
        }
      printf ( "%u %02u %03u\n", l->lseq[i].f, l->lseq[i].x,l->lseq[i].y );

      if ( l->lseq[i].f != 3 )
        {
          continue;
        }

      // we then recursively parse the son
      if ( bufr_print_tree_recursive ( b, l->sons[i] ) )
        {
          return 1;
        }

    }

  return 0;
}

void bufr_print_tree ( struct bufr *b )
{
  bufr_print_tree_recursive ( b, NULL );
};

int bufr_decode_data_subset_recursive ( struct bufr_subset_sequence_data *s, struct bufr_sequence *l, struct bufr *b )
{
  size_t i;
  struct bufr_sequence *seq;
  size_t current = 0;
  struct bufr_descriptor *replicator = NULL;
  struct bufr_descriptor *delayed = NULL;
  size_t last_desc = 0; /*!< last descriptor in sequence affected by replicator loop */
  size_t loops = 0; /*!< number of loops in replicator */
  size_t iteration = 0; /*!< current loop */

  // clean subset data
  if ( l == NULL )
    {
      memset ( s, 0, sizeof ( struct bufr_subset_sequence_data ) );
      seq = & ( b->tree->seq[0] );
      if ( b->state.subset == 0 )
        {
          b->state.bit_offset = 0;
        }
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
          if ( bufrdeco_tableb_val ( & ( s->sequence[s->nd] ), b, l->lseq[i].c ) )
            {
              return 1;
            }
          break;
        case 1:
          // Case of replicator descriptor
	  
          break;
        case 2:
          // Case of operator descriptor
          break;
        case 3:
          // Case of sequence descriptor
          if (bufr_decode_data_subset_recursive ( s, seq->sons[i], b ))
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
