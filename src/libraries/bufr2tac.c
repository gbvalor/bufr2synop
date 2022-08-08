/***************************************************************************
 *   Copyright (C) 2013-2022 by Guillermo Ballester Valor                  *
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
 \file bufr2tac.c
 \brief This file has the code for functions gettong information about library bufr2tac
*/
#ifndef CONFIG_H
# include "config.h"
# define CONFIG_H
#endif

#include "bufr2tac.h"

/*!
  \fn char *bufr2tac_get_version(char *version, char *build, char *builder, int *version_major, int *version_minor, int *version_patch)
  \brief Get strings with version information and build date and time
  \param version  pointer to string with version as result if not NULL
  \param build pointer to string with compiler and compilation date and time if not NULL 
  \param builder pointer to string with builder utility. 'cmake' or 'autotools' if not NULL 
  \param version_major pointer to string with version_major component if not NULL
  \param version_minor pointer to string with version_minor component if not NULL
  \param version_patch pointer to string with version_patch component if not NULL
  
  Retuns string pointer \ref version.  
 */
char *bufr2tac_get_version(char *version, char *build, char *builder, int *version_major, int *version_minor, int *version_patch)
{
  int major = 0, minor = 0, patch = 0;
  char *c;
  
  if (version == NULL)
    return NULL;
  sprintf(version, "%s", VERSION);
  // default values
  sscanf(version, "%d.%d.%d", &major, &minor, &patch);
  
  if (build)
    {
       c = build;
#ifdef __INTEL_COMPILER
       c += sprintf(build, "using INTEL C compiler icc %s ", __VERSION__);
#elifdef __GNUC__ 
       c += sprintf(build, "using GNU C compiler gcc %d.%d.%d ", __GNUC__ , __GNUC_MINOR__ , __GNUC_PATCHLEVEL__);
#endif
       sprintf(c,"at %s %s",__DATE__,__TIME__);
    }
    
  if (builder)
#ifdef BUILD_USING_CMAKE
    strcpy(builder, "cmake");
#else
    strcpy(builder, "autotools");
#endif    
  if (version_major)
    *version_major = major;
  if (version_minor)
    *version_minor = minor;
  if (version_patch)
    *version_patch = patch;
  return version;  
}
