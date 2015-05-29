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
project(minizip C)

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
    add_definitions(-D_CRT_NONSTDC_NO_DEPRECATE)
    add_definitions(/wd4244 /wd4267 /wd4456)
endif()

add_definitions(-DHAVE_AES)
add_definitions(-DHAVE_BZIP2)

set(libunzip_source_files
    crypt.h
    ioapi.c
    ioapi.h
    ioapi_buf.c
    ioapi_buf.h
    ioapi_mem.c
    ioapi_mem.h
    iowin32.c
    iowin32.h
    unzip.c
    unzip.h
)

set(libzip_source_files
    minizip.c
    zip.c
    zip.h
)

source_group("Source Files" FILES ${libzip_source_files} ${libunzip_source_files})

file(GLOB aes_files aes/*.c aes/*.h)
source_group("Source Files\\aes" FILES ${aes_files})

add_library(libunzip STATIC ${EXCLUDE_FROM_ALL}
    ${libunzip_source_files}
    ${aes_files}
)

if(NOT WIN32)
    set_source_files_properties(iowin32.c PROPERTIES HEADER_FILE_ONLY TRUE)
endif()

set_target_properties(libunzip PROPERTIES OUTPUT_NAME "unzip")
target_include_directories(libunzip BEFORE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(libunzip libz libbzip2)


add_library(libzip STATIC ${EXCLUDE_FROM_ALL}
    ${libzip_source_files}
)

set_target_properties(libzip PROPERTIES OUTPUT_NAME "zip")
target_include_directories(libzip BEFORE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_definitions(libzip PRIVATE LIBZIP)
target_link_libraries(libzip libz libbzip2 libunzip)

add_executable(minizip ${EXCLUDE_FROM_ALL} minizip.c)
target_link_libraries(minizip libzip)

add_executable(miniunz ${EXCLUDE_FROM_ALL} miniunz.c)
target_link_libraries(miniunz libunzip)
