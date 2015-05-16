
cmake_minimum_required(VERSION 3.1)
project(libmng C)

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

configure_file(config.h.in "${CMAKE_CURRENT_BINARY_DIR}/config.h")
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

file(GLOB source_files *.c *.h)
source_group("Source Files" FILES ${source_files})

add_library(libmng STATIC ${EXCLUDE_FROM_ALL} ${source_files})
set_target_properties(libmng PROPERTIES OUTPUT_NAME "mng")
target_include_directories(libmng BEFORE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(libmng lcms2 zlib libjpeg)
