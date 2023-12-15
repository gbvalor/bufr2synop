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
 \file bufrdeco_memory.c
 \brief This file has the memory stufs for library bufrdeco
*/
#include "bufrdeco.h"

/*!
  \fn int bufrdeco_init_tables ( struct bufr_tables **t )
  \brief Init a struct \ref bufr_tables allocating space
  \param t pointer to the target pointer to struct \ref bufr_tables
  \return 0 if succeeded, 1 otherwise
*/
int bufrdeco_init_tables ( struct bufr_tables **t )
{
  bufrdeco_assert ( t != NULL );
  
  if ( *t != NULL )
    free ( ( void * ) *t );

  if ( ( *t = ( struct bufr_tables * ) calloc ( 1, sizeof ( struct bufr_tables ) ) ) == NULL )
    return 1;
  return 0;
}


/*!
  \fn int bufrdeco_free_tables ( struct bufr_tables **t )
  \brief Frees the allocated space for a struct \ref bufr_tables
  \param t pointer to the target pointer to struct \ref bufr_tables
  \return 0 and \a *t is set to NULL
*/
int bufrdeco_free_tables ( struct bufr_tables **t )
{
  bufrdeco_assert ( t != NULL );

  if ( *t != NULL )
    {
      free ( ( void * ) *t );
      *t = NULL;
    }
  return 0;
}

/*!
  \fn int bufrdeco_init_expanded_tree ( struct bufrdeco_expanded_tree **t )
  \brief Init a struct \ref bufrdeco_expanded_tree allocating space
  \param t pointer to the target pointer to struct \ref bufrdeco_expanded_tree

  \return 0 if succeeded, 1 otherwise
*/
int bufrdeco_init_expanded_tree ( struct bufrdeco_expanded_tree **t )
{
  bufrdeco_assert ( t != NULL );

  if ( *t != NULL )
    free ( ( void * ) *t );

  if ( ( *t = ( struct bufrdeco_expanded_tree * ) calloc ( 1, sizeof ( struct bufrdeco_expanded_tree ) ) ) == NULL )
    return 1;
  return 0;
}

/*!
  \fn int bufrdeco_free_expanded_tree ( struct bufrdeco_expanded_tree **t )
  \brief Frees the allocated space for a struct \ref bufrdeco_expanded_tree
  \param t pointer to the target pointer to struct \ref bufrdeco_expanded_tree

  \return 0 and \a *t is set to NULL
*/
int bufrdeco_free_expanded_tree ( struct bufrdeco_expanded_tree **t )
{
  bufrdeco_assert ( t != NULL );

  if ( *t != NULL )
    {
      free ( ( void * ) *t );
      *t = NULL;
    }
  return 0;
}

/*!
  \fn int bufrdeco_substitute_tables ( struct bufr_tables **replaced, struct bufr_tables *source, struct bufrdeco *b )
  \brief substitute an struct \ref bufr_tables into a struct \ref bufrdeco
  \param replaced  Pointer where to set the replaced pointer
  \param source Pointer to a struct \ref bufr_tables
  \param b pointer to the container basic struct \ref bufrdeco
  \return 0 if succeeded, 1 otherwise

  Remember that the struct \ref bufr_tables used in bufrdeco library is the one which pointer is in struct
  \ref bufrdeco . To avoid problems the struct must be initialized before substituted in this fucntion.
  Both source and replaced structs are not modified.

  This is useful if we do not want to read and parse tables again if the caller has a pool of
  already readed tables.
*/
int bufrdeco_substitute_tables ( struct bufr_tables **replaced, struct bufr_tables *source, struct bufrdeco *b )
{
  bufrdeco_assert ( b != NULL );
  
  *replaced = b->tables;
  if ( source == NULL )
    {
      // allocate memory for table
      return bufrdeco_init_tables ( & ( b->tables ) );
    }
  else
    b->tables = source;
  return 0;
}

