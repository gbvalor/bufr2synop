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
 \file bufr2syn_sqparse.c
 \brief file with the code to parse a sequence of descriptors for a subset
 */
#include "bufr2synop.h"

int find_descriptor(int *haystack, size_t nlst, int needle)
{
  size_t i = 0;
  while (haystack[i] != needle && i < nlst)
    i++;
  return (i < nlst);
}

int find_descriptor_interval(int *haystack, size_t nlst, int needlemin, int needlemax)
{
  size_t i = 0;
  while ((haystack[i] > needlemax || haystack[i] < needlemin) && i < nlst)
    i++;
  return (i < nlst);
}


int parse_subset_sequence(struct bufr_subset_sequence_data *sq, int *kdtlst, size_t nlst, int *ksec1, char *err)
{
  /* First task to do is figure out the type of report */

  TYPE[0] = '\0'; // clean type
  switch (ksec1[5])
    {
    case 0:
      if (find_descriptor_interval(kdtlst, nlst, 307079, 307086) ||
          find_descriptor(kdtlst, nlst,307091))
        strcpy(TYPE,"AAXX"); // synop
      break;
    case 1:
      if (find_descriptor_interval(kdtlst, nlst, 308004, 308005))
        strcpy(TYPE,"BBXX"); // ship
      else if (find_descriptor_interval(kdtlst, nlst, 308001, 308003) ||
        find_descriptor(kdtlst, nlst,1005) ||
        find_descriptor(kdtlst, nlst,2036) ||
        find_descriptor(kdtlst, nlst,2149))
        strcpy(TYPE,"ZZXX"); // buoy
      break;
    case 2:
      break;
    case 7:
      if (find_descriptor_interval(kdtlst, nlst, 307079, 307086))
        strcpy(TYPE,"AAXX");
      break;
    default:
      sprintf(err, "The data category %d is not parsed at the moment");
      return 1;
    }

  if (TYPE[0] == '\0')
  {
    sprintf(err, "Cannot find the report type\n");
    return 1;
  }
#ifdef DEBUG 
    printf("Going to parse a %s report\n", TYPE);
#endif

  if (strstr(TYPE,"AAXX") == 0)
    parse_subset_as_aaxx(&SYNOP, sq, kdtlst, nlst, ksec1, err);

  return 0;
}
