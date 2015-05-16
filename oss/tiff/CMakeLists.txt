
cmake_minimum_required(VERSION 3.1)
project(tiff C)

add_definitions(-D__STDC_FORMAT_MACROS=1)
if(APPLE)
    add_definitions(-DHAVE_APPLE_OPENGL_FRAMEWORK)
endif()

include(CheckIncludeFile)
check_include_file(assert.h HAVE_ASSERT_H)
check_include_file(stdint.h HAVE_STDINT_H)
check_include_file(inttypes.h HAVE_INTTYPES_H)
check_include_file(unistd.h HAVE_UNISTD_H)
check_include_file(search.h HAVE_SEARCH_H)
check_include_file(fcntl.h HAVE_FCNTL_H)
check_include_file(io.h HAVE_IO_H)
check_include_file(strings.h HAVE_STRINGS_H)
check_include_file(string.h HAVE_STRING_H)
check_include_file(sys/types.h HAVE_SYS_TYPES_H)

include(TestBigEndian)
test_big_endian(BIGENDIAN)
if(BIGENDIAN)
    set(HOST_BIGENDIAN 1)
    set(HOST_FILLORDER FILLORDER_MSB2LSB)
    set(WORDS_BIGENDIAN TRUE)
else()
    set(HOST_BIGENDIAN 0)
    set(HOST_FILLORDER FILLORDER_LSB2MSB)
    unset(WORDS_BIGENDIAN)
endif()

include(CheckTypeSize)
check_type_size("unsigned long" SIZEOF_UNSIGNED_LONG)

include(CheckFunctionExists)
check_function_exists(getopt HAVE_GETOPT)
check_function_exists(mmap HAVE_MMAP)
check_function_exists(setmode HAVE_SETMODE)

configure_file(tif_config.h.in "${CMAKE_CURRENT_BINARY_DIR}/tif_config.h")
configure_file(tif_config.h.in "${CMAKE_CURRENT_BINARY_DIR}/tiffconf.h")

file(GLOB source_files libtiff/*.c libtiff/*.h libtiff/*.cpp libtiff/*.cxx libtiff/*.hxx)
set_source_files_properties(libtiff/tif_stream.cxx PROPERTIES HEADER_FILE_ONLY TRUE)
source_group("Source Files" FILES ${source_files})

add_library(tiff STATIC ${EXCLUDE_FROM_ALL}
    "${CMAKE_CURRENT_BINARY_DIR}/tif_config.h"
    "${CMAKE_CURRENT_BINARY_DIR}/tiffconf.h"
    ${source_files}
)

if(WIN32)
    set_source_files_properties(libtiff/tif_unix.c PROPERTIES HEADER_FILE_ONLY TRUE)
else()
    set_source_files_properties(libtiff/tif_win32.c PROPERTIES HEADER_FILE_ONLY TRUE)
endif()

target_include_directories(tiff BEFORE PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/libtiff"
    "${CMAKE_CURRENT_BINARY_DIR}"
)

target_link_libraries(tiff jpeg zlib)

add_subdirectory(tools)
