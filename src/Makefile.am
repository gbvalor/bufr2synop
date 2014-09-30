bin_PROGRAMS = bufr2synop bufr_decode

bufr_decode_SOURCES = bufr_decode.c

include_HEADERS = bufr2syn.h metbuoy.h metsynop.h 

noinst_HEADERS = bufr2synop.h

# set the include path found by configure
AM_CPPFLAGS= $(all_includes)

# the library search path.
bufr_decode_LDFLAGS = $(all_libraries)

bufr_decode_LDADD = -lm -lgfortran -lbufr
bufr2synop_SOURCES = bufr2synop.c bufr2synop_io.c 
bufr2synop_LDADD = $(top_builddir)/src/libbufr2syn.la -lbufr -lgfortran -lm
lib_LTLIBRARIES = libbufr2syn.la
libbufr2syn_la_SOURCES = bufr2syn_buoy.c bufr2syn_csv.c bufr2syn_env.c \
	bufr2syn_io.c bufr2syn_json.c bufr2syn_mrproper.c bufr2syn_print.c bufr2syn_sqparse.c \
	bufr2syn_synop.c bufr2syn_tablec.c bufr2syn_utils.c bufr2syn_x01.c bufr2syn_x02.c \
	bufr2syn_x04.c bufr2syn_x05.c bufr2syn_x06.c bufr2syn_x07.c bufr2syn_x08.c \
	bufr2syn_x10.c bufr2syn_x11.c bufr2syn_x12.c bufr2syn_x13.c bufr2syn_x14.c \
	bufr2syn_x20.c bufr2syn_x22.c bufr2syn_x31.c bufr2syn_x33.c bufr2syn_xml.c
