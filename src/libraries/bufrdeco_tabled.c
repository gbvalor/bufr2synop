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
 \file bufrdeco_tabled.c
 \brief file with the code to read table D data (code and flag tables)
 */
#include "bufrdeco.h"
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
