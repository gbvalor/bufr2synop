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

/*!
  \fn int synop_YYYYMMDDHHmm_to_YYGG(struct synop_chunks *syn)
  \brief Sets YYGG from YYYYMMDDHHmm extended group
  \param syn pointer to the target struct \ref synop_chunks
*/
int synop_YYYYMMDDHHmm_to_YYGG ( struct synop_chunks *syn )
{
  char aux[20];
  time_t t;
  struct tm tim;

  if ( strlen ( syn->e.YYYY ) &&
       strlen ( syn->e.MM ) &&
       strlen ( syn->e.DD ) &&
       strlen ( syn->e.HH ) &&
       strlen ( syn->e.MM ) )
    sprintf ( aux,"%s%s%s%s%s", syn->e.YYYY, syn->e.MM, syn->e.DD, syn->e.HH, syn->e.mm );

  if ( strlen ( aux ) != 12 )
    return 1;

  memset ( &tim, 0, sizeof ( struct tm ) );
  strptime ( aux, "%Y%m%d%H%M", &tim );

  t = mktime ( &tim ) + 1799 ; // rounding trick
  gmtime_r ( &t, &tim );
  sprintf ( syn->s0.YY, "%02d", tim.tm_mday );
  sprintf ( syn->s0.GG, "%02d", tim.tm_hour );
  return 0;
}

// return NULL if not guessed
char *guess_WMO_region(struct synop_chunks *syn)
{
  char aux[8];

  if (syn->s0.A1[0])
    {
      return syn->s0.A1;
    }

  if (syn->s0.II[0] == 0  || syn->s0.iii[0] == 0)
    return syn->s0.A1;

  sprintf(aux,"%s%s",syn->s0.II, syn->s0.iii);

  if ((syn->s0.II[0] == '0' && (strstr(aux,"042") != aux) && (strstr(aux,"043") != aux)  && (strstr(aux,"0441") != aux)  && (strstr(aux,"0858") != aux) &&  (strstr(aux,"0859") != aux)) ||
      syn->s0.II[0] == '1' || (strstr(aux,"201") == aux) ||
      strcmp(syn->s0.II,"22") == 0 || strcmp(syn->s0.II,"26") == 0 || strcmp(syn->s0.II,"27") == 0 ||
      strcmp(syn->s0.II,"33") == 0 || strcmp(syn->s0.II,"34") == 0 || strcmp(syn->s0.II,"22") == 0 ||
      (strstr(aux,"201") == aux))
    {
      // Reg 6
      syn->s0.A1[0] = '6';
      strcpy(syn->s0.Reg,"VI");
      return syn->s0.A1;
    }
  return syn->s0.A1;
}


