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
 \file bufrdeco_ecmwf.c
 \brief This file has the code to read bufr files from ECMWF package
*/
#include "bufrdeco.h"

/*!
   These are the default directories where the ECMWF package install bufrtables
*/
const char DEFAULT_BUFRTABLES_ECMWF_DIR1[] = "/usr/local/lib/bufrtables/";
const char DEFAULT_BUFRTABLES_ECMWF_DIR2[] = "/usr/lib/bufrtables/";

/*!
  \fn int get_ecmwf_tablenames ( struct bufrdeco *b, const char *bufrtables_dir)
  \brief Get the complete pathnames for EMCWF table files needed by a bufr message
  \param bufrtables_dir string with path to bufr file tables dir
  \param b pointer for a struct \ref bufrdeco

  In ECMWF library the name of a table file is Kssswwwwwxxxxxyyyzzz.TXT , where
       - K - type of table, i.e, 'B', 'C', or 'D'
       - sss - Master table number (zero for WMO meteorological tables)
       - wwwww - Originating sub-centre
       - xxxxx - Originating centre
       - yyy - Version number of master table used
       - zzz - Version number of local table used
*/
int get_ecmwf_tablenames ( struct bufrdeco *b )
{
  struct stat st;
  char aux[128];

  // Check argument
  if (b == NULL)
    return 1;
  
  if ( b->bufrtables_dir[0] == '\0' )
    {
      // try to guess directory
      if ( stat ( DEFAULT_BUFRTABLES_ECMWF_DIR1, &st ) )
        {
          if ( stat ( DEFAULT_BUFRTABLES_ECMWF_DIR2, &st ) )
            {
              snprintf(b->error, sizeof (b->error), "%s(): Cannot stat dir '%s'\n", __func__, DEFAULT_BUFRTABLES_ECMWF_DIR2);
              return 1;
            }
          else
            {
              if ( S_ISDIR ( st.st_mode ) )
                {
                  strcpy_safe ( aux, DEFAULT_BUFRTABLES_ECMWF_DIR2 );
                }
            }
        }
      else
        {
          if ( S_ISDIR ( st.st_mode ) )
            {
              strcpy_safe ( aux, DEFAULT_BUFRTABLES_ECMWF_DIR1 );
            }
        }
    }
  else
    {
      strcpy_safe ( aux, b->bufrtables_dir );
    }

  snprintf ( b->tables->b.path, sizeof (b->tables->b.path), "%sB%03d%05d%05d%03d%03d.TXT", aux, b->sec1.master,
            b->sec1.subcentre, b->sec1.centre, b->sec1.master_version, b->sec1.master_local );
  snprintf ( b->tables->c.path, sizeof (b->tables->c.path),"%sC%03d%05d%05d%03d%03d.TXT", aux, b->sec1.master,
            b->sec1.subcentre, b->sec1.centre, b->sec1.master_version, b->sec1.master_local );
  snprintf ( b->tables->d.path, sizeof (b->tables->d.path), "%sD%03d%05d%05d%03d%03d.TXT", aux, b->sec1.master,
            b->sec1.subcentre, b->sec1.centre, b->sec1.master_version, b->sec1.master_local );

  /* check for table b, if problems then we try alternative */
  if ( stat ( b->tables->b.path, &st ) )
    {
      // here we set originating centre xxxxx to 00000 for WMO tables
      if ( b->sec1.master != 0 ) // case of not WMO tables
        {
          snprintf ( b->tables->b.path, sizeof (b->tables->b.path), "%sB%03d%05d%05d%03d%03d.TXT", aux, b->sec1.master,
                    b->sec1.subcentre, b->sec1.centre, b->sec1.master_version, b->sec1.master_local );
          snprintf ( b->tables->c.path, sizeof (b->tables->c.path), "%sC%03d%05d%05d%03d%03d.TXT", aux, b->sec1.master,
                    b->sec1.subcentre, b->sec1.centre, b->sec1.master_version, b->sec1.master_local );
          snprintf ( b->tables->d.path, sizeof (b->tables->d.path), "%sD%03d%05d%05d%03d%03d.TXT", aux, b->sec1.master,
                    b->sec1.subcentre, b->sec1.centre, b->sec1.master_version, b->sec1.master_local );
        }
      else
        {
          snprintf ( b->tables->b.path, sizeof (b->tables->b.path), "%sB000%05d00000%03d%03d.TXT", aux, b->sec1.subcentre,
                    b->sec1.master_version, b->sec1.master_local );
          snprintf ( b->tables->c.path, sizeof (b->tables->c.path),"%sC000%05d00000%03d%03d.TXT", aux, b->sec1.subcentre,
                    b->sec1.master_version, b->sec1.master_local );
          snprintf ( b->tables->d.path, sizeof (b->tables->d.path), "%sD000%05d00000%03d%03d.TXT", aux, b->sec1.subcentre,
                    b->sec1.master_version, b->sec1.master_local );
        }

      if ( stat ( b->tables->b.path, &st ) )
        {
          // Another chance. Set local zzz to 000
          if ( b->sec1.master != 0 ) // case of not WMO tables
            {
              snprintf ( b->tables->b.path, sizeof (b->tables->b.path), "%sB%03d%05d%05d%03d000.TXT", aux, b->sec1.master,
                        b->sec1.subcentre, b->sec1.centre, b->sec1.master_version );
              snprintf ( b->tables->c.path, sizeof (b->tables->c.path),"%sC%03d%05d%05d%03d000.TXT", aux, b->sec1.master,
                        b->sec1.subcentre, b->sec1.centre, b->sec1.master_version );
              snprintf ( b->tables->d.path, sizeof (b->tables->d.path),"%sD%03d%05d%05d%03d000.TXT", aux, b->sec1.master,
                        b->sec1.subcentre, b->sec1.centre, b->sec1.master_version );
            }
          else
            {
              snprintf ( b->tables->b.path, sizeof (b->tables->b.path),"%sB000%05d00000%03d000.TXT", aux, b->sec1.subcentre,
                        b->sec1.master_version );
              snprintf ( b->tables->c.path, sizeof (b->tables->c.path),"%sC000%05d00000%03d000.TXT", aux, b->sec1.subcentre,
                        b->sec1.master_version );
              snprintf ( b->tables->d.path, sizeof (b->tables->d.path),"%sD000%05d00000%03d000.TXT", aux, b->sec1.subcentre,
                        b->sec1.master_version );
            }

          if ( stat ( b->tables->b.path, &st ) )
            {
              // Another chance. Set subcentre wwwww to 00000
              if ( b->sec1.master != 0 ) // case of not WMO tables
                {
                  snprintf ( b->tables->b.path, sizeof (b->tables->b.path),"%sB%03d%05d%05d%03d000.TXT", aux, b->sec1.master,
                            b->sec1.subcentre, b->sec1.centre, b->sec1.master_version );
                  snprintf ( b->tables->c.path, sizeof (b->tables->c.path),"%sC%03d%05d%05d%03d000.TXT", aux, b->sec1.master,
                            b->sec1.subcentre, b->sec1.centre, b->sec1.master_version );
                  snprintf ( b->tables->d.path, sizeof (b->tables->d.path),"%sD%03d%05d%05d%03d000.TXT", aux, b->sec1.master,
                            b->sec1.subcentre, b->sec1.centre, b->sec1.master_version );
                }
              else
                {
                  snprintf ( b->tables->b.path, sizeof (b->tables->b.path),"%sB0000000000000%03d000.TXT", aux, b->sec1.master_version );
                  snprintf ( b->tables->c.path, sizeof (b->tables->c.path),"%sC0000000000000%03d000.TXT", aux, b->sec1.master_version );
                  snprintf ( b->tables->d.path, sizeof (b->tables->d.path),"%sD0000000000000%03d000.TXT", aux, b->sec1.master_version );
                }
            }
        }
    }
  return 0;
}

