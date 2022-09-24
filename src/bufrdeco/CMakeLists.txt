###########################################################################
#   Project: bufr2synop                                                   #
#                                                                         #
#   Copyright (C) 2016-2022 by Guillermo Ballester Valor                  #
#   gbvalor@ogimet.com                                                    #
#                                                                         #
#   GNU General Public License v2 or above                                #
###########################################################################
# 
# CMalkeLists.txt file for bufr2synop/src/bufrdeco
#

INCLUDE_DIRECTORIES( ${CMAKE_BINARY_DIR} ${all_includes})

LINK_DIRECTORIES( ${dir_libs} )

add_library(bufrdeco SHARED bufrdeco.h bufrdeco.c bufrdeco_read.c bufrdeco_tableB.c bufrdeco_tableC.c
        bufrdeco_tableD.c bufrdeco_utils.c bufrdeco_tree.c bufrdeco_data.c bufrdeco_compressed.c bufrdeco_f2.c 
        bufrdeco_print.c bufrdeco_memory.c bufrdeco_csv.c bufrdeco_wmo.c bufrdeco_print_html.c bufrdeco_data_json.c )
target_link_libraries(bufrdeco m)
SET_TARGET_PROPERTIES (bufrdeco PROPERTIES 
                       VERSION ${PROJECT_VERSION} 
                       SOVERSION ${PROJECT_VERSION_MAJOR})

INSTALL(FILES bufrdeco.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ)
  
INSTALL(TARGETS bufrdeco DESTINATION ${CMAKE_INSTALL_LIBDIR})
