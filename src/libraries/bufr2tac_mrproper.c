/***************************************************************************
 *   Copyright (C) 2013-2017 by Guillermo Ballester Valor                  *
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
 \file bufr2tac_mrproper.c
 \brief file with the code to clean and init structures
 */
#include "bufr2tac.h"

/*! \fn void clean_syn_sec0(struct synop_sec0 *s)
    \brief clean a synop_sec0 struct
    \param s pointer to the synop_sec0 struct
*/
void clean_syn_sec0 ( struct synop_sec0 *s )
{
  memset ( s, 0, sizeof ( struct synop_sec0 ) );
}

/*! \fn void clean_syn_sec1(struct synop_sec1 *s)
    \brief clean a synop_sec1 struct
    \param s pointer to the synop_sec1 struct
*/
void clean_syn_sec1 ( struct synop_sec1 *s )
{
  memset ( s, 0, sizeof ( struct synop_sec1 ) );
}

/*! \fn void clean_syn_sec2(struct synop_sec2 *s)
    \brief clean a synop_sec2 struct
    \param s pointer to the synop_sec2 struct
*/
void clean_syn_sec2 ( struct synop_sec2 *s )
{
  memset ( s, 0, sizeof ( struct synop_sec2 ) );
}

/*! \fn void clean_syn_sec3(struct synop_sec3 *s)
    \brief clean a synop_sec3 struct
    \param s pointer to the synop_sec3 struct
*/
void clean_syn_sec3 ( struct synop_sec3 *s )
{
  memset ( s, 0, sizeof ( struct synop_sec3 ) );
}

/*! \fn void clean_syn_sec4(struct synop_sec4 *s)
    \brief clean a synop_sec4 struct
    \param s pointer to the synop_sec4 struct
*/
void clean_syn_sec4 ( struct synop_sec4 *s )
{
  memset ( s, 0, sizeof ( struct synop_sec4 ) );
}

/*! \fn void clean_syn_sec5(struct synop_sec5 *s)
    \brief clean a synop_sec5 struct
    \param s pointer to the synop_sec5 struct
*/
void clean_syn_sec5 ( struct synop_sec5 *s )
{
  memset ( s, 0, sizeof ( struct synop_sec5 ) );
}


/*! \fn void clean_report_date_ext(struct report_date_ext *s)
    \brief clean a synop_ext struct
    \param s pointer to the synop_ext struct
*/
void clean_report_date_ext ( struct report_date_ext *s )
{
  memset ( s, 0, sizeof ( struct report_date_ext ) );
}

/*!
  \fn void clean_synop_chunks( struct synop_chunks *syn)
  \brief cleans a \ref synop_chunks struct
  \param syn pointer to the struct to clean
*/
void clean_synop_chunks ( struct synop_chunks *syn )
{
  syn->mask = 0;
  clean_report_date_ext ( & ( syn->e ) );
  clean_syn_sec0 ( & ( syn->s0 ) );
  clean_syn_sec1 ( & ( syn->s1 ) );
  clean_syn_sec2 ( & ( syn->s2 ) );
  clean_syn_sec3 ( & ( syn->s3 ) );
  clean_syn_sec5 ( & ( syn->s5 ) );

  // default
  strcpy ( syn->s0.iw,"/" );
  strcpy ( syn->s1.ir,"/" );
  strcpy ( syn->s1.ix,"/" );
  strcpy ( syn->s1.h,"/" );
  strcpy ( syn->s1.VV,"//" );
  strcpy ( syn->s1.N,"/" );
  strcpy ( syn->s1.dd,"//" );
  strcpy ( syn->s1.ff,"//" );

  syn->error[0] = '\0';
}

/*! \fn void clean_buoy_sec0(struct buoy_sec0 *s)
    \brief clean a buoy_sec0 struct
    \param s pointer to the buoy_sec0 struct
*/
void clean_buoy_sec0 ( struct buoy_sec0 *s )
{
  memset ( s, 0, sizeof ( struct buoy_sec0 ) );
}

/*! \fn void clean_buoy_sec1(struct buoy_sec1 *s)
    \brief clean a buoy_sec1 struct
    \param s pointer to the buoy_sec1 struct
*/
void clean_buoy_sec1 ( struct buoy_sec1 *s )
{
  memset ( s, 0, sizeof ( struct buoy_sec1 ) );
}

