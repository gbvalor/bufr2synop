/***************************************************************************
 *   Copyright (C) 2013-2017 by Guillermo Ballester Valor                  *
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
 \file bufrdeco_tableb.c
 \brief file with the code to read table B data (code and flag tables)
 */
#include "bufrdeco.h"

#define BUFR_TABLEB_CHANGED_BITS (1)
#define BUFR_TABLEB_CHANGED_SCALE (2)
#define BUFR_TABLEB_CHANGED_REFERENCE (4)

const int32_t pow10pos_int[10]= {1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000};
const double pow10pos[8]= {1.0, 10.0, 100.0, 1000.0, 10000.0, 100000.0, 1000000.0, 10000000.0};
const double pow10neg[8]= {1.0,  0.1,  0.01,  0.001,  0.0001,  0.00001,  0.000001,  0.0000001};


/*!
  \fn int bufr_read_tableb ( struct bufr_tableb *tb, char *error )
  \brief Read a Table B file and set the result in a struct \ref bufr_tableb
  \param tb pointer to the struct \ref bufr_tableb where to set the results
  \param error string with the error explanation if any

  Note that this function assumes that file is formatted as EMWCF table B
  Return 0 if success, 1 otherwise
*/
int bufr_read_tableb ( struct bufr_tableb *tb, char *error )
{
  char *c;
  FILE *t;
  size_t i = 0;
  uint32_t ix;
  char l[180];
  char caux[256];
  struct bufr_descriptor desc;

  if ( tb->path[0] == 0 )
    {
      return 1;
    }

  // If we've already readed this table. We just regenerate the table with original values
  if ( strcmp ( tb->path, tb->old_path ) == 0 )
    {
      //printf ("Reutilizo tablas\n");
      for ( i = 0; i < tb->nlines ; i++ )
        {
          tb->item[i].scale = tb->item[i].scale_ori;
          tb->item[i].reference = tb->item[i].reference_ori;
          tb->item[i].nbits = tb->item[i].nbits_ori;
          tb->item[i].changed = 0;
        }
      return 0; // all done
    }

  strcpy ( caux, tb->path );
  memset ( tb, 0, sizeof ( struct bufr_tableb ) );
  strcpy ( tb->path,caux );
  if ( ( t = fopen ( tb->path, "r" ) ) == NULL )
    {
      sprintf ( error,"Unable to open table B file '%s'\n", tb->path );
      return 1;
    }

  while ( fgets ( l, 180, t ) != NULL && i < BUFR_MAXLINES_TABLEB )
    {
      // supress the newline
      if ( ( c = strrchr ( l,'\n' ) ) != NULL )
        {
          *c = '\0';
        }

      // First we build the descriptor
      ix = strtoul ( & ( l[0] ), &c, 10 );
      uint32_t_to_descriptor ( &desc, ix );
      tb->item[i].changed = 0; // Original from table B
      tb->item[i].x = desc.x; // x
      tb->item[i].y = desc.y; // y
      strcpy ( tb->item[i].key, desc.c ); // key
      if ( tb->x_start[desc.x] == 0 )
        {
          tb->x_start[desc.x] = i;  // marc the start
        }
      tb->y_ref[desc.x][desc.y] = i - tb->x_start[desc.x]; // marc the position from start of first x

      // detailed name
      bufr_charray_to_string ( tb->item[i].name, &l[8], 64 );
      bufr_adjust_string ( tb->item[i].name ); // supress trailing blanks

      // tyoe
      bufr_charray_to_string ( tb->item[i].unit ,&l[73], 24 );
      bufr_adjust_string ( tb->item[i].unit );

      // escale
      tb->item[i].scale_ori = strtol ( &l[97], &c, 10 );
      tb->item[i].scale = tb->item[i].scale_ori;

      // reference
      tb->item[i].reference_ori = strtol ( &l[102], &c, 10 );
      tb->item[i].reference = tb->item[i].reference_ori;

      // bits
      tb->item[i].nbits_ori = strtol ( &l[115], &c, 10 );
      tb->item[i].nbits = tb->item[i].nbits_ori;

      /*printf("%s %s %s %d %u %lu\n", tb->item[i].key, tb->item[i].name, tb->item[i].unit,
       tb->item[i].escale, tb->item[i].reference, tb->item[i].nbits);*/
      i++;
    }
  tb->x_start[0] = 0; // fix the start for x = 0
  fclose ( t );
  tb->nlines = i;
  tb->wmo_table = 0;
  strcpy ( tb->old_path, tb->path ); // store latest path
  return 0;
}

