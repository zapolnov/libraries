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
project(jpeg-compressor C)

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

# jpgd

set(jpgd_source_files jpgd.cpp jpgd.h)
source_group("Source Files" FILES ${jpgd_source_files})
add_library(libjpgd STATIC ${EXCLUDE_FROM_ALL} ${jpgd_source_files})

set_target_properties(libjpgd PROPERTIES OUTPUT_NAME "jpgd")
target_include_directories(libjpgd BEFORE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

# jpge

set(jpge_source_files jpge.cpp jpge.h)
source_group("Source Files" FILES ${jpge_source_files})
add_library(libjpge STATIC ${EXCLUDE_FROM_ALL} ${jpge_source_files})

set_target_properties(libjpge PROPERTIES OUTPUT_NAME "jpge")
target_include_directories(libjpge BEFORE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
