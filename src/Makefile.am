bin_PROGRAMS = bufr2synop bufr_decode

bufr_decode_SOURCES = bufr_decode.c

noinst_HEADERS= bufr2synop.h

# set the include path found by configure
INCLUDES= $(all_includes)

# the library search path.
bufr_decode_LDFLAGS = $(all_libraries)

bufr_decode_LDADD = -lm -lgfortran -lbufr
bufr2synop_SOURCES = bufr2syn_env.c bufr2syn_io.c bufr2syn_mrproper.c \
	bufr2syn_utils.c bufr2synop.c
bufr2synop_LDADD = -lgfortran -lbufr -lm
