LINK_DIRECTORIES(/usr/lib /usr/lib64 /usr/local/lib /usr/local/lib64)

add_library(bufrdeco bufrdeco.h bufrdeco_read.c bufrdeco_memory.c bufrdeco_tableb.c bufrdeco_tablec.c bufrdeco_tabled.c bufrdeco_utils.c 
        bufrdeco_tree.c bufrdeco_data.c bufrdeco_compressed.c bufrdeco_f2.c bufrdeco_print.c)
target_link_libraries(bufrdeco m)

INSTALL(FILES bufrdeco.h DESTINATION include PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ)
  