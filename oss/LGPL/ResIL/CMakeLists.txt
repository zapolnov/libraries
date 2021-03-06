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
project(ResIL)

if(MSVC)
    add_definitions(/wd4018)
    add_definitions(/wd4244)
endif()

include(CheckIncludeFile)
check_include_file(mm_malloc.h HAVE_MM_MALLOC_H)

include(CheckFunctionExists)
check_function_exists(memalign HAVE_MEMALIGN)
check_function_exists(posix_memalign HAVE_POSIX_MEMALIGN)
check_function_exists(_mm_malloc HAVE__MM_MALLOC)
check_function_exists(valloc HAVE_VALLOC)

include(TestBigEndian)
test_big_endian(BIGENDIAN)
if(BIGENDIAN)
    set(WORDS_BIGENDIAN TRUE)
else()
    unset(WORDS_BIGENDIAN)
endif()

include(CheckCXXSourceCompiles)
check_cxx_source_compiles("
    void iSwapUShort(unsigned short *s)  {
        asm(\"ror $8,%0\"
        : \"=r\" (*s)
        : \"0\" (*s));
    }
    int main() { return 0; }"
    GCC_X86_ASM)

file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/include")
configure_file(config.h.in "${CMAKE_CURRENT_BINARY_DIR}/include/ilut_config.h")

configure_file(config.h.in "${CMAKE_CURRENT_BINARY_DIR}/config.h")
add_definitions(-DHAVE_CONFIG_H)

file(GLOB include_files include/IL/*.h include/IL/*.hpp)
file(GLOB il_include_files src-IL/include/*.h)
file(GLOB il_source_files src-IL/src/*.c*)
file(GLOB ilu_include_files src-ILU/include/*.h)
file(GLOB ilu_include_error_files src-ILU/include/ilu_error/*.h)
file(GLOB ilu_source_files src-ILU/src/*.c*)

source_group("Source Files\\include\\IL" FILES ${include_files})
source_group("Source Files\\src-IL\\include" FILES ${il_include_files})
source_group("Source Files\\src-IL\\src" FILES ${il_source_files})
source_group("Source Files\\src-ILU\\include" FILES ${ilu_include_files})
source_group("Source Files\\src-ILU\\include\\ilu_error" FILES ${ilu_include_error_files})
source_group("Source Files\\src-ILU\\src" FILES ${ilu_source_files})

add_library(ResIL SHARED ${EXCLUDE_FROM_ALL}
    ${include_files}
    ${il_include_files}
    ${il_source_files}
    ${ilu_include_files}
    ${ilu_include_error_files}
    ${ilu_source_files}
)

set_target_properties(ResIL PROPERTIES CXX_STANDARD 11)

target_include_directories(ResIL BEFORE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_include_directories(ResIL BEFORE PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/include")
target_include_directories(ResIL BEFORE PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
target_include_directories(ResIL BEFORE PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src-IL/include")
target_include_directories(ResIL BEFORE PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src-ILU/include")

target_link_libraries(ResIL libopenexr libjasper libtiff libmng libpng libjpeg lcms2)