/*!
  \fn int parse_subset_as_aaxx(struct synop_chunks *syn, struct bufr_subset_sequence_data *sq, int *kdtlst, size_t nlst, int *ksec1, char *err)
  \brief parses a subset sequence as an Land fixed SYNOP-12 FM report
  \param syn pointer to a struct \ref synop_chunks where set the results
  \param sq pointer to a struct \ref bufr_subset_sequence_data with the parsed sequence on input
  \param kdtlst array of descriptors (as integers) from the non-expanded bufr list
  \param nlst size of kdtlst array
  \param ksec1 array of integers set by ECMWF bufrdc library
  \param err string with detected errors, if any

  It return 0 if all is OK. Otherwise it also fills the \a err string
*/
int parse_subset_as_aaxx ( struct synop_chunks *syn, struct bufr_subset_sequence_data *sq, int *kdtlst, size_t nlst, int *ksec1, char *err )
{
  int ival;
  double val;
  size_t is;
  int disp; // time displacement in seconds
  time_t tt, itval;
  struct tm tim;
  struct bufr_subset_state s;
  char aux[16];

  // clean data
  clean_synop_chunks ( syn );
  memset(&s, 0, sizeof(struct bufr_subset_state));

  sprintf ( syn->s0.MiMi, "AA" );
  sprintf ( syn->s0.MjMj, "XX" );

  syn->mask = SYNOP_SEC1;

  /**** First pass *****/
  for ( is = 0, itval = 0; is < sq->nd; is++ )
    {
      if ( sq->sequence[is].mask & DESCRIPTOR_VALUE_MISSING )
        continue;
      s.i = is;
      s.ival = ( int ) sq->sequence[is].val;
      ival = ival;
      s.val = sq->sequence[is].val;
      val = s.val;
      s.a = &sq->sequence[is];
      switch ( sq->sequence[is].desc.x )
        {
        case 1: //localization descriptors
          syn_parse_x01 ( syn, &s, err );
          break;

        case 2: //Date time descriptors
          syn_parse_x02 ( syn, &s, err );
          break;

        case 4: //Date time descriptors
          syn_parse_x04 ( syn, &s, err );
          break;

        case 5: //Position
          syn_parse_x05 ( syn, &s, err );
          break;

        case 10: // Air Pressure descriptors
          syn_parse_x10 ( syn, &s, err );
          break;

        case 11: // wind  data
          syn_parse_x11 ( syn, &s, err );
          break;

        case 12: //Temperature descriptors
          syn_parse_x12 ( syn, &s, err );
          break;

        case 13: // Humidity and precipitation data
          syn_parse_x13 ( syn, &s, err );
          break;

        case 14: // Radiation
          syn_parse_x14 ( syn, &s, err );
          break;

        case 20: // Cloud data
          syn_parse_x20 ( syn, &s, err );
          break;

        default:
          break;
        }

    }

  /****** Second pass ************/
  // set ir
  if ( syn->s1.RRR[0] == 0 && syn->s3.RRR[0] == 0 && syn->s3.RRRR24[0] == 0 )
    {
      // no prec data case
      syn->s1.ir[0] = '4';
    }
  else if ( syn->s1.RRR[0] == 0 && syn->s3.RRR[0] != 0 )
    syn->s1.ir[0] = '2';
  else if ( syn->s1.RRR[0] != 0 && syn->s3.RRR[0] == 0 )
    syn->s1.ir[0] = '1';
  else
    syn->s1.ir[0] = '0';


  // adjust iw
  if ( syn->s0.iw[0] == '/' && syn->s1.ff[0] != '/' )
    syn->s0.iw[0] = '1';

  // adjust ix
  if (syn->s1.ix[0] == '/' && syn->s1.ww[0] == 0)
    {
      if ((s.mask & SUBSET_MASK_HAVE_NO_SIGNIFICANT_WW) &&  
          (s.mask & SUBSET_MASK_HAVE_NO_SIGNIFICANT_W1) && 
          (s.mask & SUBSET_MASK_HAVE_NO_SIGNIFICANT_W2)  
         )
        {
          if (s.mask & SUBSET_MASK_HAVE_TYPE_STATION)
            {
              if (s.type == 1 || s.type == 2)
                strcpy(syn->s1.ix,"2");
              else if (s.type == 0)
                strcpy(syn->s1.ix,"5");
            }
        }
      else if (s.mask & SUBSET_MASK_HAVE_TYPE_STATION)
        {
          if (s.type == 1)
            strcpy(syn->s1.ix,"3");
          else if (s.type == 0)
            strcpy(syn->s1.ix,"6");
        }
    }

  /****** Final Adjust ***********/
  // fix YYGG according with YYYYMMDDHHmm
  synop_YYYYMMDDHHmm_to_YYGG ( syn );
  if ( strcmp(syn->e.mm,"00"))
    {
      strcpy ( syn->s1.GG, syn->e.HH );
      strcpy ( syn->s1.gg, syn->e.mm );
    }
  syn->mask |= SYNOP_EXT;
  return 0;
}

