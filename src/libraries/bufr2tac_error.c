/***************************************************************************
 *   Copyright (C) 2013-2025 by Guillermo Ballester Valor                  *
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
 \file bufr2tac_error.c
 \brief This file has the code for functions to manage errors in bufr2tac library
*/
#include "bufr2tac.h"

int bufr2tac_push_error ( struct bufr2tac_error_stack *e, int severity, const char *description )
{
  // avoid segfaults
  if ( e == NULL )
    return -1; // Fatal error

  if ( e->ne < BUFR2TAC_ERROR_STACK_DIM )
    {
      e->err[e->ne].severity = severity;
      strcpy ( e->err[e->ne].description, description );
      ( e->ne )++;
      return 1;
    }
  else if ( e->ne == BUFR2TAC_ERROR_STACK_DIM )
    {
      // Do not pull error, but annotate the stack as rebased
      if ( e->full < severity )
        e->full = severity;
      return 0;
    }
  return -1; // bad e->ne
}

int bufr2tac_clean_error_stack ( struct bufr2tac_error_stack *e )
{
  if ( e != NULL )
    {
      memset ( e, 0, sizeof ( struct bufr2tac_error_stack ) );
      return 0;
    }
  return -1;
}

int bufr2tac_set_error ( struct bufr2tac_subset_state *s, int severity, const char *origin, const char *explanation )
{
  char description[BUFR2TAC_ERROR_DESCRIPTION_LENGTH], *c;

  c = description;
  c += sprintf ( c, "%s: ", origin );
  c += sprintf ( c, " Descriptor: %u %02u %03u: \"%s\"",s->a->desc.f, s->a->desc.x, s->a->desc.y, s->a->name );

  if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
    {
      c+= sprintf ( c, " = MISSING. " );
    }
  else
    {
      if ( s->a->cval[0] )
        c += sprintf ( c, " = '%s'. ", s->a->cval );
      else if ( s->a->desc.x == 2 )
        c += sprintf ( c, " = '%s'. ", s->a->ctable );
      else
        c += sprintf ( c, " = %lf . ", s->a->val );
    }

  sprintf ( c, "%s", explanation );


  return bufr2tac_push_error ( &s->e, severity, description );
}

int bufr2tac_print_error ( const struct bufr2tac_error_stack *e )
{
  unsigned int i;
  if ( e->ne == 0 )
    {
      printf ( "# No info/warning/error \n" );
      return 0;
    }

  for ( i = 0; i < e->ne ; i++ )
    {
      if ( e->err[i].severity == 0 )
        printf ( "# INFO: " );
      else if ( e->err[i].severity == 1 )
        printf ( "# WARNING: " );
      else if ( e->err[i].severity == 2 )
        printf ( "# ERROR: " );
      else
        {
          printf ( "# %d\n", e->err[i].severity );
          return 1;
        }
      printf ( "%s\n", e->err[i].description );
    }
  if ( e->full > 0 )
    printf ( "# More debug info follows, stack of logs full\n" );
  return 0;
}


