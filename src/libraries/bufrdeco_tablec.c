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
 \file bufrdeco_tablec.c
 \brief file with the code to read table C data (code and flag tables)
 */
#include "bufrdeco.h"

/*!
  \fn int get_ecmwf_tablenames ( struct bufr *b, const char *bufrtables_dir)
  \brief Get the complete pathnames for EMCWF table files needed by a bufr message
  \param bufrtables_dir string with path to bufr file tables dir
  \param b pointer for a struct \ref bufr

  In ECMWF library the name of a table file is Kssswwwwwxxxxxyyyzzz.TXT , where
       - K - type of table, i.e, 'B', 'C', or 'D'
       - sss - Master table number (zero for WMO meteorological tables)
       - wwwww - Originating sub-centre
       - xxxxx - Originating centre
       - yyy - Version number of master table used
       - zzz - Version number of local table used

  If standard WMO tables are used, the Originating centre xxxxx will be set to
*/
int get_ecmwf_tablenames ( struct bufr *b, const char *bufrtables_dir )
{
  struct stat st;
  char aux[256];

  if ( bufrtables_dir == NULL )
    {
      // try to guess directory
      if ( stat ( DEFAULT_BUFRTABLES_DIR1, &st ) )
        {
          if ( stat ( DEFAULT_BUFRTABLES_DIR2, &st ) )
            {
              return 1;
            }
          else
            {
              if ( S_ISDIR ( st.st_mode ) )
                strcpy ( aux,DEFAULT_BUFRTABLES_DIR2 );
            }
        }
      else
        {
          if ( S_ISDIR ( st.st_mode ) )
            strcpy ( aux,DEFAULT_BUFRTABLES_DIR1 );
        }
    }
  else
    strcpy ( aux, bufrtables_dir );

  if ( b->sec1.master != 0 ) // case of not WMO tables
    {
      sprintf ( b->table->b.path,"%sB%03d%05d%05d%03d%03d.TXT", aux, b->sec1.master,
                b->sec1.subcentre, b->sec1.centre, b->sec1.master_version, b->sec1.master_local );
      sprintf ( b->table->c.path,"%sC%03d%05d%05d%03d%03d.TXT", aux, b->sec1.master,
                b->sec1.subcentre, b->sec1.centre, b->sec1.master_version, b->sec1.master_local );
      sprintf ( b->table->d.path,"%sD%03d%05d%05d%03d%03d.TXT", aux, b->sec1.master,
                b->sec1.subcentre, b->sec1.centre, b->sec1.master_version, b->sec1.master_local );
    }
  else
    {
      sprintf ( b->table->b.path,"%sB000%05d00000%03d%03d.TXT", aux, b->sec1.subcentre,
                b->sec1.master_version, b->sec1.master_local );
      sprintf ( b->table->c.path,"%sC000%05d00000%03d%03d.TXT", aux, b->sec1.subcentre,
                b->sec1.master_version, b->sec1.master_local );
      sprintf ( b->table->d.path,"%sD000%05d00000%03d%03d.TXT", aux, b->sec1.subcentre,
                b->sec1.master_version, b->sec1.master_local );
    }
  return 0;
}

int bufr_read_tablec(struct bufr_tablec *tc, char *error)
{
  char *c;
  FILE *t;
  size_t i = 0;

  if (tc->path == NULL)
    return 1;
  
  tc->nlines = 0;
  if ( ( t = fopen ( tc->path, "r" ) ) == NULL )
    {
      sprintf ( error,"Unable to open table C file '%s'\n", tc->path);
      return 1;
    }

  while ( fgets ( tc->l[i], 96, t ) != NULL && i < BUFR_MAXLINES_TABLEC )
    {
      // supress the newline
      if ( ( c = strrchr ( tc->l[i],'\n' ) ) != NULL )
        *c = '\0';
      i++;
    }
  fclose ( t );
  tc->nlines = i;
  return 0;
}

int bufr_read_tableb(struct bufr_tableb *tb, char *error)
{
  char *c;
  FILE *t;
  size_t i = 0;

  if (tb->path == NULL)
    return 1;
  
  tb->nlines = 0;
  if ( ( t = fopen ( tb->path, "r" ) ) == NULL )
    {
      sprintf ( error,"Unable to open table B file '%s'\n", tb->path);
      return 1;
    }

  while ( fgets ( tb->l[i], 180, t ) != NULL && i < BUFR_MAXLINES_TABLEC )
    {
      // supress the newline
      if ( ( c = strrchr ( tb->l[i],'\n' ) ) != NULL )
        *c = '\0';
      i++;
    }
  fclose ( t );
  tb->nlines = i;
  return 0;
}

int bufr_read_tabled(struct bufr_tabled *td, char *error)
{
  char *c;
  FILE *t;
  size_t i = 0;

  if ( td->path == NULL)
    return 1;
  
  td->nlines = 0;
  if ( ( t = fopen ( td->path, "r" ) ) == NULL )
    {
      sprintf ( error,"Unable to open table D file '%s'\n", td->path);
      return 1;
    }

  while ( fgets ( td->l[i], 124, t ) != NULL && i < BUFR_MAXLINES_TABLEC )
    {
      // supress the newline
      if ( ( c = strrchr ( td->l[i],'\n' ) ) != NULL )
        *c = '\0';
      i++;
    }
  fclose ( t );
  td->nlines = i;
  return 0;
}
