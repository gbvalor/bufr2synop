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
int synop_YYYYMMDDHHmm_to_YYGG(struct synop_chunks *syn)
{
   char aux[20];
   time_t t;
   struct tm tim;

   if (strlen(syn->e.YYYY) && 
       strlen(syn->e.MM) &&
       strlen(syn->e.DD) &&
       strlen(syn->e.HH) &&
       strlen(syn->e.MM))
    sprintf(aux,"%s%s%s%s%s", syn->e.YYYY, syn->e.MM, syn->e.DD, syn->e.HH, syn->e.mm);

    if (strlen(aux) != 12)
       return 1;

    memset(&tim, 0, sizeof(struct tm));
    strptime(aux, "%Y%m%d%H%M", &tim);

    t = mktime(&tim) + 1799 ; // rounding trick
    gmtime_r(&t, &tim);
    sprintf(syn->s0.YY, "%02d", tim.tm_mday); 
    sprintf(syn->s0.GG, "%02d", tim.tm_hour); 
    return 0;
}

char * kelvin_to_snTTT(char *target, double T)
{
   int ic;
   ic = (int) 100.0 * T - 27315.0;
   ic /= 10;
   if (ic < 0)
     sprintf(target, "1%03d", -ic);
   else
     sprintf(target, "%04d", ic);
   return target;
}

char * pascal_to_PPPP(char *target, double P)
{
   int ic;
   ic = (int) (P * 0.1);
   sprintf(target, "%04d", ic % 10000);
   return target;
}

char * m_to_h(char *target, double h)
{
    if (h < 50.0)
      strcpy(target,"0");
    else if ( h < 100.0)
      strcpy(target,"1");
    else if ( h < 200.0)
      strcpy(target,"2");
    else if ( h < 300.0)
      strcpy(target,"3");
    else if ( h < 600.0)
      strcpy(target,"4");
    else if ( h < 1000.0)
      strcpy(target,"5");
    else if ( h < 1500.0)
      strcpy(target,"6");
    else if ( h < 2000.0)
      strcpy(target,"7");
    else if ( h < 2500.0)
      strcpy(target,"8");
    else 
      strcpy(target,"9");
   return target;

}

char * pascal_to_ppp(char *target, double P)
{
   int ic;
   if (P > 0)
     ic = (int) (P * 0.1);
   else
     ic = (int) (-P * 0.1);
   sprintf(target, "%03d", ic % 1000);
   return target;
}

char * vism_to_VV(char *target, double V)
{
   if (V < 100.0)
     strcpy(target, "00");
   else if ( V <= 5000.0)
     sprintf(target, "%02d", (int) (V + 0.1) / 100);
   else if ( V <= 30000.0)
     sprintf(target, "%02d", (int) (V + 0.1) / 1000 + 50);
   else if (V <= 70000.0)
     sprintf(target, "%02d", (int) (V - 30000.0) / 5000 + 80);
   else
     strcpy(target, "89");
   return target;
}

