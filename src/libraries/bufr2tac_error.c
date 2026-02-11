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
 \file bufr2tac_error.c
 \brief This file has the code for functions to manage errors in bufr2tac library
*/
#include "bufr2tac.h"

/*!
  \fn int bufr2tac_push_error(struct bufr2tac_error_stack* e, int severity, const char* description)
  \brief Push an error onto the error stack
  \param [in,out] e Pointer to struct \ref bufr2tac_error_stack where to push the error
  \param [in] severity Severity level: 0=INFO, 1=WARNING, 2=ERROR
  \param [in] description Error description string
  \return 1 if error pushed successfully, 0 if stack is full, -1 on fatal error
*/
int bufr2tac_push_error(struct bufr2tac_error_stack* e, int severity, const char* description)
{
    // avoid segfaults
    if (e == NULL)
        return -1; // Fatal error

    if (e->ne < BUFR2TAC_ERROR_STACK_DIM) {
        e->err[e->ne].severity = severity;
        strncpy(e->err[e->ne].description, description, sizeof(e->err[e->ne].description) - 1);
        e->err[e->ne].description[sizeof(e->err[e->ne].description) - 1] = '\0';
        (e->ne)++;
        return 1;
    } else if (e->ne == BUFR2TAC_ERROR_STACK_DIM) {
        // Do not pull error, but annotate the stack as rebased
        if (e->full < severity)
            e->full = severity;
        return 0;
    }
    return -1; // bad e->ne
}

/*!
  \fn int bufr2tac_clean_error_stack(struct bufr2tac_error_stack* e)
  \brief Clean and reset the error stack
  \param [in,out] e Pointer to struct \ref bufr2tac_error_stack to clean
  \return 0 on success, -1 if e is NULL
*/
int bufr2tac_clean_error_stack(struct bufr2tac_error_stack* e)
{
    if (e != NULL) {
        memset(e, 0, sizeof(struct bufr2tac_error_stack));
        return 0;
    }
    return -1;
}

/*!
  \fn int bufr2tac_set_error(struct bufr2tac_subset_state* s, int severity, const char* origin, const char* explanation)
  \brief Set an error with descriptor context information
  \param [in,out] s Pointer to struct \ref bufr2tac_subset_state containing current parsing state
  \param [in] severity Severity level: 0=INFO, 1=WARNING, 2=ERROR
  \param [in] origin String identifying the function or location where error occurred
  \param [in] explanation Error explanation string
  \return Result from bufr2tac_push_error()
*/
int bufr2tac_set_error(struct bufr2tac_subset_state* s, int severity, const char* origin, const char* explanation)
{
    char description[BUFR2TAC_ERROR_DESCRIPTION_LENGTH];
    char* c = description;
    size_t rem = sizeof(description);
    int n;

    n = snprintf(c, rem, "%s: ", origin);
    if (n >= 0 && (size_t)n < rem) {
        c += n;
        rem -= n;
    } else {
        rem = 0;
    }

    if (rem > 0) {
        n = snprintf(c, rem, " Descriptor: %u %02u %03u: \"%s\"", s->a->desc.f, s->a->desc.x, s->a->desc.y, s->a->name);
        if (n >= 0 && (size_t)n < rem) {
            c += n;
            rem -= n;
        } else {
            rem = 0;
        }
    }

    if (rem > 0) {
        if (s->a->mask & DESCRIPTOR_VALUE_MISSING) {
            n = snprintf(c, rem, " = MISSING. ");
            if (n >= 0 && (size_t)n < rem) {
                c += n;
                rem -= n;
            } else {
                rem = 0;
            }
        } else {
            if (s->a->cval[0])
                n = snprintf(c, rem, " = '%s'. ", s->a->cval);
            else if (s->a->desc.x == 2)
                n = snprintf(c, rem, " = '%s'. ", s->a->ctable);
            else
                n = snprintf(c, rem, " = %lf . ", s->a->val);

            if (n >= 0 && (size_t)n < rem) {
                c += n;
                rem -= n;
            } else {
                rem = 0;
            }
        }
    }

    if (rem > 0) {
        snprintf(c, rem, "%s", explanation);
    }

    return bufr2tac_push_error(&s->e, severity, description);
}

/*!
  \fn int bufr2tac_print_error(const struct bufr2tac_error_stack* e)
  \brief Print all errors in the error stack to stdout
  \param [in] e Pointer to struct \ref bufr2tac_error_stack to print
  \return 0 on success, 1 if unknown severity found
*/
int bufr2tac_print_error(const struct bufr2tac_error_stack* e)
{
    unsigned int i;
    if (e->ne == 0) {
        printf("# No info/warning/error \n");
        return 0;
    }

    for (i = 0; i < e->ne; i++) {
        if (e->err[i].severity == 0)
            printf("# INFO: ");
        else if (e->err[i].severity == 1)
            printf("# WARNING: ");
        else if (e->err[i].severity == 2)
            printf("# ERROR: ");
        else {
            printf("# %d\n", e->err[i].severity);
            return 1;
        }
        printf("%s\n", e->err[i].description);
    }
    if (e->full > 0)
        printf("# More debug info follows, stack of logs full\n");
    return 0;
}
