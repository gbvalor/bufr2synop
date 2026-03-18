/***************************************************************************
 *   Copyright (C) 2013-2026 by Guillermo Ballester Valor                  *
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
 \file bufrdeco_tableB.c
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
  \struct bufr_tableB_csv_row
  \brief struct with the data readed from a csv line of a table B file
*/
struct bufr_tableB_csv_row {
    uint32_t fxy; 
    char name[BUFR_TABLEB_NAME_LENGTH];
    char unit[BUFR_TABLEB_UNIT_LENGTH];
    int32_t scale;
    int32_t reference;
    buf_t nbits;
};

/*!
  \fn static int bufr_tableB_read_next_csv_row ( FILE *f, const char *path, struct bufr_tableB_csv_row *row, struct bufrdeco *b )
  \brief Read the next line of a csv file with the format of a WMO table B and set the data in a struct \ref bufr_tableB_csv_row
  \param [in] f Pointer to the file stream to read
  \param [in] path String with the path of the file being readed, used for error messages
  \param [out] row Pointer to the struct where to set the data readed from csv line
  \param [in,out] b Pointer to the struct \ref bufrdeco where to set error messages if any
  \return 1 if a line is readed and set in row, 0 if end of file is reached, -1 if an error is detected
*/
static int bufr_tableB_read_next_csv_row ( FILE *f, const char *path, struct bufr_tableB_csv_row *row, struct bufrdeco *b )
{
  char l[CSV_MAXL];
  char caux[BUFR_TABLEB_NAME_LENGTH];
  char *tk[16];
  int nt;
  char *c;

  // read line by line until we get a valid line or end of file
  while ( fgets ( l, CSV_MAXL, f ) != NULL )
    {
      // Parse line as csv and get tokens in tk array. nt is the number of tokens
      if ( parse_csv_line ( &nt, tk, l ) < 0 || nt != 7 )
        {
          snprintf ( b->error, sizeof ( b->error ),"Error parsing csv line from table B file '%s'\n", path );
          return -1;
        }

      // Set data in row struct  
      row->fxy = strtoul ( tk[0], &c, 10 );

      if ( tk[2][0] )
        {
          snprintf ( caux, sizeof ( caux ), "%s %s", tk[1], tk[2] );
        }
      else
        {
          strncpy_safe ( caux, tk[1], sizeof ( caux ) );
        }

      memcpy ( row->name, caux, sizeof ( row->name ) );
      strncpy_safe ( row->unit, tk[3], sizeof ( row->unit ) );
      row->scale = strtol ( tk[4], &c, 10 );
      row->reference = strtol ( tk[5], &c, 10 );
      row->nbits = strtol ( tk[6], &c, 10 );

      // If we get here then we have readed a line and set the data in row, so we return 1
      return 1;
    }

  if ( ferror ( f ) )
    {
      snprintf ( b->error, sizeof ( b->error ),"Error reading from table B file '%s'\n", path );
      return -1;
    }
  return 0;
}

