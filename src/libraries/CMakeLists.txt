###########################################################################
#   Project: bufr2synop                                                   #
#                                                                         #
#   Copyright (C) 2016-2024 by Guillermo Ballester Valor                  #
#   gbvalor@ogimet.com                                                    #
#                                                                         #
#   GNU General Public License v2 or above                                #
###########################################################################
#
# CMakeLists.txt for bufr2synop/src/libraries
#

INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR} ${bufr2synop_SOURCE_DIR}/src/bufrdeco ${all_includes})
LINK_DIRECTORIES(${bufr2synop_SOURCE_DIR}/src/libraries ${dir_libs})

add_library(bufr2tac SHARED bufr2tac.h bufr2tac.c metbuoy.h metsynop.h mettemp.h metclimat.h 
    metcommon.h bufr2tac_buoy.c bufr2tac_csv.c bufr2tac_env.c bufr2tac_error.c
    bufr2tac_io.c bufr2tac_json.c bufr2tac_mrproper.c bufr2tac_print.c bufr2tac_sqparse.c 
    bufr2tac_tablec.c bufr2tac_synop.c bufr2tac_temp.c bufr2tac_utils.c bufr2tac_x01.c 
    bufr2tac_x02.c bufr2tac_x04.c bufr2tac_x05.c bufr2tac_x06.c bufr2tac_x07.c 
    bufr2tac_x08.c bufr2tac_x10.c bufr2tac_x11.c bufr2tac_x12.c bufr2tac_x13.c 
    bufr2tac_x14.c bufr2tac_x20.c bufr2tac_x22.c bufr2tac_x31.c bufr2tac_x33.c 
    bufr2tac_xml.c bufr2tac_climat.c bufr2tac_print_temp.c bufr2tac_print_buoy.c 
    bufr2tac_print_climat.c bufr2tac_print_synop.c)
target_link_libraries(bufr2tac bufrdeco m)
SET_TARGET_PROPERTIES (bufr2tac PROPERTIES 
                       VERSION ${PROJECT_VERSION} 
                       SOVERSION ${PROJECT_VERSION_MAJOR})

INSTALL(FILES bufr2tac.h metbuoy.h metsynop.h mettemp.h metclimat.h metcommon.h
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ)
  
INSTALL(TARGETS bufr2tac DESTINATION ${CMAKE_INSTALL_LIBDIR})

