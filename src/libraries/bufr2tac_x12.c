/***************************************************************************
 *   Copyright (C) 2013-2018 by Guillermo Ballester Valor                  *
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
 \file bufr2tac_x12.c
 \brief decodes the descriptors with X = 12 (Temperature)
 */
#include "bufr2tac.h"

/*!
  \fn char * kelvin_to_snTTT ( char *target, double T )
  \brief converts a kelvin temperature value into a snTTT string
  \param T the temperature ( Kelvin )
  \param target string with the result
*/
char * kelvin_to_snTTT ( char *target, double T )
{
  int ic;
  double Tx = T;

  if ( Tx > 15 && Tx < 34.0 ) // guess there is an scale error on coding , we better manage T * 10
    {
      Tx *= 10.0;
    }
  else if ( T < 150.0 || T > 340.0 )
    {
      return NULL;
    }

  // tenths of degree (Celsius)
  ic = ( int ) ( floor ( 10.0 * ( Tx - 273.15 ) + 0.5 ) );
  if ( ic < 0 )
    {
      sprintf ( target, "1%03d",  -ic );
    }
  else
    {
      sprintf ( target, "0%03d",  ic );
    }
  return target;
}

/*!
  \fn char * kelvin_to_snTT ( char *target, double T )
  \brief converts a kelvin temperature value into a snTT string
  \param T the temperature ( Kelvin )
  \param target string with the result
*/
char * kelvin_to_snTT ( char *target, double T )
{
  int ic;
  if ( T < 150.0 || T > 340.0 )
    {
      return NULL;
    }

  // Whole degrees (Celsius)
  ic = ( int ) ( floor ( ( T - 273.15 ) + 0.5 ) );
  if ( ic < 0 )
    {
      sprintf ( target, "1%02d", -ic );
    }
  else
    {
      sprintf ( target, "%03d", ic );
    }
  return target;
}

/*!
  \fn char * kelvin_to_TT ( char *target, double T )
  \brief converts a kelvin temperature value into a TT string
  \param T the temperature ( Kelvin )
  \param target string with the result
*/
char * kelvin_to_TT ( char *target, double T )
{
  int ic;
  if ( T < 150.0 || T > 340.0 )
    {
      return NULL;
    }

  // Whole degrees (Celsius)
  ic = ( int ) ( floor ( ( T - 273.15 ) + 0.5 ) );
  if ( ic < 0 )
    {
      sprintf ( target, "%02d", 50 - ic );
    }
  else
    {
      sprintf ( target, "%02d", ic );
    }
  return target;
}

/*!
  \fn char * kelvin_to_TTTT ( char *target, double T )
  \brief converts a kelvin temperature value into a TTTT string
  \param T the temperature ( Kelvin )
  \param target string with the result
*/
char * kelvin_to_TTTT ( char *target, double T )
{
  int ic;
  if ( T < 150.0 || T > 340.0 )
    {
      return NULL;
    }
  // hundreth of degrees (Celsius)
  ic = ( int ) ( floor ( 100.0 * ( T - 273.15 ) + 0.5 ) );
  if ( ic < 0 )
    {
      ic = 5000 - ic;
    }
  sprintf ( target, "%04d", ic );
  return target;
}


/*!
  \fn char * kelvin_to_TTTa(char *target, double T)
  \brief Set temperature TTTa
  \param target result as string
  \param T temperature (kelvin)
*/
char * kelvin_to_TTTa ( char *target, double T )
{
  int ix;

  if ( T == MISSING_REAL )
    {
      sprintf ( target, "///" );
      return target;
    }

  ix = ( int ) ( 10.0 * ( T - 273.15 ) );

  if ( ix >= 0 )
    {
      ix &= ~1;
    }
  else
    {
      ix = ( -ix );
      ix |= 1;
    }
  sprintf ( target, "%03d", ix );
  return target;
}

/*!
  \fn  char * dewpoint_depression_to_DnDn ( char * target, double T, double Td )
  \brief Set DnDn (dewpoint depression)
  \param target string to set as result
  \param T temperature (Kelvin)
  \param Td dewpoint (Kelvin)
*/
char * dewpoint_depression_to_DnDn ( char * target, double T, double Td )
{
  int ix;
  double dpd = T - Td;

  if ( T == MISSING_REAL || Td == MISSING_REAL )
    {
      strcpy ( target, "//" );
      return target;
    }

  if ( dpd < 0.0 )
    {
      return NULL;
    }

  if ( dpd > 50.0 )
    {
      strcpy ( target, "//" );
      return target;
    }

  ix = ( int ) ( 10.0 * dpd );

  if ( ix >= 55 )
    {
      ix = 50 + ( ix + 5 ) / 10;
    }
  sprintf ( target, "%02d", ix );

  return target;
}