/*! \fn void clean_buoy_sec2(struct buoy_sec2 *s)
    \brief clean a buoy_sec2 struct
    \param s pointer to the buoy_sec2 struct
*/
void clean_buoy_sec2 ( struct buoy_sec2 *s )
{
  memset ( s, 0, sizeof ( struct buoy_sec2 ) );
}

/*! \fn void clean_buoy_sec3(struct buoy_sec3 *s)
    \brief clean a buoy_sec3 struct
    \param s pointer to the buoy_sec3 struct
*/
void clean_buoy_sec3 ( struct buoy_sec3 *s )
{
  memset ( s, 0, sizeof ( struct buoy_sec3 ) );
}

/*! \fn void clean_buoy_sec4(struct buoy_sec4 *s)
    \brief clean a buoy_sec4 struct
    \param s pointer to the buoy_sec3 struct
*/
void clean_buoy_sec4 ( struct buoy_sec4 *s )
{
  memset ( s, 0, sizeof ( struct buoy_sec4 ) );
}

/*!
  \fn void clean_buoy_chunks( struct buoy_chunks *b)
  \brief cleans a \ref buoy_chunks struct
  \param b pointer to the struct to clean
*/
void clean_buoy_chunks ( struct buoy_chunks *b )
{
  b->mask = 0;
  clean_report_date_ext ( & ( b->e ) );
  clean_buoy_sec0 ( & ( b->s0 ) );
  clean_buoy_sec1 ( & ( b->s1 ) );
  clean_buoy_sec2 ( & ( b->s2 ) );
  clean_buoy_sec3 ( & ( b->s3 ) );
  clean_buoy_sec4 ( & ( b->s4 ) );

  b->error[0] = '\0';
}

/*!
  \fn void clean_temp_chunks( struct temp_chunks *t)
  \brief cleans a \ref buoy_chunks struct
  \param t pointer to the struct to clean
*/
void clean_temp_chunks ( struct temp_chunks *t )
{
  memset ( t, 0, sizeof ( struct temp_chunks ) );
}

/*! \fn void clean_climat_sec0(struct climat_sec0 *s)
    \brief clean a climat_sec0 struct
    \param s pointer to the climat_sec0 struct
*/
void clean_climat_sec0 ( struct climat_sec0 *s )
{
  memset ( s, 0, sizeof ( struct climat_sec0 ) );
}

/*! \fn void clean_climat_sec1(struct climat_sec1 *s)
    \brief clean a climat_sec1 struct
    \param s pointer to the climat_sec1 struct
*/
void clean_climat_sec1 ( struct climat_sec1 *s )
{
  memset ( s, 0, sizeof ( struct climat_sec1 ) );
}

/*! \fn void clean_climat_sec2(struct climat_sec2 *s)
    \brief clean a climat_sec2 struct
    \param s pointer to the climat_sec2 struct
*/
void clean_climat_sec2 ( struct climat_sec2 *s )
{
  memset ( s, 0, sizeof ( struct climat_sec2 ) );
}

/*! \fn void clean_climat_sec3(struct climat_sec3 *s)
    \brief clean a climat_sec3 struct
    \param s pointer to the climat_sec3 struct
*/
void clean_climat_sec3 ( struct climat_sec3 *s )
{
  memset ( s, 0, sizeof ( struct climat_sec3 ) );
}

/*! \fn void clean_climat_sec4(struct climat_sec4 *s)
    \brief clean a climat_sec4 struct
    \param s pointer to the climat_sec4 struct
*/
void clean_climat_sec4 ( struct climat_sec4 *s )
{
  memset ( s, 0, sizeof ( struct climat_sec4 ) );
}

/*! \fn void clean_climat_old(struct climat_old *s)
    \brief clean a climat_sec4 struct
    \param s pointer to the climat_old struct
*/
void clean_climat_old ( struct climat_old *s )
{
  memset ( s, 0, sizeof ( struct climat_old ) );
}


/*!
  \fn void clean_climat_chunks( struct climat_chunks *c)
  \brief cleans a \ref climat_chunks struct
  \param c pointer to the struct to clean
*/
void clean_climat_chunks ( struct climat_chunks *c )
{
  c->mask = 0;
  clean_report_date_ext ( & ( c->e ) );
  clean_climat_sec0 ( & ( c->s0 ) );
  clean_climat_sec1 ( & ( c->s1 ) );
  clean_climat_sec2 ( & ( c->s2 ) );
  clean_climat_sec3 ( & ( c->s3 ) );
  clean_climat_sec4 ( & ( c->s4 ) ); 
  clean_climat_old ( & (c->o) );
  c->error[0] = '\0';
}
