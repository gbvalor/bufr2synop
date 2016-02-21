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

int bufrdeco_tabled_get_descritors_array(struct bufr_sequence *s, struct bufr *b, const char *needle)
{
  size_t i, j;
  uint32_t nv, v;
  char *c;
  struct bufr_tabled *td;
  
  td = & ( b->table->d );

  // Reject wrong arguments
  if ( s == NULL || b == NULL || needle == NULL )
  {
    sprintf(b->error,"bufrdeco_tabled_get_descritors_array(): Wrong entry arguments\n");
    return 1;
  }
  
  // search source descriptor on data
  // Find first line for descriptor
  for ( i = 0; i <  td->nlines ; i++ )
    {
      if ( td->l[i][0] != ' ' ||
           td->l[i][1] != needle[0] ||
           td->l[i][2] != needle[1] ||
           td->l[i][3] != needle[2] ||
           td->l[i][4] != needle[3] ||
           td->l[i][5] != needle[4] ||
           td->l[i][6] != needle[5] )
        continue;
      else
        break;
    }

  if ( i == td->nlines )
    {
      sprintf(b->error, "bufrdeco_tabled_get_descritors_array(): Cannot find key '%s' in provided table D\n",
	needle);
      return 1; // descritor not found
    }

  // reads the amount of possible values
  if ( td->l[i][7] == ' ' )
    nv = strtoul ( &td->l[i][7], &c, 10 );
  else
  {
    sprintf(b->error, "bufrdeco_tabled_get_descritors_array(): Error when parsing provided table D\n");
    return 1;
  }
  // s->level must be set by caller
  // s->father must be set by caller

  s->ndesc = 0;
  
  // read all descriptors
  for ( j = 0; j < nv && (i + j) < td->nlines ; j++ )
    {
      v = strtoul(&(td->l[i+j][11]), &c, 10);
      uint32_t_to_descriptor ( &(s->lseq[j]), v );
      if (b->sec3.compressed && is_a_delayed_descriptor( &(s->lseq[j])))
      {
	 sprintf(b->error, "bufrdeco_tabled_get_descritors_array(): Found a delayed descriptor in a compressed bufr\n");
	 return 1;
      }
      (s->ndesc)++; 
    }
  // s->sons are not set here
  return 0;
}
