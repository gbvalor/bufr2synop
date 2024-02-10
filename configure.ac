###########################################################################
#   Project: bufr2synop                                                   #
#                                                                         #
#   Copyright (C) 2016-2023 by Guillermo Ballester Valor                  #
#   gbvalor@ogimet.com                                                    #
#                                                                         #
#   GNU General Public License v2 or above                                #
###########################################################################
#
#  configure.ac for bufr2synop project
#

AC_INIT([bufr2synop],[0.29.1])
AC_CONFIG_SRCDIR([src/apps/bufrtotac.c])
AM_INIT_AUTOMAKE

AC_CONFIG_HEADERS(config.h)

AC_LANG([C])
AC_PROG_CC([gcc icc])
LT_INIT
AC_CONFIG_MACRO_DIR([m4])


# Checks for libraries.
# 
# AC_CHECK_LIB([bufr], [bufrread], , [AC_MSG_ERROR("You must install ECMWF bufrdc library")])

dnl
dnl This block finds the compiler 
dnl Default values for CFLAGS
dnl 
{ 
case "${CC}" in
        gcc*)
                CFLAGS="$CFLAGS -O2 -Wall -Wextra -W -g -Wno-stringop-truncation"
                ;;
        icc)
                CFLAGS="$CFLAGS -O2 -xHost -fstack-protector -Wall -g"
                ;;
esac
}

AC_CONFIG_FILES([Makefile src/Makefile src/apps/Makefile src/bufrdeco/Makefile src/libraries/Makefile src/scripts/Makefile examples/Makefile share/Makefile])
AC_OUTPUT 