/*!
   \fn int bufrdeco_init_subset_sequence_data ( struct bufrdeco_subset_sequence_data *ba )
   \brief Init a struct \ref bufrdeco_subset_sequence_data
   \param ba pointer to the target struct
   \return 0 if succeeded, 1 otherwise

   It is supossed that no memory is allocated for sequence. If we are not sure better use
   function \ref bufrdeco_clean_subset_sequence_data
*/
int bufrdeco_init_subset_sequence_data ( struct bufrdeco_subset_sequence_data *ba )
{
  bufrdeco_assert ( ba != NULL );
  
  memset ( ba, 0, sizeof ( struct bufrdeco_subset_sequence_data ) );
  if ( ( ba->sequence = ( struct bufr_atom_data * ) calloc ( 1, BUFR_NMAXSEQ * sizeof ( struct bufr_atom_data ) ) ) == NULL )
    {
      fprintf ( stderr,"%s():Cannot allocate memory for atom data array\n", __func__ );
      return 1;
    }
  ba->dim = BUFR_NMAXSEQ;
  return 0;
}

/*!
  \fn int bufrdeco_clean_subset_sequence_data ( struct bufrdeco_subset_sequence_data *ba )
  \brief Cleans a struct \ref bufrdeco_subset_sequence_data
  \param ba Pointer to struct \ref bufrdeco_subset_sequence_data to clean
  \return 0 if succeeded, 1 otherwise

  For eficience, if sequence in the struct \ref bufrdeco_subset_sequence_data is allocated, just set the used
  elements to zero. If is still no allocated memory for sequence inits the struct

*/
int bufrdeco_clean_subset_sequence_data ( struct bufrdeco_subset_sequence_data *ba )
{
  bufrdeco_assert ( ba != NULL );
  
  if ( ba->sequence != NULL )
    {
      ba->nd = 0;
      return 0;
    }
  else
    return bufrdeco_init_subset_sequence_data ( ba );
}

/*!
 \fn  int bufrdeco_free_subset_sequence_data ( struct bufrdeco_subset_sequence_data *ba )
 \brief Free the memory for sequence array in a struct \ref bufrdeco_subset_sequence_data
 \param ba pointer to the target struct to free
 \return 0 if succeeded
*/
int bufrdeco_free_subset_sequence_data ( struct bufrdeco_subset_sequence_data *ba )
{
  bufrdeco_assert ( ba != NULL );
  
  if ( ba->sequence != NULL )
    {
      free ( ( void * ) ba->sequence );
      ba->sequence = NULL;
    }
  return 0;
}