/*!
  \fn int syn_parse_x12 ( struct synop_chunks *syn, struct bufr2tac_subset_state *s )
  \brief Parse a expanded descriptor with X = 12
  \param syn pointer to a struct \ref synop_chunks where to set the results
  \param s pointer to a struct \ref bufr2tac_subset_state where is stored needed information in sequential analysis

  It returns 0 if success, 1 if problems when processing. If a descriptor is not processed returns 0 anyway
*/
int syn_parse_x12 ( struct synop_chunks *syn, struct bufr2tac_subset_state *s )
{
  char aux[16];

  if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
    {
      return 0;
    }

  switch ( s->a->desc.y )
    {
    case 1: // 0 12 001 . Air temperature
    case 4: // 0 12 004 . Air temperatura at 2 m
    case 101: // 0 12 101 . Air temperature
    case 104: // 0 12 104 . Air temperature at 2 m
      if ( syn->s1.TTT[0] == 0 )
        {
          if ( kelvin_to_snTTT ( aux, s->val ) )
            {
              syn->s1.sn1[0] = aux[0];
              strcpy ( syn->s1.TTT, aux + 1 );
              syn->mask |= SYNOP_SEC1;
            }
        }
      break;

    case 3: // 0 12 003 . Dewpoint temperature
    case 6: // 0 12 006 . Dewpoint temperature at 2 m
    case 103: // 0 12 103 . Dewpoint temperature
    case 106: // 0 12 106 . Dewpoint temperature at 2 m
      if ( syn->s1.TdTdTd[0] == 0 )
        {
          if ( kelvin_to_snTTT ( aux, s->val ) )
            {
              syn->s1.sn2[0] = aux[0];
              strcpy ( syn->s1.TdTdTd, aux + 1 );
              syn->mask |= SYNOP_SEC1;
            }
        }
      break;

    case 14: // 0 12 014 . Maximum temperature at 2 m , past 12 hours
    case 21: // 0 12 021 . Maximum temperature.
    case 114: // 0 12 114 . Maximum temperature at 2 m , past 12 hours
    case 116: // 0 12 116 . Maximum temperature at 2 m , past 24 hours
      if ( syn->s3.TxTxTx[0] == 0 && ( s->itval % ( 3 * 3600 ) ) == 0 ) // only for 3, 6 ... hours
        {
          if ( kelvin_to_snTTT ( aux, s->val ) )
            {
              syn->s3.snx[0] = aux[0];
              strcpy ( syn->s3.TxTxTx, aux + 1 );
              syn->mask |= SYNOP_SEC3;
            }
        }
      break;

    case 11: // 0 12 011 . Maximum temperature at heigh and over the period specified
    case 111: // 0 12 111 . Maximum temperature at heigh and over the period specified
      if ( syn->s3.TxTxTx[0] == 0 &&
           ( s->hsensor >= 1.0 || s->hsensor <= 0 ) &&
           ( time_period_duration ( s ) % ( 12 * 3600 ) ) == 0 ) // only for 12 , 24 hours
        {
          if ( kelvin_to_snTTT ( aux, s->val ) )
            {
              syn->s3.snx[0] = aux[0];
              strcpy ( syn->s3.TxTxTx, aux + 1 );
              syn->mask |= SYNOP_SEC3;
            }
        }
      break;


    case 15: // 0 12 015 . Minimum temperature at 2 m , past 12 hours
    case 22: // 0 12 022 . Minimum temperature.
    case 115: // 0 12 115 . Minimum temperature at 2 m , past 12 hours
    case 117: // 0 12 117 . Minimum temperature at 2 m , past 24 hours
      if ( syn->s3.TnTnTn[0] == 0 && ( s->itval % ( 3 * 3600 ) ) == 0 ) // only for 3, 6 ... hours
        {
          if ( kelvin_to_snTTT ( aux, s->val ) )
            {
              syn->s3.snn[0] = aux[0];
              strcpy ( syn->s3.TnTnTn, aux + 1 );
              syn->mask |= SYNOP_SEC3;
            }
        }
      break;

    case 12: // 0 12 012 . Minimum temperature at heigh and over the period specified
    case 112: // 0 12 112 . Minimum temperature at heigh and over the period specified
      if ( syn->s3.TnTnTn[0] == 0 &&
           ( s->hsensor >= 1.0 || s->hsensor <= 0 ) &&
           ( time_period_duration ( s ) % ( 12 * 3600 ) ) == 0 ) // only for 12 , 24 hours
        {
          if ( kelvin_to_snTTT ( aux, s->val ) )
            {
              syn->s3.snn[0] = aux[0];
              strcpy ( syn->s3.TnTnTn, aux + 1 );
              syn->mask |= SYNOP_SEC3;
            }
        }
      break;

    case 2: // 0 12 002 . Wet bulb temperature
    case 5: // 0 12 005 . Wet bulb temperature at 2 m
    case 102: // 0 12 102 . Wet bulb temperature
    case 105: // 0 12 105 . Wet bulb temperature at 2 m
      if ( syn->s2.TbTbTb[0] == 0 )
        {
          if ( kelvin_to_snTTT ( aux, s->val ) )
            {
              syn->s2.sw[0] = aux[0];
              strcpy ( syn->s2.TbTbTb, aux + 1 );
              syn->mask |= SYNOP_SEC2;
            }
        }
      break;

    case 113: // 0 12 113 . Ground minimum temperature, past 12 hours
      if ( strcmp ( "6", syn->s0.A1 ) == 0 ||
           strcmp ( "3", syn->s0.A1 ) == 0 ||
           strcmp ( "2", syn->s0.A1 ) == 0
         ) // region III or VI
        {
          if ( kelvin_to_snTT ( aux, s->val ) )
            {
              strcpy ( syn->s3.jjj, aux );
              syn->mask |= SYNOP_SEC3;
            }
        }
      else if ( strcmp ( "1", syn->s0.A1 ) == 0 )
        {
          if ( kelvin_to_TT ( aux, s->val ) )
            {
              syn->s3.XoXoXoXo[0] = aux[0];
              syn->s3.XoXoXoXo[1] = aux[1];
              if ( syn->s3.XoXoXoXo[2] == 0 )
                {
                  syn->s3.XoXoXoXo[2] = '/';
                }
              if ( syn->s3.XoXoXoXo[3] == 0 )
                {
                  syn->s3.XoXoXoXo[3] = '/';
                }
              syn->mask |= SYNOP_SEC3;
            }
        }
      break;

    case 121: // 0 12 121 . Ground minimum temperature (at the time of observation)
      if ( strcmp ( "2", syn->s0.A1 ) == 0 ) // region II
        {
          if ( kelvin_to_snTT ( aux, s->val ) )
            {
              syn->s3.XoXoXoXo[1] = aux[0];
              syn->s3.XoXoXoXo[2] = aux[1];
              syn->s3.XoXoXoXo[3] = aux[2];
              syn->mask |= SYNOP_SEC3;
            }
        }
      break;

    case 122: // 0 12 122 . Ground minimum temperature during preceding night
      if ( strcmp ( "1", syn->s0.A1 ) == 0 ) // Only for Region I
        {
          if ( kelvin_to_TT ( aux, s->val ) )
            {
              syn->s3.XoXoXoXo[0] = aux[0];
              syn->s3.XoXoXoXo[1] = aux[1];
              if ( syn->s3.XoXoXoXo[2] == 0 )
                {
                  syn->s3.XoXoXoXo[2] = '/';
                }
              if ( syn->s3.XoXoXoXo[3] == 0 )
                {
                  syn->s3.XoXoXoXo[3] = '/';
                }
              syn->mask |= SYNOP_SEC3;
            }
        }
      else if ( strcmp ( "2", syn->s0.A1 ) == 0 ||
                strcmp ( "3", syn->s0.A1 ) == 0 ) // Region II and III
        {
          if ( kelvin_to_snTT ( aux, s->val ) )
            {
              strcpy ( syn->s3.jjj, aux );
              syn->mask |= SYNOP_SEC3;
            }
        }
      break;

    default:
      break;
    }
  return 0;
}