char * percent_to_okta(char *target, double perc)
{
   if (perc == 0.0)
     strcpy(target,"0");
   else if (perc <= 12.5)
     strcpy(target,"1");
   else if (perc <= 25.0)
     strcpy(target,"2");
   else if (perc <= 37.5)
     strcpy(target,"3");
   else if (perc <= 50.0)
     strcpy(target,"4");
   else if (perc <= 62.5)
     strcpy(target,"5");
   else if (perc <= 75.0)
     strcpy(target,"6");
   else if (perc <= 87.5)
     strcpy(target,"7");
   else if (perc <= 100.0)
     strcpy(target,"8");
   else if (perc == 113.0)
     strcpy(target,"/");
   return target;
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
int parse_subset_as_aaxx(struct synop_chunks *syn, struct bufr_subset_sequence_data *sq, int *kdtlst, size_t nlst, int *ksec1, char *err)
{
    int ival;
    double val;
    size_t is;
    time_t tt;
    struct tm tim;
    char aux[16];

    // clean data
    clean_synop_chunks(syn);

    sprintf(syn->s0.MiMi, "AA");
    sprintf(syn->s0.MjMj, "XX");


    /**** First pass *****/
    for (is = 0; is < sq->nd; is++)
    {
       if (sq->sequence[is].mask & DESCRIPTOR_VALUE_MISSING)
         continue;
       ival = (int) sq->sequence[is].val;
       val = sq->sequence[is].val;
       switch (sq->sequence[is].desc.x)
       {
          case 1: //localization descriptors
            switch(sq->sequence[is].desc.y)
            {
              case 1: // 0 01 001
                 sprintf(syn->s0.II, "%02d", ival);
              break;
              case 2: // 0 01 002
                 sprintf(syn->s0.iii, "%03d", ival);
              break;
              default:
              break;
            }
          break;
          case 4: //Date time descriptors
            switch(sq->sequence[is].desc.y)
            {
              case 1: // 0 04 001
                 sprintf(syn->e.YYYY, "%04d", ival);
              break;
              case 2: // 0 04 002
                 sprintf(syn->e.MM, "%02d", ival);
              break;
              case 3: // 0 04 003
                 sprintf(syn->e.DD, "%02d", ival);
                 //sprintf(syn->s0.YY, "%02d", (int) sq->sequence[is].val);
              break;
              case 4: // 0 04 004
                 sprintf(syn->e.HH, "%02d", ival);
                 //sprintf(syn->s0.GG, "%02d", (int) sq->sequence[is].val);
              break;
              case 5: // 0 04 005
                 sprintf(syn->e.mm, "%02d", ival);
              break;
            }
          break;
          case 2: // Intrumentation descriptors
            {
              switch(sq->sequence[is].desc.y)
              {
              case 2: // 0 02 002
                 if (ival & 4)
                   strcpy(syn->s0.iw, "4");
                 else
                   strcpy(syn->s0.iw, "1");
              break;
              }
            }
          break;
          case 12: //Temperature descriptors
              switch(sq->sequence[is].desc.y)
              {
                case 1: // 0 12 001
                case 4: // 0 12 004
                case 101: // 0 12 101
                case 104: // 0 12 104
                  kelvin_to_snTTT(aux, val);
                  syn->s1.sn1[0] = aux[0];
                  strcpy(syn->s1.TTT, aux + 1); 
                break;
                case 3: // 0 12 003
                case 6: // 0 12 006
                case 103: // 0 12 103
                case 106: // 0 12 106
                  kelvin_to_snTTT(aux, val);
                  syn->s1.sn2[0] = aux[0];
                  strcpy(syn->s1.TdTdTd, aux + 1); 
                break;

                default:
                break;
              } 
           break;
          case 10: // Air Pressure descriptors
              switch(sq->sequence[is].desc.y)
              {
                case 4: // 0 10 004
                  pascal_to_PPPP(aux, val);
                  strcpy(syn->s1.PoPoPoPo, aux); 
                break;
                case 51: // 0 10 051
                  pascal_to_PPPP(aux, val);
                  strcpy(syn->s1.PPPP, aux); 
                break;
                case 63: // 0 10 063
                  sprintf(syn->s1.a, "%1d",ival); 
                break;
                case 61: // 0 10 061
                  pascal_to_ppp(aux, val);
                  sprintf(syn->s1.ppp, "%s", aux); 
                break;
                default:
                break;
              } 
           break;
          case 11: // wind  data
              switch(sq->sequence[is].desc.y)
              {
                case 1: // 0 11 001
                  sprintf(syn->s1.dd, "%02d", (ival + 5) / 10);
                break;
                case 2: // 0 11 002
                  if (strlen(syn->s0.iw) && syn->s0.iw[0] == '4')
                     val *= 1.94384449;
                  if (val < 100.0)
                    sprintf(syn->s1.ff, "%02d", (int) (val + 0.5));
                  else
                  {
                     sprintf(syn->s1.ff,"99");
                     sprintf(syn->s1.fff, "%03d", (int) (val + 0.5));
                  }
                break;
                default:
                break;
              } 
           break;

          case 20: // Cloud data
              switch(sq->sequence[is].desc.y)
              {
                case 1: // 0 20 001
                  vism_to_VV(syn->s1.VV, val);
                break;
                case 10: // 0 20 010
                  percent_to_okta(syn->s1.N, val);
                break;
                case 11: // 0 20 011
                  if (ival <= 8)
                    sprintf(syn->s1.N, "%1d", ival);
                  else if (ival <= 10)
                    sprintf(syn->s1.N, "9", ival);
                  else if (ival == 15)
                    sprintf(syn->s1.N, "/", ival);
                break;
                 case 13: // 0 20 013
                  percent_to_okta(syn->s1.h, val);
                break;
               default:
                break;
              } 
           break;

          default:
          break;
       }

    }

    /****** Second pass ************/


    /****** Final Adjust ***********/
    // fix YYGG according with YYYYMMDDHHmm
    synop_YYYYMMDDHHmm_to_YYGG(syn);
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
int print_synop(char *report, size_t lmax, struct synop_chunks *syn)
{
   char *c;

   c = report;
   // print time extension
   c += sprintf(c, "%s%s%s%s%s", syn->e.YYYY, syn->e.MM, syn->e.DD, syn->e.HH, syn->e.mm);

   // Print type
   c += sprintf(c, " %s%s", syn->s0.MiMi, syn->s0.MjMj);

   // print YYGGiw
   c += sprintf(c, " %s%s%s", syn->s0.YY, syn->s0.GG, syn->s0.iw);

   // print IIiii
   c += sprintf(c, " %s%s", syn->s0.II, syn->s0.iii);

   // printf Nddff
   c += sprintf(c, " %s%s%s%s", syn->s1.ir, syn->s1.ix, syn->s1.h, syn->s1.VV);

   // printf Nddff
   c += sprintf(c, " %s%s%s", syn->s1.N, syn->s1.dd, syn->s1.ff);

   if (strlen(syn->s1.fff))
     c += sprintf(c, " 00%s", syn->s1.fff);

   // printf 1snTTT
   c += sprintf(c, " 1%s%s", syn->s1.sn1, syn->s1.TTT);

   // printf 2snTdTdTd
   c += sprintf(c, " 2%s%s", syn->s1.sn2, syn->s1.TdTdTd);

   // printf 3PoPoPoPo
   c += sprintf(c, " 3%s", syn->s1.PoPoPoPo);

   // printf 4PPPP
   c += sprintf(c, " 4%s", syn->s1.PPPP);

   // printf 5appp
   c += sprintf(c, " 5%s%s", syn->s1.a, syn->s1.ppp);

   
   c += sprintf(c, "=");

   return 0;
}
