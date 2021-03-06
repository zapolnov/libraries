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
project(ilmbase)

find_package(Threads)

if(MSVC)
    add_definitions(/wd4018)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

if(NOT WIN32)
    add_definitions(-pthread)
endif()

file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/include")
configure_file(IlmBaseConfig.h.in "${CMAKE_CURRENT_BINARY_DIR}/include/IlmBaseConfig.h")

file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/include/OpenEXR")
include(InstallHeaders.cmake)

set(half_sources
    Half/eLut.h
    Half/half.cpp
    Half/half.h
    Half/halfExport.h
    Half/halfFunction.h
    Half/halfLimits.h
    Half/toFloat.h
)

file(GLOB iex_sources Iex/*.cpp Iex/*.h)
file(GLOB iexmath_sources IexMath/*.cpp IexMath/*.h)
file(GLOB ilmthread_sources IlmThread/*.cpp IlmThread/*.h)
file(GLOB imath_sources Imath/*.cpp Imath/*.h)

source_group("Source Files\\Half" FILES ${half_sources})
source_group("Source Files\\Iex" FILES ${iex_sources})
source_group("Source Files\\IexMath" FILES ${iexmath_sources})
source_group("Source Files\\IlmThread" FILES ${ilmthread_sources})
source_group("Source Files\\Imath" FILES ${imath_sources})

if(NOT WIN32)
    set_source_files_properties(
        IlmThread/IlmThreadMutexWin32.cpp
        IlmThread/IlmThreadSemaphoreWin32.cpp
        IlmThread/IlmThreadWin32.cpp
        PROPERTIES HEADER_FILE_ONLY TRUE)
endif()

add_library(ilmbase STATIC ${EXCLUDE_FROM_ALL}
    ${half_sources}
    ${iex_sources}
    ${iexmath_sources}
    ${ilmthread_sources}
    ${imath_sources}
)

target_include_directories(ilmbase PUBLIC BEFORE
    "${CMAKE_CURRENT_BINARY_DIR}/include"
    "${CMAKE_CURRENT_BINARY_DIR}/include/OpenEXR"
)

target_include_directories(ilmbase PRIVATE BEFORE
    "${CMAKE_CURRENT_SOURCE_DIR}/Half"
    "${CMAKE_CURRENT_SOURCE_DIR}/Iex"
    "${CMAKE_CURRENT_SOURCE_DIR}/IexMath"
    "${CMAKE_CURRENT_SOURCE_DIR}/IlmThread"
    "${CMAKE_CURRENT_SOURCE_DIR}/Imath"
)

target_link_libraries(ilmbase ${CMAKE_THREAD_LIBS_INIT})
