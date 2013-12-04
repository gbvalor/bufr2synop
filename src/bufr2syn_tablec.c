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
 \file bufr2syn_tablec.c
 \brief file with the code to read table C data (code and flag tables)
 */
#include "bufr2synop.h"

/*!
  \fn char * get_ecmwf_tablename(char *target, char type)
  \brief Get the complete pathname of a table file needed by a bufr message
  \param target the resulting name
  \param type a char with type, i.e, 'B', 'C', or 'D'

  In ECMWF library the name of a table file is Kssswwwwwxxxxxyyyzzz.TXT , where
       - K - type of table, i.e, 'B', 'C', or 'D'
       - sss - Master table number (zero for WMO meteorological tables)
       - wwwww - Originating sub-centre
       - xxxxx - Originating centre
       - yyy - Version number of master table used
       - zzz - Version number of local table used

  If standard WMO tables are used, the Originating centre xxxxx will be set to
*/
char * get_ecmwf_tablename(char *target, char type)
{
  if (KSEC1[13] != 0) // case of not WMO tables
    sprintf(target,"%s%c%03d%05d%05d%03d%03d.TXT", BUFRTABLES_DIR, type, KSEC1[13], KSEC1[15], KSEC1[2], KSEC1[14], KSEC1[7]);
  else
    sprintf(target,"%s%c000%05d00000%03d%03d.TXT", BUFRTABLES_DIR, type, KSEC1[15], KSEC1[14], KSEC1[7]);
  return target;
}

/*!
  \fn int read_table_c(void)
  \brief read a Table C file with code TABLE and flag descriptors
*/
int read_table_c(void)
{
  char *c, file[256];
  FILE *t;
  size_t i = 0;

  NLINES_TABLEC = 0;

  get_ecmwf_tablename(&file[0], 'C');

  if ((t = fopen(file, "r")) == NULL)
    {
      fprintf(stderr,"Unable to open table C file '%s'\n", file);
      return 0;
    }

  while (fgets(TABLEC[i], 90, t) != NULL && i < MAXLINES_TABLEC)
    {
      // supress the newline
      if ((c = strrchr(TABLEC[i],'\n')) != NULL)
        *c = '\0';
      i++;
    }
  fclose(t);
  NLINES_TABLEC = i;
  return i;
}

/*!
  \fn char * get_explained_table_val(char *expl, size_t dim, struct bufr_descriptor *d, int ival)
  \brief gets a strung with the meaning of a value for a code table descriptor
  \param expl string with resulting meaning
  \param dim max length alowed for \a expl string
  \param d pointer to the source descriptor
  \param ival integer value for the descriptos

  If something went wrong, it returns NULL . Otherwise it returns \a expl
*/
char * get_explained_table_val(char *expl, size_t dim, struct bufr_descriptor *d, int ival)
{
  char *c;
  long nv, v,  nl;
  size_t i, j;

  // Find first line for descriptor
  for (i = 0; i <  NLINES_TABLEC; i++)
    {
      if (TABLEC[i][0] != d->c[0] ||
          TABLEC[i][1] != d->c[1] ||
          TABLEC[i][2] != d->c[2] ||
          TABLEC[i][3] != d->c[3] ||
          TABLEC[i][4] != d->c[4] ||
          TABLEC[i][5] != d->c[5])
        continue;
      else
        break;
    }

  if (i == NLINES_TABLEC)
    {
      //printf("Descriptor %s No encontrado\n", d->c);
      return NULL;
    }
  //printf("Descriptor %s en linea %d\n", d->c, i);

  // reads the amount of possible values
  if (TABLEC[i][7] != ' ')
    nv = strtol(&TABLEC[i][7], &c, 10);
  else
    return NULL;

  // read a value
  for (j = 0; j < nv && i < NLINES_TABLEC ; i++)
    {
      if (TABLEC[i][12] != ' ')
        {
          v = strtol(&TABLEC[i][12], &c, 10);
          j++;
          if (v != ival)
            continue;
          break;
        }
    }

  if (j == nv || i == NLINES_TABLEC)
    return NULL; // Value not found

  // read how many lines for the descriptors
  nl = strtol(&TABLEC[i][21], &c, 10);


  // if match then we have finished the search
  strcpy(expl, &TABLEC[i][24]);
  if (nl > 1)
    {
      for (nv = 1 ; nv < nl; nv++)
        if ((strlen(expl) + strlen(&TABLEC[i + nv][22])) < dim)
          strcat(expl, &TABLEC[i + nv][22]);
    }

  return expl;
}

