#
# Copyright (c) 2015 Nikolay Zapolnov (zapolnov@gmail.com)
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#

cmake_minimum_required(VERSION 3.2)
project(tiff C)

add_definitions(-D__STDC_FORMAT_MACROS=1)
if(APPLE)
    add_definitions(-DHAVE_APPLE_OPENGL_FRAMEWORK)
endif()

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_definitions(-D_CRT_NONSTDC_NO_DEPRECATE)
    add_definitions(/wd4018)
    add_definitions(/wd4244)
    add_definitions(/wd4267)
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

set(libport_source_files)
if(NOT HAVE_GETOPT)
    add_definitions(-DNEED_LIBPORT)
    include_directories(port)
    set(libport_source_files port/getopt.c port/libport.h)
    source_group("Source Files\\port" FILES ${libport_source_files})
endif()

configure_file(tif_config.h.in "${CMAKE_CURRENT_BINARY_DIR}/tif_config.h")
configure_file(tif_config.h.in "${CMAKE_CURRENT_BINARY_DIR}/tiffconf.h")

file(GLOB source_files libtiff/*.c libtiff/*.h libtiff/*.cpp libtiff/*.cxx libtiff/*.hxx)
set_source_files_properties(libtiff/tif_stream.cxx PROPERTIES HEADER_FILE_ONLY TRUE)

source_group("Source Files" FILES ${source_files})
source_group("Generated Files" FILES
    "${CMAKE_CURRENT_BINARY_DIR}/tif_config.h"
    "${CMAKE_CURRENT_BINARY_DIR}/tiffconf.h"
)

add_library(libtiff STATIC ${EXCLUDE_FROM_ALL}
    "${CMAKE_CURRENT_BINARY_DIR}/tif_config.h"
    "${CMAKE_CURRENT_BINARY_DIR}/tiffconf.h"
    ${source_files}
    ${libport_source_files}
)

if(WIN32)
    set_source_files_properties(libtiff/tif_unix.c PROPERTIES HEADER_FILE_ONLY TRUE)
else()
    set_source_files_properties(libtiff/tif_win32.c PROPERTIES HEADER_FILE_ONLY TRUE)
endif()
set_source_files_properties(libtiff/mkg3states.c PROPERTIES HEADER_FILE_ONLY TRUE)

set_target_properties(libtiff PROPERTIES OUTPUT_NAME "tiff")
target_include_directories(libtiff BEFORE PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/libtiff"
    "${CMAKE_CURRENT_BINARY_DIR}"
)

target_link_libraries(libtiff libjpeg libz)

add_subdirectory(tools)
