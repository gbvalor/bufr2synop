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
 \file bufr2tac_x20.c
 \brief decodes the descriptors with X = 20 (Observed phenomena)
 */
#include "bufr2tac.h"

/*!
  \fn char * percent_to_okta ( char *target, double perc )
  \brief Converts percent cloud cover into okta
  \param perc the precent cloud cover
  \param target the resulting okta string
*/
char * percent_to_okta ( char *target, double perc )
{
  if ( perc < 12.5 )
    {
      strcpy ( target,"0" );
    }
  else if ( perc < 25.0 )
    {
      strcpy ( target,"1" );
    }
  else if ( perc < 37.5 )
    {
      strcpy ( target,"2" );
    }
  else if ( perc < 50.0 )
    {
      strcpy ( target,"3" );
    }
  else if ( perc < 62.5 )
    {
      strcpy ( target,"4" );
    }
  else if ( perc < 75.0 )
    {
      strcpy ( target,"5" );
    }
  else if ( perc < 87.5 )
    {
      strcpy ( target,"6" );
    }
  else if ( perc < 100.0 )
    {
      strcpy ( target,"7" );
    }
  else if ( perc <= 112.5 )
    {
      strcpy ( target,"8" );
    }
  else if ( perc == 113.0 )
    {
      strcpy ( target,"9" );
    }
  return target;
}

/*!
  \fn char * m_to_h ( char *target, double h )
  \brief converts the altitude of cloud layer into h string code
  \param h the altitude in meters
  \param target the resulting h coded string
*/
char * m_to_h ( char *target, double h )
{
  if ( h < 50.0 )
    {
      strcpy ( target,"0" );
    }
  else if ( h < 100.0 )
    {
      strcpy ( target,"1" );
    }
  else if ( h < 200.0 )
    {
      strcpy ( target,"2" );
    }
  else if ( h < 300.0 )
    {
      strcpy ( target,"3" );
    }
  else if ( h < 600.0 )
    {
      strcpy ( target,"4" );
    }
  else if ( h < 1000.0 )
    {
      strcpy ( target,"5" );
    }
  else if ( h < 1500.0 )
    {
      strcpy ( target,"6" );
    }
  else if ( h < 2000.0 )
    {
      strcpy ( target,"7" );
    }
  else if ( h < 2500.0 )
    {
      strcpy ( target,"8" );
    }
  else
    {
      strcpy ( target,"9" );
    }
  return target;

}

/*!
  \fn char * m_to_hh ( char *target, double h )
  \brief converts the altitude of cloud layer into hh string code
  \param h the altitude in meters
  \param target the resulting h coded string
*/
char * m_to_hh ( char *target, double h )
{
  int ih = ( int ) h;

  if ( ih <= 1500 )
    {
      sprintf ( target, "%02d", ih / 30 );
    }
  else if ( ih <= 9000 )
    {
      sprintf ( target, "%2d", ( ih / 300 ) + 50 );
    }
  else if ( ih <= 21000 )
    {
      sprintf ( target, "%2d", ( ih / 500 ) + 50 );
    }
  else
    {
      strcpy ( target,"89" );
    }
  return target;

}


/*!
  \fn char * vism_to_VV ( char *target, double V )
  \brief Convert horizontal visibilty in meters to a VV string
  \param V the visibility (m)
  \param target the resulting VV string
*/
char * vism_to_VV ( char *target, double V )
{
  if ( V < 100.0 )
    {
      strcpy ( target, "00" );
    }
  else if ( V <= 5000.0 )
    {
      sprintf ( target, "%02d", ( int ) ( V + 0.1 ) / 100 );
    }
  else if ( V < 6000.0 )
    {
      sprintf ( target, "50" );  // this is to avoid 51-55 range
    }
  else if ( V <= 30000.0 )
    {
      sprintf ( target, "%02d", ( int ) ( V + 0.1 ) / 1000 + 50 );
    }
  else if ( V <= 70000.0 )
    {
      sprintf ( target, "%02d", ( int ) ( V - 30000.0 ) / 5000 + 80 );
    }
  else
    {
      strcpy ( target, "89" );
    }
  return target;
}

/*!
  \fn char * m_to_RR ( char *target, double m )
  \brief Convert distance (m) in RR code (3570)
  \param m the distance/diameter (m)
  \param target the resulting RR string
*/
char * m_to_RR ( char *target, double m )
{
  int ix;
  ix = ( int ) ( m * 1000.0 + 0.5 );

  if ( m > 0.4 )
    {
      strcpy ( target, "98" );
    }
  if ( m > 0.0 && m < 0.00065 )
    {
      sprintf ( target, "%02d", ( int ) ( m * 10000.0 + 0.5 ) );
    }
  else if ( ix <= 55 )
    {
      sprintf ( target, "%02d", ix );
    }
  else if ( ix <= 400 )
    {
      sprintf ( target, "%02d", ( ix / 10 ) + 50 );
    }
  else
    {
      strcpy ( target, "99" );
    }
  return target;
}


