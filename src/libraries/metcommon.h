/***************************************************************************
 *   Copyright (C) 2004-2018 by Guillermo Ballester Valor                  *
 *   gbv@ogimet.com                                                        *
 *                                                                         *
 *   This file is part of bufr2synop                                       *
 *                                                                         *
 *   Ogimet is free software; you can redistribute it and/or modify        *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   Ogimet is distributed in the hope that it will be useful,             *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, see <http://www.gnu.org/licenses/>.  *
 *                                                                         *
 ***************************************************************************/
/*!
   \file metcommon.h
   \brief Main header file for common report structures
*/
#ifndef METCOMMON_H
#define METCOMMON_H

/*! \struct report_date_ext
    \brief contains extensions, not in wmo. Date/time UTC information
*/
struct report_date_ext
{
  char YYYY[6]; /*!< Year four digits */
  char MM[4]; /*!< Month, two digits 01..12 */
  char DD[4]; /*!< Day of the month, two digits 01..31 */
  char HH[4]; /*!< Hour (UTC) 00..23 */
  char mm[4]; /*!< Minute 00..59 */
  char ss[4]; /*!< Second 00..59 */
};

#ifndef MET_DATETIME
#define MET_DATETIME
/*!
   \struct met_datetime
   \brief stores date and time reference of a report, commonly the observation time
*/
struct met_datetime
{
  time_t t; /*!< Unix instant for report date/time reference */
  struct tm tim;  /*!<  struct tm with report date/time info (UTC) */
  char datime[16]; /*!< date/time reference (UTC) as string with YYYYMMDDHHmm[ss] format */
};
#endif



#endif
