###########################################################################
#   Project: bufr2synop                                                   #
#                                                                         #
#   Copyright (C) 2016-2022 by Guillermo Ballester Valor                  #
#   gbvalor@ogimet.com                                                    #
#                                                                         #
#   GNU General Public License v2 or above                                #
###########################################################################
#
#  CMakeLists.txt for bufr2synop/share
#

FILE(GLOB ficheros BUFR* )
INSTALL(FILES ${ficheros} DESTINATION ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME})