/*!
 \fn int print_synop(char *report, size_t lmax, struct synop_chunks *syn)
 \brief prints a synop into a string
 \param report target string
 \param lmax max size of string
 \param syn pointer to a struct \ref synop_chuncks with the result of parse tasks

 returns 0 if all went right
*/
int print_synop ( char *report, size_t lmax, struct synop_chunks *syn )
{
  size_t i;
  char *c;

  c = report;

  // print time extension
  if (syn->e.YYYY[0] == 0)
    return 1;

  c += sprintf ( c, "%s%s%s%s%s", syn->e.YYYY, syn->e.MM, syn->e.DD, syn->e.HH, syn->e.mm );

  // Print type
  c += sprintf ( c, " %s%s", syn->s0.MiMi, syn->s0.MjMj );

  // print YYGGiw
  c += sprintf ( c, " %s%s%s", syn->s0.YY, syn->s0.GG, syn->s0.iw );

  // print IIiii
  c += sprintf ( c, " %s%s", syn->s0.II, syn->s0.iii );

  // printf Nddff
  c += sprintf ( c, " %s%s%s%s", syn->s1.ir, syn->s1.ix, syn->s1.h, syn->s1.VV );

  // printf Nddff
  c += sprintf ( c, " %s%s%s", syn->s1.N, syn->s1.dd, syn->s1.ff );

  if ( strlen ( syn->s1.fff ) )
    c += sprintf ( c, " 00%s", syn->s1.fff );

  // printf 1snTTT
  if ( syn->s1.TTT[0] )
    c += sprintf ( c, " 1%s%s", syn->s1.sn1, syn->s1.TTT );

  // printf 2snTdTdTd
  if ( syn->s1.TdTdTd[0] )
    c += sprintf ( c, " 2%s%s", syn->s1.sn2, syn->s1.TdTdTd );

  // printf 3PoPoPoPo
  if ( syn->s1.PoPoPoPo[0] )
    c += sprintf ( c, " 3%s", syn->s1.PoPoPoPo );

  // printf 4PPPP
  if ( syn->s1.PPPP[0] )
    c += sprintf ( c, " 4%s", syn->s1.PPPP );

  // printf 5appp
  if ( syn->s1.a[0] || syn->s1.ppp[0] )
    {
      if ( syn->s1.a[0] == 0 )
        syn->s1.a[0] = '/';
      if ( syn->s1.ppp[0] == 0 )
        strcpy ( syn->s1.ppp, "///" );
      c += sprintf ( c, " 5%s%s", syn->s1.a, syn->s1.ppp );
    }

  // printf 6RRRtr
  if ( syn->s1.tr[0] || syn->s1.RRR[0] )
    {
      if ( syn->s1.tr[0] == 0 )
        syn->s1.tr[0] = '/';
      if ( syn->s1.RRR[0] == 0 )
        strcpy ( syn->s1.RRR, "///" );
      c += sprintf ( c, " 6%s%s", syn->s1.RRR, syn->s1.tr);
    }

  if ( syn->s1.ww[0] || syn->s1.W1[0] || syn->s1.W2[0] )
    {
      if ( syn->s1.ww[0] == 0 )
        strcpy ( syn->s1.ww, "//" );
      if ( syn->s1.W1[0] == 0 )
        strcpy ( syn->s1.W1, "/" );
      if ( syn->s1.W2[0] == 0 )
        strcpy ( syn->s1.W2, "/" );
      c += sprintf ( c, " 7%s%s%s", syn->s1.ww, syn->s1.W1, syn->s1.W2 );
    }

  if ( (syn->s1.Nh[0] && syn->s1.Nh[0] != '0') || 
       (syn->s1.Cl[0] && syn->s1.Cl[0] != '0') || 
       (syn->s1.Cm[0] && syn->s1.Cm[0] != '0') || 
       (syn->s1.Ch[0] && syn->s1.Ch[0] != '0'))
    {
      if ( syn->s1.Nh[0] == 0 )
        strcpy ( syn->s1.Nh, "/" );
      if ( syn->s1.Cl[0] == 0 )
        strcpy ( syn->s1.Cl, "/" );
      if ( syn->s1.Cm[0] == 0 )
        strcpy ( syn->s1.Cm, "/" );
      if ( syn->s1.Ch[0] == 0 )
        strcpy ( syn->s1.Ch, "/" );
      c += sprintf(c, " 8%s%s%s%s", syn->s1.Nh, syn->s1.Cl, syn->s1.Cm, syn->s1.Ch);
    }

  if ( syn->s1.GG[0] )
    c+= sprintf ( c, " 9%s%s", syn->s1.GG, syn->s1.gg );

  if ( syn->mask & SYNOP_SEC3 )
    {
      c += sprintf ( c, " 333" );

      // printf 1snxTxTxTx
      if (syn->s3.snx[0])
        {
          c += sprintf(c, " 1%s%s", syn->s3.snx, syn->s3.TxTxTx);
        }

      // printf 1snnTnTnTn
      if (syn->s3.snn[0])
        {
          c += sprintf(c, " 2%s%s", syn->s3.snn, syn->s3.TnTnTn);
        }

      // printf 3Ejjj
      if (syn->s3.E[0] || syn->s3.jjj[0])
        {
          if (syn->s3.E[0] == 0)
            syn->s3.E[0] = '/';
          if (syn->s3.jjj[0] == 0)
            strcpy(syn->s3.jjj, "///");
          c += sprintf(c, " 3%s%s", syn->s3.E, syn->s3.jjj);
        }

      /**** Radiation Sunshine gropus ***/

      // print 55SSS
      if (syn->s3.SSS[0])
        {
          c += sprintf(c, " 55%s", syn->s3.SSS);
          for (i = 0; i < 7; i++)
            {
              if (syn->s3.j524[i][0])
                sprintf(c, " %s%s", syn->s3.j524[i], syn->s3.FFFF24[i]);
            }
        }

      // print 553SS
      if (syn->s3.SS[0])
        {
          c += sprintf(c, " 553%s", syn->s3.SS);
          for (i = 0; i < 7; i++)
            {
              if (syn->s3.j5[i][0])
                sprintf(c, " %s%s", syn->s3.j5[i], syn->s3.FFFF[i]);
            }

        }
      // printf 6RRRtr
      if ( syn->s3.tr[0] || syn->s3.RRR[0] )
        {
          if ( syn->s3.tr[0] == 0 )
            syn->s3.tr[0] = '/';
          if ( syn->s3.RRR[0] == 0 )
            strcpy ( syn->s3.RRR, "///" );
          c += sprintf ( c, " 6%s%s", syn->s3.RRR ,syn->s3.tr);
        }

      if ( syn->s3.RRRR24[0] )
        c += sprintf ( c, " 7%s", syn->s3.RRRR24);

    }
  c += sprintf ( c, "=" );

  return 0;
}