/*!
  \fn char * get_explained_flag_val(char *expl, size_t dim, struct bufr_descriptor *d, unsigned long ival)
  \brief gets a strung with the meaning of a value for a flag table descriptor
  \param expl string with resulting meaning
  \param dim max length alowed for \a expl string
  \param d pointer to the source descriptor
  \param ival integer value for the descriptos

  Remember that in FLAG tables for bufr, bit 1 is most significant and N the less one. Bit N only is set to
  1 when all others are also set to one, i.e. in case of missing value.

  If something went wrong, it returns NULL . Otherwise it returns \a expl
*/
char * get_explained_flag_val(char *expl, size_t dim, struct bufr_descriptor *d, unsigned long ival)
{
  char *c, *s;
  unsigned long test, test0;
  unsigned long nb, nx, v,  nl;
  size_t i, j;

  // Find first line for descriptor
  for (i = 0; i <  NLINES_TABLEC; i++)
    {
      if (TABLEC[i][0] != d->c[0] ||
          TABLEC[i][1] != d->c[1] ||
          TABLEC[i][2] != d->c[2] ||
          TABLEC[i][3] != d->c[3] ||
          TABLEC[i][4] != d->c[4] ||
          TABLEC[i][5] != d->c[5])
        continue;
      else
        break;
    }

  if (i == NLINES_TABLEC)
    {
      //printf("Descriptor %s No encontrado\n", d->c);
      return NULL;
    }
  //printf("Descriptor %s en linea %d\n", d->c, i);

  // reads the amount of possible bits
  if (TABLEC[i][7] != ' ')
    nb = strtol(&TABLEC[i][7], &c, 10);
  else
    return NULL;

  // read a value
  s = expl;
  s[0] = '\0';

  for (j = 0, test0 = 2; j < nb && i < NLINES_TABLEC ; i++)
    {
      if (TABLEC[i][12] != ' ')
        {
          v = strtol(&TABLEC[i][12], &c, 10); // v is the bit number
          j++;

          // case 0 with meaning 
          if (v == 0)
            {
              test0 = 1;
              if (ival == 0)
                {
                  nl = strtol(&TABLEC[i][21], &c, 10);
                  if (strlen(expl) && (strlen(expl) + 1) < dim)
                    s += sprintf(s, "|");
                  s += sprintf(s,"%s", &TABLEC[i][24]);
                  if (nl > 1)
                    {
                      for (nx = 1 ; nx < nl; nx++)
                        if ((strlen(expl) + strlen(&TABLEC[i + nx][22]))  < dim)
                          {
                            s += sprintf(s, "%s", &TABLEC[i + nx][22]);
                          }
                    }
                  return expl;
                }
            }

          test = test0 << (nb - v);

          if (v && (test & ival) != 0)
            {
              // bit match
              // read how many lines for the descriptors
              nl = strtol(&TABLEC[i][21], &c, 10);
              if (strlen(expl) && (strlen(expl) + 1) < dim)
                s += sprintf(s, "|");
              s += sprintf(s,"%s", &TABLEC[i][24]);
              if (nl > 1)
                {
                  for (nx = 1 ; nx < nl; nx++)
                    if ((strlen(expl) + strlen(&TABLEC[i + nx][22]))  < dim)
                      {
                        s += sprintf(s, "%s", &TABLEC[i + nx][22]);
                      }
                }

            }
          else
            continue;
        }
    }

  // if match then we have finished the search

  return expl;
}
