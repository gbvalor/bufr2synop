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
 \file bufrdeco_compressed.c
 \brief This file has the code to deal with compressed a compressed bufr report
*/
#include "bufrdeco.h"


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


int bufrdeco_parse_compressed_recursive ( struct bufrdeco_compressed_data_references *r, struct bufr_sequence *l, struct bufr *b )
{
  int res;
  size_t i;
  struct bufr_sequence *seq;
  struct bufrdeco_compressed_ref *rf;

  if ( l == NULL )
    {
      //inicia el array
      r->nd = 0;
      seq = & ( b->tree->seq[0] );
      b->state.bit_offset = 0;
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
          rf = & ( r->refs[r->nd] );
          // First associated field
          res = bufrdeco_tableb_compressed ( rf, b, & ( seq->lseq[i] ), 1 );
          if ( res > 0 )
            {
              return 1; // case of error
            }
          else if ( res == 0 )
            {
              // associated field read with success
              if ( r->nd <  BUFR_NMAXSEQ )
                {
                  r->nd += 1;
                }
              else
                {
                  sprintf ( b->error, "bufr_parse_compressed_recursive(): Reached limit. Consider increas BUFR_NMAXSEQ\n" );
                  return 1;
                }
              rf = & ( r->refs[r->nd] );
            }

          // then the data itself
          if ( bufrdeco_tableb_compressed ( rf, b, & ( seq->lseq[i] ), 0 ) )
            {
              return 1;
            }
          if ( r->nd <  BUFR_NMAXSEQ )
            {
              r->nd += 1;
            }
          else
            {
              sprintf ( b->error, "bufr_parse_compressed_recursive(): Reached limit. Consider increas BUFR_NMAXSEQ\n" );
              return 1;
            }
          break;

        case 2:
          // Case of operator descriptor
          if ( bufrdeco_parse_f2_compressed ( r, & ( seq->lseq[i] ), b ) )
	  {
            return 1;
	  }
          if ( r->nd <  BUFR_NMAXSEQ )
            {
              r->nd += 1;
            }
          else
            {
              sprintf ( b->error, "bufr_parse_compressed_recursive(): Reached limit. Consider increas BUFR_NMAXSEQ\n" );
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
          sprintf ( b->error, "bufr_parse_compressed_recursive(): Found bad 'f' in descriptor\n" );
          return 1;
          break;
        }
    }
  return 0;
}


int bufrdeco_parse_compressed ( struct bufrdeco_compressed_data_references *r, struct bufr *b )
{
  if ( bufrdeco_init_compressed_data_references ( r ) )
    {
      return 1;
    }

  if ( bufrdeco_parse_compressed_recursive ( r, NULL, b ) )
    {
      return 1;
    }

  return 0;
}
