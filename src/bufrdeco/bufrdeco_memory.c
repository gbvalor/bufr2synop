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
  if ( init_bufr ( b ) )
    return 1;
  return 0;
}

int bufrdeco_close ( struct bufr *b )
{
  if ( close_bufr ( b ) )
    return 1;
  return 0;
}

int init_bufr_tables (struct bufr_tables **t)
{
  if ( ( *t = ( struct bufr_tables * ) calloc ( 1, sizeof ( struct bufr_tables ) ) ) == NULL )
    return 1;
  return 0;
}

int free_bufr_tables (struct bufr_tables *t)
{
  if (t == NULL)
    return 1; // already free
  free( (void *) t);
  return 0;
}

/*


*/
int bufr_substitute_tables (struct bufr_tables **replaced, struct bufr_tables *source, struct bufr *b)
{
  *replaced = b->table;
  if (source == NULL) 
  { // allocate memory for table
    return init_bufr_tables (& (b->table));
  }
  return 0;
}

/*!

*/
int init_bufr ( struct bufr *b )
{
  memset ( b, 0, sizeof ( struct bufr ) );
  if ( ( b->sec4.raw = ( uint8_t * ) calloc ( 1, BUFR_LEN ) ) == NULL )
    return 1;
  b->sec4.allocated = BUFR_LEN;

  if ( init_bufr_tables (& (b->table)))
    {
      free ( ( void * ) b->sec4.raw );
      b->sec4.allocated = 0;
      return 1;
    }

  if ( ( b->tree = ( struct bufr_expanded_tree * ) calloc ( 1, sizeof ( struct bufr_expanded_tree ) ) ) == NULL )
    {
      free ( ( void * ) b->sec4.raw );
      free ( ( void * ) b->table );
      return 1;
    }

  return 0;
}

/*!

*/
int close_bufr ( struct bufr *b )
{
  if ( b->sec4.allocated > 0 &&  b->sec4.raw != NULL )
    free ( ( void * ) b->sec4.raw );
  if ( b->table != NULL )
    free ( ( void * ) b->table );
  if ( b->tree != NULL )
    free ( ( void * ) b->tree );
  memset ( b, 0, sizeof ( struct bufr ) );
  return 0;
}



int bufrdeco_init_subset_sequence_data ( struct bufrdeco_subset_sequence_data *ba )
{
  if ( ba->dim == 0 )
    {
      if ( ( ba->sequence = ( struct bufr_atom_data * ) malloc ( BUFR_NMAXSEQ * sizeof ( struct bufr_atom_data ) ) ) == NULL )
        {
          fprintf ( stderr,"bufr_init_subset_sequence_data():Cannot allocate memory for atom data array\n" );
          return 1;
        }
      ba->dim = BUFR_NMAXSEQ;
    }
  ba->nd = 0;
  return 0;
}

int bufrdeco_clean_subset_sequence_data ( struct bufrdeco_subset_sequence_data *ba )
{
  if ( ba->sequence != NULL )
    free ( ( void * ) ba->sequence );
  memset ( ba, 0, sizeof ( struct bufrdeco_subset_sequence_data ) );
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
  if ( rf->dim == 0 )
    {
      if ( ( rf->refs = ( struct bufrdeco_compressed_ref * ) malloc ( BUFR_NMAXSEQ * sizeof ( struct bufrdeco_compressed_ref ) ) ) == NULL )
        {
          fprintf ( stderr,"bufr_init_compressed_data_references():Cannot allocate memory for bufrdeco_compressed_ref array\n" );
          return 1;
        }
      rf->dim = BUFR_NMAXSEQ;
    }
  rf->nd = 0;
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
