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
project(byacc C)

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_DEPRECATE=1)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS=1)
    add_definitions(-D_CRT_NONSTDC_NO_WARNINGS=1)
    add_definitions(/wd4715)
endif()

include(CheckIncludeFile)
check_include_file(fcntl.h HAVE_FCNTL_H)

include(CheckFunctionExists)
check_function_exists(mkstemp HAVE_MKSTEMP)
check_function_exists(vsnprintf HAVE_VSNPRINTF)

configure_file(config.h.in "${CMAKE_CURRENT_BINARY_DIR}/config.h")
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

add_definitions(-DHAVE_CONFIG_H)

set(source_files
    closure.c
    defs.h
    error.c
    graph.c
    lalr.c
    lr0.c
    main.c
    mkpar.c
    mstring.c
    output.c
    reader.c
    symtab.c
    verbose.c
    warshall.c
    yaccpar.c
)

add_executable(byacc ${EXCLUDE_FROM_ALL} ${source_files})
source_group("Source Files" FILES ${source_files})
