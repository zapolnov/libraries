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
project(lcms2 C)

file(GLOB source_files src/*.c src/*.h)
file(GLOB include_files include/*.c include/*.h)
source_group("Source Files\\src" FILES ${source_files})
source_group("Source Files\\include" FILES ${include_files})

add_library(lcms2 STATIC ${EXCLUDE_FROM_ALL}
    ${source_files}
    ${include_files}
)

target_include_directories(lcms2 BEFORE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

find_library(m m)
if(m)
    target_link_libraries(lcms2 ${m})
endif()

add_subdirectory(utils)