/*!
  \fn int buoy_parse_x12 ( struct buoy_chunks *b, struct bufr2tac_subset_state *s )
  \brief Parse a expanded descriptor with X = 12
  \param b pointer to a struct \ref buoy_chunks where to set the results
  \param s pointer to a struct \ref bufr2tac_subset_state where is stored needed information in sequential analysis

  It returns 0 if success, 1 if problems when processing. If a descriptor is not processed returns 0 anyway
*/
int buoy_parse_x12 ( struct buoy_chunks *b, struct bufr2tac_subset_state *s )
{
  char aux[16];

  if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
    {
      return 0;
    }

  switch ( s->a->desc.y )
    {
    case 1: // 0 12 001 . Air temperature
    case 4: // 0 12 004 . Air temperatura at 2 m
    case 101: // 0 12 101 . Air temperature
    case 104: // 0 12 104 . Air temperature at 2 m
      if ( b->s1.TTT[0] == 0 )
        {
          if ( kelvin_to_snTTT ( aux, s->val ) )
            {
              b->s1.sn1[0] = aux[0];
              strcpy ( b->s1.TTT, aux + 1 );
              b->mask |= BUOY_SEC1;
            }
        }
      break;
    case 3: // 0 12 003 . Dewpoint temperature
    case 6: // 0 12 006 . Dewpoint temperature at 2 m
    case 103: // 0 12 103 . Dewpoint temperature
    case 106: // 0 12 106 . Dewpoint temperature at 2 m
      if ( b->s1.TdTdTd[0] == 0 )
        {
          if ( kelvin_to_snTTT ( aux, s->val ) )
            {
              b->s1.sn2[0] = aux[0];
              strcpy ( b->s1.TdTdTd, aux + 1 );
              b->mask |= BUOY_SEC1;
            }
        }
      break;

    default:
      break;
    }
  return 0;
}

