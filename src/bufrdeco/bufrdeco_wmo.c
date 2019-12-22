/***************************************************************************
 *   Copyright (C) 2013-2018 by Guillermo Ballester Valor                  *
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
  char aux[128];

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
              strcpy ( aux,DEFAULT_BUFRTABLES_WMO_CSV_DIR1 );
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
      sprintf ( b->tables->b.path,"%sBUFR_13_0_0_TableB_en.csv", aux );
      sprintf ( b->tables->c.path,"%sBUFR_13_0_0_TableC_en.csv", aux );
      sprintf ( b->tables->d.path,"%sBUFR_13_0_0_TableD_en.csv", aux );
      break;
    case 18:
      sprintf ( b->tables->b.path,"%sBUFR_18_1_0_TableB_en.csv", aux );
      sprintf ( b->tables->c.path,"%sBUFR_18_1_0_TableC_en.csv", aux );
      sprintf ( b->tables->d.path,"%sBUFR_18_1_0_TableD_en.csv", aux );
      break;
    case 19:
      sprintf ( b->tables->b.path,"%sBUFR_19_1_1_TableB_en.csv", aux );
      sprintf ( b->tables->c.path,"%sBUFR_19_1_1_TableC_en.csv", aux );
      sprintf ( b->tables->d.path,"%sBUFR_19_1_1_TableD_en.csv", aux );
      break;
    case 22:
      sprintf ( b->tables->b.path,"%sBUFR_22_0_1_TableB_en.csv", aux );
      sprintf ( b->tables->c.path,"%sBUFR_22_0_1_TableC_en.csv", aux );
      sprintf ( b->tables->d.path,"%sBUFR_22_0_1_TableD_en.csv", aux );
      break;
    case 14:
    case 15:
    case 16:
    case 17:
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
      sprintf ( b->tables->b.path,"%sBUFR_%d_0_0_TableB_en.csv", aux, b->sec1.master_version );
      sprintf ( b->tables->c.path,"%sBUFR_%d_0_0_TableC_en.csv", aux, b->sec1.master_version );
      sprintf ( b->tables->d.path,"%sBUFR_%d_0_0_TableD_en.csv", aux, b->sec1.master_version );
      break;
    default:
      sprintf ( b->tables->b.path,"%sBUFR_33_0_0_TableB_en.csv", aux );
      sprintf ( b->tables->c.path,"%sBUFR_33_0_0_TableC_en.csv", aux );
      sprintf ( b->tables->d.path,"%sBUFR_33_0_0_TableD_en.csv", aux );
      break;
    }
  return 0;
}

/*!
  \fn int bufr_read_tables_wmo (struct bufrdeco *b)
  \brief Read the tables according with bufr file data from a bufr table directory
  \param b basic struct with needed data

  The default directories where to search bufr tables are stored in \ref DEFAULT_BUFRTABLES_WMO_CSV_DIR1 and \ref DEFAULT_BUFRTABLES_WMO_CSV_DIR2
*/
int bufr_read_tables_wmo ( struct bufrdeco *b )
{

  // get tablenames
  if ( get_wmo_tablenames ( b ) )
    {
      sprintf ( b->error, "bufrdeco_read_tables_ecmwf(): Cannot find bufr tables\n" );
      return 1;
    }

  // If tables still not initialized then do it
  if ( b->tables == NULL && bufrdeco_init_tables ( & ( b->tables ) ) )
    {
      sprintf ( b->error, "bufrdeco_read_tables_ecmwf(): Cannot allocate memory for tables\n" );
      return 1;
    }

  // And now read tables
  if ( bufr_read_tableb_csv ( & ( b->tables->b ), b->error ) )
    {
      return 1;
    }

  if ( bufr_read_tablec_csv ( & ( b->tables->c ), b->error ) )
    {
      return 1;
    }

  if ( bufr_read_tabled_csv ( & ( b->tables->d ), b->error ) )
    {
      return 1;
    }
  return 0;
}
