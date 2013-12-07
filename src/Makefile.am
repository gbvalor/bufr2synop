bin_PROGRAMS = bufr2synop bufr_decode

bufr_decode_SOURCES = bufr_decode.c

noinst_HEADERS = bufr2synop.h metbuoy.h metsynop.h

# set the include path found by configure
INCLUDES= $(all_includes)

# the library search path.
bufr_decode_LDFLAGS = $(all_libraries)

bufr_decode_LDADD = -lm -lgfortran -lbufr
bufr2synop_SOURCES = bufr2syn_env.c bufr2syn_io.c bufr2syn_mrproper.c \
	bufr2syn_sqparse.c bufr2syn_synop.c bufr2syn_tablec.c bufr2syn_utils.c bufr2syn_x01.c \
	bufr2syn_x02.c bufr2syn_x04.c bufr2syn_x10.c bufr2syn_x11.c bufr2syn_x12.c \
	bufr2syn_x13.c bufr2syn_x20.c bufr2synop.c
bufr2synop_LDADD = -lgfortran -lbufr -lm
