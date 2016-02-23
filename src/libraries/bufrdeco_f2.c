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
 \file bufrdeco_f2.c
 \brief file with the code to process some of the operations in B for F = 2
 */
#include "bufrdeco.h"

int bufrdeco_parse_f2_descriptor ( struct bufrdeco_subset_sequence_data *s, struct bufr_descriptor *d, struct bufr *b )
{
  size_t nbits;
  struct bufr_atom_data *a;
  uint8_t has_data;

  if ( d->f != 2 )
    return 0; // nothing to do here

  switch ( d->x )
    {
    case 1:
      // Add (YYY–128) bits to the data width given for each
      // data element in Table B, other than CCITT IA5
      // (character) data, code or flag tables.
      if ( d->y )
        b->state.added_bit_length = d->y - 128;
      else
        b->state.added_bit_length = 0;
      break;

    case 2:
      // Add YYY–128 to the scale for each data element in
      // Table B, other than CCITT IA5 (character) data, code
      // or flag tables.
      if ( d->y )
        b->state.added_scale = d->y - 128;
      else
        b->state.added_scale = 0;
      break;

    case 3:
      // Subsequent element descriptors define new reference
      // values for corresponding Table B entries. Each new
      // reference value is represented by YYY bits in the Data
      // section. Definition of new reference values is concluded
      // by coding this operator with YYY = 255. Negative
      // reference values shall be represented by a positive
      // integer with the left-most bit (bit 1) set to 1.
      b->state.changing_reference = d->y;
      break;

    case 4:
      // Precede each data element with YYY bits of
      // information. This operation associates a data field
      // (e.g. quality control information) of YYY bits with each
      // data element.
      b->state.assoc_bits = d->y;
      break;

    case 5:
      // YYY characters (CCITT International Alphabet No. 5) are
      // inserted as a data field of YYY x 8 bits in length.
      nbits = 8 * d->y;
      a = & ( s->sequence[s->nd] );
      memcpy ( &a->desc, d, sizeof ( struct bufr_descriptor ) );
      if ( get_bits_as_char_array ( a->cval, &has_data, &b->sec4.raw[4], & ( b->state.bit_offset ), nbits ) == 0 )
        {
          sprintf ( b->error, "bufrdeco_parse_f2_descriptor(): Cannot get %u uchars from '%s'\n", d->y, d->c );
          return 1;
        }
      if ( has_data == 0 )
        a->mask |= DESCRIPTOR_VALUE_MISSING;
      else
        a->mask |= DESCRIPTOR_HAVE_STRING_VALUE;
      strcpy ( a->name, "SIGNIFY CHARACTER" );
      strcpy ( a->unit, "CCITTIA5" ); // unit
      break;
    default:
      sprintf ( b->error, "bufrdeco_parse_f2_descriptor(): Still no proccessed descriptor '%s' in "
                "current library version\n", d->c );
      return 1;
    }
  return 0;
}
