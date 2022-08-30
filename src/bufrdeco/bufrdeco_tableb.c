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
  \fn int bufr_read_tableb_csv ( struct bufrdeco b )
  \brief Read a Table B file from a WMO csv formatted file and set the result in a struct \ref bufr_tableb
  \param b pointer to the struct \ref bufrdeco where to set the results

  Note that this function assumes that file is csv formatted as WMO table B
  Return 0 if success, 1 otherwise
*/
int bufr_read_tableb_csv ( struct bufrdeco *b )
{
  char *c;
  FILE *t;
  buf_t i = 0;
  int nt;
  uint32_t ix;
  char l[CSV_MAXL];
  char caux[512];
  char *tk[16];
  struct bufr_tableb *tb;
  struct bufr_descriptor desc;

  //bufrdeco_assert ( b != NULL );
  
  tb = &(b->tables->b);
  if ( tb->path[0] == 0 )
    {
      return 1;
    }

  // If we've already readed this table. We just regenerate the table with original values
  if ( strcmp ( tb->path, tb->old_path ) == 0 )
    {
#ifdef __DEBUG      
      printf ("# Reused table %s\n", tb->path);
#endif      
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
  strcpy ( tb->path, caux );
  if ( ( t = fopen ( tb->path, "r" ) ) == NULL )
    {
      snprintf ( b->error, sizeof (b->error),"Unable to open table B file '%s'\n", tb->path );
      return 1;
    }

  // read first line, it is ignored
  fgets ( l, CSV_MAXL, t );

  while ( fgets ( l, CSV_MAXL, t ) != NULL && i < BUFR_MAXLINES_TABLEB )
    {
      // Parse line
      if ( parse_csv_line ( &nt, tk, l ) < 0 || nt != 7 )
        {
          snprintf ( b->error, sizeof (b->error),"Error parsing csv line from table B file '%s'\n", tb->path );
          return 1;
        }

      // First we build the descriptor
      ix = strtoul ( tk[0], &c, 10 );
      uint32_t_to_descriptor ( &desc, ix );
      tb->item[i].changed = 0; // Original from table B
      tb->item[i].x = desc.x; // x
      tb->item[i].y = desc.y; // y
      strcpy ( tb->item[i].key, desc.c ); // key
      if ( tb->num[desc.x] == 0 )
        {
          tb->x_start[desc.x] = i;  // marc the start
        }
      tb->y_ref[desc.x][desc.y] = i - tb->x_start[desc.x]; // marc the position from start of first x
      ( tb->num[desc.x] )++;

      // detailed name
      if ( tk[2][0] )
        {
          sprintf ( caux,"%s %s", tk[1], tk[2] );
        }
      else
        {
          strcpy ( caux, tk[1] );
        }

      // and finally copy
      strcpy ( tb->item[i].name, caux );

      // unit
      strcpy ( caux, tk[3] );
      strcpy ( tb->item[i].unit, caux );

      // escale
      tb->item[i].scale_ori = strtol ( tk[4], &c, 10 );
      tb->item[i].scale = tb->item[i].scale_ori;

      // reference
      tb->item[i].reference_ori = strtol ( tk[5], &c, 10 );
      tb->item[i].reference = tb->item[i].reference_ori;

      // bits
      tb->item[i].nbits_ori = strtol ( tk[6], &c, 10 );
      tb->item[i].nbits = tb->item[i].nbits_ori;

      i++;
    }
  tb->x_start[0] = 0; // fix the start for x = 0
  fclose ( t );
  tb->nlines = i;
  tb->wmo_table = 1;
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
  buf_t i;

  bufrdeco_assert ( b != NULL && tb != NULL );
  
  if ( bufr_find_tableb_index ( &i, tb, key ) )
    {
      snprintf (b->error, sizeof (b->error), "%s(): descriptor '%s' not found in table B\n", __func__, key );
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
  \fn int bufr_find_tableb_index ( buf_t *index, struct bufr_tableb *tb, const char *key )
  \brief found a descriptor index in a struct \ref bufr_tableb
  \param index pointer  to a size_t where to set the result if success
  \param tb pointer to struct \ref bufr_tableb where are stored all table B data
  \param key descriptor string in format FXXYYY

  Return 0 if success, 1 otherwise
*/
int bufr_find_tableb_index ( buf_t *index, struct bufr_tableb *tb, const char *key )
{
  uint32_t ix;
  buf_t i, i0;
  char *c;
  struct bufr_descriptor desc;

  //bufrdeco_assert ( tb != NULL);
  
  ix = strtoul ( key, &c, 10 );
  uint32_t_to_descriptor ( &desc, ix );

  // first chance
  i0 = tb->x_start[desc.x] + tb->y_ref[desc.x][desc.y];
  if ( tb->item[i0].x == desc.x && tb->item[i0].y == desc.y )
    {
      *index = i0;
      return 0;
    }
  // Second chance
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
  buf_t i;
  uint32_t ival;
  uint8_t has_data;
  struct bufr_tableb *tb;
  
  bufrdeco_assert ( b != NULL && r != NULL && d != NULL );
  
  tb = & ( b->tables->b );

  // if mode 1 then we search for associated_bits
  if ( mode && b->state.assoc_bits == 0 )
    {
      // This is not an associated field, return -1
      return -1;
    }

  if ( mode )
    {
      r->is_associated = 1; // Mark this reference as asociated
    }

  if ( is_a_local_descriptor ( d ) )
    {
      // if is a local descriptor we just skip the needed bits signified by operator 2 06 YYY
      // we assign ref = 0 and ref0 as readed reserved bits. We also will assume escale = 0
      memcpy ( & ( r->desc ), d, sizeof ( struct bufr_descriptor ) );
      r->bits = b->state.local_bit_reserved;
      r->bit0 = b->state.bit_offset; // OFFSET
      strcpy_safe ( r->name, "LOCAL DESCRIPTOR" );
      strcpy_safe ( r->unit, "UNKNOWN" );

      // get bits for ref0
      if ( get_bits_as_uint32_t ( &r->ref0, &r->has_data, &b->sec4.raw[4], & ( b->state.bit_offset ),
                                  b->state.local_bit_reserved ) == 0 )
        {
          snprintf (b->error, sizeof (b->error), "%s(): Cannot get bits from '%s'\n", __func__, d->c );
          return 1;
        }

      // and get 6 bits for inc_bits
      if ( get_bits_as_uint32_t ( &ival, &has_data, &b->sec4.raw[4], & ( b->state.bit_offset ), 6 ) == 0 )
        {
          snprintf (b->error, sizeof (b->error), "%s(): Cannot get 6 bits for inc_bits from '%s'\n", __func__, d->c );
          return 1;
        }

      r->escale = 0;
      r->inc_bits = ( uint8_t ) ival; // set the inc_bits
      // Update the bit offset
      b->state.bit_offset += r->inc_bits * b->sec3.subsets;
      b->state.local_bit_reserved = 0; // Clean the reserved bits

      // All is done for this local descriptor
      return 0;
    }

  // build the index for tableB
  i = tb->x_start[d->x] + tb->y_ref[d->x][d->y];

  // copy the descriptor to reference member desc
  memcpy ( & ( r->desc ), d, sizeof ( struct bufr_descriptor ) );
  r->ref = tb->item[i].reference_ori; // copy the reference value from tableB, first from original
  r->bits = tb->item[i].nbits + b->state.added_bit_length; // copy the bits from tableB
  r->escale = tb->item[i].scale; // copy the scale from Tableb
  strcpy_safe ( r->name, tb->item[i].name ); // copy the name
  strcpy_safe ( r->unit, tb->item[i].unit ); // copy the unit name

  // Add the added_scaled if not flag or code table
  if ( strstr ( r->unit, "CODE TABLE" ) != r->unit &&  strstr ( r->unit,"FLAG" ) != r->unit &&
       strstr ( r->unit, "Code table" ) != r->unit &&  strstr ( r->unit,"Flag" ) != r->unit )
    {
      r->escale += b->state.added_scale;
    }
  r->bit0 = b->state.bit_offset; // Sets the reference offset to current state offset
  r->cref0[0] = '\0'; // default
  r->ref0 = 0 ; // default

  // check if is changing reference then we change reference value
  if ( b->state.changing_reference != 255 )
    {
      // The descriptor operator 2 03 YYY is on action
      // get the bits
      if ( get_bits_as_uint32_t ( &ival, &r->has_data, &b->sec4.raw[4], & ( b->state.bit_offset ), b->state.changing_reference ) == 0 )
        {
          snprintf (b->error, sizeof (b->error), "%s(): Cannot get bits from '%s'\n", __func__, d->c );
          return 1;
        }

      // Change the new reference value in tableB with value previously readed because of rules for negative numbers
      // (we still have the original value in reference_ori)
      if ( get_table_b_reference_from_uint32_t ( & ( tb->item[i].reference ), b->state.changing_reference, ival ) )
        {
          snprintf (b->error, sizeof (b->error), "%s(): Cannot change reference in 2 03 YYY operator for '%s'\n", __func__, d->c );
          return 1;
        }
        
      strcpy_safe ( r->unit, "NEW REFERENCE" );
      r->ref = tb->item[i].reference;

      // extracting inc_bits from next 6 bits
      if ( get_bits_as_uint32_t ( &ival, &has_data, &b->sec4.raw[4], & ( b->state.bit_offset ), 6 ) == 0 )
        {
          snprintf (b->error, sizeof (b->error), "%s(): Cannot get 6 bits for inc_bits from '%s'\n", __func__, d->c );
          return 1;
        }
      // Here is supossed that all subsets will have the same reference change,
      // so inc_bits must be 0 and no inc data should be present
      if ( ival )
        {
          snprintf (b->error, sizeof (b->error), "%s(): Bad format for compressed data when changing reference from '%s'\n", __func__, d->c );
          return 1;
        }
      r->ref0 = 0; // here we also assume that ref0 = 0
      r->inc_bits = 0;
      return 0;
    }

  if ( strstr ( r->unit, "CCITT" ) != NULL )
    {
      // Case of CCITT string as unit

      if ( b->state.fixed_ccitt != 0 ) // can be changed by 2 08 YYY operator
        r->bits = 8 * b->state.fixed_ccitt;

      if ( get_bits_as_char_array ( r->cref0, &r->has_data, &b->sec4.raw[4], & ( b->state.bit_offset ), r->bits ) == 0 )
        {
          snprintf (b->error, sizeof (b->error), "%s(): Cannot get uchars from '%s'\n", __func__, d->c );
          return 1;
        }
      // extracting inc_bits from next 6 bits
      if ( get_bits_as_uint32_t ( &ival, &has_data, &b->sec4.raw[4], & ( b->state.bit_offset ), 6 ) == 0 )
        {
          snprintf (b->error, sizeof (b->error), "%s(): Cannot get 6 bits for inc_bits from '%s'\n", __func__, d->c );
          return 1;
        }
      r->inc_bits = ival;
      r->ref0 = 0;

      // Set the bit_offset after all bits needed by this element in all subsets
      b->state.bit_offset += r->inc_bits * 8 * b->sec3.subsets;

      // all is done here !!
      return 0;
    }

  // is a numeric field, i.e, a data value, a flag code or a code
  // get reference value
  if ( mode ) // case of associated field
    {
      if ( get_bits_as_uint32_t ( &r->ref0, &r->has_data, &b->sec4.raw[4], & ( b->state.bit_offset ), b->state.assoc_bits ) == 0 )
        {
          snprintf (b->error, sizeof (b->error), "%s(): Cannot get associated bits from '%s'\n", __func__, d->c );
          return 1;
        }
      // patch for delayed descriptor: it allways have data
      if ( is_a_delayed_descriptor ( d ) || is_a_short_delayed_descriptor( d ) )
        r->has_data = 1;
    }
  else // case of data
    {
      if ( get_bits_as_uint32_t ( &r->ref0, &r->has_data, &b->sec4.raw[4], & ( b->state.bit_offset ), r->bits ) == 0 )
        {
          snprintf (b->error, sizeof (b->error), "%s(): Cannot get the data bits from '%s'\n", __func__, d->c );
          return 1;
        }
      // patch for delayed descriptor: it allways have data
      if ( is_a_delayed_descriptor ( d ) || is_a_short_delayed_descriptor ( d ) )
        r->has_data = 1;
    }

  // extracting inc_bits from next 6 bits for inc_bits
  if ( get_bits_as_uint32_t ( &ival, &has_data, &b->sec4.raw[4], & ( b->state.bit_offset ), 6 ) == 0 )
    {
      snprintf (b->error, sizeof (b->error), "%s(): Cannot get 6 bits for inc_bits from '%s'\n", __func__, d->c );
      return 1;
    }
  r->inc_bits = ival;

  // if is a delayed descriptor then inc_bits MUST be 0.
  if ( (is_a_delayed_descriptor ( d ) || is_a_short_delayed_descriptor( d ) ) && r->inc_bits )
    {
      snprintf (b->error, sizeof (b->error), "%s(): Found a delayed descriptor with inc_bits != 0\n", __func__ );
      return 1;
    }
    
  // Set the bit_offset after all bits needed by this element in all subsets
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
  buf_t i, nbits = 0;
  uint32_t ival;
  uint8_t has_data;
  int32_t /*escale = 0,*/ reference = 0;
  struct bufr_tableb *tb;

  bufrdeco_assert ( a != NULL && b != NULL && d != NULL );
  
  tb = & ( b->tables->b );

  if ( is_a_local_descriptor ( d ) )
    {
      // if is a local descriptor we just skip the bits signified by operator 2 06 YYY
      a->mask = DESCRIPTOR_IS_LOCAL;
      strcpy_safe ( a->name, "LOCAL DESCRIPTOR" );
      strcpy_safe ( a->unit, "UNKNOWN" );
      if ( get_bits_as_uint32_t ( &ival, &has_data, &b->sec4.raw[4], & ( b->state.bit_offset ), b->state.local_bit_reserved ) == 0 )
        {
          snprintf (b->error, sizeof (b->error), "%s(): Cannot get bits from '%s'\n", __func__, d->c );
          return 1;
        }
      a->val = ival; // we assume escale = 0 and ref = 0
      b->state.local_bit_reserved = 0; // Clean the reserved bits
      return 0;
    }

  i = tb->x_start[d->x] + tb->y_ref[d->x][d->y];

  memcpy ( & ( a->desc ), d, sizeof ( struct bufr_descriptor ) );
  a->mask = 0;
  strcpy_safe ( a->name, tb->item[i].name );
  strcpy_safe ( a->unit, tb->item[i].unit );
  a->escale = tb->item[i].scale;

  // Case of difference statistics active
  if ( b->state.dstat_active )
    nbits = tb->item[i].nbits + 1;
  else
    nbits = tb->item[i].nbits;

  if ( b->state.changing_reference != 255 )
    {
      // The descriptor operator 2 03 YYY is on action
      // Get preliminar value
      if ( get_bits_as_uint32_t ( &ival, &has_data, &b->sec4.raw[4], & ( b->state.bit_offset ), b->state.changing_reference ) == 0 )
        {
          snprintf (b->error, sizeof (b->error), "%s(): Cannot get bits from '%s'\n", __func__, d->c );
          return 1;
        }
        
      // Then change the preliminar reference value because of rules for negative numbers
      if ( get_table_b_reference_from_uint32_t ( & ( tb->item[i].reference ), b->state.changing_reference, ival ) )
        {
          snprintf (b->error, sizeof (b->error), "%s(): Cannot change reference in 2 03 YYY operator for '%s'\n", __func__, d->c );
          return 1;
        }
      strcpy_safe ( a->unit, "NEW REFERENCE" );
      a->val = ( double ) tb->item[i].reference;
      return 0;
    }

  // case of difference statistics active  
  if ( b->state.dstat_active )
    reference = - ( ( int32_t ) 1 << ( tb->item[i].nbits ) );
  else
    reference = tb->item[i].reference;

  //printf(" escale = %d  reference = %d nbits = %lu\n", escale, reference, nbits);
  if ( strstr ( a->unit, "CCITT" ) != NULL )
    {
      if ( b->state.fixed_ccitt != 0 ) // can be changed by 2 08 YYY operator
        nbits = 8 * b->state.fixed_ccitt;

      if ( get_bits_as_char_array ( a->cval, &has_data, &b->sec4.raw[4], & ( b->state.bit_offset ), nbits ) == 0 )
        {
          snprintf (b->error, sizeof (b->error), "%s(): Cannot get uchars from '%s'\n", __func__, d->c );
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
      snprintf (b->error, sizeof (b->error), "%s(): Cannot get associated bits from '%s'\n", __func__, d->c );
      return 1;
    }
  else
    {
      a->associated = MISSING_INTEGER;
    }

  if ( strstr ( a->unit, "CODE TABLE" ) != a->unit &&  strstr ( a->unit,"FLAG" ) != a->unit &&
       strstr ( a->unit, "Code table" ) != a->unit &&  strstr ( a->unit,"Flag" ) != a->unit )
    {
      // case of numeric, no string nor code nor flag
      nbits += b->state.added_bit_length;
    }

  if ( get_bits_as_uint32_t ( &ival, &has_data, &b->sec4.raw[4], & ( b->state.bit_offset ), nbits ) == 0 )
    {
      snprintf (b->error, sizeof (b->error), "%s(): Cannot get bits from '%s'\n", __func__, d->c );
      return 1;
    }

  // patch for delayed descriptor: it allways have data
  if ( a->desc.x == 31 )
    has_data = 1;

  if ( has_data )
    {
      if ( strstr ( a->unit, "CODE TABLE" ) != a->unit &&  strstr ( a->unit,"FLAG" ) != a->unit &&
           strstr ( a->unit, "Code table" ) != a->unit &&  strstr ( a->unit,"Flag" ) != a->unit )
        {
          a->escale += b->state.added_scale;
          reference += b->state.added_reference;
          if ( b->state.factor_reference > 1 )
            reference *= b->state.factor_reference;
        }
      // Get a numeric number
      if ( a->escale >= 0 && a->escale < 8 )
        {
          a->val = ( double ) ( ( int32_t ) ival + reference ) * pow10neg[ ( size_t ) a->escale];
        }
      else if ( a->escale < 0 && a->escale > -8 )
        {
          a->val = ( double ) ( ( int32_t ) ival + reference ) * pow10pos[ ( size_t ) ( -a->escale )];
        }
      else
        {
          a->val = ( double ) ( ( int32_t ) ival + reference ) * exp10 ( ( double ) ( -a->escale ) );
        }

      if ( strstr ( a->unit, "CODE TABLE" ) == a->unit || strstr ( a->unit, "Code table" ) == a->unit )
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