/*!
  \fn int bufr_read_tableB ( struct bufrdeco b )
  \brief Read a Table B file from a WMO csv formatted file and set the result in a struct \ref bufr_tableB
  \param [in,out] b Pointer to the struct \ref bufrdeco where to set the results
  \return  0 if success, 1 otherwise

  Note that this function assumes that file is csv formatted as WMO table B
*/
int bufr_read_tableB ( struct bufrdeco *b )
{
  FILE *t;
  FILE *t_local;
  buf_t i = 0;
  char caux[512];
  int have_master;
  int have_local;
  struct bufr_tableB *tb;
  struct bufr_descriptor desc;
  struct bufr_tableB_csv_row row_master;
  struct bufr_tableB_csv_row row_local;
  const struct bufr_tableB_csv_row *row;

  bufrdeco_assert ( b != NULL );

  tb = & ( b->tables->b );
  if ( tb->path[0] == 0 )
    {
      return 1;
    }

  // If we've already read this table. We just regenerate the table with original values
  if ( strcmp ( tb->path, tb->old_path ) == 0 && strcmp ( tb->local_path, tb->local_path_old ) == 0 )
    {
#ifdef __DEBUG
      printf ( "# Reused table %s\n", tb->path );
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

  memcpy ( caux, tb->path, sizeof (tb->path) );
  memset ( tb, 0, sizeof ( struct bufr_tableB ) );
  memcpy ( tb->path, caux, sizeof (tb->path) );

  t_local = NULL; // initialize to NULL in case we don't have a local table
  // Open master table B file
  if ( ( t = fopen ( tb->path, "r" ) ) == NULL )
    {
      snprintf ( b->error, sizeof ( b->error ),"Unable to open table B file '%s'\n", tb->path );
      return 1;
    }

  if ( tb->local_path[0] )
    {
      // Open local table B file
      t_local = fopen ( tb->local_path, "r" );
    }

  // read first line, it is ignored
  if ( fgets ( caux, sizeof ( caux ), t ) == NULL && ferror ( t ) )
    {
      snprintf ( b->error, sizeof ( b->error ),"Error reading from table B file '%s'\n", tb->path );
      fclose ( t );
      if ( t_local != NULL )
        {
          fclose ( t_local );
        }
      return 1;
    }

  if ( t_local != NULL )
    {

      if ( fgets ( caux, sizeof ( caux ), t_local ) == NULL && ferror ( t_local ) )
        {
          fclose ( t );
          fclose ( t_local );
          snprintf ( b->error, sizeof ( b->error ),"Error reading from table B file '%s'\n", tb->local_path );
          return 1;
        }
    }

  // read first line of master table
  have_master = bufr_tableB_read_next_csv_row ( t, tb->path, &row_master, b );
  if ( have_master < 0 )
    {
      fclose ( t );
      if ( t_local != NULL )
        {
          fclose ( t_local );
        }
      return 1;
    }


  have_local = 0;
  if ( t_local != NULL )
    {
      // read first line of local table
      have_local = bufr_tableB_read_next_csv_row ( t_local, tb->local_path, &row_local, b );
      if ( have_local < 0 )
        {
          fclose ( t );
          fclose ( t_local );
          return 1;
        }
    }

  // We read both tables at the same time, comparing the fxy of each line and taking the lowest one. 
  // In this way we can merge both tables in one pass. We also stop when we reach the maximum number of lines allowed in table B  
  while ( ( have_master || have_local ) && i < BUFR_MAXLINES_TABLEB )
    {
      if ( have_local && ( ! have_master || row_local.fxy <= row_master.fxy ) )
        {
          row = &row_local;
          if ( have_master && row_local.fxy == row_master.fxy )
            {
              have_master = bufr_tableB_read_next_csv_row ( t, tb->path, &row_master, b );
              if ( have_master < 0 )
                {
                  fclose ( t );
                  fclose ( t_local );
                  return 1;
                }
            }

          have_local = bufr_tableB_read_next_csv_row ( t_local, tb->local_path, &row_local, b );
          if ( have_local < 0 )
            {
              fclose ( t );
              fclose ( t_local );
              return 1;
            }
        }
      else
        {
          row = &row_master;
          have_master = bufr_tableB_read_next_csv_row ( t, tb->path, &row_master, b );
          if ( have_master < 0 )
            {
              fclose ( t );
              if ( t_local != NULL )
                {
                  fclose ( t_local );
                }
              return 1;
            }
        }

      uint32_t_to_descriptor ( &desc, row->fxy );
      tb->item[i].changed = 0; // Original from table B
      tb->item[i].x = desc.x; // x
      tb->item[i].y = desc.y; // y
      memcpy ( tb->item[i].key, desc.c, sizeof ( tb->item[i].key ) ); // key
      tb->item[i].key[sizeof ( tb->item[i].key ) - 1] = 0; // ensure null termination
      if ( tb->num[desc.x] == 0 )
        {
          tb->x_start[desc.x] = i;  // mark the start
        }
      tb->y_ref[desc.x][desc.y] = i - tb->x_start[desc.x]; // mark the position from start of first x
      ( tb->num[desc.x] )++;

      memcpy ( tb->item[i].name, row->name, sizeof ( tb->item[i].name ) );
      memcpy ( tb->item[i].unit, row->unit, sizeof ( tb->item[i].unit ) );

      tb->item[i].scale_ori = row->scale;
      tb->item[i].scale = tb->item[i].scale_ori;

      tb->item[i].reference_ori = row->reference;
      tb->item[i].reference = tb->item[i].reference_ori;

      tb->item[i].nbits_ori = row->nbits;
      tb->item[i].nbits = tb->item[i].nbits_ori;

      i++;
    }

  tb->x_start[0] = 0; // fix the start for x = 0
  fclose ( t );
  if ( t_local != NULL )
    {
      fclose ( t_local );
    }
  tb->nlines = i;
  tb->wmo_table = 1;
  memcpy ( tb->old_path, tb->path, sizeof ( tb->old_path ) ); // store latest path
  memcpy ( tb->local_path_old, tb->local_path, sizeof ( tb->local_path_old ) ); // store latest local path
  return 0;
}


/*!
  \fn int bufr_restore_original_tableB_item ( struct bufr_tableB *tb, struct bufrdeco *b, uint8_t mode, const char *key )
  \brief Restores the original table B parameters for a BUFR descriptor
  \param [in,out] tb Pointer to struct \ref bufr_tableB where are stored all table B data
  \param [in,out] b Pointer to the basic struct \ref bufrdeco
  \param [in] mode Integer with bit mask about changed parameters by operator descriptors
  \param [in] key Descriptor string in format FXXYYY
  \return  0 if success, 1 otherwise
*/
int bufr_restore_original_tableB_item ( struct bufr_tableB *tb, struct bufrdeco *b, uint8_t mode, const char *key )
{
  buf_t i;

  bufrdeco_assert ( b != NULL && tb != NULL );

  if ( bufr_find_tableB_index ( &i, tb, key ) )
    {
      snprintf ( b->error, sizeof ( b->error ), "%s(): descriptor '%s' not found in table B\n", __func__, key );
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
  \fn int bufr_find_tableB_index ( buf_t *index, struct bufr_tableB *tb, const char *key )
  \brief found a descriptor index in a struct \ref bufr_tableB
  \param [out] index Pointer to a size_t where to set the result if success
  \param [in] tb Pointer to struct \ref bufr_tableB where are stored all table B data
  \param [in] key Descriptor string in format FXXYYY
  \return  0 if success, 1 otherwise
*/
int bufr_find_tableB_index ( buf_t *index, struct bufr_tableB *tb, const char *key )
{
  uint32_t ix;
  buf_t i, i0;
  char *c;
  struct bufr_descriptor desc;

  bufrdeco_assert ( tb != NULL);

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
  \fn int bufrdeco_tableB_compressed ( struct bufrdeco_compressed_ref *r, struct bufrdeco *b, struct bufr_descriptor *d, int mode )
  \brief get data from table B when parsing compressed data references
  \param [out] r Pointer to a struct \ref bufrdeco_compressed_ref where to set results
  \param [in,out] b Basic container struct \ref bufrdeco
  \param [in] d Pointer to the reference struct \ref bufr_descriptor
  \param [in] mode If 0 then we are getting the data itself for a descriptor. If mode = 1 then we are dealing with associated bits
  \return If succeeded returns 0. If problem returns 1. If mode = 1 and no associated bits returns -1
*/
int bufrdeco_tableB_compressed ( struct bufrdeco_compressed_ref *r, struct bufrdeco *b, struct bufr_descriptor *d, int mode )
{
  buf_t i;
  uint32_t ival;
  uint8_t has_data;
  struct bufr_tableB *tb;

  //bufrdeco_assert ( b != NULL && r != NULL && d != NULL );
  bufrdeco_assert ( b != NULL );
  bufrdeco_assert ( r != NULL );
  bufrdeco_assert ( d != NULL );
  tb = & ( b->tables->b );

  // if mode != 0 then we search for associated_bits
  if ( mode && b->state.associated.nd == 0 )
    {
      // This is not an associated field, return -1
      return -1;
    }

  // Check for assoicated field
  if ( mode )
    {
      r->is_associated = mode; // Mark the index of array in associated fields
      r->desc = d; // Borrow de descriptor to which is associated
      r->ref = 0 ; // The references is always 0 for associated field
      r->bits = b->assoc.afield[mode - 1].assoc_bits; // copy the bits from associated field stack
      r->escale = 0; // The scale is 0 for associated field
      // build the index for tableB
      //i = tb->x_start[d->x] + tb->y_ref[d->x][d->y];
      memcpy ( r->name, b->state.associated.afield[mode - 1].cval, sizeof ( r->name ) ); // copy the name from associated field stack
      r->name[sizeof ( r->name ) - 1] = 0; // ensure null termination
      strcpy ( r->unit, "Code table" ); // copy the unit name
      r->bit0 = b->state.bit_offset; // Sets the reference offset to current state offset

      if ( get_bits_as_uint32_t ( &r->ref0, &r->has_data, &b->sec4.raw[4], & ( b->state.bit_offset ), r->bits ) == 0 )
        {
          snprintf ( b->error, sizeof ( b->error ), "%s(): Cannot get associated bits from '%s'\n", __func__, d->c );
          return 1;
        }
      // extracting inc_bits from next 6 bits for inc_bits
      if ( get_bits_as_uint32_t ( &ival, &has_data, &b->sec4.raw[4], & ( b->state.bit_offset ), 6 ) == 0 )
        {
          snprintf ( b->error, sizeof ( b->error ), "%s(): Cannot get 6 bits for inc_bits from '%s'\n", __func__, d->c );
          return 1;
        }
      r->inc_bits = ival;

      // Set the bit_offset after all bits needed by this element in all subsets
      b->state.bit_offset += r->inc_bits * b->sec3.subsets;

      return 0;
    }

  if ( is_a_local_descriptor ( d ) )
    {
      // if is a local descriptor we just skip the needed bits signified by operator 2 06 YYY
      // we assign ref = 0 and ref0 as readed reserved bits. We also will assume escale = 0
      r->desc = d;
      r->bits = b->state.local_bit_reserved;
      r->bit0 = b->state.bit_offset; // OFFSET
      strcpy ( r->name, "LOCAL DESCRIPTOR" );
      strcpy ( r->unit, "UNKNOWN" );

      // get bits for ref0
      if ( get_bits_as_uint32_t ( &r->ref0, &r->has_data, &b->sec4.raw[4], & ( b->state.bit_offset ),
                                  b->state.local_bit_reserved ) == 0 )
        {
          snprintf ( b->error, sizeof ( b->error ), "%s(): Cannot get bits from '%s'\n", __func__, d->c );
          return 1;
        }

      // and get 6 bits for inc_bits
      if ( get_bits_as_uint32_t ( &ival, &has_data, &b->sec4.raw[4], & ( b->state.bit_offset ), 6 ) == 0 )
        {
          snprintf ( b->error, sizeof ( b->error ), "%s(): Cannot get 6 bits for inc_bits from '%s'\n", __func__, d->c );
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
  r->desc = d;
  r->ref = tb->item[i].reference_ori; // copy the reference value from tableB, first from original
  r->bits = tb->item[i].nbits + b->state.added_bit_length; // copy the bits from tableB
  r->escale = tb->item[i].scale; // copy the scale from Tableb
  memcpy ( r->name, tb->item[i].name, sizeof ( r->name ) ); // copy the name
  memcpy ( r->unit, tb->item[i].unit, sizeof ( r->unit ) ); // copy the unit name

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
          snprintf ( b->error, sizeof ( b->error ), "%s(): Cannot get bits from '%s'\n", __func__, d->c );
          return 1;
        }

      // Change the new reference value in tableB with value previously readed because of rules for negative numbers
      // (we still have the original value in reference_ori)
      if ( get_table_b_reference_from_uint32_t ( & ( tb->item[i].reference ), b->state.changing_reference, ival ) )
        {
          snprintf ( b->error, sizeof ( b->error ), "%s(): Cannot change reference in 2 03 YYY operator for '%s'\n", __func__, d->c );
          return 1;
        }

      memcpy ( r->unit, "NEW REFERENCE", sizeof ( "NEW REFERENCE" ) );
      r->ref = tb->item[i].reference;

      // extracting inc_bits from next 6 bits
      if ( get_bits_as_uint32_t ( &ival, &has_data, &b->sec4.raw[4], & ( b->state.bit_offset ), 6 ) == 0 )
        {
          snprintf ( b->error, sizeof ( b->error ), "%s(): Cannot get 6 bits for inc_bits from '%s'\n", __func__, d->c );
          return 1;
        }
      // Here is supossed that all subsets will have the same reference change,
      // so inc_bits must be 0 and no inc data should be present
      if ( ival )
        {
          snprintf ( b->error, sizeof ( b->error ), "%s(): Bad format for compressed data when changing reference from '%s'\n", __func__, d->c );
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
          snprintf ( b->error, sizeof ( b->error ), "%s(): Cannot get uchars from '%s'\n", __func__, d->c );
          return 1;
        }
      // extracting inc_bits from next 6 bits
      if ( get_bits_as_uint32_t ( &ival, &has_data, &b->sec4.raw[4], & ( b->state.bit_offset ), 6 ) == 0 )
        {
          snprintf ( b->error, sizeof ( b->error ), "%s(): Cannot get 6 bits for inc_bits from '%s'\n", __func__, d->c );
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
  if ( get_bits_as_uint32_t ( &r->ref0, &r->has_data, &b->sec4.raw[4], & ( b->state.bit_offset ), r->bits ) == 0 )
    {
      snprintf ( b->error, sizeof ( b->error ), "%s(): Cannot get the data bits from '%s'\n", __func__, d->c );
      return 1;
    }
  // patch for delayed descriptor: it allways have data
  if ( is_a_delayed_descriptor ( d ) || is_a_short_delayed_descriptor ( d ) )
    r->has_data = 1;

  // extracting inc_bits from next 6 bits for inc_bits
  if ( get_bits_as_uint32_t ( &ival, &has_data, &b->sec4.raw[4], & ( b->state.bit_offset ), 6 ) == 0 )
    {
      snprintf ( b->error, sizeof ( b->error ), "%s(): Cannot get 6 bits for inc_bits from '%s'\n", __func__, d->c );
      return 1;
    }
  r->inc_bits = ival;

  // if is a delayed descriptor then inc_bits MUST be 0.
  if ( ( is_a_delayed_descriptor ( d ) || is_a_short_delayed_descriptor ( d ) ) && r->inc_bits )
    {
      snprintf ( b->error, sizeof ( b->error ), "%s(): Found a delayed descriptor with inc_bits != 0\n", __func__ );
      return 1;
    }

  // Set the bit_offset after all bits needed by this element in all subsets
  b->state.bit_offset += r->inc_bits * b->sec3.subsets;

  return 0;
}

/*!
  \fn int bufrdeco_tableB_val ( struct bufr_atom_data *a, struct bufrdeco *b, const struct bufr_descriptor *d )
  \brief Get data from a table B descriptor
  \param [out] a Pointer to a struct \ref bufr_atom_data where to set the results
  \param [in,out] b Pointer to the basic struct \ref bufrdeco
  \param [in] d Pointer to the target descriptor
  \return  0 if success, 1 otherwise
*/
int bufrdeco_tableB_val ( struct bufr_atom_data *a, struct bufrdeco *b, const struct bufr_descriptor *d, buf_t mode )
{
  buf_t i, nbits = 0;
  uint32_t ival;
  uint8_t has_data;
  int32_t /*escale = 0,*/ reference = 0;
  struct bufr_tableB *tb;

  bufrdeco_assert ( a != NULL && b != NULL && d != NULL );

  tb = & ( b->tables->b );

  // if mode != 0 then we search for associated_bits
  if ( mode && b->state.associated.nd == 0 )
    {
      // This is not an associated field, return -1
      return -1;
    }

  // Check for assoicated field
  if ( mode )
    {
      memcpy ( & ( a->desc ), d, sizeof ( struct bufr_descriptor ) ); // Copy
      nbits = b->state.associated.afield[mode - 1].assoc_bits; // copy the bits from associated field stack
      //printf("nbits=%u\n", nbits );
      a->escale = 0; // The scale is 0 for associated field

      memcpy ( a->name, "Associated value", sizeof ( "Associated value" ) ); // copy the name from associated field stack
      memcpy ( a->unit, "Code table", sizeof ( "Code table" ) ); // copy the unit name

      // get associated bits
      if ( get_bits_as_uint32_t ( & ( a->associated ), &has_data, &b->sec4.raw[4], & ( b->state.bit_offset ), nbits ) == 0 )
        {
          snprintf ( b->error, sizeof ( b->error ), "%s(): Cannot get associated bits from '%s'\n", __func__, d->c );
          return 1;
        }

      a->val = ( double ) a->associated;
      return 0;
    }


  if ( is_a_local_descriptor ( d ) )
    {
      // if is a local descriptor we just skip the bits signified by operator 2 06 YYY
      a->mask = DESCRIPTOR_IS_LOCAL;
      memcpy ( a->name, "LOCAL DESCRIPTOR", sizeof ( "LOCAL DESCRIPTOR" ) );
      memcpy ( a->unit, "UNKNOWN", sizeof ( "UNKNOWN" ) );
      if ( get_bits_as_uint32_t ( &ival, &has_data, &b->sec4.raw[4], & ( b->state.bit_offset ), b->state.local_bit_reserved ) == 0 )
        {
          snprintf ( b->error, sizeof ( b->error ), "%s(): Cannot get bits from '%s'\n", __func__, d->c );
          return 1;
        }
      a->val = ival; // we assume escale = 0 and ref = 0
      b->state.local_bit_reserved = 0; // Clean the reserved bits
      return 0;
    }

  /*if ( bufr_find_tableB_index ( &i, &b->tables->b, d->c ))  
    {
      snprintf ( b->error, sizeof (b->error ), "%s(): descriptor '%s' not found in table B\n", __func__, d->c );
      return 1; // descritor not found
    }*/
  i = b->tables->b.x_start[d->x] + b->tables->b.y_ref[d->x][d->y];

  memcpy ( & ( a->desc ), d, sizeof ( struct bufr_descriptor ) );
  a->mask = 0;
  memcpy ( a->name, tb->item[i].name, sizeof ( a->name ) );
  memcpy ( a->unit, tb->item[i].unit, sizeof ( a->unit ) );
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
          snprintf ( b->error, sizeof ( b->error ), "%s(): Cannot get bits from '%s'\n", __func__, d->c );
          return 1;
        }

      // Then change the preliminar reference value because of rules for negative numbers
      if ( get_table_b_reference_from_uint32_t ( & ( tb->item[i].reference ), b->state.changing_reference, ival ) )
        {
          snprintf ( b->error, sizeof ( b->error ), "%s(): Cannot change reference in 2 03 YYY operator for '%s'\n", __func__, d->c );
          return 1;
        }
      memcpy ( a->unit, "NEW REFERENCE", sizeof ( "NEW REFERENCE" ) );
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
          snprintf ( b->error, sizeof ( b->error ), "%s(): Cannot get uchars from '%s'\n", __func__, d->c );
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
      snprintf ( b->error, sizeof ( b->error ), "%s(): Cannot get associated bits from '%s'\n", __func__, d->c );
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
      snprintf ( b->error, sizeof ( b->error ), "%s(): Cannot get bits from '%s'\n", __func__, d->c );
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
          a->val = ( double ) ( ( int32_t ) ival + reference ) * Exp10 ( ( double ) ( -a->escale ) );
        }

      if ( strstr ( a->unit, "CODE TABLE" ) == a->unit || strstr ( a->unit, "Code table" ) == a->unit )
        {
          ival = ( uint32_t ) ( a->val + 0.5 );
          a->mask |= DESCRIPTOR_IS_CODE_TABLE;
          if ( bufrdeco_explained_table_val ( a->ctable, 256, & ( b->tables->c ), & ( tb->item[i].tableC_ref ), & ( a->desc ), ival ) != NULL )
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