/*!
  \fn int bufr_read_tables_ecmwf (struct bufrdeco *b, char *tables_dir)
  \brief Read the tables according with bufr file data from a bufr table directory
  \param b basic struct with needed data

  The default directories where to search bufr tables are stored in \ref DEFAULT_BUFRTABLES_ECMWF_DIR1 and \ref DEFAULT_BUFRTABLES_ECMWF_DIR2
*/
int bufr_read_tables_ecmwf ( struct bufrdeco *b )
{

  // Check argument
  if (b == NULL)
    return 1;
  
  // get tablenames
  if ( get_ecmwf_tablenames ( b ) )
    {
      snprintf ( b->error, sizeof (b->error), "%s(): Cannot find bufr tables\n" , __func__ );
      return 1;
    }

  // If tables still not initialized then do it
  if ( b->tables == NULL && bufrdeco_init_tables ( & ( b->tables ) ) )
    {
      snprintf ( b->error, sizeof (b->error), "%s(): Cannot allocate memory for tables\n" , __func__ );
      return 1;
    }

  // And now read tables
  if ( bufr_read_tableb ( b ) )
    {
      return 1;
    }
  if ( bufr_read_tablec ( b ) )
    {
      return 1;
    }
  if ( bufr_read_tabled ( b ) )
    {
      return 1;
    }
  return 0;
}
