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
 \file bufrdeco_memory.c
 \brief This file has the memory stufs for library bufrdeco
*/
#include "bufrdeco.h"

int bufrdeco_init_tables ( struct bufr_tables **t )
{
  if ( *t != NULL )
    free ( ( void * ) *t );

  if ( ( *t = ( struct bufr_tables * ) calloc ( 1, sizeof ( struct bufr_tables ) ) ) == NULL )
    return 1;
  return 0;
}

int bufrdeco_free_tables ( struct bufr_tables **t )
{
  if ( *t != NULL )
    {
      free ( ( void * ) *t );
      *t = NULL;
    }
  return 0;
}

int bufrdeco_init_expanded_tree ( struct bufrdeco_expanded_tree **t )
{
  if ( *t != NULL )
    free ( ( void * ) *t );

  if ( ( *t = ( struct bufrdeco_expanded_tree * ) calloc ( 1, sizeof ( struct bufrdeco_expanded_tree ) ) ) == NULL )
    return 1;
  return 0;
}

int bufrdeco_free_expanded_tree ( struct bufrdeco_expanded_tree **t )
{
  if ( *t != NULL )
    {
      free ( ( void * ) *t );
      *t = NULL;
    }
  return 0;
}
/*

  This is useful for not to read and parse tables again.
*/
int bufr_substitute_tables ( struct bufr_tables **replaced, struct bufr_tables *source, struct bufrdeco *b )
{
  *replaced = b->table;
  if ( source == NULL )
    {
      // allocate memory for table
      return bufrdeco_init_tables ( & ( b->table ) );
    }
  else
    b->table = source;
  return 0;
}

/*!


*/
int init_bufr ( struct bufrdeco *b, struct bufr_tables *t )
{

  memset ( b, 0, sizeof ( struct bufrdeco ) );
  if ( t == NULL )
    {
      if ( bufrdeco_init_tables ( & ( b->table ) ) )
        {
          return 1;
        }
    }
  else
    {
      b->table = t;
    }

// Allocate for a new tree struct
  if ( ( b->tree = ( struct bufrdeco_expanded_tree * ) calloc ( 1, sizeof ( struct bufrdeco_expanded_tree ) ) ) == NULL )
    {
      return 1;
    }
  return 0;
}


/*!

*/
int close_bufr ( struct bufrdeco *b, struct bufr_tables **t )
{
  if ( t != NULL )
    *t = b->table;
  else
    free ( ( void * ) b->table );
  if ( b->tree != NULL )
    free ( ( void * ) b->tree );
  memset ( b, 0, sizeof ( struct bufrdeco ) );
  return 0;
}



int bufrdeco_init_subset_sequence_data ( struct bufrdeco_subset_sequence_data *ba )
{
  memset ( ba, 0, sizeof ( struct bufrdeco_subset_sequence_data ) );
  if ( ( ba->sequence = ( struct bufr_atom_data * ) calloc ( 1, BUFR_NMAXSEQ * sizeof ( struct bufr_atom_data ) ) ) == NULL )
    {
      fprintf ( stderr,"bufr_init_subset_sequence_data():Cannot allocate memory for atom data array\n" );
      return 1;
    }
  ba->dim = BUFR_NMAXSEQ;
  return 0;
}

int bufrdeco_clean_subset_sequence_data ( struct bufrdeco_subset_sequence_data *ba )
{
  if ( ba->sequence != NULL)
  {
    ba->nd = 0;
    return 0;
  }
  else
    return bufrdeco_init_subset_sequence_data(ba);
  /*
  if ( ba->sequence != NULL )
    free ( ( void * ) ba->sequence );
  return bufrdeco_init_subset_sequence_data ( ba );
  */
}

int bufrdeco_free_subset_sequence_data ( struct bufrdeco_subset_sequence_data *ba )
{
  if ( ba->sequence != NULL )
    {
      free ( ( void * ) ba->sequence );
      ba->sequence = NULL;
    }
  return 0;
}

int bufrdeco_init_compressed_data_references ( struct bufrdeco_compressed_data_references *rf )
{
  if ( rf->dim != 0 )
    {
      /*free ( ( void * ) rf->refs );
      memset ( rf, 0, sizeof ( struct bufrdeco_compressed_data_references ) );*/
      rf->nd = 0;
    }
  if ( rf->dim == 0 )
    {
      if ( ( rf->refs = ( struct bufrdeco_compressed_ref * ) calloc ( 1, BUFR_NMAXSEQ * sizeof ( struct bufrdeco_compressed_ref ) ) ) == NULL )
        {
          fprintf ( stderr,"bufr_init_compressed_data_references():Cannot allocate memory for bufrdeco_compressed_ref array\n" );
          return 1;
        }
      rf->nd = 0;
      rf->dim = BUFR_NMAXSEQ;
    }
  return 0;
}

int bufrdeco_clean_compressed_data_references ( struct bufrdeco_compressed_data_references *rf )
{
  rf->nd = 0;
  return 0;
}


int bufrdeco_free_compressed_data_references ( struct bufrdeco_compressed_data_references *rf )
{
  if ( rf->refs != NULL )
    {
      free ( ( void * ) rf->refs );
      rf->refs = NULL;
    }
  return 0;
}

/*!
   \fn int bufrdeco_init(struct bufrdeco *b)
   \brief Inits and allocate memory for a struct \ref bufrdeco
   \param b pointer to the target struct

   This function only must be called once. When finished the function \ref close_bufrdeco must be called to
   free all needed memory
*/
int bufrdeco_init ( struct bufrdeco *b )
{
  if ( b == NULL )
    return 1;

  // First clear all
  memset ( b, 0, sizeof ( struct bufrdeco ) );

  // Then allocate all needed memory when starting. Further we will need to allocate some more depending on
  // data

  // allocate memory for Tables
  if ( bufrdeco_init_tables ( &b->table ) )
    {
      sprintf ( b->error,"init_bufrdeco(): Cannot allocate space for tables\n" );
      return 1;
    }

  // allocate memory for expanded tree of descriptors
  if ( bufrdeco_init_expanded_tree ( &b->tree ) )
    {
      sprintf ( b->error,"init_bufrdeco(): Cannot allocate space for expanded tree of descriptors\n" );
      return 1;
    }


  return 0;
}

int bufrdeco_reset (struct bufrdeco *b)
{
  memset(&(b->header), 0, sizeof(struct gts_header));
  memset(&(b->sec0), 0, sizeof(struct bufr_sec0));
  memset(&(b->sec1), 0, sizeof(struct bufr_sec1));
  memset(&(b->sec2), 0, sizeof(struct bufr_sec2));
  memset(&(b->sec3), 0, sizeof(struct bufr_sec3));
  memset(&(b->sec4), 0, sizeof(struct bufr_sec4));
  memset(b->tree, 0, sizeof(struct bufrdeco_expanded_tree));
  memset(&(b->state), 0, sizeof(struct bufrdeco_decoding_data_state));
  b->refs.nd = 0;
  b->seq.nd = 0;
  return 0;
}

/*!
  \fn int bufrdeco_close ( struct bufrdeco *b )
  \brief Free all allocated memory
  \param b pointer to the target struct
  
  This function must be called at the end when no more calls t bufrdeco library is needed
*/
int bufrdeco_close ( struct bufrdeco *b )
{
  // first deallocate all memory
  bufrdeco_free_subset_sequence_data ( & ( b->seq ) );
  bufrdeco_free_compressed_data_references ( & ( b->refs ) );
  bufrdeco_free_expanded_tree ( & ( b->tree ) );
  bufrdeco_free_tables ( & ( b->table ) );
  return 0;
}
