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
 \file bufrdeco_f2.c
 \brief file with the code to process some of the operations in B for F = 2
 */
#ifndef CONFIG_H
# include "config.h"
# define CONFIG_H
#endif

#include "bufrdeco.h"

/*!
  \fn int bufrdeco_parse_f2_descriptor ( struct bufrdeco_subset_sequence_data *s, struct bufr_descriptor *d, struct bufrdeco *b )
  \brief parse a descritor with f = 2
  \param s pointer to a struct \ref  bufrdeco_subset_sequence_data where to set data if any
  \param d pointer to the source descriptor
  \param b pointer to the base struct \ref bufrdeco

  If succeded return 0, otherwise 1
 */
int bufrdeco_parse_f2_descriptor ( struct bufrdeco_subset_sequence_data *s, struct bufr_descriptor *d, struct bufrdeco *b )
{
  size_t nbits;
  struct bufr_atom_data *a;
  uint8_t has_data;

  if ( d->f != 2 )
    {
      return 0;  // nothing to do here
    }

  switch ( d->x )
    {
    case 1:
      // Add (YYY–128) bits to the data width given for each
      // data element in Table B, other than CCITT IA5
      // (character) data, code or flag tables.
      if ( d->y )
        {
          b->state.added_bit_length = d->y - 128;
        }
      else
        {
          b->state.added_bit_length = 0;
        }
      break;

    case 2:
      // Add YYY–128 to the scale for each data element in
      // Table B, other than CCITT IA5 (character) data, code
      // or flag tables.
      if ( d->y )
        {
          b->state.added_scale = d->y - 128;
        }
      else
        {
          b->state.added_scale = 0;
        }
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
        {
          a->mask |= DESCRIPTOR_VALUE_MISSING;
        }
      else
        {
          a->mask |= DESCRIPTOR_HAVE_STRING_VALUE;
        }
      strcpy ( a->name, "SIGNIFY CHARACTER" );
      strcpy ( a->unit, "CCITTIA5" ); // unit
      if ( s->nd < ( s->dim - 1 ) )
        {
          ( s->nd ) ++;
        }
      else if ( bufrdeco_increase_data_array ( s ) == 0 )
        {
          ( s->nd ) ++;
        }
      else
        {
          sprintf ( b->error, "bufrdeco_parse_f2_descriptor(): No more bufr_atom_data available. Check BUFR_NMAXSEQ\n" );
          return 1;
        }
      break;

    case 6:
      // YYY bits of data are described by the immediately
      // following local descriptor.
      b->state.local_bit_reserved = d->y;
      break;

    case 7:
      // For Table B elements, which are not CCITT IA5
      // (character data), code tables, or flag tables:
      // 1. Add YYY to the existing scale factor
      //  YYY
      // 2. Multiply the existing reference value by 10
      // 3. Calculate ((10 x YYY) + 2) ÷ 3, disregard any
      // fractional remainder and add the result to the
      // existing bit width.
      if ( d->y )
        {
          b->state.added_scale = d->y;
          if ( d->y < 10 )
            b->state.factor_reference = pow10pos_int[d->y];
          else
            {
              sprintf ( b->error, "bufrdeco_parse_f2_descriptor(): Too much %u increase bits for operator '%s'", d->y,
                        d->c );
              return 1;
            }
          b->state.added_bit_length = ( int8_t ) ( ( 10.0 * d->y + 2.0 ) / 3.0 );
        }
      else
        {
          b->state.added_scale = 0;
          b->state.factor_reference = 1;
          b->state.added_bit_length = 0;
        }
      break;

    case 8:
      // YYY characters from CCITT International Alphabet
      // No. 5 (representing YYY x 8 bits in length) replace the
      // specified data width given for each CCITT IA5
      // element in Table B.
      b->state.fixed_ccitt = d->y;
      break;

    case 21:
      // Data not present
      //
      // Data values present in Section 4 (Data section)
      // corresponding to the following YYY descriptors shall
      // be limited to data from Classes 01–09, and Class 31.

      // It is nor processed here, it is already processed when parsing tree of expanded descriptors
      // adjusting the proper struct \ref bufr_sequence_index_range no_data_present
      break;

    case 22:
      // Quality information follows
      // The values of Class 33 elements which follow relate to
      // the data defined by the data present bit-map.
      //
      // Here we jsut activate the quality_active flag
      b->state.quality_active = 1;
      b->state.subs_active = 0;
      b->state.retained_active = 0;
      b->state.stat1_active = 0;
      b->state.dstat_active = 0;
      break;

    case 23:
      if ( d->y == 0 )
        {
          // Substituted operator values
          //
          // The substituted values which follow relate to the data
          // defined by the data present bit-map.
          b->state.quality_active = 0;
          b->state.subs_active = 1;
          b->state.retained_active = 0;
          b->state.stat1_active = 0;
          b->state.dstat_active = 0;
        }
      break;

    case 24:
      if ( d->y == 0 )
        {
          // First-order statistical values follow
          //
          // The statistical values which follow relate to the data
          // defined by the data present bit-map.
          b->state.quality_active = 0;
          b->state.subs_active = 0;
          b->state.retained_active = 0;
          b->state.stat1_active = 1;
          b->state.dstat_active = 0;
        }
      break;

    case 25:
      if ( d->y == 0 )
        {
          // Difference statistical values follow
          //
          // The statistical values which follow relate to the data
          // defined by the data present bit-map.
          b->state.quality_active = 0;
          b->state.subs_active = 0;
          b->state.retained_active = 0;
          b->state.stat1_active = 0;
          b->state.dstat_active = 1;
        }
      break;

    case 32:
      if ( d->y == 0 )
        {
          // Replaced/retained values follow
          //
          // The replaced/retained values which follow relate to the
          // data defined by the data present bit-map.
          b->state.quality_active = 0;
          b->state.subs_active = 0;
          b->state.retained_active = 1;
          b->state.stat1_active = 0;
          b->state.dstat_active = 0;
        }
      break;

    case 35:
      // Cancel backward data reference
      //
      // This operator terminates all previously defined backward
      // reference and cancels any previously defined
      // data present bit-map; it causes the next data present
      // bit-map to refer to the data descriptors which
      // immediately precede the operator to which it relates.

      // Set the bitmap pointer to NULL.
      b->state.bitmap = NULL;
      break;

    case 36:
      // Define data present bit-map
      //
      // This operator defines the data present bit-map which
      // follows for possible re-use; only one data present
      // bit-map may be defined between this operator and the
      // cancel use defined data present bit-map operator.

      // Here we just add a bufrdeco_bitmap struct set the current bitmap
      if ( bufrdeco_allocate_bitmap ( b ) )
        {
          // Cannot allocate another bitmap
          return 1;
        }
      // Set the new current bitmap
      b->state.bitmap = b->bitmap.bmap[b->bitmap.nba - 1];

      // Now set the bitmaping backward count to 1 to flag that next replicator descriptor must be set it properly
      b->state.bitmaping = 1;

      break;

    case 37:
      //  if y = 0000 use defined data present bit-map
      //  This operator causes the defined data present bit-map to be used again.
      //
      //  if y = 255  Cancel use defined data present bit-map
      // This operator cancels the re-use of the defined data present bit-map.
      if ( d->y == 0 )
        {
          if ( b->bitmap.nba && b->state.bitmap == NULL )
            b->state.bitmap =  b->bitmap.bmap[b->bitmap.nba - 1];
        }
      else if ( d->y == 255 )
        {
          b->state.bitmap = NULL;
        }
      break;

    case 41:
      // This operator denotes the beginning of the definition of an event.
      //
      // An event, as defined for use with operators 2 41 000 and 2 42 000,
      // is a set of one or more circumstances described using appropriate
      // Table B descriptors along with their corresponding data values. The
      // grouping of such descriptors together as a single “event” allows
      // them to be collectively assigned as the target of a separate descriptor
      // such as 0 33 045 or 0 33 046. When defining a circumstance within an
      // event, descriptor 0 33 042 may be employed preceding the appropriate
      // Table B descriptor in order to indicate that the corresponding value
      // is actually a bound for a range of values.
      //
      // If y = 255 then it  denotes the conclusion of the
      // event definition that was begun via the previous 2 41 000
      // operator.
      //
      // It is nor processed here, it is already processed when parsing tree of expanded descriptors
      // adjusting the proper struct \ref bufr_sequence_index_range event
      break;

    case 42:
      // This operator denotes the beginning of the definition of a conditioning event.
      //
      // An event, as defined for use with operators 2 41 000 and 2 42 000,
      // is a set of one or more circumstances described using appropriate
      // Table B descriptors along with their corresponding data values. The
      // grouping of such descriptors together as a single “event” allows
      // them to be collectively assigned as the target of a separate descriptor
      // such as 0 33 045 or 0 33 046. When defining a circumstance within an
      // event, descriptor 0 33 042 may be employed preceding the appropriate
      // Table B descriptor in order to indicate that the corresponding value
      // is actually a bound for a range of values.
      //
      // If y = 255 then it denotes the conclusion of the conditioning
      // event definition that was begun via the previous 2 42 000
      // operator.
      //
      // It is nor processed here, it is already processed when parsing tree of expanded descriptors
      // adjusting the proper struct \ref bufr_sequence_index_range cond_event
      break;

    case 43:
      // The values which follow are categorical forecast values
      //
      // A categorical forecast value represents a “best guess” from among a set of related,
      // and often mutually exclusive, data values or categories. Operator 2 43 000 may be
      // used to designate one or more values as categorical forecast values, and descriptor
      // 0 33 042 may be employed preceding any such value in order to indicate that that value
      // is actually a bound for a range of value
      //
      // If y == 255 then it denotes the conclusion of the definition of categorical forecast
      // values that was begun via theprevious 2 43 000 operator.
      //
      // It is nor processed here, it is already processed when parsing tree of expanded descriptors
      // adjusting the proper struct \ref bufr_sequence_index_range cat_forecast
      break;

    default:
      sprintf ( b->error, "bufrdeco_parse_f2_descriptor(): Still no proccessed descriptor '%s' in "
                "current library version %s\n", d->c, VERSION );
      return 1;
    }
  return 0;
}


