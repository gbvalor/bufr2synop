/***************************************************************************
 *   Copyright (C) 2013-2026 by Guillermo Ballester Valor                  *
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
 \file bufr2tac_sqparse.c
 \brief file with the code to parse a sequence of descriptors for a subset
 */
#include "bufr2tac.h"

/*!
  \fn int find_descriptor(const int *haystack, size_t nlst, int needle)
  \brief Try to find a descriptor in an array
  \param [in] haystack Array of integers as descriptors
  \param [in] nlst Number of descriptors in array
  \param [in] needle Descriptor to find
  \return 1 if found, 0 otherwise
*/
int find_descriptor ( const int *haystack, size_t nlst, int needle )
{
  size_t i = 0;
  while ( ( i < nlst ) && ( haystack[i] != needle ) )
    {
      i++;
    }
  return ( i < nlst );
}


/*!
  \fn int find_descriptor_interval(const int *haystack, size_t nlst, int needlemin, int needlemax)
  \brief Try to find a descriptor in an array between two limits
  \param [in] haystack Array of integers as descriptors
  \param [in] nlst Number of descriptors in array
  \param [in] needlemin Minimum descriptor to find
  \param [in] needlemax Maximum descriptor to find
  \return 1 if found (descriptor >= needlemin and <= needlemax), 0 otherwise
*/
int find_descriptor_interval ( const int *haystack, size_t nlst, int needlemin, int needlemax )
{
  size_t i = 0;

  while ( ( i < nlst ) && ( haystack[i] > needlemax || haystack[i] < needlemin ) )
    {
      i++;
    }
  return ( i < nlst );
}

