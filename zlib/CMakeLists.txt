
cmake_minimum_required(VERSION 3.1)
project(zlib C)

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
    add_definitions(-D_CRT_NONSTDC_NO_DEPRECATE)
endif()

add_library(zlib STATIC ${EXCLUDE_FROM_ALL}
    adler32.c
    compress.c
    crc32.c
    crc32.h
    deflate.c
    deflate.h
    gzclose.c
    gzguts.h
    gzlib.c
    gzread.c
    gzwrite.c
    infback.c
    inffast.c
    inffast.h
    inffixed.h
    inflate.c
    inflate.h
    inftrees.c
    inftrees.h
    trees.c
    trees.h
    uncompr.c
    zconf.h
    zlib.h
    zutil.c
    zutil.h
)

set_target_properties(zlib PROPERTIES OUTPUT_NAME "z")
target_include_directories(zlib BEFORE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

include(CheckIncludeFile)
check_include_file(unistd.h HAVE_UNISTD_H)
if(HAVE_UNISTD_H)
    target_compile_definitions(zlib PUBLIC "Z_HAVE_UNISTD_H")
endif()

include(CheckTypeSize)
set(CMAKE_REQUIRED_DEFINITIONS "-D_LARGEFILE64_SOURCE=1")
check_type_size(off64_t OFF64_T)
if(HAVE_OFF64_T)
    target_compile_definitions(zlib PUBLIC "_LARGEFILE64_SOURCE=1")
endif()
