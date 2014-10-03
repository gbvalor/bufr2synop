AC_INIT([bufr2synop], 0.4)
AC_CONFIG_SRCDIR([src/apps/bufr2synop.c])
AM_INIT_AUTOMAKE

AM_CONFIG_HEADER(config.h)

AC_LANG_C
AC_PROG_CC
AM_PROG_LIBTOOL


# Checks for libraries.
# 
AC_CHECK_LIB([bufr], [bufrread], , [AC_MSG_ERROR("You must install ECMWF bufrdc library")])


AC_OUTPUT(Makefile src/Makefile src/apps/Makefile src/libraries/Makefile)
