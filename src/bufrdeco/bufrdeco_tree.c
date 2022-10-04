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
 \file bufrdeco_tree.c
 \brief This file has the code which parse sequences in a bufr
*/
#include "bufrdeco.h"

/*!
  \fn int get_unexpanded_descriptor_array_from_sec3 ( struct bufr_sequence *s, struct bufrdeco *b )
  \brief Set to a struct \ref bufr_sequence an unexpanded descriptor array from sec3 in a BUFR report
  \param s Pointer to the target struct \ref bufr_sequence
  \param b Pointer to the base struct \ref bufrdeco
  \return If succeded return 0

  Here we get a struct \ref bufr_sequence from the data about descriptors stored in sec3 of a bufr report
*/
int get_unexpanded_descriptor_array_from_sec3 ( struct bufr_sequence *s, struct bufrdeco *b )
{
  buf_t i;

  bufrdeco_assert (b != NULL && s != NULL);
  
  // First we copy the array descriptors in sec3 as level0
  for ( i = 0; i < b->sec3.ndesc ; i++ )
    {
      memcpy ( & ( s->lseq[i] ), & ( b->sec3.unexpanded[i] ), sizeof ( struct bufr_descriptor ) );
    }
  s->ndesc = b->sec3.ndesc;
  return 0;
}


/*!
  \fn int bufrdeco_parse_tree_recursive ( struct bufrdeco *b, struct bufr_sequence *father, buf_t father_idesc, const char *key )
  \brief Parse the descriptor tree in a recursive way
  \param key string with descriptor in form 'FXXYYY'
  \param father pointer to the father struct \ref bufr_sequence
  \param father_idesc index of sequence descriptor in father \ref bufr_sequence which this sequence derived  
  \param b pointer to the base struct \ref bufrdeco
  \return 0 if success, 1 otherwise
 */
int bufrdeco_parse_tree_recursive ( struct bufrdeco *b, struct bufr_sequence *father,  buf_t father_idesc, const char *key )
{
  buf_t i, j, nl;
  struct bufr_sequence *l;
  struct bufr_descriptor *d;

  //bufrdeco_assert (b != NULL);
  
  if ( key == NULL )
    {
      // case first layer
      memset ( b->tree, 0, sizeof ( struct bufrdeco_expanded_tree ) ); //reset memory
      b->tree->nseq = 1; // Set current number of sequences in tree, i.e. 1
      l = & ( b->tree->seq[0] ); // This is to write easily
      strcpy ( l->key, "000000" ); // Key '000000' is the first descriptor of first sequence of level 0
      l->level = 0; // Level 0
      l->father = NULL; // This layer is God, it has not father
      l->father_idesc = father_idesc; // No meaning here
      l->iseq = 0; // first
      strcpy ( l->name, "Main sequence from SEC3" );
      // here we get l->ndesc and l->lsec[] array
      if ( get_unexpanded_descriptor_array_from_sec3 ( l, b ) )
        {
          return 1;
        }
    }
  else
    {
      // First increase nseq counter
      if ( b->tree->nseq < NMAXSEQ_DESCRIPTORS )
        {
          ( b->tree->nseq ) ++;
        }
      else
        {
          // No more bufr_sequence permitted
          snprintf ( b->error, sizeof (b->error),"%s(): Reached max number of bufr_sequence. "
                    "Use bigger NMAXSEQ_LAYER \n" , __func__);
          return 1;
        }
      nl = b->tree->nseq; // To write code easily
      l = & ( b->tree->seq[nl - 1] ); // To write code easily
      strcpy ( l->key, key ); // Set the key of sequence in table d (f == 3)
      l->level = father->level + 1; // level for sequence
      l->father = father; // set the father
      l->father_idesc = father_idesc; // Set the father idesc
      l->iseq = nl - 1; // index of sequence in tree

      // here we get ndesc and lsec array from table d
      if ( bufrdeco_tableD_get_descriptors_array ( l, b, key ) )
        {
          return 1; // something went wrong
        }
    }

  // Set init replication level values from father, copying the one in replicated[0]
  for ( i = 1 ; i < l->ndesc ; i++)
  {
    l->replicated[i] = l->replicated[0];
  }
    
  // Checks for replication levels, no data present, event or conditioning event
  for ( i = 0; i < l->ndesc ; i++ )
    {
      d = & ( l->lseq[i] ); // To write/read code easily

      // Case of replication
      if ( d->f == 1 )
        {
          for ( j = 0; j < d->x; j++ )
            {
              if ( d->y == 0 )
                l->replicated[j + i + 2]++; // a delayed replicator follow
              else
                l->replicated[j + i + 1]++; // No delayed descriptor
            }
        }


      if ( d->f == 2 )
        {
          // check for no data present descriptors
          if ( d->x == 21 &&
               d->y > 0 )
            {
              if ( ( d->y + i ) < l->ndesc )
                {
                  l->no_data_present.active = 1;
                  l->no_data_present.first = i + 1;
                  l->no_data_present.last = i + d->y;

                }
              else
                {
                  // range of no data present out of sequence
                  snprintf ( b->error, sizeof (b->error),"%s(): Range of 'no data present' out of sequence limits\n" , __func__);
                  return 1;
                }
            }

          // Check for event descriptors
          if ( d->x == 41 )
            {
              if ( d->y == 0 && ( i + 1 ) < l->ndesc )
                {
                  l->event.active = 1;
                  l->event.first = i + 1;
                  l->event.last = l->ndesc - 1; // at the moment mark upto final sequence
                }
              else if ( ( d->y == 255 ) &&  l->event.active )
                {
                  l->event.last = i;
                }
              else
                {
                  snprintf ( b->error, sizeof (b->error),"%s(): unexpected or wrong event descriptor\n" , __func__);
                  return 1;
                }
            }

          // Check for conditioning event descriptors
          if ( d->x == 42 )
            {
              if ( d->y == 0 && ( i + 1 ) < l->ndesc )
                {
                  l->cond_event.active = 1;
                  l->cond_event.first = i + 1;
                  l->cond_event.last = l->ndesc - 1; // at the moment mark upto final sequence
                }
              else if ( ( d->y == 255 ) &&  l->cond_event.active )
                {
                  l->cond_event.last = i;
                }
              else
                {
                  snprintf ( b->error, sizeof (b->error),"%s(): unexpected or wrong conditioning event event descriptor\n" , __func__);
                  return 1;
                }
            }

          // Check for categorical forecasts descriptors
          if ( d->x == 43 )
            {
              if ( d->y == 0 && ( i + 1 ) < l->ndesc )
                {
                  l->cat_forecast.active = 1;
                  l->cat_forecast.first = i + 1;
                  l->cat_forecast.last = l->ndesc - 1; // at the moment mark upto final sequence
                }
              else if ( ( d->y == 255 ) &&  l->cat_forecast.active )
                {
                  l->cat_forecast.last = i;
                }
              else
                {
                  snprintf ( b->error, sizeof (b->error),"%s(): unexpected or wrong categorical forecasts descriptor\n" , __func__);
                  return 1;
                }
            }
        }
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
      l->sons[i]->replicated[0] = l->replicated[i]; // This is a trick to pass replication level to sons
      
      // we then recursively parse the son
      if ( bufrdeco_parse_tree_recursive ( b, l, i, l->lseq[i].c ) )
        {
          return 1;
        }
    }

  // if we are here all gone well
  return 0;
}