/*!
  \fn int climat_parse_x12 ( struct climat_chunks *c, struct bufr2tac_subset_state *s )
  \brief Parse a expanded descriptor with X = 12
  \param c pointer to a struct \ref climat_chunks where to set the results
  \param s pointer to a struct \ref bufr2tac_subset_state where is stored needed information in sequential analysis

  It returns 0 if success, 1 if problems when processing. If a descriptor is not processed returns 0 anyway
*/
int climat_parse_x12 ( struct climat_chunks *c, struct bufr2tac_subset_state *s )
{
  char aux[16];

  if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
    {
      return 0;
    }

  if ( c == NULL )
    {
      return 1;
    }

  switch ( s->a->desc.y )
    {
    case 101:
      if ( s->is_normal == 0 )
        {
          if ( s->isq_val == 4 ) // mean value
            {
              if ( kelvin_to_snTTT ( aux, s->val ) )
                {
                  c->s1.s[0] = aux[0];
                  strcpy ( c->s1.TTT, aux + 1 );
                  c->mask |= CLIMAT_SEC1;
                }
            }
          else if ( s->isq_val == 2 ) // maximum value
            {
              if ( kelvin_to_snTTT ( aux, s->val ) )
                {
                  c->s4.sax[0] = aux[0];
                  strcpy ( c->s4.Tax, aux + 1 );
                  if ( s->more_days == 0 )
                    {
                      sprintf ( c->s4.yax, "%02d", s->day );
                    }
                  else
                    {
                      sprintf ( c->s4.yax, "%02d", (s->day + 50) % 100 );
                    }
                  c->mask |= CLIMAT_SEC4;
                }

            }
          else if ( s->isq_val == 3 ) // minimum value
            {
              if ( kelvin_to_snTTT ( aux, s->val ) )
                {
                  c->s4.san[0] = aux[0];
                  strcpy ( c->s4.Tan, aux + 1 );
                  if ( s->more_days == 0 )
                    {
                      sprintf ( c->s4.yan, "%02d", s->day );
                    }
                  else
                    {
                      sprintf ( c->s4.yan, "%02d", (s->day + 50) % 100 );
                    }
                  c->mask |= CLIMAT_SEC4;
                }
            }
        }
      else
        {
          if ( c->s2.TTT[0] == 0 )
            {
              if ( kelvin_to_snTTT ( aux, s->val ) )
                {
                  c->s2.s[0] = aux[0];
                  strcpy ( c->s2.TTT, aux + 1 );
                  c->mask |= CLIMAT_SEC2;
                }
            }

        }
      break;

    case 118: // 0 12 118 . Maximum temperature at heigh specified, past 24 h
      if ( s->is_normal == 0 )
        {
          if ( c->s1.TxTxTx[0] == 0 )
            {
              if ( kelvin_to_snTTT ( aux, s->val ) )
                {
                  c->s1.sx[0] = aux[0];
                  strcpy ( c->s1.TxTxTx, aux + 1 );
                  c->mask |= CLIMAT_SEC1;
                }
            }
        }
      else
        {
          if ( c->s2.TxTxTx[0] == 0 )
            {
              if ( kelvin_to_snTTT ( aux, s->val ) )
                {
                  c->s2.sx[0] = aux[0];
                  strcpy ( c->s2.TxTxTx, aux + 1 );
                  c->mask |= CLIMAT_SEC2;
                }
            }
        }
      break;

    case 119: // 0 12 119 . Minimum temperature at heigh specified, past 24 h
      if ( s->is_normal == 0 )
        {
          if ( c->s1.TnTnTn[0] == 0 )
            {
              if ( kelvin_to_snTTT ( aux, s->val ) )
                {
                  c->s1.sn[0] = aux[0];
                  strcpy ( c->s1.TnTnTn, aux + 1 );
                  c->mask |= CLIMAT_SEC1;
                }
            }
        }
      else
        {
          if ( c->s2.TnTnTn[0] == 0 )
            {
              if ( kelvin_to_snTTT ( aux, s->val ) )
                {
                  c->s2.sn[0] = aux[0];
                  strcpy ( c->s2.TnTnTn, aux + 1 );
                  c->mask |= CLIMAT_SEC2;
                }
            }
        }
      break;

    case 151: // 0 12 151 . Standard deviation of daily mean temperature
      sprintf ( aux, "%03d", ( int ) ( s->val * 10.0 + 0.5 ) );
      if ( s->is_normal == 0 )
        {
          strcpy ( c->s1.ststst, aux );
          c->mask |= CLIMAT_SEC1;
        }
      else
        {
          strcpy ( c->s2.ststst, aux );
          c->mask |= CLIMAT_SEC2;
        }
      break;

    case 152: // 0 12 152 . Highest daily mean temperature
      if ( s->a1->desc.x == 4 && s->a1->desc.y == 3 )
        {
          if ( kelvin_to_snTTT ( aux, s->val ) )
            {
              c->s4.sx[0] = aux[0];
              strcpy ( c->s4.Txd, aux + 1 );
            }
          if ( s->isq_val == 0 )
            {
              sprintf ( c->s4.yx, "%02d", s->day );
            }
          else
            {
              sprintf ( c->s4.yx, "%02d", (s->day + 50) % 100 );
            }
          c->mask |= CLIMAT_SEC4;
        }
      break;

    case 153: // 0 12 153 . Lowest daily mean temperature
      if ( s->a1->desc.x == 4 && s->a1->desc.y == 3 )
        {
          if ( kelvin_to_snTTT ( aux, s->val ) )
            {
              c->s4.sn[0] = aux[0];
              strcpy ( c->s4.Tnd, aux + 1 );
            }
          if ( s->isq_val == 0 )
            {
              sprintf ( c->s4.yn, "%02d", s->day );
            }
          else
            {
              sprintf ( c->s4.yn, "%02d", (s->day + 50) % 100 );
            }
          c->mask |= CLIMAT_SEC4;
        }
      break;

    default:
      break;
    }
  return 0;
}

