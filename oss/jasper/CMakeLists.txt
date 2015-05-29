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
project(jasper C)

if(WIN32)
    add_definitions(-DWIN32)
endif()

if(MSVC)
    add_definitions(/wd4018)
    add_definitions(/wd4244)
    add_definitions(/wd4267)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_definitions(-D_CRT_NONSTDC_NO_DEPRECATE)
else()
    add_definitions(-w)
endif()

include(CheckIncludeFile)
check_include_file(fnctl.h HAVE_FCNTL_H)
check_include_file(unistd.h HAVE_UNISTD_H)
check_include_file(io.h HAVE_IO_H)
check_include_file(sys/types.h HAVE_SYS_TYPES_H)
check_include_file(sys/time.h HAVE_SYS_TIME_H)
check_include_file(stdlib.h HAVE_STDLIB_H)
check_include_file(stddef.h HAVE_STDDEF_H)
check_include_file(stdbool.h HAVE_STDBOOL_H)
check_include_file(stdint.h HAVE_STDINT_H)

include(CheckFunctionExists)
check_function_exists(gettimeofday HAVE_GETTIMEOFDAY)
check_function_exists(getrusage HAVE_GETRUSAGE)

file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/jasper")
configure_file(jas_config.h.in "${CMAKE_CURRENT_BINARY_DIR}/jasper/jas_config.h")

file(GLOB base_files src/libjasper/base/*.c)
file(GLOB include_files src/libjasper/include/jasper/*.h)
file(GLOB bmp_files src/libjasper/bmp/*.c src/libjasper/bmp/*.h)
file(GLOB jp2_files src/libjasper/jp2/*.c src/libjasper/jp2/*.h)
file(GLOB jpc_files src/libjasper/jpc/*.c src/libjasper/jpc/*.h)
file(GLOB jpg_files src/libjasper/jpg/*.c src/libjasper/jpg/*.h)
file(GLOB mif_files src/libjasper/mif/*.c src/libjasper/mif/*.h)
file(GLOB pgx_files src/libjasper/pgx/*.c src/libjasper/pgx/*.h)
file(GLOB pnm_files src/libjasper/pnm/*.c src/libjasper/pnm/*.h)
file(GLOB ras_files src/libjasper/ras/*.c src/libjasper/ras/*.h)

source_group("Source Files\\base" FILES ${base_files})
source_group("Source Files\\include" FILES ${include_files})
source_group("Source Files\\bmp" FILES ${bmp_files})
source_group("Source Files\\jp2" FILES ${jp2_files})
source_group("Source Files\\jpc" FILES ${jpc_files})
source_group("Source Files\\jpg" FILES ${jpg_files})
source_group("Source Files\\mif" FILES ${mif_files})
source_group("Source Files\\pgx" FILES ${pgx_files})
source_group("Source Files\\pnm" FILES ${pnm_files})
source_group("Source Files\\ras" FILES ${ras_files})
source_group("Generated Files" FILES "${CMAKE_CURRENT_BINARY_DIR}/jasper/jas_config.h")

add_library(libjasper STATIC ${EXCLUDE_FROM_ALL}
    "${CMAKE_CURRENT_BINARY_DIR}/jasper/jas_config.h"
    ${base_files}
    ${include_files}
    ${bmp_files}
    ${jp2_files}
    ${jpc_files}
    ${jpg_files}
    ${mif_files}
    ${pgx_files}
    ${pnm_files}
    ${ras_files}
)

set_target_properties(libjasper PROPERTIES OUTPUT_NAME "jasper")
target_link_libraries(libjasper libjpeg)
target_include_directories(libjasper BEFORE PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src/libjasper/include"
    "${CMAKE_CURRENT_BINARY_DIR}"
)

find_library(m m)
if(m)
    target_link_libraries(libjasper ${m})
endif()

add_subdirectory(src/appl)
