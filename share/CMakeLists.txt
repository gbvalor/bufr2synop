#
#  CMakeLists.txt for bufr2synop/share
#

FILE(GLOB ficheros BUFR* )
INSTALL(FILES ${ficheros} DESTINATION ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME})

