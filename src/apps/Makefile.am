METASOURCES = AUTO

bin_PROGRAMS = bufr2synop bufr_decode bufrnoaa bufrdeco_test bufrtotac

bufr_decode_SOURCES = bufr_decode.c

noinst_HEADERS = bufr2synop.h bufrtotac.h bufrnoaa.h

# set the include path found by configure
AM_CPPFLAGS= $(all_includes) -I$(top_srcdir)/src/bufrdeco -I$(top_srcdir)/src/libraries

# the library search path.
bufr_decode_LDFLAGS = $(all_libraries)

bufr_decode_LDADD = -lm -lgfortran -lbufr
bufr2synop_SOURCES = bufr2synop.c bufr2synop_io.c 
bufr2synop_LDADD = $(top_builddir)/src/libraries/libbufr2tac.la -lbufr -lgfortran -lm

AM_CFLAGS = -W -Wall
bufrnoaa_SOURCES = bufrnoaa.c bufrnoaa_io.c bufrnoaa_utils.c
bufrdeco_test_SOURCES = bufrdeco_test.c
bufrdeco_test_LDADD = $(top_builddir)/src/bufrdeco/libbufrdeco.la -lm 
bufrtotac_SOURCES = bufrtotac.c bufrtotac_io.c
bufrtotac_LDADD = $(top_builddir)/src/bufrdeco/libbufrdeco.la $(top_builddir)/src/libraries/libbufr2tac.la -lm 
EXTRA_DIST = CMakeLists.txt