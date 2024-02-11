###########################################################################
#   Project: bufr2synop                                                   #
#                                                                         #
#   Copyright (C) 2016-2024 by Guillermo Ballester Valor                  #
#   gbvalor@ogimet.com                                                    #
#                                                                         #
#   GNU General Public License v2 or above                                #
###########################################################################
#
#  CMakeLists.txt for bufr2synop/examples
#

INSTALL(FILES sn.0000.bin 20141018211119_ISIN03_EGRR_182100.bufr 
       20150705121512_ISCD01_LIIB_050000.bufr 20160402121749_IUSH01_DRRN_021100.bufr 
       DESTINATION ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME})
