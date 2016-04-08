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
 \file bufr2tac_temp.c
 \brief file with the code to parse a sequence as a FM-35 TEMP, FM-36 TEMP SHIP, FM-37 TEMP DROP, FM-38 TEMP MOBIL
 */
#include "bufr2tac.h"


/*!
  \fn char *guess_WMO_region_temp(struct synop_chunks *syn)
  \brief Try to find WMO region if it is not already set and WMO Block and number index are known
  \param syn pointer to the struct \ref synop_chunks with all known data for a synop

  It returns a pointer to the string with WMO region
*/
char *guess_WMO_region_temp ( struct temp_chunks *t )
{
  if ( t->a.s1.A1[0] )
    {
      return t->a.s1.A1;
    }

  if ( t->a.s1.II[0] == 0  || t->a.s1.iii[0] == 0 )
    return t->a.s1.A1;

  if ( guess_WMO_region ( t->a.s1.A1, t->a.s1.Reg, t->a.s1.II, t->a.s1.iii ) != NULL )
    {
      // copy the results in part A to parts A,B and C
      strcpy ( t->b.s1.A1, t->a.s1.A1 );
      strcpy ( t->c.s1.A1, t->a.s1.A1 );
      strcpy ( t->d.s1.A1, t->a.s1.A1 );
      strcpy ( t->b.s1.Reg, t->a.s1.Reg );
      strcpy ( t->c.s1.Reg, t->a.s1.Reg );
      strcpy ( t->d.s1.Reg, t->a.s1.Reg );
      return t->a.s1.A1;
    }
  else
    return NULL;
}



