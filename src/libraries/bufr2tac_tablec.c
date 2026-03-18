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
 \file bufr2tac_tablec.c
 \brief file with the code to read table C data (code and flag tables)
 */
#include "bufr2tac.h"

/*!
  \fn char * get_ecmwf_tablename(char *target, char type, const char *bufrtables_dir, const int ksec1[40])
  \brief Get the complete pathname of a table file needed by a bufr message
  \param [out] target The resulting pathname
  \param [in] type A char with type, i.e, 'B', 'C', or 'D'
  \param [in] bufrtables_dir String with path to bufr file tables dir
  \param [in] ksec1 Array of integers processed previously by ECMWF bufr library
  \return Pointer to target

  In ECMWF library the name of a table file is Kssswwwwwxxxxxyyyzzz.TXT , where
       - K - type of table, i.e, 'B', 'C', or 'D'
       - sss - Master table number (zero for WMO meteorological tables)
       - wwwww - Originating sub-centre
       - xxxxx - Originating centre
       - yyy - Version number of master table used
       - zzz - Version number of local table used

  If standard WMO tables are used, the Originating centre xxxxx will be set to
*/
char* get_ecmwf_tablename(char* target, char type, const char* bufrtables_dir, const int ksec1[40])
{
    if (0) {
        if (ksec1[13] != 0) // case of not WMO tables
            snprintf(target, 256, "%s%c%03d%05d%05d%03d%03d.TXT", bufrtables_dir, type, ksec1[13], ksec1[15], ksec1[2], ksec1[14], ksec1[7]);
        else
            snprintf(target, 256, "%s%c000%05d00000%03d%03d.TXT", bufrtables_dir, type, ksec1[15], ksec1[14], ksec1[7]);
    } else {
        // FIXME Some countries uses latest C tables instead of the version they said in bufr
        snprintf(target, 256, "%s%c0000000000000019000.TXT", bufrtables_dir, type);
    }
    return target;
}

/*!
  \fn int read_table_c(char tablec[MAXLINES_TABLEC][92], size_t* nlines_tablec, const char* bufrtables_dir, int ksec1[40])
  \brief read a Table C file with code TABLE and flag descriptors
  \param [out] tablec Array to store table C lines
  \param [out] nlines_tablec Pointer to store number of lines read
  \param [in] bufrtables_dir Path to bufr tables directory
  \param [in] ksec1 Array of integers from ECMWF bufr library
  \return Number of lines read, 0 on error
*/
int read_table_c(char tablec[MAXLINES_TABLEC][92], size_t* nlines_tablec, const char* bufrtables_dir, int ksec1[40])
{
    char file[256];
    FILE* t;
    size_t i = 0;

    *nlines_tablec = 0;

    get_ecmwf_tablename(&file[0], 'C', bufrtables_dir, ksec1);

    if ((t = fopen(file, "r")) == NULL) {
        fprintf(stderr, "Unable to open table C file '%s'\n", file);
        return 0;
    }

    while (i < MAXLINES_TABLEC && fgets(tablec[i], 90, t) != NULL) {
        char *c;
        // supress the newline
        if ((c = strrchr(tablec[i], '\n')) != NULL)
            *c = '\0';
        i++;
    }
    fclose(t);
    *nlines_tablec = i;
    // printf("file='%s', NLINES_TABLEC=%d\n", file,NLINES_TABLEC);
    return i;
}