/*!
  \fn int bufrdeco_increase_data_array ( struct bufrdeco_subset_sequence_data *d )
  \brief doubles the allocated space for a struct \ref bufrdeco_subset_sequence_data whenever is posible
  \param d pointer to source struct \ref bufrdeco_subset_sequence_data
  \return 0 when success, otherwise return 1 and the struct is unmodified

  The amount of data in a bufr must be huge. In a first moment, the dimension of a sequence of structs
  \ref bufr_atom_data is \ref BUFR_NMAXSEQ but may be increased. This function task is try to double the
  allocated dimension and reallocate it.

*/
int bufrdeco_increase_data_array ( struct bufrdeco_subset_sequence_data *d )
{
  bufrdeco_assert ( d != NULL );

  if ( d->dim < ( BUFR_NMAXSEQ * 16 ) ) // check if reached the limit
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
 * \fn int bufrdeco_increase_subset_sequence_data_count ( struct bufrdeco_subset_sequence_data *d, struct bufrdeco *b )
 * \brief Increment the count of a struct \ref bufrdeco_subset_sequence_data
 * \param d pointer to the target struct
 * \param b pointer to the current active struct \ref bufrdeco
 *
 * \return 0 if succeeded, 1 otherwise
 */
int bufrdeco_increase_subset_sequence_data_count ( struct bufrdeco_subset_sequence_data *d, struct bufrdeco *b )
{
  if ( d->nd < ( d->dim - 1 ) )
    {
      d->nd += 1;
      return 0;
    }
  else if ( bufrdeco_increase_data_array ( d ) )
    {
      snprintf ( b->error, sizeof ( b->error ), "%s(): Reached limit. Consider increase BUFR_NMAXSEQ\n", __func__ );
      return 1;
    }

  // increase now
  d->nd += 1;
  return 0;
}

/*!
  \fn int bufrdeco_init_compressed_data_references ( struct bufrdeco_compressed_data_references *rf )
  \brief Init a struct bufrdeco_compressed_data_references
  \param rf pointer ti the target struct
  \return 0 if succeded, otherwise 1

  If already memory is allocated for array of references then just adjust the used index to zero. Otherwise
  it allocate the needed memory and init the struct

*/
int bufrdeco_init_compressed_data_references ( struct bufrdeco_compressed_data_references *rf )
{
  bufrdeco_assert ( rf != NULL );
  
  if ( rf->refs != NULL && rf->dim != 0 )
    {
      rf->nd = 0; // Here we set the used elements to 0 of dim
    }
  else if ( rf->refs == NULL )
    {
      // Here memory is still not allocated. Proceed to allocate with BUFR_NMAXSEQ
      if ( ( rf->refs = ( struct bufrdeco_compressed_ref * ) calloc ( 1, BUFR_NMAXSEQ * sizeof ( struct bufrdeco_compressed_ref ) ) ) == NULL )
        {
          fprintf ( stderr,"%s(): Cannot allocate memory for bufrdeco_compressed_ref array\n", __func__ );
          return 1;
        }
      rf->nd = 0; // Set de used elements (bufrdeco_compressed_ref)
      rf->dim = BUFR_NMAXSEQ; // Set de allocated bufr_compressed_rer elements
    }
  return 0;
}

/*!
  \fn int bufrdeco_clean_compressed_data_references ( struct bufrdeco_compressed_data_references *rf )
  \brief Clean a struct \ref bufrdeco_compressed_data_references
  \param rf pointer to the target struct \ref bufrdeco_compressed_data_references to clean
  \return If succeeded return 0, otherwise 1
*/
int bufrdeco_clean_compressed_data_references ( struct bufrdeco_compressed_data_references *rf )
{
  bufrdeco_assert ( rf != NULL );
  
  if ( rf->refs != NULL && rf->nd != 0 )
    rf->nd = 0;
  else
    return bufrdeco_init_compressed_data_references ( rf );
  return 0;
}

/*!
  \fn int bufrdeco_free_compressed_data_references ( struct bufrdeco_compressed_data_references *rf )
  \brief Free the memory allocated for array of references in a struct \ref bufrdeco_compressed_data_references
  \param rf pointer to the target struct \ref bufrdeco_compressed_data_references to free
  \return If succeeded return 0, otherwise 1
*/
int bufrdeco_free_compressed_data_references ( struct bufrdeco_compressed_data_references *rf )
{
  bufrdeco_assert ( rf != NULL );
  
  if ( rf->refs != NULL )
    {
      free ( ( void * ) rf->refs );
      rf->refs = NULL;
    }
  return 0;
}

/*!
  \fn int bufrdeco_increase_compressed_ref_array ( struct bufrdeco_compressed_data_references *r )
  \brief doubles the allocated space for a struct \ref bufrdeco_compressed_data_references whenever is posible
  \param r pointer to source struct \ref bufrdeco_compressed_data_references
  \return 0 when success, otherwise return 1 and the struct is unmodified

  The amount of data in a bufr must be huge. In a first moment, the dimension of a sequence of structs
  \ref bufr_atom_data is \ref BUFR_NMAXSEQ but may be increased. This function task is try to double the
  allocated dimension and reallocate it.
*/
int bufrdeco_increase_compressed_ref_array ( struct bufrdeco_compressed_data_references *r )
{
  bufrdeco_assert ( r != NULL );

  if ( r->dim < ( BUFR_NMAXSEQ * 16 ) ) // check if reached the limit
    {
      if ( ( r->refs = ( struct bufrdeco_compressed_ref * ) realloc ( ( void * ) r->refs,
                           r->dim * 2 * sizeof ( struct bufrdeco_compressed_ref ) ) ) == NULL )
        {
          return 1;
        }
      else
        {
          r->dim *= 2;
          return 0;
        }
    }
  else
    {
      return 1;
    }
}

/*!
 * \fn int bufrdeco_increase_compressed_data_references_count ( struct bufrdeco_compressed_data_references *r, struct bufrdeco *b )
 * \brief Increment the count of a struct \ref bufrdeco_compressed_data_references
 * \param r pointer to the target struct
 * \param b pointer to the current active struct \ref bufrdeco
 *
 * \return 0 if succeeded, 1 otherwise
 */
int bufrdeco_increase_compressed_data_references_count ( struct bufrdeco_compressed_data_references *r, struct bufrdeco *b )
{
  if ( r->nd < ( r->dim - 1 ) )
    {
      r->nd += 1;
      return 0;
    }
  else if ( bufrdeco_increase_compressed_ref_array ( r ) )
    {
      snprintf ( b->error, sizeof ( b->error ), "%s(): Reached limit. Consider increase BUFR_NMAXSEQ\n", __func__ );
      return 1;
    }

  // increase now
  r->nd += 1;
  return 0;
}


/*!
 * \fn int bufrdeco_allocate_bitmap ( struct bufrdeco *b )
 * \brief allocate bitmap
 * \param b the active struct \ref bufrdeco
 * \return If succeeded return 0, otherwise 1
 */
int bufrdeco_allocate_bitmap ( struct bufrdeco *b )
{
  buf_t nba;
  
  bufrdeco_assert ( b != NULL );
    
  nba = b->bitmap.nba;

  if ( nba < BUFR_MAX_BITMAPS )
    {
      if ( b->bitmap.bmap[nba] != NULL )
        {
          // the bitmap already is allocated
          return 0;
        }
      // let's try to allocate it!
      if ( ( b->bitmap.bmap[nba] = ( struct bufrdeco_bitmap * ) calloc ( 1, sizeof ( struct bufrdeco_bitmap ) ) ) == NULL )
        {
          snprintf ( b->error, sizeof ( b->error ),"%s(): Cannot allocate space for struct bufrdeco_bitmap\n",  __func__ );
          return 1;
        }
      // Update de counter
      ( b->bitmap.nba )++;
      return 0;
    }
  else
    {
      snprintf ( b->error, sizeof ( b->error ),"%s(): Too much bitmaps to allocate. The limit is %d\n", __func__, BUFR_MAX_BITMAPS );
      return 1;
    }
}


/*!
 *  \fn int bufrdeco_clean_bitmaps ( struct bufrdeco *b )
 *  \brief Clean all allocated bitmaps, but still is in memory
 * \param b the active struct \ref bufrdeco
 * \return If succeeded return 0, otherwise 1
 */
int bufrdeco_clean_bitmaps ( struct bufrdeco *b )
{
  buf_t i;

  bufrdeco_assert ( b != NULL );
  
  for ( i = 0; i < b->bitmap.nba ; i++ )
    {
      if ( b->bitmap.bmap[i] == NULL )
        continue;
      memset ( b->bitmap.bmap[i], 0, sizeof ( struct bufrdeco_bitmap ) );
    }
  b->bitmap.nba = 0;
  return 0;
}


/*!
 * \fn int bufrdeco_free_bitmap_array ( struct bufrdeco_bitmap_array *a )
 * \brief Free an allocated bitmap array
 * \param a pointer to target struct \ref bufrdeco_bitmap_array to free
 * \return If succeeded return 0, otherwise 1
 */
int bufrdeco_free_bitmap_array ( struct bufrdeco_bitmap_array *a )
{
  buf_t i;

  bufrdeco_assert ( a != NULL );
  
  for ( i = 0; i < a->nba ; i++ )
    {
      if ( a->bmap[i] == NULL )
        continue;

      free ( ( void* ) a->bmap[i] );
      a->bmap[i] = NULL;
    }
  a->nba = 0;
  return 0;
}

/*!
 *  \fn int bufrdeco_add_event_to_bitacora ( struct bufrdeco *b )
 *  \brief Add an element into a struct \ref bufrdeco_compressed_data_registry 
 * \param b pointer to the current active struct \ref bufrdeco
 *
 * \return 0 if succeeded, 1 otherwise
 *   
 */
int bufrdeco_add_event_to_bitacora ( struct bufrdeco *b, struct bufrdeco_decode_subset_event *event )
{
  if ( b->bitacora.nd < b->bitacora.dim - 1 )
    {
      // Copy the source event to array
      memcpy (&(b->bitacora.event[b->bitacora.nd]), event, sizeof (struct bufrdeco_decode_subset_event ) );
#ifdef __DEBUG      
      bufrdeco_print_event(event, b);
#endif
      ( b->bitacora.nd )++;
      return 0;
    }
  else if ( bufrdeco_increase_decode_subset_bitacora_array( &(b->bitacora) ) )
    {
      snprintf ( b->error, sizeof ( b->error ), "%s(): Reached limit. Consider increas BUFR_NMAXSEQ\n", __func__ );
      return 1;
    }
    
  // add now  
  memcpy (&(b->bitacora.event[b->bitacora.nd]), event, sizeof (struct bufrdeco_decode_subset_event ) );
  ( b->bitacora.nd )++;
   return 0;
}

/*!
 *  \fn int bufrdeco_init_subset_bitacora ( struct bufrdeco *b)
 *  \brief Init struct \ref bufrdeco_decode_subset_bitacora 
 *  \param active struct \ref bufrdeco
 *
 *  \return if succeeded return 0. Exit with failure otherwise 
 */
int bufrdeco_init_subset_bitacora ( struct bufrdeco *b)
{
  bufrdeco_assert ( b != NULL );
  
  if ( b->bitacora.event != NULL && b->bitacora.dim != 0 )
    {
      b->bitacora.nd = 0; // Here we set the used elements to 0 of dim
    }
  else if ( b->bitacora.event == NULL )
    {
      // Here memory is still not allocated. Proceed to allocate with BUFR_NMAXSEQ
      if ( ( b->bitacora.event = ( struct bufrdeco_decode_subset_event * ) calloc ( 1, BUFR_NMAXSEQ * sizeof ( struct bufrdeco_decode_subset_event ) ) ) == NULL )
        {
          fprintf ( b->err,"%s(): Cannot allocate memory for bufrdeco_decode_subset_bitacora array\n", __func__ );
          return 1;
        }
      b->bitacora.nd = 0; // Set de used elements 
      b->bitacora.dim = BUFR_NMAXSEQ; // Set de allocated buf_t elements in index array
    }
  return 0;
}

/*!
  \fn int bufrdeco_increase_subset_bitacora_array ( struct bufrdeco_deocde_subset_bitacora *dsb )
  \brief doubles the allocated space for a struct \ref bufrdeco_decode_subset_bitacora whenever is posible
  \param dsb pointer to source struct \ref bufrdeco_decode_subset_bitacora
  \return 0 when success, otherwise return 1 and the struct is unmodified

  The amount of data in a bufr must be huge. In a first moment, the dimension of a sequence of structs
  \ref bufrdeco_decode_subset_bitacora is \ref BUFR_NMAXSEQ but may be increased. This function task is try to double the
  allocated dimension and reallocate it.
*/
int bufrdeco_increase_decode_subset_bitacora_array ( struct bufrdeco_decode_subset_bitacora *dsb )
{
  bufrdeco_assert ( dsb != NULL );

  if ( dsb->dim < ( BUFR_NMAXSEQ * 16 ) ) // check if reached the limit
    {
      if ( ( dsb->event = ( struct bufrdeco_decode_subset_event * ) realloc ( ( void * ) dsb->event,
                           dsb->dim * 2 * sizeof ( struct bufrdeco_decode_subset_event ) ) ) == NULL )
        {
          return 1;
        }
      else
        {
          dsb->dim *= 2;
          return 0;
        }
    }
  else
    {
      return 1;
    }
}

/*!
  \fn int bufrdeco_free_subset_data_registry ( struct bufrdeco_subset_data_registry *sdr )
  \brief Free the memory allocated for array of indexes \ref bufrdeco_subset_data_registry
  \param rf pointer to the target struct \ref bufrdeco_subset_data_registry to free
  \return If succeeded return 0, otherwise 1
*/
int bufrdeco_free_decode_subset_bitacora ( struct bufrdeco_decode_subset_bitacora *dsb )
{
  bufrdeco_assert ( dsb != NULL );
  
  if ( dsb->event != NULL )
    {
      free ( ( void * ) dsb->event );
      dsb->event = NULL;
    }
  return 0;
}

/*! \fn int bufrdeco_pop_associated_field (struct bufrdeco_associated_field *popped, struct bufrdeco_associated_field_stack *afs )
 *  \brief pop a struct \ref bufrdeco_associated_field into a struct \ref bufrdeco_associated_field_stack 
 *  \param popped pointer to the struct where the popped struct is moved
 *  \param afs pointer to the target struct \ref bufrdeco_associated_field_stack
 *  \return If succeeded return 0, otherwise 1
 */
int bufrdeco_pop_associated_field (struct bufrdeco_associated_field *popped, struct bufrdeco_associated_field_stack *afs )
{
  
  bufrdeco_assert (afs != NULL && popped != NULL );
  if ( afs->nd == 0)
    return 1; // cannot pop associated field
    
  // copy the struct  
  memcpy (popped, &(afs->afield[afs->nd -1]), sizeof (struct bufrdeco_associated_field ));
  // clear in the stack
  memset (&afs->afield[afs->nd - 1], 0, sizeof (struct bufrdeco_associated_field ));
  // set the count
  (afs->nd)--;
  return 0;
}

/*! \fn int bufrdeco_push_associated_field (struct bufrdeco_associated_field *pushed, struct bufrdeco_associated_field_stack *afs )
 *  \brief push a struct \ref bufrdeco_associated_field into a struct \ref bufrdeco_associated_field_stack 
 *  \param pushed pointer to the struct to push
 *  \param afs pointer to the target struct \ref bufrdeco_associated_field_stack
 *  \return If succeeded return 0, otherwise 1
 */
int bufrdeco_push_associated_field (const struct bufrdeco_associated_field *pushed, struct bufrdeco_associated_field_stack *afs )
{
  bufrdeco_assert (afs != NULL && pushed != NULL );

  if ( afs->nd == BUFRDECO_MAX_ASSOCIATED_FIELD_STACK )
    return 1; // cannot pop associated field
    
  // copy the struct  
  memcpy (&(afs->afield[afs->nd]), pushed, sizeof (struct bufrdeco_associated_field ));
  // set the count
  (afs->nd)++;
  return 0;
}

/*! \fn int bufrdeco_add_associated_field (struct bufrdeco_associated_field *pushed, struct bufrdeco_associated_field_stack *afa )
 *  \brief add a struct \ref bufrdeco_associated_field into a struct \ref bufrdeco_associated_field_array 
 *  \param pushed pointer to the struct to push
 *  \param afa pointer to the target struct \ref bufrdeco_associated_field_array
 *  \return If succeeded return 0, otherwise 1
 */
int bufrdeco_add_associated_field (const struct bufrdeco_associated_field *added, struct bufrdeco_associated_field_array *afa )
{
  bufrdeco_assert (afa != NULL && added != NULL );

  if ( afa->nd == (BUFRDECO_MAX_ASSOCIATED_FIELD_STACK * 4) )
    return 1; // cannot pop associated field
    
  // copy the struct  
  memcpy (&(afa->afield[afa->nd]), added, sizeof (struct bufrdeco_associated_field ));
  // set the count
  (afa->nd)++;
  return 0;
}
