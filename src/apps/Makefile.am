###########################################################################
#   Project: bufr2synop                                                   #
#                                                                         #
#   Copyright (C) 2016-2022 by Guillermo Ballester Valor                  #
#   gbvalor@ogimet.com                                                    #
#                                                                         #
#   GNU General Public License v2 or above                                #
###########################################################################
#
# Makefile.am for bufr2synop/src/apps
#
METASOURCES = AUTO

# set the include path found by configure
AM_CPPFLAGS= $(all_includes) -I/$(top_builddir) -I$(top_srcdir)/src/bufrdeco -I$(top_srcdir)/src/libraries

# the library search path.
AM_CFLAGS = -W -Wall

if BUFRDC
  bin_PROGRAMS = bufr2synop bufr_decode bufrnoaa bufrdeco_test bufrtotac build_bufrdeco_tables update_tableD
  noinst_HEADERS = bufr2synop.h bufrtotac.h bufrnoaa.h
else
  bin_PROGRAMS = bufrnoaa bufrdeco_test bufrtotac build_bufrdeco_tables update_tableD
  noinst_HEADERS = bufrtotac.h bufrnoaa.h
endif

if BUFRDC
  bufr_decode_SOURCES = bufr_decode.c
  bufr_decode_LDFLAGS = $(all_libraries)
  bufr_decode_LDADD = -lbufr -lgfortran -lm

  bufr2synop_SOURCES = bufr2synop.c bufr2synop_io.c 
  bufr2synop_LDADD = $(top_builddir)/src/libraries/libbufr2tac.la -lbufr -lgfortran -lm
endif

bufrnoaa_SOURCES = bufrnoaa.c bufrnoaa_io.c bufrnoaa_utils.c

bufrdeco_test_SOURCES = bufrdeco_test.c
bufrdeco_test_LDADD = $(top_builddir)/src/bufrdeco/libbufrdeco.la -lm 

bufrtotac_SOURCES = bufrtotac.c bufrtotac_io.c
bufrtotac_LDADD = $(top_builddir)/src/bufrdeco/libbufrdeco.la $(top_builddir)/src/libraries/libbufr2tac.la -lm

build_bufrdeco_tables_SOURCES = build_bufrdeco_tables.c
build_bufrdeco_tables_LDADD = $(top_builddir)/src/bufrdeco/libbufrdeco.la -lm 

update_tableD = update_tableD.c
update_tableD_LDADD = $(top_builddir)/src/bufrdeco/libbufrdeco.la -lm 

EXTRA_DIST = CMakeLists.txt
