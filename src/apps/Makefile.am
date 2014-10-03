METASOURCES = AUTO

bin_PROGRAMS = bufr2synop bufr_decode

bufr_decode_SOURCES = bufr_decode.c

noinst_HEADERS = bufr2synop.h

# set the include path found by configure
AM_CPPFLAGS= $(all_includes) -I$(top_srcdir)/src/libraries

# the library search path.
bufr_decode_LDFLAGS = $(all_libraries)

bufr_decode_LDADD = -lm -lgfortran -lbufr
bufr2synop_SOURCES = bufr2synop.c bufr2synop_io.c 
bufr2synop_LDADD = $(top_builddir)/src/libraries/libbufr2syn.la -lbufr \
	-lgfortran -lm

