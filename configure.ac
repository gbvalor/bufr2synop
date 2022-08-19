###########################################################################
#   Project: bufr2synop                                                   #
#                                                                         #
#   Copyright (C) 2016-2022 by Guillermo Ballester Valor                  #
#   gbvalor@ogimet.com                                                    #
#                                                                         #
#   GNU General Public License v2 or above                                #
###########################################################################
#
#  configure.ac for bufr2synop project
#

AC_INIT([bufr2synop],[0.20.0])
AC_CONFIG_SRCDIR([src/apps/bufr2synop.c])
AM_INIT_AUTOMAKE

AC_CONFIG_HEADERS(config.h)

AC_LANG([C])
AC_PROG_CC
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
                CFLAGS="$CFLAGS -O2 -Wall -Wextra -W -g"
                ;;
        icc)
                CFLAGS="$CFLAGS -O2 -xHost -fstack-protector -Wall -g"
                ;;
esac
}


dnl
dnl enable for ECMWF bufrdc library support (legacy features)
dnl
AC_ARG_ENABLE([bufrdc],
[  --enable-bufrdc    Enable legacy use of ECMWF bufrdc library],
[case "${enableval}" in
  yes) bufrdc=true 
     AC_DEFINE(USE_BUFRDC, 1, use ECMWF bufrdc)
     CFLAGS="${CFLAGS} -DUSE_BUFRDC"
     ;;
  no)  bufrdc=false 
     ;;
  *) AC_MSG_ERROR([bad value ${enableval} for --enable-bufrdc]) ;;
esac],[bufrdc=false])
AM_CONDITIONAL([BUFRDC], [test x$bufrdc = xtrue])
AC_CONFIG_FILES([Makefile src/Makefile src/apps/Makefile src/bufrdeco/Makefile src/libraries/Makefile src/scripts/Makefile examples/Makefile share/Makefile])
AC_OUTPUT 