/*!
  \fn int parse_subset_as_temp(struct metreport *m, char *type, struct bufr_subset_sequence_data *sq, char *err)
  \brief parses a subset sequence as an Land fixed TEMP FM-35, TEMP SHIP FM-36, TEMP DROP FM-37 or TEMP MOBIL FM-38 report
  \param m pointer to a struct \ref metreport where set some results
  \param type strint with MiMiMjMj to choose the type of temp, temp ship, temp drop or temp mobil)
  \param sq pointer to a struct \ref bufr_subset_sequence_data with the parsed sequence on input
  \param err string with detected errors, if any

  It return 0 if all is OK. Otherwise returns 1 and it also fills the \a err string
*/
int parse_subset_as_temp ( struct metreport *m, struct bufr2tac_subset_state *s, struct bufr_subset_sequence_data *sq, char *err )
{
  size_t is;
  char aux[16];
  struct temp_chunks *t;
  struct met_datetime dtm;
  struct temp_raw_data *r;
  struct temp_raw_wind_shear_data *w;

  if ( sq == NULL )
    return 1;

  t = &m->temp;

  // clean data
  clean_temp_chunks ( t );

  // clean r
  memset ( &r, 0, sizeof ( struct temp_raw_point_data ) );

  // allocate memory for array of points in raw form
  if ( ( r = calloc ( 1, sizeof ( struct temp_raw_data ) ) ) == NULL )
    {
      sprintf ( err,"bufr2tac: parse_subset_as_temp(): Cannot allocate memory for raw data" );
      return 1;
    }

  if ( ( w = calloc ( 1, sizeof ( struct temp_raw_wind_shear_data ) ) ) == NULL )
    {
      sprintf ( err,"bufr2tac: parse_subset_as_temp(): Cannot allocate memory for raw data" );
      free ( ( void * ) ( r ) );
      return 1;
    }

  // set pointers
  s->r = r;
  s->w = w;

  // reject if still not coded type
  if ( strcmp ( s->type_report,"TTXX" ) == 0  && 0 )
    {
      // FIXME
      sprintf ( err,"bufr2tac: parse_subset_as_temp(): '%s' reports still not decoded in this software", s->type_report );
      free ( ( void * ) ( r ) );
      free ( ( void * ) ( w ) );
      return 1;
    }

  // Set the part. We are sure of this
  strcpy ( t->a.s1.MjMj, "AA" );
  strcpy ( t->b.s1.MjMj, "BB" );
  strcpy ( t->c.s1.MjMj, "CC" );
  strcpy ( t->d.s1.MjMj, "DD" );

  /**** First pass, sequential analysis *****/
  for ( is = 0; is < sq->nd; is++ )
    {
      s->i = is;
      s->ival = ( int ) sq->sequence[is].val;
      s->val = sq->sequence[is].val;
      s->a = &sq->sequence[is];
      if ( is > 0 )
        {
          s->a1 = &sq->sequence[is - 1];
        }

      // check if is a significance qualifier
      if ( sq->sequence[is].desc.x == 8 )
        {
          temp_parse_x08 ( t, s );
        }

      switch ( sq->sequence[is].desc.x )
        {
        case 1: //localization descriptors
          if ( temp_parse_x01 ( t, s ) )
            {
              free ( ( void * ) ( r ) );
              free ( ( void * ) ( w ) );
              return 1;
            }
          break;

        case 2: //Type of station descriptors
          if ( temp_parse_x02 ( t, s ) )
            {
              free ( ( void * ) ( r ) );
              free ( ( void * ) ( w ) );
              return 1;
            }
          break;

        case 4: //Date and time descriptors
          if ( temp_parse_x04 ( t, s ) )
            {
              free ( ( void * ) ( r ) );
              free ( ( void * ) ( w ) );
              return 1;
            }
          break;

        case 5: // Horizontal position. Latitude
          if ( temp_parse_x05 ( t, s ) )
            {
              free ( ( void * ) ( r ) );
              free ( ( void * ) ( w ) );
              return 1;
            }
          break;

        case 6: // Horizontal position. Longitude
          if ( temp_parse_x06 ( t, s ) )
            {
              free ( ( void * ) ( r ) );
              free ( ( void * ) ( w ) );
              return 1;
            }
          break;

        case 7: // Vertical position
          if ( temp_parse_x07 ( t, s ) )
            {
              free ( ( void * ) ( r ) );
              free ( ( void * ) ( w ) );
              return 1;
            }
          break;

        case 10: // Air Pressure descriptors
          if ( temp_parse_x10 ( t, s ) )
            {
              free ( ( void * ) ( r ) );
              free ( ( void * ) ( w ) );
              return 1;
            }
          break;

        case 11: // wind  data
          if ( temp_parse_x11 ( t, s ) )
            {
              free ( ( void * ) ( r ) );
              free ( ( void * ) ( w ) );
              return 1;
            }
          break;

        case 12: //Temperature descriptors
          if ( temp_parse_x12 ( t, s ) )
            {
              free ( ( void * ) ( r ) );
              free ( ( void * ) ( w ) );
              return 1;
            }
          break;

        case 20: // Cloud data
          if ( temp_parse_x20 ( t, s ) )
            {
              free ( ( void * ) ( r ) );
              free ( ( void * ) ( w ) );
              return 1;
            }
          break;

        case 22: // Oceanographic data
          if ( temp_parse_x22 ( t, s ) )
            {
              free ( ( void * ) ( r ) );
              free ( ( void * ) ( w ) );
              return 1;
            }
          break;

        case 31: // Replicators
          if ( temp_parse_x31 ( t, s ) )
            {
              free ( ( void * ) ( r ) );
              free ( ( void * ) ( w ) );
              return 1;
            }
          break;

        case 33: // Quality data
          if ( temp_parse_x33 ( t, s ) )
            {
              free ( ( void * ) ( r ) );
              free ( ( void * ) ( w ) );
              return 1;
            }
          break;

        default:
          break;
        }
    }

  /* Check about needed descriptors */
  if ( ( ( s->mask & SUBSET_MASK_HAVE_LATITUDE ) == 0 ) ||
       ( ( s->mask & SUBSET_MASK_HAVE_LONGITUDE ) == 0 ) ||
       ( ( s->mask & SUBSET_MASK_HAVE_YEAR ) == 0 ) ||
       ( ( s->mask & SUBSET_MASK_HAVE_MONTH ) == 0 ) ||
       ( ( s->mask & SUBSET_MASK_HAVE_DAY ) == 0 ) ||
       ( ( s->mask & SUBSET_MASK_HAVE_HOUR ) == 0 ) ||
       ( ( s->mask & SUBSET_MASK_HAVE_SECOND ) == 0 )
     )
    {
      sprintf ( err,"bufr2tac: parse_subset_as_temp(): lack of mandatory descriptor in sequence" );
      free ( ( void * ) ( r ) );
      free ( ( void * ) ( w ) );
      return 1;
    }

  /* Guess the type of TEMP report according with data parsed */
  if ( t->a.s1.II[0] )
    {
      // it seems this is a TEMP report
      strcpy ( t->a.s1.MiMi, "TT" );
      strcpy ( t->b.s1.MiMi, "TT" );
      strcpy ( t->c.s1.MiMi, "TT" );
      strcpy ( t->d.s1.MiMi, "TT" );
    }
  else if ( t->a.s1.h0h0h0h0[0] )
    {
      // it seems this is a TEMP MOBIL report
      strcpy ( t->a.s1.MiMi, "II" );
      strcpy ( t->b.s1.MiMi, "II" );
      strcpy ( t->c.s1.MiMi, "II" );
      strcpy ( t->d.s1.MiMi, "II" );
    }
  else if ( t->a.s1.D_D[0] )
    {
      // it seems this is a TEMP SHIP report
      strcpy ( t->a.s1.MiMi, "UU" );
      strcpy ( t->b.s1.MiMi, "UU" );
      strcpy ( t->c.s1.MiMi, "UU" );
      strcpy ( t->d.s1.MiMi, "UU" );
    }
  else if ( t->a.s1.Ula[0] )
    {
      // it seems this is a TEMP DROP report
      strcpy ( t->a.s1.MiMi, "XX" );
      strcpy ( t->b.s1.MiMi, "XX" );
      strcpy ( t->c.s1.MiMi, "XX" );
      strcpy ( t->d.s1.MiMi, "XX" );
    }
  else
    {
      sprintf ( err,"bufr2tac: parse_subset_as_temp(): Unknown type TEMP report" );
      free ( ( void * ) ( r ) );
      free ( ( void * ) ( w ) );
      return 1;
    }
  sprintf ( m->type, "%s%s" , t->a.s1.MiMi, t->a.s1.MjMj );
  sprintf ( m->type2, "%s%s" , t->b.s1.MiMi, t->b.s1.MjMj );
  sprintf ( m->type3, "%s%s" , t->c.s1.MiMi, t->c.s1.MjMj );
  sprintf ( m->type4, "%s%s" , t->d.s1.MiMi, t->d.s1.MjMj );

  /****** Second pass. Global results and consistence analysis ************/
  sprintf ( aux,"%s%s%s%s%s%s", t->a.e.YYYY, t->a.e.MM, t->a.e.DD, t->a.e.HH, t->a.e.mm, t->a.e.ss );
  YYYYMMDDHHmm_to_met_datetime ( &dtm, aux );
  round_met_datetime_to_hour ( &m->t, &dtm );
  memcpy ( &m->temp.t, &m->t, sizeof ( struct met_datetime ) );

  met_datetime_to_YYGG ( t->a.s1.YYGG, &dtm );
  strcpy ( t->b.s1.YYGG, t->a.s1.YYGG );
  strcpy ( t->c.s1.YYGG, t->a.s1.YYGG );
  strcpy ( t->d.s1.YYGG, t->a.s1.YYGG );

  if ( strlen ( t->a.s1.II ) )
    {
      strcpy ( m->g.index, t->a.s1.II );
      strcat ( m->g.index, t->a.s1.iii );
    }
  else if ( strlen ( t->a.s1.D_D ) )
    {
      strcpy ( m->g.index, t->a.s1.D_D );
    }
  else if ( strlen ( t->a.s1.IIIII ) )
    {
      strcpy ( m->g.index, t->a.s1.IIIII );
    }

  if ( s->mask & SUBSET_MASK_HAVE_LATITUDE )
    {
      m->g.lat = s->lat;
    }
  if ( s->mask & SUBSET_MASK_HAVE_LONGITUDE )
    {
      m->g.lon = s->lon;
    }
  if ( s->mask & SUBSET_MASK_HAVE_ALTITUDE )
    {
      m->g.alt = s->alt;
    }
  if ( s->mask & SUBSET_MASK_HAVE_NAME )
    {
      strcpy ( m->g.name, s->name );
    }
  if ( s->mask & SUBSET_MASK_HAVE_COUNTRY )
    {
      strcpy ( m->g.country, s->country );
    }



  //print_temp_raw_data ( r );
  //print_temp_raw_wind_shear_data ( w );

  // Finally parse raw data to fill all needed points for a TEMP
  parse_temp_raw_data ( t, r );
  parse_temp_raw_wind_shear_data ( t, w );

  // Free memory
  free ( ( void * ) ( r ) );
  free ( ( void * ) ( w ) );
  return 0;
}