/*!
  \fn int bufr_restore_original_tableb_item ( struct bufr_tableb *tb, struct bufrdeco *b, uint8_t mode, char *key )
  \brief Restores the original table B parameters for a BUFR descriptor
  \param tb pointer to struct \ref bufr_tableb where are stored all table B data
  \param b pointer to the basic struct \ref bufrdeco
  \param mode integer with bit mask about changed parameteres by operator descriptors
  \param key descriptor string in format FXXYYY

  Return 0 if success, 1 otherwise
*/
int bufr_restore_original_tableb_item ( struct bufr_tableb *tb, struct bufrdeco *b, uint8_t mode, char *key )
{
  size_t i;

  if ( bufr_find_tableb_index ( &i, tb, key ) )
    {
      sprintf ( b->error, "bufr_restore_original_tableb_item(): descriptor '%s' not found in table B\n", key );
      return 1; // descritor not found
    }

  // escale
  if ( mode & BUFR_TABLEB_CHANGED_SCALE || mode == 0 )
    {
      tb->item[i].scale = tb->item[i].scale_ori;
      tb->item[i].changed &= ~ ( BUFR_TABLEB_CHANGED_SCALE ); // Clear bit mask
    }

  // reference
  if ( mode & BUFR_TABLEB_CHANGED_REFERENCE || mode == 0 )
    {
      tb->item[i].reference = tb->item[i].reference_ori;
      tb->item[i].changed &= ~ ( BUFR_TABLEB_CHANGED_REFERENCE ); // Clear bit mask
    }
  // bits
  if ( mode & BUFR_TABLEB_CHANGED_BITS || mode == 0 )
    {
      tb->item[i].nbits = tb->item[i].nbits_ori;
      tb->item[i].changed &= ~ ( BUFR_TABLEB_CHANGED_BITS ); // Clear bit mask
    }
  return 0;
}

/*!
  \fn int bufr_find_tableb_index ( size_t *index, struct bufr_tableb *tb, const char *key )
  \brief found a descriptor index in a struct \ref bufr_tableb
  \param index pointer  to a size_t where to set the result if success
  \param tb pointer to struct \ref bufr_tableb where are stored all table B data
  \param key descriptor string in format FXXYYY

  Return 0 if success, 1 otherwise
*/
int bufr_find_tableb_index ( size_t *index, struct bufr_tableb *tb, const char *key )
{
  uint32_t ix;
  size_t i, i0;
  char *c;
  struct bufr_descriptor desc;

  ix = strtoul ( key, &c, 10 );
  uint32_t_to_descriptor ( &desc, ix );
  i0 = tb->x_start[desc.x];
  for ( i = i0 ; i < i0 + tb->num[desc.x] ; i++ )
    {
      if ( tb->item[i].x != desc.x )
        {
          return 1; // descriptor not found
        }
      if ( tb->item[i].y == desc.y )
        {
          *index = i;
          return 0; // found
        }
    }
  return 1; // not found
}

