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
 \file bufrdeco_wmo.c
 \brief This file has the code to read bufr files from WMO csv files
*/
#include "bufrdeco.h"

/*!
   And these are the default directories when using WMO csv table files
*/
const char DEFAULT_BUFRTABLES_WMO_CSV_DIR1[] = "/usr/local/share/bufr2synop/";
const char DEFAULT_BUFRTABLES_WMO_CSV_DIR2[] = "/usr/share/bufr2synop/";

/*!
  \fn int get_wmo_tablenames ( struct bufrdeco *b)
  \brief Get the complete pathnames for WMO csv table files needed by a bufr message
  \param b pointer for a struct \ref bufrdeco

  For WMO files this format is adopted
       BUFR_XX_Y_Z_TableB_en for table B
       BUFR_XX_Y_Z_CodeFlag for Code table and Flag table. This is equivalent to table C in ECMWF package
       BUFR_XX_Y_Z_TableD_en for table D

  XX is the Version number of master table used
  Y is the revision (currently ignored)
  Z is minor revision (currently ignored)
*/
int get_wmo_tablenames ( struct bufrdeco *b )
{
  struct stat st;
  char aux[BUFRDECO_PATH_LENGTH - 32];

  bufrdeco_assert ( b != NULL );

  if ( b->bufrtables_dir[0] == '\0' )
    {
      // try to guess directory
      if ( stat ( DEFAULT_BUFRTABLES_WMO_CSV_DIR1, &st ) )
        {
          if ( stat ( DEFAULT_BUFRTABLES_WMO_CSV_DIR2, &st ) )
            {
              return 1;
            }
          else
            {
              if ( S_ISDIR ( st.st_mode ) )
                {
                  strcpy ( aux, DEFAULT_BUFRTABLES_WMO_CSV_DIR2 );
                }
            }
        }
      else
        {
          if ( S_ISDIR ( st.st_mode ) )
            {
              strcpy ( aux, DEFAULT_BUFRTABLES_WMO_CSV_DIR1 );
            }
        }
    }
  else
    {
      strcpy ( aux, b->bufrtables_dir );
    }

  switch ( b->sec1.master_version )
    {
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
      snprintf ( b->tables->b.path, sizeof ( b->tables->b.path ),"%sBUFR_13_0_0_TableB_en.csv", aux );
      snprintf ( b->tables->c.path, sizeof ( b->tables->c.path ),"%sBUFR_13_0_0_TableC_en.csv", aux );
      snprintf ( b->tables->d.path, sizeof ( b->tables->d.path ),"%sBUFR_13_0_0_TableD_en.csv", aux );
      break;
    case 18:
      snprintf ( b->tables->b.path, sizeof ( b->tables->b.path ),"%sBUFR_18_1_0_TableB_en.csv", aux );
      snprintf ( b->tables->c.path, sizeof ( b->tables->c.path ),"%sBUFR_18_1_0_TableC_en.csv", aux );
      snprintf ( b->tables->d.path, sizeof ( b->tables->d.path ),"%sBUFR_18_1_0_TableD_en.csv", aux );
      break;
    case 19:
      snprintf ( b->tables->b.path, sizeof ( b->tables->b.path ),"%sBUFR_19_1_1_TableB_en.csv", aux );
      snprintf ( b->tables->c.path, sizeof ( b->tables->c.path ),"%sBUFR_19_1_1_TableC_en.csv", aux );
      snprintf ( b->tables->d.path, sizeof ( b->tables->d.path ),"%sBUFR_19_1_1_TableD_en.csv", aux );
      break;
    case 22:
      snprintf ( b->tables->b.path, sizeof ( b->tables->b.path ),"%sBUFR_22_0_1_TableB_en.csv", aux );
      snprintf ( b->tables->c.path, sizeof ( b->tables->c.path ),"%sBUFR_22_0_1_TableC_en.csv", aux );
      snprintf ( b->tables->d.path, sizeof ( b->tables->d.path ),"%sBUFR_22_0_1_TableD_en.csv", aux );
      break;
    case 14:
    case 15:
    case 16:
    case 17:
    case 20:
    case 21:
    case 23:
    case 24:
    case 25:
    case 26:
    case 27:
    case 28:
    case 29:
    case 30:
    case 31:
    case 32:
    case 33:
    case 34:
    case 35:
    case 36:
    case 37:
      snprintf ( b->tables->b.path, sizeof ( b->tables->b.path ),"%sBUFR_%d_0_0_TableB_en.csv", aux, b->sec1.master_version );
      snprintf ( b->tables->c.path, sizeof ( b->tables->c.path ),"%sBUFR_%d_0_0_TableC_en.csv", aux, b->sec1.master_version );
      snprintf ( b->tables->d.path, sizeof ( b->tables->d.path ),"%sBUFR_%d_0_0_TableD_en.csv", aux, b->sec1.master_version );
      break;
    case 38:
      snprintf ( b->tables->b.path, sizeof ( b->tables->b.path ),"%sBUFR_%d_1_0_TableB_en.csv", aux, b->sec1.master_version );
      snprintf ( b->tables->c.path, sizeof ( b->tables->c.path ),"%sBUFR_%d_1_0_TableC_en.csv", aux, b->sec1.master_version );
      snprintf ( b->tables->d.path, sizeof ( b->tables->d.path ),"%sBUFR_%d_1_0_TableD_en.csv", aux, b->sec1.master_version );
      break;
    default:
      snprintf ( b->tables->b.path, sizeof ( b->tables->b.path ),"%sBUFR_38_1_0_TableB_en.csv", aux );
      snprintf ( b->tables->c.path, sizeof ( b->tables->c.path ),"%sBUFR_38_1_0_TableC_en.csv", aux );
      snprintf ( b->tables->d.path, sizeof ( b->tables->d.path ),"%sBUFR_38_1_0_TableD_en.csv", aux );
      break;
    }
  return 0;
}

