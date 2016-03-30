METASOURCES = AUTO

# set the include path found by configure
AM_CPPFLAGS= $(all_includes)

include_HEADERS = bufrdeco.h

lib_LTLIBRARIES = libbufrdeco.la

libbufrdeco_la_SOURCES = bufrdeco_read.c bufrdeco_tableb.c bufrdeco_tablec.c bufrdeco_tabled.c \
	bufrdeco_utils.c bufrdeco_tree.c bufrdeco_data.c bufrdeco_compressed.c bufrdeco_f2.c \
	bufrdeco_print.c bufrdeco_memory.c
libbufrdeco_la_LIBADD = -lm

AM_CFLAGS = -W -Wall

EXTRA_DIST = CMakeLists.txt