/*!
  \fn int parse_subset_sequence(struct metreport *m, struct bufr_subset_sequence_data *sq, struct bufr2tac_subset_state *st,  const int *kdtlst, size_t nlst, const int *ksec1, char *err)
  \brief Parse a sequence of expanded descriptors for a subset
  \param [in,out] m Pointer to struct \ref metreport where to set the data
  \param [in] sq Pointer to struct \ref bufr_subset_sequence_data where the values for sequence of descriptors for a subset has been decoded
  \param [in,out] st Pointer to struct \ref bufr2tac_subset_state
  \param [in] kdtlst Array of integers with descriptors
  \param [in] nlst Number of descriptors in kdtlst
  \param [in] ksec1 Array of auxiliary integers decoded by bufrdc ECMWF library
  \param [out] err String where to write errors if any
  \return 0 on success, 1 on error
*/
int parse_subset_sequence ( struct metreport *m, struct bufr_subset_sequence_data *sq, struct bufr2tac_subset_state *st,  const int *kdtlst, size_t nlst, const int *ksec1, char *err )
{
  /* Clean the state */
  memset ( st, 0, sizeof ( struct bufr2tac_subset_state ) );

  /* First task to do is figure out the type of report */
  switch ( ksec1[5] )
    {
    case 0:
      if ( find_descriptor_interval ( kdtlst, nlst, 307071, 307073 ) )
        {
          strcpy ( st->type_report, "CLIMAT" );  // FM-71 CLIMAT
        }
      else if ( find_descriptor_interval ( kdtlst, nlst, 307079, 307086 ) ||
                find_descriptor ( kdtlst, nlst,307091 ) ||
                find_descriptor ( kdtlst, nlst,307092 ) ||
                find_descriptor ( kdtlst, nlst,307096 ) ||
                find_descriptor ( kdtlst, nlst,307182 ) ||
                ksec1[6] == 0 || ksec1[6] == 1 || ksec1[6] == 2 )
        {
          strcpy ( st->type_report,"AAXX" );  // FM-12 synop
        }
      else if ( find_descriptor ( kdtlst, nlst,307090 ) ||
                find_descriptor ( kdtlst, nlst,301092 ) ||
                ksec1[6] == 3 || ksec1[6] == 4 || ksec1[6] == 5 )
        {
          strcpy ( st->type_report,"OOXX" );  // FM-14 synop-mobil
        }
      break;
    case 1:
      if ( find_descriptor_interval ( kdtlst, nlst, 308004, 308005 ) ||
           find_descriptor ( kdtlst, nlst,301093 ) ||
           find_descriptor ( kdtlst, nlst,308009 ) || 
           find_descriptor ( kdtlst, nlst,1011 ) )
        {
          strcpy ( st->type_report,"BBXX" );  // FM-13 ship
        }
      else if ( find_descriptor_interval ( kdtlst, nlst, 308001, 308003 ) ||
                find_descriptor ( kdtlst, nlst,315008 ) ||
                find_descriptor ( kdtlst, nlst,315009 ) ||
                find_descriptor ( kdtlst, nlst,1005 ) ||
                find_descriptor ( kdtlst, nlst,2036 ) ||
                find_descriptor ( kdtlst, nlst,2149 ) ||
                ksec1[6] == 25 )
        {
          strcpy ( st->type_report,"ZZYY" );  // FM-18 buoy
        }
      else if ( find_descriptor_interval ( kdtlst, nlst, 308011, 308013 ) )
        {
          strcpy ( st->type_report, "CLIMAT SHIP" );  // FM-71 CLIMAT SHIP
        }
      else if ( find_descriptor ( kdtlst, nlst,307090 ) )
        {
          // FIXME Some FM-14 are coded as category 1
          strcpy ( st->type_report,"OOXX" );  // FM-14 synop-mobil
        }
      break;
    case 2:
      if ( find_descriptor_interval ( kdtlst, nlst, 309050, 309051 ) )
        {
          strcpy ( st->type_report,"PPXX" );  // PILOT, PILOT SHIP, PILOT DROP or PILOT MOBIL
        }
      else if ( find_descriptor ( kdtlst, nlst, 309052 ) ||
                find_descriptor ( kdtlst, nlst, 309057 ) 
              )
        {
          strcpy ( st->type_report,"TTXX" );  // TEMP, TEMP SHIP, TEMP MOBIL
        }
      break;
    default:
      snprintf ( err, ERR_SIZE, "The data category %d is not parsed at the moment", ksec1[5] );
      return 1;
    }
  
  
  if ( st->type_report[0] == '\0' )
    {
      snprintf ( err, ERR_SIZE, "Cannot find the report type\n" );
      return 1;
    }

  //if(DEBUG)
  //  printf("Going to parse a %s report\n", st->type_report);


  if ( strcmp ( st->type_report,"AAXX" ) == 0 || strcmp ( st->type_report,"BBXX" ) == 0 || strcmp ( st->type_report,"OOXX" ) == 0 )
    {
      // Parse FM-12, FM-13 and FM-14
      if ( parse_subset_as_synop ( m, st, sq, err ) == 0 )
        {
          if (BUFR2TAC_DEBUG_LEVEL)
            bufr2tac_print_error(&st->e);
          return print_synop_report ( m );
        }
        
    }
  else if ( strcmp ( st->type_report,"ZZYY" ) == 0 )
    {
      // parse BUOY
      if ( parse_subset_as_buoy ( m, st, sq, err ) == 0 )
        {
          if (BUFR2TAC_DEBUG_LEVEL)
            bufr2tac_print_error(&st->e);
          return print_buoy_report ( m );
        }
    }
  else if ( strcmp ( st->type_report,"TTXX" ) == 0 )
    {
      // psrse TEMP
      if ( parse_subset_as_temp ( m, st, sq, err ) == 0 )
        {
          if (BUFR2TAC_DEBUG_LEVEL)
            bufr2tac_print_error(&st->e);
          return print_temp_report ( m );
        }
    }
  else if ( strcmp ( st->type_report,"CLIMAT" ) == 0 )
    {
      // psrse CLIMAT
      if ( parse_subset_as_climat ( m, st, sq, err ) == 0 )
        {
          if (BUFR2TAC_DEBUG_LEVEL)
            bufr2tac_print_error(&st->e);
          return print_climat_report ( m );
        }
    }

  // when reached this point we have han error
  if (BUFR2TAC_DEBUG_LEVEL)
    bufr2tac_print_error(&st->e);
  return 1;
}