/*!
  \fn char * get_explained_table_val(char *expl, size_t dim, const char tablec[MAXLINES_TABLEC][92], size_t nlines_tablec, const struct bufr_descriptor *d, int ival)
  \brief gets a string with the meaning of a value for a code table descriptor
  \param [out] expl String with resulting meaning
  \param [in] dim Max length allowed for expl string
  \param [in] tablec Table C data array
  \param [in] nlines_tablec Number of lines in tablec
  \param [in] d Pointer to the source descriptor
  \param [in] ival Integer value for the descriptor
  \return Pointer to expl on success, NULL on error
*/
char* get_explained_table_val(char* expl, size_t dim, const char tablec[MAXLINES_TABLEC][92], size_t nlines_tablec, const struct bufr_descriptor* d, int ival)
{
    char* c;
    long nv, nl;
    size_t i, j;

    // Find first line for descriptor
    for (i = 0; i < nlines_tablec; i++) {
        if (tablec[i][0] != d->c[0] || tablec[i][1] != d->c[1] || tablec[i][2] != d->c[2] || tablec[i][3] != d->c[3] || tablec[i][4] != d->c[4] || tablec[i][5] != d->c[5])
            continue;
        else
            break;
    }

    if (i == nlines_tablec) {
        // printf("Descriptor %s No encontrado\n", d->c);
        return NULL;
    }
    // printf("Descriptor %s en linea %d\n", d->c, i);

    // reads the amount of possible values
    if (tablec[i][7] != ' ')
        nv = strtol(&tablec[i][7], &c, 10);
    else
        return NULL;

    // read a value
    for (j = 0; (long)j < nv && i < nlines_tablec; i++) {
        if (tablec[i][12] != ' ') {
            long v = strtol(&tablec[i][12], &c, 10);
            j++;
            if (v != ival)
                continue;
            break;
        }
    }

    if ((long)j == nv || i == nlines_tablec)
        return NULL; // Value not found

    // read how many lines for the descriptors
    nl = strtol(&tablec[i][21], &c, 10);

    // if match then we have finished the search
    strncpy_safe(expl, &tablec[i][24], dim);
    if (nl > 1) {
        for (nv = 1; nv < nl; nv++)
            strcat_safe(expl, &tablec[i + nv][22], dim);
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
char* get_explained_flag_val(char* expl, size_t dim, const char tablec[MAXLINES_TABLEC][92], size_t nlines_tablec, const struct bufr_descriptor* d, unsigned long ival)
{
    char* c;
    unsigned long test0;
    unsigned long nb, nx, v, nl;
    size_t i, j;

    // Find first line for descriptor
    for (i = 0; i < nlines_tablec; i++) {
        if (tablec[i][0] != d->c[0] || tablec[i][1] != d->c[1] || tablec[i][2] != d->c[2] || tablec[i][3] != d->c[3] || tablec[i][4] != d->c[4] || tablec[i][5] != d->c[5])
            continue;
        else
            break;
    }

    if (i == nlines_tablec) {
        // printf("Descriptor %s No encontrado\n", d->c);
        return NULL;
    }
    // printf("Descriptor %s en linea %d\n", d->c, i);

    // reads the amount of possible bits
    if (tablec[i][7] != ' ')
        nb = strtol(&tablec[i][7], &c, 10);
    else
        return NULL;

    // read a value
    expl[0] = '\0';

    for (j = 0, test0 = 2; j < nb && i < nlines_tablec; i++) {
        if (tablec[i][12] != ' ') {
            v = strtol(&tablec[i][12], &c, 10); // v is the bit number
            j++;

            // case 0 with meaning
            if (v == 0) {
                test0 = 1;
                if (ival == 0) {
                    nl = strtol(&tablec[i][21], &c, 10);
                    if (strlen(expl) && (strlen(expl) + 1) < dim)
                        strcat_safe(expl, "|", dim);

                    strcat_safe(expl, &tablec[i][24], dim);

                    if (nl > 1) {
                        for (nx = 1; nx < nl; nx++)
                            if ((strlen(expl) + strlen(&tablec[i + nx][22])) < dim) {
                                strcat_safe(expl, &tablec[i + nx][22], dim);
                            }
                    }
                    return expl;
                }
            }

            unsigned long test = test0 << (nb - v);

            if (v && (test & ival) != 0) {
                // bit match
                // read how many lines for the descriptors
                nl = strtol(&tablec[i][21], &c, 10);
                if (strlen(expl) && (strlen(expl) + 1) < dim)
                    strcat_safe(expl, "|", dim);

                strcat_safe(expl, &tablec[i][24], dim);

                if (nl > 1) {
                    for (nx = 1; nx < nl; nx++)
                        if ((strlen(expl) + strlen(&tablec[i + nx][22])) < dim) {
                            strcat_safe(expl, &tablec[i + nx][22], dim);
                        }
                }

            } else
                continue;
        }
    }

    // if match then we have finished the search

    return expl;
}
