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

cmake_minimum_required(VERSION 3.1)

include(../cmake/Qt5.cmake)
include(../cmake/TargetPlatform.cmake)

file(GLOB qt5_sources qt5/*.cpp qt5/*.h)
file(GLOB root_sources *.cpp *.h)

add_library(main-nz STATIC ${EXCLUDE_FROM_ALL}
    ${qt5_sources}
    ${root_sources}
)

z_qt5_sources(${qt5_sources})

source_group("Source Files\\qt5" FILES ${qt5_sources})
source_group("Source Files" FILES ${root_sources})

set_target_properties(main-nz PROPERTIES CXX_STANDARD 11)
target_include_directories(main-nz BEFORE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/..")
target_link_libraries(main-nz utility-nz)
