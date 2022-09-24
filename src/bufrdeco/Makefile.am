###########################################################################
#   Project: bufr2synop                                                   #
#                                                                         #
#   Copyright (C) 2016-2022 by Guillermo Ballester Valor                  #
#   gbvalor@ogimet.com                                                    #
#                                                                         #
#   GNU General Public License v2 or above                                #
###########################################################################
#
#  Makefile.am for bufr2synop/src/bufrdeco
#

METASOURCES = AUTO

# set the include path found by configure
AM_CPPFLAGS= $(all_includes)

include_HEADERS = bufrdeco.h

lib_LTLIBRARIES = libbufrdeco.la

libbufrdeco_la_SOURCES = bufrdeco.h bufrdeco.c bufrdeco_read.c bufrdeco_tableB.c bufrdeco_tableC.c \
	bufrdeco_utils.c bufrdeco_tree.c bufrdeco_data.c bufrdeco_compressed.c bufrdeco_f2.c \
	bufrdeco_print.c bufrdeco_memory.c bufrdeco_csv.c bufrdeco_tableD.c bufrdeco_wmo.c bufrdeco_print_html.c \
	bufrdeco_data_tree.c
 
libbufrdeco_la_LIBADD = -lm

AM_CFLAGS = -W -Wall

EXTRA_DIST = CMakeLists.txt