/*!
  \fn int syn_parse_x20 ( struct synop_chunks *syn, struct bufr2tac_subset_state *s )
  \brief Parse a expanded descriptor with X = 20
  \param syn pointer to a struct \ref synop_chunks where to set the results
  \param s pointer to a struct \ref bufr2tac_subset_state where is stored needed information in sequential analysis

  It returns 0 if success, 1 if problems when processing. If a descriptor is not processed returns 0 anyway
*/
int syn_parse_x20 ( struct synop_chunks *syn, struct bufr2tac_subset_state *s )
{
  char aux[16];

  switch ( s->a->desc.y )
    {
    case 1: // 0 20 001 . Horizontal visibility
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          return 0;
        }
      vism_to_VV ( syn->s1.VV, s->val );
      break;

    case 3: // 0 20 003 . Present weather
      // there are some cases
      // Case 1)
      // On current templates, sec1 WW is NOT preceeded by a time displacament descriptor
      // In some national cases, a further complementary info about weather conditions is
      // precedeed by two time displacament descriptors giving the time interval from where
      // significant weather is described. In such cases we will use 902tt and 903tt, or
      // 964.. if the period macthes with W1W2 period
      // If WW = 20-29 and period matches last hour we then use 962.. group
      // as the indication of period and 966WW as weather
      if ( ( s->i - 1 ) == s->k_itval  && ( s->i - 2 ) == s->k_jtval )
        {
          // check missing value
          if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
            {
              return 0;
            }
          // check no significan weather.
          if ( s->ival == 100 || s->ival == 508 )
            {
              return 0;
            }

          // parse period
          if ( s->itval == 0 && s->jtval == s->tw1w2 )
            {
              // Case of W1W2 interval
              if ( syn->s3.d9.n  == SYNOP_NMISC )
                {
                  return 0;
                }
              sprintf ( syn->s3.d9.misc[syn->s3.d9.n].SpSp, "964" );
              s->SnSn = 964;
            }
          else if ( s->itval == 0 && s->jtval == -3600 && syn->s1.ww[0] == '2' )
            {
              // case of ww 20-29 and period equal to last hour. We will use 962...
              if ( syn->s3.d9.n  == SYNOP_NMISC )
                {
                  return 0;
                }
              sprintf ( syn->s3.d9.misc[syn->s3.d9.n].SpSp, "962" );
              s->SnSn = 962;
            }
          else
            {
              if ( ( syn->s3.d9.n + 2 ) >= SYNOP_NMISC )
                {
                  return 0;
                }
              sprintf ( syn->s3.d9.misc[syn->s3.d9.n].SpSp, "902" );
              secs_to_tt ( syn->s3.d9.misc[syn->s3.d9.n].spsp, s->jtval );
              syn->s3.d9.n++;
              sprintf ( syn->s3.d9.misc[syn->s3.d9.n].SpSp, "903" );
              secs_to_tt ( syn->s3.d9.misc[syn->s3.d9.n].spsp, s->itval );
              syn->s3.d9.n++;
              sprintf ( syn->s3.d9.misc[syn->s3.d9.n].SpSp, "966" );
              s->SnSn = 966;
            }

          // now set the value
          if ( s->ival < 100 )
            {
              sprintf ( syn->s3.d9.misc[syn->s3.d9.n].spsp , "%02d", s->ival );
              syn->mask |= SYNOP_SEC3;
            }
          else if ( s->ival < 200 )
            {
              sprintf ( syn->s3.d9.misc[syn->s3.d9.n].spsp, "%02d", s->ival % 100 );
              syn->mask |= SYNOP_SEC3;
            }
          syn->s3.d9.n++;
        }

      //  Case 2)
      // Complementary present weather after 7wwW1W1 has been set.
      // If last itval == 0 the we assume that the info should be set on 960.. groups
      else if ( syn->s1.ww[0] && s->itval == 0 )
        {
          // check if overflowed 9 struct
          if ( syn->s3.d9.n == SYNOP_NMISC )
            {
              return 0;
            }
          sprintf ( syn->s3.d9.misc[syn->s3.d9.n].SpSp, "960" );
          s->SnSn = 960;

          if ( s->ival < 100 )
            {
              sprintf ( syn->s3.d9.misc[syn->s3.d9.n].spsp , "%02d", s->ival );
              syn->mask |= SYNOP_SEC3;
            }
          else if ( s->ival < 200 )
            {
              sprintf ( syn->s3.d9.misc[syn->s3.d9.n].spsp, "%02d", s->ival % 100 );
              syn->mask |= SYNOP_SEC3;
            }
          syn->s3.d9.n++;
        }
      else
        {
          // 7wwW1W2 group
          if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
            {
              s->mask |= ( SUBSET_MASK_HAVE_NO_SIGNIFICANT_WW | SUBSET_MASK_HAVE_NO_SIGNIFICANT_W1 | SUBSET_MASK_HAVE_NO_SIGNIFICANT_W2 );
              return 0;
            }
          if ( s->ival < 100 )
            {
              if ( syn->s1.ix[0] == '/' )
                {
                  strcpy ( syn->s1.ix,"1" );
                }
              sprintf ( syn->s1.ww, "%02d", s->ival );
              syn->mask |= SYNOP_SEC1;
            }
          else if ( s->ival == 100 )
            {
              s->mask |=  SUBSET_MASK_HAVE_NO_SIGNIFICANT_WW;
            }
          else if ( s->ival < 200 )
            {
              if ( syn->s1.ix[0] == '/' )
                {
                  strcpy ( syn->s1.ix,"7" );
                }
              sprintf ( syn->s1.ww, "%02d", s->ival % 100 );
              syn->mask |= SYNOP_SEC1;
            }
          else if ( s->ival == 508 )
            {
              s->mask |= ( SUBSET_MASK_HAVE_NO_SIGNIFICANT_WW | SUBSET_MASK_HAVE_NO_SIGNIFICANT_W1 | SUBSET_MASK_HAVE_NO_SIGNIFICANT_W2 );
            }
          //printf("%s%s* %s\n", syn->s0.II, syn->s0.iii, syn->s1.ww);
        }
      break;

    case 4: // 0 20 004 . Past weather (1)
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          s->mask |=  SUBSET_MASK_HAVE_NO_SIGNIFICANT_W1;
          return 0;
        }

      // stores w1w2 period
      s->tw1w2 = s->itval;
      if ( s->ival < 10 )
        {
          if ( syn->s1.ix[0] == '/' )
            {
              strcpy ( syn->s1.ix,"1" );
            }
          sprintf ( syn->s1.W1, "%d", s->ival );
          syn->mask |= SYNOP_SEC1;
        }
      else if ( s->ival == 10 )
        {
          s->mask |= SUBSET_MASK_HAVE_NO_SIGNIFICANT_W1;
        }
      else
        {
          if ( syn->s1.ix[0] == '/' )
            {
              strcpy ( syn->s1.ix,"7" );
            }
          sprintf ( syn->s1.W1, "%d", s->ival % 10 );
          syn->mask |= SYNOP_SEC1;
        }
      break;

    case 5: // 0 20 005 . Past weather (2)
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          s->mask |=  SUBSET_MASK_HAVE_NO_SIGNIFICANT_W2;
          return 0;
        }
      if ( s->ival < 10 )
        {
          if ( syn->s1.ix[0] == '/' )
            {
              strcpy ( syn->s1.ix,"1" );
            }
          sprintf ( syn->s1.W2, "%d", s->ival );
          syn->mask |= SYNOP_SEC1;
        }
      else if ( s->ival == 10 )
        {
          s->mask |= SUBSET_MASK_HAVE_NO_SIGNIFICANT_W2;
        }
      else
        {
          if ( syn->s1.ix[0] == '/' )
            {
              strcpy ( syn->s1.ix,"7" );
            }
          sprintf ( syn->s1.W2, "%d", s->ival % 10 );
          syn->mask |= SYNOP_SEC1;
        }
      break;

    case 10: // 0 20 010 . Cloud cover (total)
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          return 0;
        }
      percent_to_okta ( syn->s1.N, s->val );
      break;

    case 11: // 0 20 011 . Cloud amount
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          return 0;
        }
      if ( s->clayer == 0 || s->clayer == 5 || s->clayer == 7 || s->clayer == 8 || s->clayer == 62)
        {
          if ( s->ival <= 8 )
            {
              sprintf ( syn->s1.Nh, "%1d", s->ival );
            }
          else if ( s->ival <= 10 )
            {
              sprintf ( syn->s1.Nh, "9" );
            }
          else if ( s->ival == 15 )
            {
              sprintf ( syn->s1.Nh, "/" );
            }
          syn->mask |= SYNOP_SEC1;
        }
      else if ( s->clayer < 0 )
        {
          if ( s->ival <= 8 )
            {
              sprintf ( syn->s4.N1, "%1d", s->ival );
            }
          else if ( s->ival <= 10 )
            {
              sprintf ( syn->s4.N1, "9" );
            }
          else if ( s->ival == 15 )
            {
              sprintf ( syn->s4.N1, "/" );
            }
          syn->mask |= SYNOP_SEC4;
        }
      else if ( s->clayer > 0 && s->clayer < 5 )
        {
          if ( s->ival <= 8 )
            {
              sprintf ( syn->s3.nub[s->clayer - 1].Ns, "%1d", s->ival );
            }
          else if ( s->ival <= 10 )
            {
              sprintf ( syn->s3.nub[s->clayer - 1].Ns, "9" );
            }
          else if ( s->ival == 15 )
            {
              sprintf ( syn->s3.nub[s->clayer - 1].Ns, "/" );
            }
          syn->mask |= SYNOP_SEC3;
        }
      break;

    case 12: // 0 20 012 . Cloud type
      if ( s->clayer == 0 || s->clayer == 5 || s->clayer == 7 || s->clayer == 8 || s->clayer == 62)
        {
          if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
            {
              return 0;
            }
          if ( s->ival < 10 )
            {
              sprintf ( syn->s3.C, "%d", s->ival );
              syn->mask |= SYNOP_SEC3;
            }
          else if ( s->ival >= 10 && s->ival < 20 )
            {
              sprintf ( syn->s1.Ch, "%1d", s->ival % 10 );
              syn->mask |= SYNOP_SEC1;
            }
          else if ( s->ival >= 20 && s->ival < 30 )
            {
              sprintf ( syn->s1.Cm, "%1d", s->ival % 10 );
              syn->mask |= SYNOP_SEC1;
            }
          else if ( s->ival >= 30 && s->ival < 40 )
            {
              sprintf ( syn->s1.Cl, "%1d", s->ival % 10 );
              syn->mask |= SYNOP_SEC1;
            }
          else if ( s->ival == 59 )
            {
              sprintf ( syn->s1.Nh, "/" );
              syn->mask |= SYNOP_SEC1;
            }
          else if ( s->ival == 60 )
            {
              sprintf ( syn->s1.Ch, "/" );
              syn->mask |= SYNOP_SEC1;
            }
          else if ( s->ival == 61 )
            {
              sprintf ( syn->s1.Cm, "/" );
              syn->mask |= SYNOP_SEC1;
            }
          else if ( s->ival == 62 )
            {
              sprintf ( syn->s1.Cl, "/" );
              syn->mask |= SYNOP_SEC1;
            }
        }
      else if ( s->clayer < 0 )
        {
          // case of base of clouds below station level
          if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
            {
              return 0;
            }
          if ( s->ival == 59 )
            {
              sprintf ( syn->s4.C1, "/" );
              syn->mask |= SYNOP_SEC4;
            }
          else if ( s->ival < 10 )
            {
              sprintf ( syn->s4.C1, "%1d", s->ival );
              syn->mask |= SYNOP_SEC4;
            }
        }
      else if ( s->clayer > 0 && s->clayer < 5 )
        {
          if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
            {
              sprintf ( syn->s3.nub[s->clayer - 1].C, "/" );
              return 0;
            }
          if ( s->ival == 59 || s->ival >= 10 )
            {
              sprintf ( syn->s3.nub[s->clayer - 1].C, "/" );
            }
          else
            {
              // C clouds for 8 groups SEC 3
              sprintf ( syn->s3.nub[s->clayer - 1].C, "%1d", s->ival % 10 );
            }
          syn->mask |= SYNOP_SEC3;
        }
      break;

    case 13: // 0 20 013 . Height of base of cloud
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          return 0;
        }
      if ( s->clayer == 0 || s->clayer == 7 || s->clayer == 5 || s->clayer == 62 ) // first layer or low layer is for sec1
        {
          m_to_h ( syn->s1.h, s->val );
        }
      else if ( s->clayer > 0 && s->clayer < 5 )
        {
          m_to_hh ( syn->s3.nub[s->clayer - 1].hshs, s->val );
        }
      break;

    case 14: // 0 20 014 . Height of Top of cloud
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          return 0;
        }
      if ( s->clayer < 0 ) // base below station level
        {
          m_to_hh ( syn->s4.H1H1, s->val );
          syn->mask |= SYNOP_SEC4;
        }
      break;

    case 17:
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          return 0;
        }
      if ( s->clayer < 0 && s->ival < 10 ) // base below station level
        {
          sprintf ( syn->s4.Ct, "%d", s->ival );
          syn->mask |= SYNOP_SEC4;
        }
      break;

    case 21: // 0 20 021 . Type of precipitation
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          return 0;
        }

      if ( syn->s0.A1[0] == '6' ) // for Reg VI
        {
          // flag table width = 30 bits
          if ( s->ival & ( 1 << ( 30 - 15 ) ) &&
               ( s->ival & ( 1 << ( 30 - 16 ) ) ) == 0  &&
               ( s->ival & ( 1 << ( 30 - 20 ) ) ) == 0
             )
            {
              // Only Glaze
              strcpy ( syn->s3.d9.misc[syn->s3.d9.n].SpSp, "934" );
              s->SnSn = 934;
              // here we increase counter
              syn->s3.d9.n++;
            }
          else if ( s->ival & ( 1 << ( 30 - 16 ) ) &&
                    ( s->ival & ( 1 << ( 30 - 15 ) ) ) == 0  &&
                    ( s->ival & ( 1 << ( 30 - 20 ) ) ) == 0
                  )
            {
              // Only Rime
              strcpy ( syn->s3.d9.misc[syn->s3.d9.n].SpSp, "935" );
              s->SnSn = 935;
              // here we increase counter
              syn->s3.d9.n++;
            }
          else if ( s->ival & ( 1 << ( 30 - 20 ) ) &&
                    ( s->ival & ( 1 << ( 30 - 15 ) ) ) == 0  &&
                    ( s->ival & ( 1 << ( 30 - 16 ) ) ) == 0
                  )
            {
              // Only Wet snow
              strcpy ( syn->s3.d9.misc[syn->s3.d9.n].SpSp, "937" );
              s->SnSn = 937;
              // here we increase counter
              syn->s3.d9.n++;
            }
          else if ( s->ival & ( 1 << ( 30 - 15 ) ) ||
                    s->ival & ( 1 << ( 30 - 16 ) ) ||
                    s->ival & ( 1 << ( 30 - 20 ) ) )
            {
              // Compound deposit
              strcpy ( syn->s3.d9.misc[syn->s3.d9.n].SpSp, "936" );
              s->SnSn = 936;
              // here we increase counter
              syn->s3.d9.n++;
            }
        }
      break;

    case 23: // 0 20 023 . Other weather phenomena
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          return 0;
        }

      if ( syn->s0.A1[0] == '6' ) // for Reg VI
        {
          // flag table width = 18 bits
          if ( s->ival & ( 1 << ( 18 - 1 ) ) )
            {
              // Dust/sand whirl
              syn->s3.d9.misc[syn->s3.d9.n].spsp[0] = '5'; // moderate intensity
              syn->s3.d9.misc[syn->s3.d9.n].spsp[1] = '/'; // at the moment
              strcpy ( syn->s3.d9.misc[syn->s3.d9.n].SpSp, "919" );
              s->SnSn = 919;
              // here we increase counter
              syn->s3.d9.n++;
            }
          else if ( ( s->ival & ( 1 << ( 18 - 9 ) ) ) ||
                    ( s->ival & ( 1 << ( 18 - 10 ) ) )
                  )
            {
              // Funnel clouds
              syn->s3.d9.misc[syn->s3.d9.n].spsp[0] = '3'; // assumed less than 3 Km from station
              syn->s3.d9.misc[syn->s3.d9.n].spsp[1] = '/'; // at the moment
              strcpy ( syn->s3.d9.misc[syn->s3.d9.n].SpSp, "919" );
              s->SnSn = 919;
              // here we increase counter
              syn->s3.d9.n++;
            }
          else if ( s->ival & ( 1 << ( 18 - 12 ) ) )
            {
              syn->s3.d9.misc[syn->s3.d9.n].spsp[0] = '0'; // assumed less than 3 Km from station
              syn->s3.d9.misc[syn->s3.d9.n].spsp[1] = '/'; // at the moment
              strcpy ( syn->s3.d9.misc[syn->s3.d9.n].SpSp, "919" );
              s->SnSn = 919;
              // here we increase counter
              syn->s3.d9.n++;
            }
          else if ( s->ival & ( 1 << ( 18 - 2 ) ) )
            {
              syn->s3.d9.misc[syn->s3.d9.n].spsp[0] = '2'; // FIXME What kind of squall ?
              syn->s3.d9.misc[syn->s3.d9.n].spsp[1] = '/'; // at the moment
              strcpy ( syn->s3.d9.misc[syn->s3.d9.n].SpSp, "918" );
              s->SnSn = 918;
              // here we increase counter
              syn->s3.d9.n++;
            }
        }
      break;

    case 24: // 0 20 024 . Intensity of phenomena
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          return 0;
        }
      if ( syn->s0.A1[0] == '6' ) // for Reg VI
        {
          if ( s->SnSn == 919 )
            {
              if ( syn->s3.d9.misc[syn->s3.d9.n - 1].spsp[0] == '5' )
                {
                  if ( s->ival == 1 )
                    {
                      syn->s3.d9.misc[syn->s3.d9.n - 1].spsp[0] = '4';
                    }
                  else if ( s->ival == 3 || s->ival == 4 )
                    {
                      syn->s3.d9.misc[syn->s3.d9.n - 1].spsp[0] = '6';
                    }
                }
            }
        }
      break;

    case 25: // 0 20 025 . Obscuration
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          return 0;
        }

      if ( syn->s0.A1[0] == '6' ) // for Reg VI
        {
          // flag table width = 21 bits
          if ( s->ival & ( 1 << ( 21 - 1 ) ) )
            {
              // Dust/sand whirl
              syn->s3.d9.misc[syn->s3.d9.n].spsp[0] = '1'; // We next willl modify
              strcpy ( syn->s3.d9.misc[syn->s3.d9.n].SpSp, "929" );
              s->SnSn = 929;
              // here we increase counter
              syn->s3.d9.n++;
            }

        }
      break;

    case 26: // 9 29 026 . Character of obscuration
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          return 0;
        }
      if ( syn->s0.A1[0] == '6' ) // for Reg VI
        {
          if ( s->SnSn == 929 )
            {
              if ( s->ival == 6 )
                {
                  syn->s3.d9.misc[syn->s3.d9.n - 1].spsp[0] = '6';
                }
            }
        }
      break;

    case 27: // 0 20 027 . Phenomenon occurrence
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          if ( syn->s0.A1[0] == '6' ) // for Reg VI
            {
              // flag table width = 9 bits
              if ( s->SnSn == 919 || s->SnSn == 918 )
                {
                  syn->s3.d9.misc[syn->s3.d9.n - 1].SpSp[0] = 0;
                }
            }
          return 0;
        }
      if ( syn->s0.A1[0] == '6' ) // for Reg VI
        {
          // flag table width = 9 bits
          if ( s->SnSn == 919 || s->SnSn == 918 )
            {
              if ( ( s->ival && ( 1 << ( 9 - 3 ) ) ) == 0 )
                {
                  syn->s3.d9.misc[syn->s3.d9.n - 1].SpSp[0] = 0;  // No valid group
                }
            }
        }
      break;

    case 40: // 9 29 040 . Evolution of drift of snow
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          return 0;
        }
      if ( syn->s0.A1[0] == '6' ) // for Reg VI
        {
          if ( s->SnSn == 929 )
            {
              sprintf ( aux, "%d", s->ival % 10 );
              syn->s3.d9.misc[syn->s3.d9.n - 1].spsp[1] = aux[0]; // S8'
            }
        }
      break;

    case 54: // 0 20 054 . True direction from which clouds are moving
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          return 0;
        }
      if ( syn->s0.A1[0] == '6' ) // for Reg VI
        {
          grad_to_D ( aux, s->val );
          if ( s->SnSn == 919 || s->SnSn == 918 )
            {
              syn->s3.d9.misc[syn->s3.d9.n - 1].spsp[1] = aux[0];
            }
          else if ( s->clayer == 7 )
            {
              grad_to_D ( syn->s3.Dl, s->val );
            }
          else if ( s->clayer == 8 )
            {
              grad_to_D ( syn->s3.Dm, s->val );
            }
          else if ( s->clayer == 9 )
            {
              grad_to_D ( syn->s3.Dh, s->val );
            }
          syn->mask |= SYNOP_SEC3;
        }
      else if ( syn->s0.A1[0] == '4' ) // for Reg IV
        {
          grad_to_D ( aux, s->val );
          if ( s->clayer == 7 )
            {
              syn->s3.XoXoXoXo[1] = aux[0];
            }
          else if ( s->clayer == 8 )
            {
              syn->s3.XoXoXoXo[2] = aux[0];
            }
          else if ( s->clayer == 9 )
            {
              syn->s3.XoXoXoXo[3] = aux[0];
            }
          syn->mask |= SYNOP_SEC3;
        }
      else  // other Regions
        {
          if ( s->clayer == 7 )
            {
              grad_to_D ( syn->s3.Dl, s->val );
            }
          else if ( s->clayer == 8 )
            {
              grad_to_D ( syn->s3.Dm, s->val );
            }
          else if ( s->clayer == 9 )
            {
              grad_to_D ( syn->s3.Dh, s->val );
            }
          syn->mask |= SYNOP_SEC3;
        }
      break;

    case 55: // 0 20 055 . State of sky in tropics
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          return 0;
        }
      if ( strcmp ( syn->s0.A1, "4" ) == 0 )
        {
          // For REG IV
          if ( s->ival < 10 )
            {
              sprintf ( aux,"%d", s->ival );
              syn->s3.XoXoXoXo[0] = aux[0];
            }
          else
            {
              syn->s3.XoXoXoXo[0] = '/';
            }
          // then copy data direction of cloud drift from 3 02 047 if any
          if ( syn->s3.Dl[0] )
            {
              syn->s3.XoXoXoXo[1] = syn->s3.Dl[0];
            }
          if ( syn->s3.Dm[0] )
            {
              syn->s3.XoXoXoXo[1] = syn->s3.Dm[0];
            }
          if ( syn->s3.Dh[0] )
            {
              syn->s3.XoXoXoXo[1] = syn->s3.Dh[0];
            }
          syn->mask |= SYNOP_SEC3;
        }
      break;

    case 62: // 0 20 062 . State of the ground (with or without snow)
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          return 0;
        }
      if ( strcmp ( syn->s0.A1, "2" ) == 0 )
        {
          // For REG II
          if ( s->ival < 10 )
            {
              sprintf ( aux,"%d", s->ival );
              sprintf ( syn->s3.E, "%d", s->ival );
              syn->s3.XoXoXoXo[0] = aux[0];
            }
          else if ( s->ival < 20 )
            {
              syn->s3.XoXoXoXo[0] = '/';
              sprintf ( syn->s3.E1,"%d", s->ival % 10 );
            }
          if ( syn->s3.XoXoXoXo[1] == 0 )
            {
              // In case of still no snTgTg
              syn->s3.XoXoXoXo[1] = '/';
              syn->s3.XoXoXoXo[2] = '/';
              syn->s3.XoXoXoXo[3] = '/';
            }
        }
      else
        {
          // The Other regs
          if ( s->ival < 10 )
            {
              sprintf ( syn->s3.E,"%d", s->ival );
            }
          else if ( s->ival < 20 )
            {
              sprintf ( syn->s3.E1,"%d", s->ival % 10 );
            }
        }
      syn->mask |= SYNOP_SEC3;
      break;

    case 66: // 0 20 066 . Max diameter of hailstones
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          return 0;
        }
      if ( strcmp ( syn->s0.A1, "1" ) == 0 ) // Only for Region I
        {
          m_to_RR ( syn->s3.d9.misc[syn->s3.d9.n].spsp, s->val );
          strcpy ( syn->s3.d9.misc[syn->s3.d9.n].SpSp, "932" );
          s->SnSn = 932;
          // here we increase counter
          syn->s3.d9.n++;
        }
      break;

    case 67: // 0 20 067 . Diameter of deposit
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          return 0;
        }
      if ( strcmp ( syn->s0.A1, "1" ) == 0 ) // Only for Region I
        {
          m_to_RR ( syn->s3.d9.misc[syn->s3.d9.n - 1].spsp, s->val );
        }
      break;

    case 101: // 0 20 101. Locust (acridian) name
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          return 0;
        }
      if ( strcmp ( syn->s0.A1, "1" ) == 0 ) // Only for Region I
        {
          if ( s->ival < 10 )
            {
              sprintf ( aux, "%d", s->ival );
              syn->s3.R8[0][0] = aux[0]; // Ln
              syn->mask |= ( SYNOP_SEC3 | SYNOP_SEC3_8 );
            }
        }
      break;

    case 102: // 0 20 102. Locust (maturity) color
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          return 0;
        }
      if ( strcmp ( syn->s0.A1, "1" ) == 0 ) // Only for Region I
        {
          if ( s->ival < 10 )
            {
              sprintf ( aux, "%d", s->ival );
              syn->s3.R8[0][1] = aux[0]; // Lc
              syn->mask |= ( SYNOP_SEC3 | SYNOP_SEC3_8 );
            }
        }
      break;

    case 103: // 0 20 103. Stage of development of locusts
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          return 0;
        }
      if ( strcmp ( syn->s0.A1, "1" ) == 0 ) // Only for Region I
        {
          if ( s->ival < 10 )
            {
              sprintf ( aux, "%d", s->ival );
              syn->s3.R8[0][2] = aux[0]; // Ld
              syn->mask |= ( SYNOP_SEC3 | SYNOP_SEC3_8 );
            }
        }
      break;

    case 104: // 0 20 104. Organization state of swarm or band of locusts
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          return 0;
        }
      if ( strcmp ( syn->s0.A1, "1" ) == 0 ) // Only for Region I
        {
          if ( s->ival < 10 )
            {
              sprintf ( aux, "%d", s->ival );
              syn->s3.R8[0][3] = aux[0]; // Lg
              syn->mask |= ( SYNOP_SEC3 | SYNOP_SEC3_8 );
            }
        }
      break;

    case 105: // 0 20 105. Size of swarm or band of locusts and duration of
      // passage of swarm
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          return 0;
        }
      if ( strcmp ( syn->s0.A1, "1" ) == 0 ) // Only for Region I
        {
          if ( s->ival < 10 )
            {
              sprintf ( aux, "%d", s->ival );
              syn->s3.R8[1][0] = aux[0]; // Sl
              syn->mask |= ( SYNOP_SEC3 | SYNOP_SEC3_8 );
            }
        }
      break;

    case 106: // 0 20 106. Locust population density
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          return 0;
        }
      if ( strcmp ( syn->s0.A1, "1" ) == 0 ) // Only for Region I
        {
          if ( s->ival < 10 )
            {
              sprintf ( aux, "%d", s->ival );
              syn->s3.R8[1][1] = aux[0]; // dl
              syn->mask |= ( SYNOP_SEC3 | SYNOP_SEC3_8 );
            }
        }
      break;

    case 107: // 0 20 107. Direction of movements of locust swarm
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          return 0;
        }
      if ( strcmp ( syn->s0.A1, "1" ) == 0 ) // Only for Region I
        {
          if ( s->ival < 10 )
            {
              sprintf ( aux, "%d", s->ival );
              syn->s3.R8[1][1] = aux[0]; // Dl
              syn->mask |= ( SYNOP_SEC3 | SYNOP_SEC3_8 );
            }
        }
      break;

    case 108: // 0 20 108. Extent of vegetation
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          return 0;
        }
      if ( strcmp ( syn->s0.A1, "1" ) == 0 ) // Only for Region I
        {
          if ( s->ival < 8 )
            {
              sprintf ( aux, "%d", s->ival );
              syn->s3.R8[1][1] = aux[0]; // Dl
              syn->mask |= ( SYNOP_SEC3 | SYNOP_SEC3_8 );
            }
        }
      break;

    default:
      break;
    }

  // check about h
  if ( syn->s1.h[0] == '/' && syn->s1.N[0] == '0' )
    {
      syn->s1.h[0] = '9';
    }

  return 0;
}