/*!
  \fn int bufrdeco_parse_f2_compressed ( struct bufrdeco_compressed_data_references *r, struct bufr_descriptor *d, struct bufrdeco *b )
  \brief parse a descritor with f = 2 in case of compressed bufr
  \param r pointer to a struct \ref bufrdeco_compressed_data_references where to set data references if any
  \param d pointer to the source descriptor
  \param b pointer to the base struct \ref bufrdeco

  If succeded return 0, otherwise 1
 */
int bufrdeco_parse_f2_compressed ( struct bufrdeco_compressed_data_references *r, struct bufr_descriptor *d, struct bufrdeco *b )
{
  size_t nbits;
  uint32_t ival;
  struct bufrdeco_compressed_ref *rf;
  uint8_t has_data;

  if ( d->f != 2 )
    {
      return 0;  // nothing to do here
    }

  switch ( d->x )
    {
    case 1:
      // Add (YYY–128) bits to the data width given for each
      // data element in Table B, other than CCITT IA5
      // (character) data, code or flag tables.
      if ( d->y )
        {
          b->state.added_bit_length = d->y - 128;
        }
      else
        {
          b->state.added_bit_length = 0;
        }
      break;

    case 2:
      // Add YYY–128 to the scale for each data element in
      // Table B, other than CCITT IA5 (character) data, code
      // or flag tables.
      if ( d->y )
        {
          b->state.added_scale = d->y - 128;
        }
      else
        {
          b->state.added_scale = 0;
        }
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
      rf = & ( r->refs[r->nd] );
      if ( get_bits_as_char_array ( rf->cref0, &rf->has_data, &b->sec4.raw[4], & ( b->state.bit_offset ), nbits ) == 0 )
        {
          sprintf ( b->error, "bufrdeco_parse_f2_compressed(): Cannot get %u uchars from '%s'\n", d->y, d->c );
          return 1;
        }
      strcpy ( rf->name, "SIGNIFY CHARACTER" );
      strcpy ( rf->unit, "CCITTIA5" ); // unit

      // Is suppossed all data will have same length in all subsets
      // extracting inc_bits from next 6 bits
      if ( get_bits_as_uint32_t ( &ival, &has_data, &b->sec4.raw[4], & ( b->state.bit_offset ), 6 ) == 0 )
        {
          sprintf ( b->error, "bufrdeco_parse_f2_compressed(): Cannot get 6 bits for inc_bits from '%s'\n", d->c );
          return 1;
        }
      if ( ival != d->y )
        {
          sprintf ( b->error, "bufrdeco_parse_f2_compressed(): Bad length in inc_bits for a 2 05 YYY descriptor from '%s'\n", d->c );
          return 1;
        }
      rf->inc_bits = ival;
      b->state.bit_offset += rf->inc_bits * 8 * b->sec3.subsets;
      if ( r->nd < ( BUFR_NMAXSEQ - 1 ) )
        {
          r->nd += 1;
        }
      else
        {
          sprintf ( b->error, "bufrdeco_parse_f2_compressed(): Reached limit. Consider increas BUFR_NMAXSEQ\n" );
          return 1;
        }
      break;

    case 6:
      // YYY bits of data are described by the immediately
      // following local descriptor.
      b->state.local_bit_reserved = d->y;
      break;

    case 7:
      // For Table B elements, which are not CCITT IA5
      // (character data), code tables, or flag tables:
      // 1. Add YYY to the existing scale factor
      //  YYY
      // 2. Multiply the existing reference value by 10
      // 3. Calculate ((10 x YYY) + 2) ÷ 3, disregard any
      // fractional remainder and add the result to the
      // existing bit width.
      if ( d->y )
        {
          b->state.added_scale = d->y;
          if ( d->y < 10 )
            b->state.factor_reference = pow10pos_int[d->y];
          else
            {
              sprintf ( b->error, "bufrdeco_parse_f2_compressed(): Too much %u increase bits for operator '%s'", d->y,
                        d->c );
              return 1;
            }
          b->state.added_bit_length = ( int8_t ) ( ( 10.0 * d->y + 2.0 ) / 3.0 );
        }
      else
        {
          b->state.added_scale = 0;
          b->state.factor_reference = 1;
          b->state.added_bit_length = 0;
        }
      break;

    case 8:
      // YYY characters from CCITT International Alphabet
      // No. 5 (representing YYY x 8 bits in length) replace the
      // specified data width given for each CCITT IA5
      // element in Table B.
      b->state.fixed_ccitt = d->y;
      break;

    case 21:
      // Data not present
      //
      // Data values present in Section 4 (Data section)
      // corresponding to the following YYY descriptors shall
      // be limited to data from Classes 01–09, and Class 31.
      //
      // It is nor processed here, it is already processed when parsing tree of expanded descriptors
      // adjusting the proper struct \ref bufr_sequence_index_range no_data_present
      break;

    case 22:
      // Quality information follows
      // The values of Class 33 elements which follow relate to
      // the data defined by the data present bit-map.
      //
      // Here we jsut activate the quality_active flag
      b->state.quality_active = 1;
      b->state.subs_active = 0;
      b->state.retained_active = 0;
      b->state.stat1_active = 0;
      b->state.dstat_active = 0;
      break;

    case 23:
      if ( d->y == 0 )
        {
          // Substituted operator values
          //
          // The substituted values which follow relate to the data
          // defined by the data present bit-map.
          b->state.quality_active = 0;
          b->state.subs_active = 1;
          b->state.retained_active = 0;
          b->state.stat1_active = 0;
          b->state.dstat_active = 0;
        }
      break;

    case 24:
      if ( d->y == 0 )
        {
          // First-order statistical values follow
          //
          // The statistical values which follow relate to the data
          // defined by the data present bit-map.
          b->state.quality_active = 0;
          b->state.subs_active = 0;
          b->state.retained_active = 0;
          b->state.stat1_active = 1;
          b->state.dstat_active = 0;
        }
      break;

    case 25:
      if ( d->y == 0 )
        {
          // Difference statistical values follow
          //
          // The statistical values which follow relate to the data
          // defined by the data present bit-map.
          b->state.quality_active = 0;
          b->state.subs_active = 0;
          b->state.retained_active = 0;
          b->state.stat1_active = 0;
          b->state.dstat_active = 1;
        }
      break;

    case 32:
      if ( d->y == 0 )
        {
          // Replaced/retained values follow
          //
          // The replaced/retained values which follow relate to the
          // data defined by the data present bit-map.
          b->state.quality_active = 0;
          b->state.subs_active = 0;
          b->state.retained_active = 1;
          b->state.stat1_active = 0;
          b->state.dstat_active = 0;
        }
      break;


    case 35:
      // Cancel backward data reference
      //
      // This operator terminates all previously defined backward
      // reference and cancels any previously defined
      // data present bit-map; it causes the next data present
      // bit-map to refer to the data descriptors which
      // immediately precede the operator to which it relates.

      // Set the bitmap pointer to NULL.
      b->state.bitmap = NULL;
      break;

    case 36:
      // Define data present bit-map
      //
      // This operator defines the data present bit-map which
      // follows for possible re-use; only one data present
      // bit-map may be defined between this operator and the
      // cancel use defined data present bit-map operator.

      // Here we just add a bufrdeco_bitmap struct set the current bitmap
      if ( bufrdeco_allocate_bitmap ( b ) )
        {
          // Cannot allocate another bitmap
          return 1;
        }
      // Set the new current bitmap
      b->state.bitmap = b->bitmap.bmap[b->bitmap.nba - 1];

      // Now set the bitmaping backward count to 1 to flag that next replicator descriptor must be set it properly
      b->state.bitmaping = 1;

      break;

    case 37:
      //  if y = 0000 use defined data present bit-map
      //  This operator causes the defined data present bit-map to be used again.
      //
      //  if y = 255  Cancel use defined data present bit-map
      // This operator cancels the re-use of the defined data present bit-map.
      if ( d->y == 0 )
        {
          if ( b->bitmap.nba && b->state.bitmap == NULL )
            b->state.bitmap =  b->bitmap.bmap[b->bitmap.nba - 1];
        }
      else if ( d->y == 255 )
        {
          b->state.bitmap = NULL;
        }
      break;

    case 41:
      // This operator denotes the beginning of the definition of an event.
      //
      // An event, as defined for use with operators 2 41 000 and 2 42 000,
      // is a set of one or more circumstances described using appropriate
      // Table B descriptors along with their corresponding data values. The
      // grouping of such descriptors together as a single “event” allows
      // them to be collectively assigned as the target of a separate descriptor
      // such as 0 33 045 or 0 33 046. When defining a circumstance within an
      // event, descriptor 0 33 042 may be employed preceding the appropriate
      // Table B descriptor in order to indicate that the corresponding value
      // is actually a bound for a range of values.
      //
      // If y = 255 then it  denotes the conclusion of the
      // event definition that was begun via the previous 2 41 000
      // operator.
      //
      // It is nor processed here, it is already processed when parsing tree of expanded descriptors
      // adjusting the proper struct \ref bufr_sequence_index_range event
      break;

    case 42:
      // This operator denotes the beginning of the definition of a conditioning event.
      //
      // An event, as defined for use with operators 2 41 000 and 2 42 000,
      // is a set of one or more circumstances described using appropriate
      // Table B descriptors along with their corresponding data values. The
      // grouping of such descriptors together as a single “event” allows
      // them to be collectively assigned as the target of a separate descriptor
      // such as 0 33 045 or 0 33 046. When defining a circumstance within an
      // event, descriptor 0 33 042 may be employed preceding the appropriate
      // Table B descriptor in order to indicate that the corresponding value
      // is actually a bound for a range of values.
      //
      // If y = 255 then it denotes the conclusion of the conditioning
      // event definition that was begun via the previous 2 42 000
      // operator.
      //
      // It is nor processed here, it is already processed when parsing tree of expanded descriptors
      // adjusting the proper struct \ref bufr_sequence_index_range cond_event
      break;

    case 43:
      // The values which follow are categorical forecast values
      //
      // A categorical forecast value represents a “best guess” from among a set of related,
      // and often mutually exclusive, data values or categories. Operator 2 43 000 may be
      // used to designate one or more values as categorical forecast values, and descriptor
      // 0 33 042 may be employed preceding any such value in order to indicate that that value
      // is actually a bound for a range of value
      //
      // If y == 255 then it denotes the conclusion of the definition of categorical forecast
      // values that was begun via theprevious 2 43 000 operator.
      //
      // It is nor processed here, it is already processed when parsing tree of expanded descriptors
      // adjusting the proper struct \ref bufr_sequence_index_range cat_forecast
      break;

    default:
      // Still not processed: 21
      sprintf ( b->error, "bufrdeco_parse_f2_compressed(): Still no proccessed descriptor '%s' in "
                "current library version %s \n", d->c, VERSION );
      return 1;
    }
  return 0;
}

