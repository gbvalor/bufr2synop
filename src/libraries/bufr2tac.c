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
 \brief This file has the code for functions to get information about library bufr2tac, and global vars 
*/
#ifndef CONFIG_H
# include "config.h"
# define CONFIG_H
#endif

#include "bufr2tac.h"

int BUFR2TAC_DEBUG_LEVEL = 0; /*!< Set debug level. 0 -> no debug. 1 -> debug. 2 -> verbose debug */

/*!
  \fn char *bufr2tac_get_version(char *version, char *build, char *builder, int *version_major, int *version_minor, int *version_patch)
  \brief Get strings with version information and build date and time
  \param version  pointer to string with version as result if not NULL
  \param dversion dimension of string \ref version
  \param build pointer to string with compiler and compilation date and time if not NULL 
  \param dbuild dimension of string \ref build 
  \param builder pointer to string with builder utility. 'cmake' or 'autotools' if not NULL 
  \param dbuilder dimension of string \ref builder
  \param version_major pointer to string with version_major component if not NULL
  \param version_minor pointer to string with version_minor component if not NULL
  \param version_patch pointer to string with version_patch component if not NULL
  
  Retuns string pointer \ref version.  
 */
char *bufr2tac_get_version(char *version, size_t dversion, char *build, size_t dbuild, char *builder, size_t dbuilder, 
                           int *version_major, int *version_minor, int *version_patch)
{
  int major = 0, minor = 0, patch = 0;
  size_t used;

  if (version == NULL)
    return NULL;
  snprintf(version, dversion, "%s", VERSION);
  // default  
  sscanf(version, "%d.%d.%d", &major, &minor, &patch);

  if (build)
    {
       used = 0;
#if defined(__INTEL_COMPILER)
       used += snprintf(build + used, dbuild - used, "using INTEL C compiler icc %d.%d ", __INTEL_COMPILER, __INTEL_COMPILER_UPDATE);
#elif defined(__clang_version__) 
       used += snprintf(build + used, dbuild - used, "using clang C compiler %s ", __clang_version__);
#elif defined(__GNUC__) 
       used += snprintf(build + used, dbuild - used, "using GNU C compiler gcc %d.%d.%d ", __GNUC__ , __GNUC_MINOR__ , __GNUC_PATCHLEVEL__);
#elif defined(_MSC_VER) 
       used += snprintf(build + used, dbuild - used, "using MICROSOFT C compiler %d ", _MSC_VER);
#else
       used += snprintf(build + used, dbuild - used, "using an unknown C compiler ");
#endif
       snprintf(build + used, dbuild - used,"at %s %s",__DATE__,__TIME__);
    }
    
  if (builder)
#ifdef BUILD_USING_CMAKE
    strncpy(builder, "cmake", dbuilder);
#else
    strncpy(builder, "autotools", dbuilder);
#endif    
  if (version_major)
    *version_major = major;
  if (version_minor)
    *version_minor = minor;
  if (version_patch)
    *version_patch = patch;
  return version;  
}


int bufr2tac_set_debug_level(int level)
{
  if (level < 0 || level > 2)
    return 1; // Bad debug level
  
  // Here we set 
  BUFR2TAC_DEBUG_LEVEL = level;
  return 0;  
}
