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
 \file bufr2syn_env.c
 \brief file with the code to set environments 
 */
#include "bufr2synop.h"

/*!
  \fn int set_environment(void)
  \brief set the environment vars needed to work properly with ECMWF bufrdc library

  During decoding Bufr table path and the names are printed. If user doeas not want that, set: VARIABLE
    PRINT_TABLE_NAMES=false

  During decoding code/flag tables could be read if code figure meaning is needed. If user want to use 
  code and flag tables set: VARIABLE USE TABLE C=true

  Then we set the proper environment here 
*/
int set_environment(void)
{
   int i;
   char aux[256], *c;
   struct stat s;

   i = stat(DEFAULT_BUFRTABLES, &s);

   if (putenv("PRINT_TABLE_NAMES=false") || putenv("USE_TABLE_C=true"))
    {
      fprintf(stderr, "%s: Failure setting the environment\n", SELF);
      exit (EXIT_FAILURE);
    }

    /*
      Default path for Bufr Tables is hard coded in the software. To change the path set environmental variable
      BUFR_TABLES . The path must end with '/'
    */
    if (BUFRTABLES_DIR[0])
    {
        sprintf(aux,"BUFR_TABLES=%s", BUFRTABLES_DIR);
        if (putenv(aux))
        {
           fprintf(stderr, "%s: Failure setting the environment\n", SELF);
           exit (EXIT_FAILURE);
        }
    }
    else if ((c = getenv("BUFR_TABLES")) != NULL)
    {
       strcpy(BUFRTABLES_DIR, c); // otherwise check if BUFRRABLES_DIR if is on environment
    }
    else if (i == 0 && S_ISDIR(s.st_mode)) // last chance, the default dir
    {  
        strcpy(BUFRTABLES_DIR, DEFAULT_BUFRTABLES);
        sprintf(aux,"BUFR_TABLES=%s", BUFRTABLES_DIR);
        if (putenv(aux))
        {
           fprintf(stderr, "%s: Failure setting the environment\n", SELF);
           exit (EXIT_FAILURE);
        }
    }
    else 
       {
          fprintf(stderr,"%s: Unable to find bufrtables directory\n");
          fprintf(stderr,"    Please set the proper enviromnet 'BUFR_TABLES=my_bufrtables_dir' or\n");
          fprintf(stderr,"    use '-t' argument . i. e.'-t my_bufrtables_dir/'\n");
          exit (EXIT_FAILURE);
       }
    return 0;
}
