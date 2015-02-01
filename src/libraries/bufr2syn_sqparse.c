/***************************************************************************
 *   Copyright (C) 2013-2015 by Guillermo Ballester Valor                  *
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
#include "bufr2syn.h"

/*!
  \fn int find_descriptor(int *haystack, size_t nlst, int needle)
  \brief Try to find a descriptor in an array
  \param haystack array of integers as descriptors
  \param nlst number of descriptors in array
  \param needle descriptor to find

  it returns 1 if found, 0 otherwise.
*/
int find_descriptor(int *haystack, size_t nlst, int needle)
{
  size_t i = 0;
  while (haystack[i] != needle && i < nlst)
    i++;
  return (i < nlst);
}


/*!
  \fn int find_descriptor(int *haystack, size_t nlst, int needlemin, int needlemax)
  \brief Try to find a descriptor in an array between two limits
  \param haystack array of integers as descriptors
  \param nlst number of descriptors in array
  \param needlemin minimum descriptor to find
  \param needlemax maximum descriptor to find

  It tries to find a descriptor in haystack which is greater or equal to needlemin and
  lesser or equal to needlemax

  it returns 1 if found, 0 otherwise.
*/
int find_descriptor_interval(int *haystack, size_t nlst, int needlemin, int needlemax)
{
  size_t i = 0;
  while ((haystack[i] > needlemax || haystack[i] < needlemin) && i < nlst)
    i++;
  return (i < nlst);
}

/*!
  \fn int parse_subset_sequence(struct metreport *m, struct bufr_subset_sequence_data *sq, struct bufr_subset_state *st, int *kdtlst, size_t nlst, int *ksec1, char *err)
  \brief Parse a sequence of expanded descriptors for a subset
  \param m pointer to a struct \ref metreport where to set the data  
  \param sq pointer to a struct \ref bufr_subset_sequence_data where the values for sequence of descriptors for a subset has been decoded
  \param st pointer to a struct \ref bufr_subset_state 
  \param kdtlst array of integers with descriptors
  \param nlst number of descriptors in \a kdtlst
  \param ksec1 array of auxiliar integers decoded by bufrdc ECMWF library
  \param err string where to write errors if any
*/
int parse_subset_sequence(struct metreport *m, struct bufr_subset_sequence_data *sq, struct bufr_subset_state *st, struct synop_chunks *synop, struct buoy_chunks *buoy, int *kdtlst, size_t nlst, int *ksec1, char *err)
{

  /* Clean the state */
  memset(st, 0, sizeof(struct bufr_subset_state));

  /* First task to do is figure out the type of report */
  switch (ksec1[5])
    {
    case 0:
      if (find_descriptor_interval(kdtlst, nlst, 307079, 307086) ||
          find_descriptor(kdtlst, nlst,307091) ||
          find_descriptor(kdtlst, nlst,307096) || 
          ksec1[6] == 0 || ksec1[6] == 1 || ksec1[6] == 2)
        strcpy(st->type_report,"AAXX"); // FM-12 synop
      else if (find_descriptor(kdtlst, nlst,307090) ||
          find_descriptor(kdtlst, nlst,301092) ||
          ksec1[6] == 3 || ksec1[6] == 4 || ksec1[6] == 5 )
        strcpy(st->type_report,"OOXX"); // FM-14 synop-mobil
      break;
    case 1:
      if (find_descriptor_interval(kdtlst, nlst, 308004, 308005) ||
          find_descriptor(kdtlst, nlst,301093) ||
          find_descriptor(kdtlst, nlst,308009) || find_descriptor(kdtlst, nlst,1011))
        strcpy(st->type_report,"BBXX"); // FM-13 ship
      else if (find_descriptor_interval(kdtlst, nlst, 308001, 308003) ||
        find_descriptor(kdtlst, nlst,1005) ||
        find_descriptor(kdtlst, nlst,2036) ||
        find_descriptor(kdtlst, nlst,2149) ||
        ksec1[6] == 25)
        strcpy(st->type_report,"ZZYY"); // FM-18 buoy
      break;
    case 2:
      if (find_descriptor_interval(kdtlst, nlst, 309050, 309052))
        strcpy(st->type_report,"TTXX"); // FM-13 ship 
      break;
    default:
      sprintf(err, "The data category %d is not parsed at the moment", ksec1[5]);
      return 1;
    }

  if (st->type_report[0] == '\0')
  {
    sprintf(err, "Cannot find the report type\n");
    return 1;
  }

  //if(DEBUG) 
  //  printf("Going to parse a %s report\n", st->type_report);
  
  // Parse FM-12, FM-13 and FM-14
  if (strcmp(st->type_report,"AAXX") == 0 || strcmp(st->type_report,"BBXX") == 0 || strcmp(st->type_report,"OOXX") == 0)
  {
    if (parse_subset_as_synop(m, synop, st, sq, err) == 0)
      return print_synop(m->alphanum, 2048, synop);
  }
  else if (strcmp(st->type_report,"ZZYY") == 0)
  {
    if (parse_subset_as_buoy(m, buoy, st, sq, err) == 0)
      return print_buoy(m->alphanum, 2048, buoy);
  }

  // when reached this point we have han error
  return 1; 
}