/*!
 *  \fn char *bufrdeco_get_f2_descriptor_explanation ( char *e, struct bufr_descriptor *d)
 *  \brief Return a stribg with brief explanation of descriptor when f = 2.
 *  \param e string with explanation as result
 *  \param d pointer to the struct \ref bufr_descriptor to explain
 *
 *  As result, returns the explanation string or null if problems.
 */
char *bufrdeco_get_f2_descriptor_explanation ( char *e, struct bufr_descriptor *d )
{
  // check the input
  if ( e == NULL || d == NULL || d->f != 2 )
    return NULL;


  switch ( d->x )
    {
    case 1:
      sprintf ( e, "Change data width %d bits.", d->y - 128 );
      break;

    case 2:
      sprintf ( e, "Change scale %d units.\n", d->y - 128 );
      break;

    case 3:
      if ( d->y != 255 )
        sprintf ( e, "Change reference values. Each new reference value has %d bits width.", d->y );
      else
        sprintf ( e, "Change reference values concluded." );
      break;

    case 4:
      sprintf ( e, "Add associated field. Precede each data element with %d bits of information.", d->y );
      break;

    case 5:
      sprintf ( e, "Signify character. %d characters CCITT IA5 are inserted as a data field of %d x 8 bits in length.", d->y, d->y );
      break;

    case 6:
      sprintf ( e, "Signify data width. %d bits of data are described by immediately following descsriptor", d->y );
      break;

    case 7:
      sprintf ( e, "Increase scale, reference value and data width." );
      break;

    case 8:
      sprintf ( e, "Change width of CCITT IA5 field to %d characters instead of the indicated in table B", d->y );
      break;

    case 21:
      sprintf ( e, "**** Next %d descriptor(s) on this sequence level do not have data present ****", d->y );
      break;

    case 22:
      sprintf ( e, "Quality information follows. The values of Class 33 elements which follow relate to the data defined by the data present bit-map." );
      break;

    case 23:
      if ( d->y == 0 )
        sprintf ( e, "Substituted values operator. The substituted values which follow relate to the data defined by the data present bit-map." );
      else if ( d->y == 255 )
        sprintf ( e, "Substituted values marker operator. This operator shall signify a data item containing a substituted value." );
      break;

    case 24:
      if ( d->y == 0 )
        sprintf ( e, "First-order statistical values follow." );
      else if ( d->y == 255 )
        sprintf ( e, "First-order statistical values marker operator" );
      break;

    case 25:
      if ( d->y == 0 )
        sprintf ( e, "Replaced/retained values follow." );
      else if ( d->y == 255 )
        sprintf ( e, "Replaced/retained value marker operator" );
      break;

    case 32:
      if ( d->y == 0 )
        sprintf ( e, "Difference statistical values follow." );
      else if ( d->y == 255 )
        sprintf ( e, "Difference statistical values marker operator" );
      break;

    case 35:
      sprintf ( e, "Cancel backward data reference" );
      break;

    case 36:
      sprintf ( e, "Define data present bit-map" );
      break;

    case 37:
      sprintf ( e, " Use defined data present bit-map" );
      break;

    case 41:
      if ( d->y == 0 )
        sprintf ( e, "@@@@ Event definition begins in next descriptor of this sequence level @@@@" );
      else
        sprintf ( e, "@@@@ Event definition ends here @@@@" );
      break;

    case 42:
      if ( d->y == 0 )
        sprintf ( e, "!!!! Conditioning event definition begins in next descriptor of this sequence level !!!!" );
      else
        sprintf ( e, "!!!! Conditioning event definition ends here !!!!" );
      break;

    case 43:
      if ( d->y == 0 )
        sprintf ( e, "&&&& Categorical forecast follow in next descriptor of this sequence level &&&&" );
      else
        sprintf ( e, "&&&& Categorical forecast ends here &&&&" );
      break;

    default:
      e[0] = 0; // void string
      break;
    }

  return e;
}
