AC_INIT([bufr2synop], 0.7.0)
AC_CONFIG_SRCDIR([src/apps/bufr2synop.c])
AM_INIT_AUTOMAKE

AM_CONFIG_HEADER(config.h)

AC_LANG_C
AC_PROG_CC
AM_PROG_LIBTOOL


# Checks for libraries.
# 
AC_CHECK_LIB([bufr], [bufrread], , [AC_MSG_ERROR("You must install ECMWF bufrdc library")])

dnl
dnl This block finds the compiler 
dnl Default values for CFLAGS
dnl 
{ 
case "${CC}" in
        gcc*)
                CFLAGS="-O2 -Wall -Wextra -W -g"
                ;;
        icc)
                CFLAGS="-O2 -xHost -fstack-protector -Wall -g"
                ;;
esac
}


AC_OUTPUT(Makefile src/Makefile src/apps/Makefile src/bufrdeco/Makefile src/libraries/Makefile src/scripts/Makefile examples/Makefile) 
