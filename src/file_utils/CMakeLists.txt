set(SOURCE_FILES_UTILS file_utils.c file_utils.h)

ADD_LIBRARY(file_utils STATIC ${SOURCE_FILES_UTILS})

if(NOT MSVC)
    set_property(TARGET file_utils string PROPERTY POSITION_INDEPENDENT_CODE ON)
endif()
