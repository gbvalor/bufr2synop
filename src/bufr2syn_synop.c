/***************************************************************************
 *   Copyright (C) 2013 by Guillermo Ballester Valor                       *
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
 \file bufr2syn_synop.c
 \brief file with the code to parse a sequence as a SYNOP FM-12
 */
#include "bufr2synop.h"

int parse_subset_as_aaxx(struct synop_chunks *syn, struct bufr_subset_sequence_data *sq, int *kdtlst, size_t nlst, int *ksec1, char *err)
{
    size_t is;

    // clean data
    clean_synop_chunks(syn);

    for (is = 0; is < sq->nd; is++)
    {
       if (sq->sequence[is].mask & DESCRIPTOR_VALUE_MISSING)
         continue;

       switch (sq->sequence[is].desc.x)
       {
          case 1:
            switch(sq->sequence[is].desc.y)
            {
            }
          break;
          default:
          break;
       }

    }
    return 0;
}