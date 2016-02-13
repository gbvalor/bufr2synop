LINK_DIRECTORIES(/usr/lib /usr/lib64 /usr/local/lib /usr/local/lib64)

add_library(bufr2syn bufr2syn_buoy.c bufr2syn_csv.c bufr2syn_env.c 
	bufr2syn_io.c bufr2syn_json.c bufr2syn_mrproper.c bufr2syn_print.c bufr2syn_sqparse.c 
	bufr2syn_synop.c bufr2syn_tablec.c bufr2syn_temp.c bufr2syn_utils.c bufr2syn_x01.c 
	bufr2syn_x02.c bufr2syn_x04.c bufr2syn_x05.c bufr2syn_x06.c bufr2syn_x07.c 
	bufr2syn_x08.c bufr2syn_x10.c bufr2syn_x11.c bufr2syn_x12.c bufr2syn_x13.c 
	bufr2syn_x14.c bufr2syn_x20.c bufr2syn_x22.c bufr2syn_x31.c bufr2syn_x33.c 
	bufr2syn_xml.c bufr2syn_climat.c bufr2syn_print_temp.c)
target_link_libraries(bufr2syn m gfortran)

INSTALL(FILES bufr2syn.h metbuoy.h metsynop.h mettemp.h metclimat.h metcommon.h
  DESTINATION include PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ)
  