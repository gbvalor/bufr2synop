METASOURCES = AUTO

# set the include path found by configure
AM_CPPFLAGS= $(all_includes)  -I$(top_srcdir)/src/bufrdeco

include_HEADERS = bufr2tac.h metbuoy.h metsynop.h mettemp.h metclimat.h metcommon.h

lib_LTLIBRARIES = libbufr2tac.la

libbufr2tac_la_SOURCES = bufr2tac_buoy.c bufr2tac_csv.c bufr2tac_env.c \
	bufr2tac_io.c bufr2tac_json.c bufr2tac_mrproper.c bufr2tac_print.c bufr2tac_sqparse.c \
	bufr2tac_tablec.c bufr2tac_synop.c bufr2tac_temp.c bufr2tac_utils.c bufr2tac_x01.c \
	bufr2tac_x02.c bufr2tac_x04.c bufr2tac_x05.c bufr2tac_x06.c bufr2tac_x07.c \
	bufr2tac_x08.c bufr2tac_x10.c bufr2tac_x11.c bufr2tac_x12.c bufr2tac_x13.c \
	bufr2tac_x14.c bufr2tac_x20.c bufr2tac_x22.c bufr2tac_x31.c bufr2tac_x33.c \
	bufr2tac_xml.c bufr2tac_climat.c bufr2tac_print_temp.c bufr2tac_print_buoy.c \
	bufr2tac_print_climat.c bufr2tac_print_synop.c
libbufr2tac_la_LIBADD = $(top_builddir)/src/bufrdeco/libbufrdeco.la -lbufr -lgfortran -lm
AM_CFLAGS = -W -Wall

EXTRA_DIST = CMakeLists.txt
