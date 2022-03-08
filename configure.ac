AC_INIT([bufr2synop], 0.15.0)
AC_CONFIG_SRCDIR([src/apps/bufr2synop.c])
AM_INIT_AUTOMAKE

AM_CONFIG_HEADER(config.h)

AC_LANG_C
AC_PROG_CC
AM_PROG_LIBTOOL


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
     ;;
  no)  bufrdc=false 
     ;;
  *) AC_MSG_ERROR([bad value ${enableval} for --enable-bufrdc]) ;;
esac],[bufrdc=false])
AM_CONDITIONAL([BUFRDC], [test x$bufrdc = xtrue])

AC_OUTPUT(Makefile src/Makefile src/apps/Makefile src/bufrdeco/Makefile src/libraries/Makefile src/scripts/Makefile examples/Makefile share/Makefile) 