/*!
  \fn int temp_parse_x12 ( struct temp_chunks *t, struct bufr2tac_subset_state *s )
  \brief Parse a expanded descriptor with X = 12
  \param t pointer to a struct \ref temp_chunks where to set the results
  \param s pointer to a struct \ref bufr2tac_subset_state where is stored needed information in sequential analysis

  It returns 0 if success, 1 if problems when processing. If a descriptor is not processed returns 0 anyway
*/
int temp_parse_x12 ( struct temp_chunks *t, struct bufr2tac_subset_state *s )
{
  if ( t == NULL )
    {
      return 1;
    }

  switch ( s->a->desc.y )
    {
    case 101: // 0 12 101. Temperature/dry-bulb temperature (scale 2)
      if ( s->rep > 0 && s->r->n > 0 )
        {
          if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
            {
              s->r->raw[s->r->n - 1].T = MISSING_REAL;
            }
          else
            {
              s->r->raw[s->r->n - 1].T = s->val;
            }
        }
      break;

    case 103: // 0 12 103. Dew-point temperature (scale 2)
      if ( s->rep > 0 && s->r->n > 0 )
        {
          if ( s->a->mask & DESCRIPTOR_VALUE_MISSING )
            {
              s->r->raw[s->r->n - 1].Td = MISSING_REAL;
            }
          else
            {
              s->r->raw[s->r->n - 1].Td = s->val;
            }
        }
      break;

    default:
      break;
    }

  return 0;
}