/*!
  \fn int bufr_read_tables (struct bufrdeco *b)
  \brief Read the tables according with bufr file data from a bufr table directory
  \param b basic struct with needed data

  The default directories where to search bufr tables are stored in \ref DEFAULT_BUFRTABLES_WMO_CSV_DIR1 and \ref DEFAULT_BUFRTABLES_WMO_CSV_DIR2
*/
int bufr_read_tables ( struct bufrdeco *b )
{
  int index;
  buf_t i;
  bufrdeco_assert ( b != NULL );
  struct bufr_tableB *tb;

  if ( b->mask & BUFRDECO_USE_TABLES_CACHE )
    {
      // When using cache, member b->tables is actually a pointer in array b->cache.tab[]

      if ( ( index = bufrdeco_cache_tables_search ( & ( b->cache ), b->sec1.master_version ) ) >= 0 )
        {
#ifdef __DEBUG
          printf ( "# Found tables in cache for version %u index %d\n", b->sec1.master_version, index );
#endif
          // hit cache, then the only task is to change member b->tables, and restore original values from item im tableB
          b->tables = b->cache.tab[index];
          tb = & ( b->tables->b );

          for ( i = 0; i < tb->nlines ; i++ )
            {
              tb->item[i].scale = tb->item[i].scale_ori;
              tb->item[i].reference = tb->item[i].reference_ori;
              tb->item[i].nbits = tb->item[i].nbits_ori;
              tb->item[i].changed = 0;
            }

          // all done
          return 0;
        }
      else
        {
#ifdef __DEBUG
          printf ( "# Tables for version %u not found in cache. Stored in index %d\n", b->sec1.master_version, b->cache.next );
#endif
          // If not in cache, the new master version tables has to be stored. This implies that
          bufrdeco_store_tables ( & ( b->tables ), & ( b->cache ), b->sec1.master_version );

          // get tablenames
          if ( get_wmo_tablenames ( b ) )
            {
              snprintf ( b->error, sizeof ( b->error ),"%s(): Cannot find bufr tables\n", __func__ );
              return 1;
            }

          // Missed cache
          if ( bufr_read_tableB ( b ) )
            {
              return 1;
            }

          if ( bufr_read_tableC ( b ) )
            {
              return 1;
            }

          if ( bufr_read_tableD ( b ) )
            {
              return 1;
            }

        }
    }
  else
    {
      // If tables still not initialized then do it
      if ( b->tables == NULL && bufrdeco_init_tables ( & ( b->tables ) ) )
        {
          snprintf ( b->error, sizeof ( b->error ), "%s(): Cannot allocate memory for tables\n", __func__ );
          return 1;
        }
      // get tablenames
      if ( get_wmo_tablenames ( b ) )
        {
          snprintf ( b->error, sizeof ( b->error ),"%s(): Cannot find bufr tables\n", __func__ );
          return 1;
        }

      // And now read tables
      if ( bufr_read_tableB ( b ) )
        {
          return 1;
        }

      if ( bufr_read_tableC ( b ) )
        {
          return 1;
        }

      if ( bufr_read_tableD ( b ) )
        {
          return 1;
        }
    }
  return 0;
}

/*!
 *  \fn int bufrdeco_store_tables ( struct bufr_tables **t, struct bufr_tables_cache *c,  uint8_t ver )
 *  \brief Init an element of array c->tab[] if still not allocated. If allocated clean it and set *t pointing to this element
 *
 */
int bufrdeco_store_tables ( struct bufr_tables **t, struct bufr_tables_cache *c,  uint8_t ver )
{
  if ( c->tab[c->next] == NULL )
    {
      // Init the array element
      bufrdeco_init_tables ( & ( c->tab[c->next] ) );

      // increase the counter of allocated elements
      ( c->nt )++;
    }
  else
    {
      // Clean the element in array with zeroes
      memset ( c->tab[c->next], 0, sizeof ( struct bufr_tables ) );

      // sets the proper version as a key of element
      c->ver[c->next] = ver;
    }

  // t will point to array element
  *t = c->tab[c->next];

  // Set the member 'next'. It marks the next element in array it will be modified when failing a search in future
  c->next = ( c->next + 1 ) % BUFRDECO_TABLES_CACHE_SIZE;
  return 0;
}

/*!
 * \fn int bufrdeco_cache_tables_search ( struct bufr_tables_cache *c, uint8_t ver )
 * \brief Search a struct \ref bufr_tables in \ref bufr_tables_cache 
 * \param c pointer to the struct \ref bufr_tables_cache where to search
 * \param ver small int with the master version acting as a key in the seardch
 * 
 * Returns the index of found struct. If no struct found returns -1
 */
int bufrdeco_cache_tables_search ( struct bufr_tables_cache *c, uint8_t ver )
{
  buf_t i = 0;

  for ( i = 0; i < BUFRDECO_TABLES_CACHE_SIZE ; i++ )
    {
      if ( c->ver[i] == ver )
        return i; // found
    }
  return -1; // Not found
}

/*!
 * \fn int bufrdeco_free_cache_tables (struct bufr_tables_cache *c)
 * \brief deallocate and clean a \ref bufr_tables_cache
 * \param c pointer to the struct to clean
 */
int bufrdeco_free_cache_tables ( struct bufr_tables_cache *c )
{
  buf_t  i;

  for ( i = 0; i < BUFRDECO_TABLES_CACHE_SIZE; i++ )
    {
      if ( c->tab[i] )
        {
          free ( ( void * ) c->tab[i] );
        }
    }
  // then clean
  memset ( c, 0, sizeof ( struct bufr_tables_cache ) );
  return 0;
}
