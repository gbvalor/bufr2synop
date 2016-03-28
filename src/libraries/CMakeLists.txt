INCLUDE_DIRECTORIES( ${bufr2synop_SOURCE_DIR}/src/bufrdeco $(all_includes) )
LINK_DIRECTORIES(${bufr2synop_SOURCE_DIR}/src/bufrdeco /usr/lib /usr/lib64 /usr/local/lib /usr/local/lib64)

add_library(bufr2tac bufr2tac.h metcommon.h metbuoy.h metsynop.h mettemp.h metclimat.h 
        bufr2tac_buoy.c bufr2tac_csv.c bufr2tac_env.c 
	bufr2tac_io.c bufr2tac_json.c bufr2tac_mrproper.c bufr2tac_print.c bufr2tac_sqparse.c 
	bufr2tac_synop.c bufr2tac_temp.c bufr2tac_utils.c bufr2tac_x01.c 
	bufr2tac_x02.c bufr2tac_x04.c bufr2tac_x05.c bufr2tac_x06.c bufr2tac_x07.c 
	bufr2tac_x08.c bufr2tac_x10.c bufr2tac_x11.c bufr2tac_x12.c bufr2tac_x13.c 
	bufr2tac_x14.c bufr2tac_x20.c bufr2tac_x22.c bufr2tac_x31.c bufr2tac_x33.c 
	bufr2tac_xml.c bufr2tac_climat.c bufr2tac_print_temp.c bufr2tac_tablec.c)
target_link_libraries(bufr2tac bufrdeco m gfortran)

INSTALL(FILES bufr2tac.h metbuoy.h metsynop.h mettemp.h metclimat.h metcommon.h
  DESTINATION include PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ)
  