/*!
  \fn int buoy_parse_x20 ( struct buoy_chunks *b, struct bufr2tac_subset_state *s )
  \brief Parse a expanded descriptor with X = 20
  \param b pointer to a struct \ref buoy_chunks where to set the results
  \param s pointer to a struct \ref bufr2tac_subset_state where is stored needed information in sequential analysis

  It returns 0 if success, 1 if problems when processing. If a descriptor is not processed returns 0 anyway
*/
int buoy_parse_x20 ( struct buoy_chunks *b, struct bufr2tac_subset_state *s )
{
  if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
    {
      return 0;
    }

  if ( b == NULL )
    {
      return 1;
    }

  switch ( s->a->desc.y )
    {
    default:
      break;
    }
  return 0;
}

/*!
  \fn int temp_parse_x20 ( struct temp_chunks *t, struct bufr2tac_subset_state *s )
  \brief Parse a expanded descriptor with X = 20
  \param t pointer to a struct \ref temp_chunks where to set the results
  \param s pointer to a struct \ref bufr2tac_subset_state where is stored needed information in sequential analysis

  It returns 0 if success, 1 if problems when processing. If a descriptor is not processed returns 0 anyway
*/
int temp_parse_x20 ( struct temp_chunks *t, struct bufr2tac_subset_state *s )
{

  switch ( s->a->desc.y )
    {
    case 11: // 0 20 011 . Cloud amount
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          return 0;
        }
      if ( s->ival <= 8 )
        {
          sprintf ( t->b.s8.Nh, "%1d", s->ival );
        }
      else if ( s->ival <= 10 )
        {
          sprintf ( t->b.s8.Nh, "9" );
        }
      else if ( s->ival == 15 )
        {
          sprintf ( t->b.s8.Nh, "/" );
        }
      t->b.mask |= TEMP_SEC_8;
      break;
    case 12: // 0 20 012 . Cloud type
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          return 0;
        }
      if ( s->ival >= 10 && s->ival < 20 )
        {
          sprintf ( t->b.s8.Ch, "%1d", s->ival % 10 );
        }
      else if ( s->ival >= 20 && s->ival < 30 )
        {
          sprintf ( t->b.s8.Cm, "%1d", s->ival % 10 );
        }
      else if ( s->ival >= 30 && s->ival < 40 )
        {
          sprintf ( t->b.s8.Cl, "%1d", s->ival % 10 );
        }
      else if ( s->ival == 59 )
        {
          sprintf ( t->b.s8.Nh, "/" );
        }
      else if ( s->ival == 60 )
        {
          sprintf ( t->b.s8.Ch, "/" );
        }
      else if ( s->ival == 61 )
        {
          sprintf ( t->b.s8.Cm, "/" );
        }
      else if ( s->ival == 62 )
        {
          sprintf ( t->b.s8.Cl, "/" );
        }
      t->b.mask |= TEMP_SEC_8;
      break;
    case 13: // 0 20 013 . Height of base of cloud
      if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
        {
          return 0;
        }
      m_to_h ( t->b.s8.h, s->val );
      break;

    default:
      break;
    }
  return 0;
}
