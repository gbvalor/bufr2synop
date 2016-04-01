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

int bufrdeco_init ( struct bufr *b )
{
  if ( init_bufr ( b, NULL ) )
    return 1;
  return 0;
}

int init_bufr_tables ( struct bufr_tables **t )
{
  if ( *t != NULL )
    free ( ( void * ) *t );

  if ( ( *t = ( struct bufr_tables * ) calloc ( 1, sizeof ( struct bufr_tables ) ) ) == NULL )
    return 1;
  return 0;
}

int free_bufr_tables ( struct bufr_tables *t )
{
  if ( t == NULL )
    return 1; // already free
  free ( ( void * ) t );
  return 0;
}

/*

  This is useful for not to read and parse tables again.
*/
int bufr_substitute_tables ( struct bufr_tables **replaced, struct bufr_tables *source, struct bufr *b )
{
  *replaced = b->table;
  if ( source == NULL )
    {
      // allocate memory for table
      return init_bufr_tables ( & ( b->table ) );
    }
  else
    b->table = source;
  return 0;
}

/*!


*/
int init_bufr ( struct bufr *b, struct bufr_tables *t )
{

  memset ( b, 0, sizeof ( struct bufr ) );
  if ( t == NULL )
    {
      if ( init_bufr_tables ( & ( b->table ) ) )
        {
          return 1;
        }
    }
  else
    {
      b->table = t;
    }

// Allocate for a new tree struct
  if ( ( b->tree = ( struct bufr_expanded_tree * ) calloc ( 1, sizeof ( struct bufr_expanded_tree ) ) ) == NULL )
    {
      return 1;
    }
  return 0;
}


/*!

*/
int close_bufr ( struct bufr *b, struct bufr_tables **t )
{
  if ( t != NULL )
    *t = b->table;
  else
    free ( ( void * ) b->table );
  if ( b->tree != NULL )
    free ( ( void * ) b->tree );
  memset ( b, 0, sizeof ( struct bufr ) );
  return 0;
}



int bufrdeco_init_subset_sequence_data ( struct bufrdeco_subset_sequence_data *ba )
{
  memset ( ba, 0, sizeof ( struct bufrdeco_subset_sequence_data ) );
  if ( ( ba->sequence = ( struct bufr_atom_data * ) calloc( 1, BUFR_NMAXSEQ * sizeof ( struct bufr_atom_data ) ) ) == NULL )
    {
      fprintf ( stderr,"bufr_init_subset_sequence_data():Cannot allocate memory for atom data array\n" );
      return 1;
    }
  ba->dim = BUFR_NMAXSEQ;
  return 0;
}

int bufrdeco_clean_subset_sequence_data ( struct bufrdeco_subset_sequence_data *ba )
{
  if ( ba->sequence != NULL )
    free ( ( void * ) ba->sequence );
  return bufrdeco_init_subset_sequence_data ( ba );
}

int bufrdeco_free_subset_sequence_data ( struct bufrdeco_subset_sequence_data *ba )
{
  if ( ba->sequence != NULL )
    free ( ( void * ) ba->sequence );
  return 0;
}

int bufrdeco_init_compressed_data_references ( struct bufrdeco_compressed_data_references *rf )
{
  if ( rf->dim != 0 )
  {
    free ((void *) rf->refs);
    memset (rf, 0, sizeof(struct bufrdeco_compressed_data_references));
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
    free ( ( void * ) rf->refs );
  return 0;
}
