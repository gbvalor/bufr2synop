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
      //printf ( "level=%lu ndesc=%lu\n", l->level, l->ndesc );
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
      if ( bufrdeco_tabled_get_descriptors_array ( l, b, key ) )
        {
          //printf("\n%s\n",b->error);
          return 1; // something went wrong
        }
      //printf ("ndesc=%lu\n",l->ndesc);
    }

  // now we detect sons and go to parse them
  for ( i = 0; i < l->ndesc ; i++ )
    {
      // we search for descriptors with f == 3
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

