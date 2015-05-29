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
project(ode)

if(APPLE)
    include(CheckCXXCompilerFlag)
    check_cxx_compiler_flag("-Wno-deprecated-declarations" HAVE_WNO_DEPRECATED_DECLARATIONS)
    if(HAVE_WNO_DEPRECATED_DECLARATIONS)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")
    endif()
endif()

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_definitions(/wd4056 /wd4244 /wd4267 /wd4305 /wd4456 /wd4457 /wd4458 /wd4756 /wd4838)
endif()

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/GIMPACT/include")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/OPCODE")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/libccd/src")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/build")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/ode/src")
add_definitions(-DHAVE_CONFIG_H)

file(GLOB build_files build/*.h)
file(GLOB gimpact_include_files GIMPACT/include/GIMPACT/*.h)
file(GLOB gimpact_src_files GIMPACT/src/*.cpp)
file(GLOB opcode_ice_files OPCODE/Ice/*.cpp OPCODE/Ice/*.h)
file(GLOB opcode_files OPCODE/*.cpp OPCODE/*.h)
file(GLOB include_ode_files include/ode/*.h)
file(GLOB libccd_files libccd/src/*.c)
file(GLOB libccd_header_files libccd/src/ccd/*.h)
file(GLOB ode_src_files ode/src/*.c ode/src/*.cpp ode/src/*.h)
file(GLOB ode_src_joints_files ode/src/joints/*.cpp ode/src/joints/*.h)

source_group("Source Files\\GIMPACT\\include\\GIMPACT" FILES ${gimpact_include_files})
source_group("Source Files\\GIMPACT\\src" FILES ${gimpact_src_files})
source_group("Source Files\\OPCODE\\Ice" FILES ${opcode_ice_files})
source_group("Source Files\\OPCODE" FILES ${opcode_files})
source_group("Source Files\\include\\ode" FILES ${include_ode_files})
source_group("Source Files\\libccd\\src" FILES ${libccd_files})
source_group("Source Files\\libccd\\src\\ccd" FILES ${libccd_header_files})
source_group("Source Files\\ode\\src" FILES ${ode_src_files})
source_group("Source Files\\ode\\src\\joints" FILES ${ode_src_joints_files})
source_group("Source Files\\build" FILES ${build_files})

add_library(ode STATIC ${EXCLUDE_FROM_ALL}
    ${build_files}
    ${gimpact_include_files}
    ${gimpact_src_files}
    ${opcode_ice_files}
    ${opcode_files}
    ${include_ode_files}
    ${libccd_files}
    ${libccd_hedaer_files}
    ${ode_src_files}
    ${ode_src_joints_files}
)

set_source_files_properties(ode/src/collision_trimesh_trimesh.cpp PROPERTIES HEADER_FILE_ONLY TRUE)

target_include_directories(ode BEFORE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

if(APPLE)
    find_library(CoreServices CoreServices)
    target_link_libraries(ode ${CoreServices})
endif()