/*!
  \fn int bufrdeco_tableb_compressed ( struct bufrdeco_compressed_ref *r, struct bufrdeco *b, struct bufr_descriptor *d, int mode )
  \brief get data from table B when parsing compressed data references
  \param r pointer to a struct \ref bufrdeco_compressed_ref where to set results
  \param b basic container struct \ref bufrdeco
  \param d pointer to the reference struct \ref bufr_descriptor
  \param mode If 0 then we are getting the data itself for a descriptor. If mode = 1 then we are dealing with associated bits

  If succeeded returns 0. If problem returns 1. If mode = 1 and no associated bits returns -1
*/
int bufrdeco_tableb_compressed ( struct bufrdeco_compressed_ref *r, struct bufrdeco *b, struct bufr_descriptor *d, int mode )
{
  size_t i;
  uint32_t ival;
  uint8_t has_data;
  struct bufr_tableb *tb;
  tb = & ( b->tables->b );

  // Reject wrong arguments
  if ( r == NULL || b == NULL || d == NULL )
    {
      return 1;
    }

  // if mode 1 then we search for associated_bits
  if ( mode && b->state.assoc_bits == 0 )
    {
      return -1;
    }

  if ( mode )
    {
      r->is_associated = 1;
    }

  if ( is_a_local_descriptor ( d ) )
    {
      // if is a local descriptor we just skip the needed bits signified by operator 2 06 YYY
      // we assign ref = 0 and ref0 as readed reserved bits. We also will assume escale = 0
      memcpy ( & ( r->desc ), d, sizeof ( struct bufr_descriptor ) );
      r->bits = b->state.local_bit_reserved;
      r->bit0 = b->state.bit_offset; // OFFSET
      strcpy ( r->name, "LOCAL DESCRIPTOR" );
      strcpy ( r->unit, "UNKNOWN" );
      if ( get_bits_as_uint32_t ( &r->ref0, &r->has_data, &b->sec4.raw[4], & ( b->state.bit_offset ),
                                  b->state.local_bit_reserved ) == 0 )
        {
          sprintf ( b->error, "bufrdeco_tableb_compressed(): Cannot get bits from '%s'\n", d->c );
          return 1;
        }
      if ( get_bits_as_uint32_t ( &ival, &has_data, &b->sec4.raw[4], & ( b->state.bit_offset ), 6 ) == 0 )
        {
          sprintf ( b->error, "bufrdeco_tableb_compressed(): Cannot get 6 bits for inc_bits from '%s'\n", d->c );
          return 1;
        }
      r->escale = 0;
      r->inc_bits = (uint8_t) ival;
      b->state.bit_offset += r->inc_bits * b->sec3.subsets;
      b->state.local_bit_reserved = 0; // Clean the reserved bits
      return 0;
    }

  i = tb->x_start[d->x] + tb->y_ref[d->x][d->y];
  memcpy ( & ( r->desc ), d, sizeof ( struct bufr_descriptor ) );
  r->ref = tb->item[i].reference;
  r->bits = tb->item[i].nbits + b->state.added_bit_length;
  r->escale = tb->item[i].scale;
  strcpy ( r->name, tb->item[i].name );
  strcpy ( r->unit, tb->item[i].unit );
  if ( strstr ( r->unit, "CODE TABLE" ) != r->unit &&  strstr ( r->unit,"FLAG" ) != r->unit && 
      strstr ( r->unit, "Code table" ) != r->unit &&  strstr ( r->unit,"Flag" ) != r->unit )
    {
      r->escale += b->state.added_scale;
      //r->reference += b->state.added_reference;
    }
  r->bit0 = b->state.bit_offset; // OFFSET
  r->cref0[0] = '\0'; // default
  r->ref0 = 0 ; // default
  if ( b->state.changing_reference != 255 )
    {
      // The descriptor operator 2 03 YYY is on action
      if ( get_bits_as_uint32_t ( &ival, &r->has_data, &b->sec4.raw[4], & ( b->state.bit_offset ), r->bits ) == 0 )
        {
          sprintf ( b->error, "bufrdeco_tableb_compressed(): Cannot get bits from '%s'\n", d->c );
          return 1;
        }
      // Change the reference value
      if ( get_table_b_reference_from_uint32_t ( & ( tb->item[i].reference ), b->state.changing_reference, ival ) )
        {
          sprintf ( b->error, "bufrdeco_tableb_val(): Cannot change reference in 2 03 YYY operator for '%s'\n", d->c );
          return 1;
        }
      strcpy ( r->unit, "NEW REFERENCE" );
      r->ref = tb->item[i].reference;

      // extracting inc_bits from next 6 bits
      if ( get_bits_as_uint32_t ( &ival, &has_data, &b->sec4.raw[4], & ( b->state.bit_offset ), 6 ) == 0 )
        {
          sprintf ( b->error, "bufrdeco_tableb_compressed(): Cannot get 6 bits for inc_bits from '%s'\n", d->c );
          return 1;
        }
      // Here is supossed that all subsets will have the same reference change,
      // so inc_bits must be 0 and no inc data should be present
      if ( ival )
        {
          sprintf ( b->error, "bufrdeco_tableb_compressed(): Bad format for compressed data when changing reference from '%s'\n", d->c );
          return 1;
        }
      r->ref0 = 0; // here we also assume that ref0 = 0
      r->inc_bits = 0;
      return 0;
    }

  if ( strstr ( r->unit, "CCITT" ) != NULL)
    {
      if ( b->state.fixed_ccitt != 0 ) // can be changed by 2 08 YYY operator
        r->bits = 8 * b->state.fixed_ccitt;

      if ( get_bits_as_char_array ( r->cref0, &r->has_data, &b->sec4.raw[4], & ( b->state.bit_offset ), r->bits ) == 0 )
        {
          sprintf ( b->error, "bufrdeco_tableb_compressed(): Cannot get uchars from '%s'\n", d->c );
          return 1;
        }
      // extracting inc_bits from next 6 bits
      if ( get_bits_as_uint32_t ( &ival, &has_data, &b->sec4.raw[4], & ( b->state.bit_offset ), 6 ) == 0 )
        {
          sprintf ( b->error, "bufrdeco_tableb_compressed(): Cannot get 6 bits for inc_bits from '%s'\n", d->c );
          return 1;
        }
      r->inc_bits = ival;
      r->ref0 = 0;

      // Set the bit_offset
      b->state.bit_offset += r->inc_bits * 8 * b->sec3.subsets;

      return 0;
    }

  // is a numeric field, i.e, a data value, a flag code or a code
  if ( mode ) // case of associated field
    {
      if ( get_bits_as_uint32_t ( &r->ref0, &r->has_data, &b->sec4.raw[4], & ( b->state.bit_offset ), b->state.assoc_bits ) == 0 )
        {
          sprintf ( b->error, "bufrdeco_tableb_compressed(): Cannot get associated bits from '%s'\n", d->c );
          return 1;
        }
      // patch for delayed descriptor: it allways have data
      if ( d->x == 31 )
        r->has_data = 1;
    }
  else // case of data
    {
      if ( get_bits_as_uint32_t ( &r->ref0, &r->has_data, &b->sec4.raw[4], & ( b->state.bit_offset ), r->bits ) == 0 )
        {
          sprintf ( b->error, "bufrdeco_tableb_compressed(): Cannot get the data bits from '%s'\n", d->c );
          return 1;
        }
      // patch for delayed descriptor: it allways have data
      if ( d->x == 31 )
        r->has_data = 1;
    }

  // extracting inc_bits from next 6 bits
  if ( get_bits_as_uint32_t ( &ival, &has_data, &b->sec4.raw[4], & ( b->state.bit_offset ), 6 ) == 0 )
    {
      sprintf ( b->error, "bufrdeco_tableb_compressed(): Cannot get 6 bits for inc_bits from '%s'\n", d->c );
      return 1;
    }
  r->inc_bits = ival;

  // if is a delayed descriptor then inc_bits MUST be 0.
  if ( is_a_delayed_descriptor ( d ) && r->inc_bits )
    {
      sprintf ( b->error, "bufrdeco_tableb_compressed(): Found a delayed descriptor with inc_bits != 0\n" );
      return 1;
    }
  // Set the bit_offset
  b->state.bit_offset += r->inc_bits * b->sec3.subsets;

  return 0;
}

