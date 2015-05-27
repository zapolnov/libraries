
cmake_minimum_required(VERSION 3.2)
project(bzip2 C)

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_definitions(-D_CRT_NONSTDC_NO_DEPRECATE)
    add_definitions(/wd4244 /wd4267)
endif()

file(GLOB root_files *.c *.h)
source_group("Source Files" FILES ${root_files})

add_library(libbzip2 STATIC ${EXCLUDE_FROM_ALL}
    blocksort.c
    bzlib.c
    bzlib.h
    bzlib_private.h
    compress.c
    crctable.c
    decompress.c
    huffman.c
    randtable.c
)

target_include_directories(libbzip2 BEFORE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(bzip2 ${EXCLUDE_FROM_ALL} bzip2.c)
target_link_libraries(bzip2 libbzip2)
