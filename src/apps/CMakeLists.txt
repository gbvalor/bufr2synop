###########################################################################
#   Project: bufr2synop                                                   #
#                                                                         #
#   Copyright (C) 2016-2022 by Guillermo Ballester Valor                  #
#   gbvalor@ogimet.com                                                    #
#                                                                         #
#   GNU General Public License v2 or above                                #
###########################################################################
#
# CMakeLists.txt for bufr2synop/src/apps
#

INCLUDE_DIRECTORIES( ${CMAKE_BINARY_DIR} ${bufr2synop_SOURCE_DIR}/src/bufrdeco 
                     ${bufr2synop_SOURCE_DIR}/src/libraries ${all_includes} )

LINK_DIRECTORIES( ${bufr2synop_SOURCE_DIR}/src/bufrdeco ${bufr2synop_SOURCE_DIR}/src/libraries 
                  ${dir_libs})

IF (BUFRDC)

  add_executable(bufr2synop bufr2synop.h bufr2synop.c bufr2synop_io.c)
  target_link_libraries(bufr2synop bufr2tac bufr m gfortran)

  add_executable(bufr_decode bufr_decode.c)
  target_link_libraries(bufr_decode bufr m gfortran)
  
  install (TARGETS bufr2synop bufr_decode DESTINATION ${CMAKE_INSTALL_BINDIR} )

ENDIF (BUFRDC)

add_executable(bufrnoaa bufrnoaa.h bufrnoaa.c bufrnoaa_io.c bufrnoaa_utils.c)

add_executable(bufrdeco_test bufrdeco_test.c)
target_link_libraries(bufrdeco_test m bufrdeco)

add_executable(bufrtotac bufrtotac.h bufrtotac.c bufrtotac_io.c)
target_link_libraries(bufrtotac m bufrdeco bufr2tac)

add_executable(build_bufrdeco_tables build_bufrdeco_tables.c)
target_link_libraries(build_bufrdeco_tables m bufrdeco)

add_executable(update_tableD update_tableD.c)
target_link_libraries(update_tableD m bufrdeco)

install (TARGETS bufrnoaa bufrtotac build_bufrdeco_tables update_tableD DESTINATION ${CMAKE_INSTALL_BINDIR} )
