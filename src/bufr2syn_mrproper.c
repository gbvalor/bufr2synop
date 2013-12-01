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
 \file bufr2syn_mrproper.c
 \brief file with the code to clean and init structures 
 */
#include "bufr2synop.h"

/*! \fn void clean_syn_sec0(struct synop_sec0 *s)
    \brief clean a synop_sec0 struct
    \param s pointer to the synop_sec0 struct
*/
void clean_syn_sec0(struct synop_sec0 *s)
{
   memset(s, 0, sizeof(struct synop_sec0));
}

/*! \fn void clean_syn_sec1(struct synop_sec1 *s)
    \brief clean a synop_sec1 struct
    \param s pointer to the synop_sec1 struct
*/
void clean_syn_sec1(struct synop_sec1 *s)
{
   memset(s, 0, sizeof(struct synop_sec1));
}

/*! \fn void clean_syn_sec2(struct synop_sec2 *s)
    \brief clean a synop_sec2 struct
    \param s pointer to the synop_sec2 struct
*/
void clean_syn_sec2(struct synop_sec2 *s)
{
   memset(s, 0, sizeof(struct synop_sec2));
}

/*! \fn void clean_syn_sec3(struct synop_sec3 *s)
    \brief clean a synop_sec3 struct
    \param s pointer to the synop_sec3 struct
*/
void clean_syn_sec3(struct synop_sec3 *s)
{
   memset(s, 0, sizeof(struct synop_sec3));
}

/*! \fn void clean_report_date_ext(struct report_date_ext *s)
    \brief clean a synop_ext struct
    \param s pointer to the synop_ext struct
*/
void clean_report_date_ext(struct report_date_ext *s)
{
  memset(s, 0, sizeof(struct report_date_ext));
}

/*!
  \fn void clean_synop_chunks( struct synop_chunks *syn)
  \brief cleans a \ref synop_chunks struct
  \param syn pointer to the struct to clean
*/
void clean_synop_chunks( struct synop_chunks *syn)
{
  syn->mask = 0;
  clean_report_date_ext( &(syn->e));
  clean_syn_sec0( &(syn->s0));
  clean_syn_sec1( &(syn->s1));
  clean_syn_sec3( &(syn->s3));
  syn->error[0] = '\0';
}

