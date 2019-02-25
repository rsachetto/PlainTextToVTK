SET(LIB_TYPE STATIC) #set the lib type

############## DEFAULT SAVE MESH LIBRARY ##############################

FIND_PACKAGE(ZLIB)

ADD_LIBRARY(vtk_utils ${LIB_TYPE} vtk_unstructured_grid.c vtk_unstructured_grid.h data_utils.c data_utils.h)

IF(ZLIB_FOUND)
    SET (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -I \"${ZLIB_INCLUDE_DIRS}\" -DCOMPILE_ZLIB")
    TARGET_LINK_LIBRARIES(vtk_utils hashes string file_utils ${ZLIB_LIBRARIES})
ELSE()
    TARGET_LINK_LIBRARIES(vtk_utils hashes string file_utils)
ENDIF(ZLIB_FOUND)

#######################################################################