/*!
  \fn int parse_temp_raw_data ( struct temp_chunks *t, struct temp_raw_data *r )
  \brief parse a struct \ref temp_raw_data to fill chunks in a struct \ref temp_chunks
*/
int parse_temp_raw_data ( struct temp_chunks *t, struct temp_raw_data *r )
{
  int ix, is_over_100;
  size_t i, j, isa = 0, isc = 0, ita = 0, itc = 0; // level counters
  size_t iwxa = 0, iwxc = 0, itb = 0, itd = 0;
  size_t iwd = 0, iwb = 0;
  struct temp_raw_point_data *d;

  if ( t == NULL || r == NULL )
    return 1;

  if ( r->n == 0 )
    return 1;

  // Some default
  t->a.s1.id[0] = '/';
  t->c.s1.id[0] = '/';
  for ( i = 0; i < r->n; i++ )
    {
      d = & ( r->raw[i] ); // to make code easy

      if ( d->p < 10000.0 ) // to select which part
        is_over_100 = 1;
      else
        is_over_100 = 0;

      // Surface data
      if ( d->flags & TEMP_POINT_MASK_SURFACE )
        {
          pascal_to_pnpnpn ( t->a.s2.lev0.PnPnPn, d->p ); //PoPoPo
          kelvin_to_TTTa ( t->a.s2.lev0.TnTnTan, d->T ); // TnTnTan
          dewpoint_depression_to_DnDn ( t->a.s2.lev0.DnDn, d->T , d->Td ); // DnDn
          wind_to_dndnfnfnfn ( t->a.s2.lev0.dndnfnfnfn, d->dd, d->ff ); // dndnfnfnfn
        }

      // standard level
      if ( d->flags & TEMP_POINT_MASK_STANDARD_LEVEL )
        {
          ix = ( int ) ( d->p * 0.01 + 0.5 ); // hPa
          ix = ( ix / 10 ) % 100; // coded
          if ( is_over_100 )
            sprintf ( t->c.s2.std[isc].PnPn, "%02d", ix ); // PnPn
          else
            sprintf ( t->a.s2.std[isa].PnPn, "%02d", ix ); // PnPn

          ix = ( int ) ( d->h + 0.5 );
          if ( d->p <= 50000.0 )
            {
              if ( is_over_100 )
                sprintf ( t->c.s2.std[isc].hnhnhn, "%03d", ( ( ix + 5 ) / 10 ) % 1000 );
              else
                sprintf ( t->a.s2.std[isa].hnhnhn, "%03d", ( ( ix + 5 ) / 10 ) % 1000 );
            }
          else
            {
              if ( is_over_100 )
                sprintf ( t->c.s2.std[isc].hnhnhn, "%03d", ix % 1000 );
              else if ( ix >= 0 )
                sprintf ( t->a.s2.std[isa].hnhnhn, "%03d", ix % 1000 );
              else
                sprintf ( t->a.s2.std[isa].hnhnhn, "%03d", ( -ix + 500 ) % 1000 );
            }
          if ( is_over_100 )
            {
              kelvin_to_TTTa ( t->c.s2.std[isc].TnTnTan, d->T ); // TnTnTan
              dewpoint_depression_to_DnDn ( t->c.s2.std[isc].DnDn, d->T, d->Td ); // DnDn
              wind_to_dndnfnfnfn ( t->c.s2.std[isc].dndnfnfnfn, d->dd, d->ff ); // dndnfnfnfn
              // Now update Id
              if ( d->ff != MISSING_REAL && d->dd != MISSING_REAL )
                {
                  t->c.s1.id[0] = t->c.s2.std[isc].PnPn[0];
                }
              if ( isc < TEMP_NSTAND_MAX )
                {
                  isc += 1;
                  t->c.s2.n = isc;
                }
            }
          else
            {
              kelvin_to_TTTa ( t->a.s2.std[isa].TnTnTan, d->T ); // TnTnTan
              dewpoint_depression_to_DnDn ( t->a.s2.std[isa].DnDn, d->T , d->Td ); // DnDn
              wind_to_dndnfnfnfn ( t->a.s2.std[isa].dndnfnfnfn, d->dd, d->ff ); // dndnfnfnfn
              // Now update Id
              if ( d->ff != MISSING_REAL && d->dd != MISSING_REAL )
                {
                  t->a.s1.id[0] = t->a.s2.std[isa].PnPn[0];
                }
              if ( isa < TEMP_NSTAND_MAX )
                {
                  isa += 1;
                  t->a.s2.n = isa;
                }
            }
        }

      // Tropopause level
      if ( d->flags & TEMP_POINT_MASK_TROPOPAUSE_LEVEL )
        {
          if ( is_over_100 )
            {
              ix = ( int ) ( d->p * 0.1 + 0.5 );
              sprintf ( t->c.s3.trop[itc].PnPnPn, "%03d", ix % 1000 ); // PnPnPn
              kelvin_to_TTTa ( t->c.s3.trop[itc].TnTnTan, d->T ); // TnTnTan
              dewpoint_depression_to_DnDn ( t->c.s3.trop[itc].DnDn, d->T , d->Td ); // DnDn
              wind_to_dndnfnfnfn ( t->c.s3.trop[itc].dndnfnfnfn, d->dd, d->ff ); // dndnfnfnfn
              if ( ix && itc < TEMP_NTROP_MAX )
                {
                  itc += 1;
                  t->c.s3.n = itc;
                }
            }
          else
            {
              ix = ( int ) ( d->p * 0.01 + 0.5 );
              sprintf ( t->a.s3.trop[ita].PnPnPn, "%03d", ix % 1000 ); // PnPnPn.
              kelvin_to_TTTa ( t->a.s3.trop[ita].TnTnTan, d->T ); // TnTnTan
              dewpoint_depression_to_DnDn ( t->a.s3.trop[ita].DnDn, d->T , d->Td ); // DnDn
              wind_to_dndnfnfnfn ( t->a.s3.trop[ita].dndnfnfnfn, d->dd, d->ff ); // dndnfnfnfn
              if ( ix && ita < TEMP_NTROP_MAX )
                {
                  ita += 1;
                  t->a.s3.n = ita;
                }
            }
        }

      // Max wind level
      if ( d->flags & TEMP_POINT_MASK_MAXIMUM_WIND_LEVEL )
        {
          if ( is_over_100 )
            {
              ix = ( int ) ( d->p * 0.1 + 0.5 );
              sprintf ( t->c.s4.windx[iwxc].PmPmPm, "%03d", ix % 1000 ); // PnPnPn
              wind_to_dndnfnfnfn ( t->c.s4.windx[iwxc].dmdmfmfmfm, d->dd, d->ff ); // dndnfnfnfn
              // check if more wind data
              for ( j = i + 1; j < r->n ; j++ )
                {
                  if ( r->raw[j].ff != MISSING_REAL )
                    {
                      t->c.s4.windx[iwxc].no_last_wind = 1;
                      break;
                    }
                }
              if ( ix && iwxc < TEMP_NMAXWIND_MAX )
                {
                  iwxc += 1;
                  t->c.s4.n = iwxc;
                }
            }
          else
            {
              ix = ( int ) ( d->p * 0.01 + 0.5 );
              sprintf ( t->a.s4.windx[iwxa].PmPmPm, "%03d", ix % 1000 ); // PnPnPn.
              wind_to_dndnfnfnfn ( t->a.s4.windx[iwxa].dmdmfmfmfm, d->dd, d->ff ); // dndnfnfnfn
              for ( j = i + 1; j < r->n ; j++ )
                {
                  if ( r->raw[j].ff != MISSING_REAL )
                    {
                      t->a.s4.windx[iwxa].no_last_wind = 1;
                      break;
                    }
                }
              if ( ix && iwxa < TEMP_NMAXWIND_MAX )
                {
                  iwxa += 1;
                  t->a.s4.n = iwxa;
                }
            }
        }

      // Significant TH points
      if ( d->flags & ( TEMP_POINT_MASK_SIGNIFICANT_TEMPERATURE_LEVEL | TEMP_POINT_MASK_SIGNIFICANT_HUMIDITY_LEVEL ) )
        {
          if ( is_over_100 )
            {
              sprintf ( t->d.s5.th[itd].nini, "%d%d", ( ( int ) itd - 1 ) %9 + 1, ( ( int ) itd - 1 ) %9 + 1 );
              ix = ( int ) ( d->p * 0.1 + 0.5 );
              sprintf ( t->d.s5.th[itd].PnPnPn, "%03d", ix ); // PnPnPn
              kelvin_to_TTTa ( t->d.s5.th[itd].TnTnTan, d->T ); // TnTnTan
              dewpoint_depression_to_DnDn ( t->d.s5.th[itd].DnDn, d->T , d->Td ); // DnDn
              if ( ix && itd < TEMP_NMAX_POINTS )
                {
                  itd += 1;
                  t->d.s5.n = itd;
                }
            }
          else
            {
              if ( d->flags & TEMP_POINT_MASK_SURFACE )
                {
                  strcpy ( t->b.s5.th[itb].nini, "00" ); // case of surface level
                }
              else
                {
                  sprintf ( t->b.s5.th[itb].nini, "%d%d", ( ( int ) itb - 1 ) %9 + 1, ( ( int ) itb - 1 ) %9 + 1 );
                }
              ix = ( int ) ( d->p * 0.01 + 0.5 );
              sprintf ( t->b.s5.th[itb].PnPnPn, "%03d", ix % 1000 ); // PnPnPn.
              kelvin_to_TTTa ( t->b.s5.th[itb].TnTnTan, d->T ); // TnTnTan
              dewpoint_depression_to_DnDn ( t->b.s5.th[itb].DnDn, d->T , d->Td ); // DnDn
              if ( ix && itb < TEMP_NMAX_POINTS )
                {
                  itb += 1;
                  t->b.s5.n = itb;
                }
            }
        }

      // Significant wind points
      if ( d->flags & TEMP_POINT_MASK_SIGNIFICANT_WIND_LEVEL )
        {
          if ( is_over_100 )
            {
              sprintf ( t->d.s6.wd[iwd].nini, "%d%d", ( ( int ) iwd - 1 ) %9 + 1, ( ( int ) iwd - 1 ) %9 + 1 );
              ix = ( int ) ( d->p * 0.1 + 0.5 );
              sprintf ( t->d.s6.wd[iwd].PnPnPn, "%03d", ix ); // PnPnPn
              wind_to_dndnfnfnfn ( t->d.s6.wd[iwd].dndnfnfnfn, d->dd, d->ff ); // dndnfnfnfn
              if ( ix && iwd < TEMP_NMAX_POINTS )
                {
                  iwd += 1;
                  t->d.s6.n = iwd;
                }
            }
          else
            {
              if ( d->flags & TEMP_POINT_MASK_SURFACE )
                {
                  strcpy ( t->b.s6.wd[iwb].nini, "00" ); // case of surface level
                }
              else
                {
                  sprintf ( t->b.s6.wd[iwb].nini, "%d%d", ( ( int ) iwb - 1 ) %9 + 1, ( ( int ) iwb - 1 ) %9 + 1 );
                }
              ix = ( int ) ( d->p * 0.01 + 0.5 );
              sprintf ( t->b.s6.wd[iwb].PnPnPn, "%03d", ix % 1000 ); // PnPnPn.
              wind_to_dndnfnfnfn ( t->b.s6.wd[iwb].dndnfnfnfn, d->dd, d->ff ); // dndnfnfnfn
              if ( ix && iwb < TEMP_NMAX_POINTS )
                {
                  iwb += 1;
                  t->b.s6.n = iwb;
                  //printf("%lu\n", t->b.s6.n);
                }
            }
        }
    }

  // Now we set some bit masks
  if ( isa )
    t->a.mask |= TEMP_SEC_2;
  if ( isc )
    t->c.mask |= TEMP_SEC_2;
  if ( ita )
    t->a.mask |= TEMP_SEC_3;
  if ( itc )
    t->c.mask |= TEMP_SEC_3;
  if ( iwxa )
    t->a.mask |= TEMP_SEC_4;
  if ( iwxc )
    t->c.mask |= TEMP_SEC_4;
  if ( itb )
    t->b.mask |= TEMP_SEC_5;
  if ( itd )
    t->d.mask |= TEMP_SEC_5;
  if ( iwb )
    t->b.mask |= TEMP_SEC_6;
  if ( iwd )
    t->d.mask |= TEMP_SEC_6;

  return 0;
}