/*!
  \fn int bufrdeco_tableb_val ( struct bufr_atom_data *a, struct bufrdeco *b, struct bufr_descriptor *d )
  \brief Get data from a table B descriptor
  \param a pointer to a struct \ref bufr_atom_data where to set the results
  \param b pointer to the basic struct \ref bufrdeco
  \param d pointer to the target descriptor

  Return 0 if success, 1 otherwise
*/
int bufrdeco_tableb_val ( struct bufr_atom_data *a, struct bufrdeco *b, struct bufr_descriptor *d )
{
  size_t i, nbits = 0;
  uint32_t ival;
  uint8_t has_data;
  int32_t escale = 0, reference = 0;
  struct bufr_tableb *tb;

  tb = & ( b->tables->b );

  // Reject wrong arguments
  if ( a == NULL || b == NULL || d == NULL )
    {
      return 1;
    }

  if ( is_a_local_descriptor ( d ) )
    {
      // if is a local descriptor we just skip the bits signified by operator 2 06 YYY
      a->mask = DESCRIPTOR_IS_LOCAL;
      strcpy ( a->name, "LOCAL DESCRIPTOR" );
      strcpy ( a->unit, "UNKNOWN" );
      if ( get_bits_as_uint32_t ( &ival, &has_data, &b->sec4.raw[4], & ( b->state.bit_offset ), b->state.local_bit_reserved ) == 0 )
        {
          sprintf ( b->error, "bufrdeco_tableb_val(): Cannot get bits from '%s'\n", d->c );
          return 1;
        }
      a->val = ival; // we assume escale = 0 and ref = 0
      b->state.local_bit_reserved = 0; // Clean the reserved bits
      return 0;
    }

  i = tb->x_start[d->x] + tb->y_ref[d->x][d->y];

  memcpy ( & ( a->desc ), d, sizeof ( struct bufr_descriptor ) );
  a->mask = 0;
  strcpy ( a->name, tb->item[i].name );
  strcpy ( a->unit, tb->item[i].unit );
  escale = tb->item[i].scale;
  nbits = tb->item[i].nbits;
  
  if ( b->state.changing_reference != 255 )
    {
      // The descriptor operator 2 03 YYY is on action
      if ( get_bits_as_uint32_t ( &ival, &has_data, &b->sec4.raw[4], & ( b->state.bit_offset ), nbits ) == 0 )
        {
          sprintf ( b->error, "bufrdeco_tableb_val(): Cannot get bits from '%s'\n", d->c );
          return 1;
        }
      // Change the reference value
      if ( get_table_b_reference_from_uint32_t ( & ( tb->item[i].reference ), b->state.changing_reference, ival ) )
        {
          sprintf ( b->error, "bufrdeco_tableb_val(): Cannot change reference in 2 03 YYY operator for '%s'\n", d->c );
          return 1;
        }
      strcpy ( a->unit, "NEW REFERENCE" );
      a->val = ( double ) tb->item[i].reference;
      return 0;
    }

  reference = tb->item[i].reference;

  //printf(" escale = %d  reference = %d nbits = %lu\n", escale, reference, nbits);
  if ( strstr ( a->unit, "CCITT" ) != NULL )
    {
      if ( b->state.fixed_ccitt != 0 ) // can be changed by 2 08 YYY operator
        nbits = 8 * b->state.fixed_ccitt;

      if ( get_bits_as_char_array ( a->cval, &has_data, &b->sec4.raw[4], & ( b->state.bit_offset ), nbits ) == 0 )
        {
          sprintf ( b->error, "bufrdeco_tableb_val(): Cannot get uchars from '%s'\n", d->c );
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
      return 0;
    }

  // is a numeric field, i.e, a data value, a flag code or a code
  // Set associated bits
  if ( b->state.assoc_bits &&
       a->desc.x != 31 &&  // Data description qualifier has not associated bits itself
       get_bits_as_uint32_t ( &a->associated, &has_data, &b->sec4.raw[4], & ( b->state.bit_offset ), b->state.assoc_bits ) == 0 )
    {
      sprintf ( b->error, "bufrdeco_tableb_val(): Cannot get associated bits from '%s'\n", d->c );
      return 1;
    }
  else
    {
      a->associated = MISSING_INTEGER;
    }
    
  if ( strstr ( a->unit, "CODE TABLE" ) != a->unit &&  strstr ( a->unit,"FLAG" ) != a->unit &&
      strstr ( a->unit, "Code table" ) != a->unit &&  strstr ( a->unit,"Flag" ) != a->unit )
    {  // case of numeric, no string nor code nor flag
       nbits += b->state.added_bit_length;
    }    
    
  if ( get_bits_as_uint32_t ( &ival, &has_data, &b->sec4.raw[4], & ( b->state.bit_offset ), nbits ) == 0 )
    {
      sprintf ( b->error, "bufrdeco_tableb_val(): Cannot get bits from '%s'\n", d->c );
      return 1;
    }

  // patch for delayed descriptor: it allways have data
  if ( a->desc.x == 31 )
    has_data = 1;

  if ( has_data )
    {
      if ( strstr ( a->unit, "CODE TABLE" ) != a->unit &&  strstr ( a->unit,"FLAG" ) != a->unit &&
      strstr ( a->unit, "Code table" ) != a->unit &&  strstr ( a->unit,"Flag" ) != a->unit  )
        {
          escale += b->state.added_scale;
          reference += b->state.added_reference;
	  if (b->state.factor_reference > 1)
	    reference *= b->state.factor_reference;
        }
      // Get a numeric number
      if ( escale >= 0 && escale < 8 )
        {
          a->val = ( double ) ( ( int32_t ) ival + reference ) * pow10neg[ ( size_t ) escale];
        }
      else if ( escale < 0 && escale > -8 )
        {
          a->val = ( double ) ( ( int32_t ) ival + reference ) * pow10pos[ ( size_t ) ( -escale )];
        }
      else
        {
          a->val = ( double ) ( ( int32_t ) ival + reference ) * pow10 ( ( double ) ( -escale ) );
        }

      if ( strstr ( a->unit, "CODE TABLE" ) == a->unit || strstr ( a->unit, "Code table" ) == a->unit)
        {
          ival = ( uint32_t ) ( a->val + 0.5 );
          a->mask |= DESCRIPTOR_IS_CODE_TABLE;
          if ( bufrdeco_explained_table_val ( a->ctable, 256, & ( b->tables->c ), & ( tb->item[i].tablec_ref ), & ( a->desc ), ival ) != NULL )
            {
              a->mask |= DESCRIPTOR_HAVE_CODE_TABLE_STRING;
            }
        }
      else if ( strstr ( a->unit,"FLAG" ) == a->unit || strstr ( a->unit,"Flag" ) == a->unit )
        {
          ival = ( uint32_t ) ( a->val + 0.5 );
          a->mask |= DESCRIPTOR_IS_FLAG_TABLE;

          if ( bufrdeco_explained_flag_val ( a->ctable, 256, & ( b->tables->c ), & ( a->desc ), ival, nbits ) != NULL )
            {
              a->mask |= DESCRIPTOR_HAVE_FLAG_TABLE_STRING;
            }
        }
    }
  else
    {
      a->val = MISSING_REAL;
      a->mask |= DESCRIPTOR_VALUE_MISSING;
    }

  return 0;
}