/*!
  \fn int parse_temp_raw_wind_shear_data (struct temp_chunks *t, struct temp_raw_wind_shear_data *w )

*/
int parse_temp_raw_wind_shear_data ( struct temp_chunks *t, struct temp_raw_wind_shear_data *w )
{
  size_t i, j;
  int ix, is_over_100;
  char aux[16];

  struct temp_raw_wind_shear_point *d;

  if ( t == NULL || w == NULL )
    return 1;

  if ( w->n == 0 )
    return 1;

  for ( i = 0; i < w->n; i++ )
    {
      d = & ( w->raw[i] ); // to make code easy

      if ( d->p < 10000.0 ) // to select which part
        is_over_100 = 1;
      else
        is_over_100 = 0;

      // set pnpnpn on aux
      if ( is_over_100 )
        {
          if ( t ->c.s4.n == 0 )
            continue;

          ix = ( int ) ( d->p * 0.1 + 0.5 );
          sprintf ( aux, "%03d", ix % 1000 ); // PnPnPn

          // checks for a significant wind level in section 4 with same pnpnpn
          for ( j = 0 ; j < t->c.s4.n ; j++ )
            {
              if ( strcmp ( t->c.s4.windx[j].PmPmPm , aux ) == 0 )
                {
                  if ( d->ws_blw != MISSING_REAL )
                    {
                      sprintf ( t->c.s4.windx[j].vbvb, "%02.0lf", d->ws_blw );
                    }
                  if ( d->ws_abv != MISSING_REAL )
                    {
                      sprintf ( t->c.s4.windx[j].vava, "%02.0lf", d->ws_abv );
                    }
                  break;
                }
            }
        }
      else
        {
          if ( t ->a.s4.n == 0 )
            continue;
          ix = ( int ) ( d->p * 0.01 + 0.5 );
          sprintf ( aux, "%03d", ix % 1000 ); // PnPnPn.
          // checks for a significant wind level in section 4 with same pnpnpn
          for ( j = 0 ; j < t->a.s4.n ; j++ )
            {
              if ( strcmp ( t->a.s4.windx[j].PmPmPm , aux ) == 0 )
                {
                  //printf("%s %s\n", aux, t->a.s4.windx[j].PmPmPm);
                  //printf("%.1lf %.1lf\n", d->ws_blw, d->ws_abv);
                  if ( d->ws_blw != MISSING_REAL )
                    {
                      sprintf ( t->a.s4.windx[j].vbvb, "%02.0lf", d->ws_blw );
                    }
                  if ( d->ws_abv != MISSING_REAL )
                    {
                      sprintf ( t->a.s4.windx[j].vava, "%02.0lf", d->ws_abv );
                    }
                  break;
                }
            }
        }
    }

  return 0;
}
