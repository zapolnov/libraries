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
project(OpenEXR)

if(MSVC)
    add_definitions(/wd4018)
    add_definitions(/wd4244)
    add_definitions(/wd4267)
    add_definitions(/wd4305)
    add_definitions(/wd4334)
    add_definitions(/wd4800)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
else()
    add_definitions(-w)
endif()

# Test for GCC-style inline asm support with AVX instructions
include(CheckCXXSourceCompiles)
check_cxx_source_compiles(
	"
	int main()
	{
	 #if defined(__GNUC__) && defined(__SSE2__) 
		 int n   = 0;
		 int eax = 0;
		 int edx = 0;
		 __asm__(
			 \"xgetbv     ;\"
			 \"vzeroupper  \"
			 : \"=a\"(eax), \"=d\"(edx) : \"c\"(n) : );
	 #else
		 #error No GCC style inline asm supported for AVX instructions
	 #endif
	}
	" OPENEXR_IMF_HAVE_GCC_INLINE_ASM_AVX)

# Check if sysconf(_SC_NPROCESSORS_ONLN) can be used for CPU count
check_cxx_source_compiles(
    "
    #include <unistd.h>
    int main()
    {
        sysconf(_SC_NPROCESSORS_ONLN);
    }
    " OPENEXR_IMF_HAVE_SYSCONF_NPROCESSORS_ONLN)

file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/include")
configure_file(OpenEXRConfig.h.in "${CMAKE_CURRENT_BINARY_DIR}/include/OpenEXRConfig.h")

file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/include/OpenEXR")
include(InstallHeaders.cmake)

file(GLOB ilmimf_files IlmImf/Imf*.cpp IlmImf/Imf*.h IlmInf/b44ExpLogTable.h IlmImf/dwaLookups.h)
file(GLOB ilmimfutil_files IlmImfUtil/*.cpp IlmImfUtil/*.h)

source_group("Source Files\\IlmImf" FILES ${ilmimf_files})
source_group("Source Files\\IlmImfUtil" FILES ${ilmimfutil_files})

add_library(libopenexr STATIC ${EXCLUDE_FROM_ALL}
    ${ilmimf_files}
    ${ilmimfutil_files}
)

set_target_properties(libopenexr PROPERTIES OUTPUT_NAME "openexr")
target_link_libraries(libopenexr ilmbase libz)

target_include_directories(libopenexr PUBLIC BEFORE
    "${CMAKE_CURRENT_BINARY_DIR}/include"
    "${CMAKE_CURRENT_BINARY_DIR}/include/OpenEXR"
)

target_include_directories(libopenexr PRIVATE BEFORE
    "${CMAKE_CURRENT_SOURCE_DIR}/IlmImf"
    "${CMAKE_CURRENT_SOURCE_DIR}/IlmImfUtil"
)

add_subdirectory(exrenvmap)
add_subdirectory(exrheader)
add_subdirectory(exrmakepreview)
add_subdirectory(exrmaketiled)
add_subdirectory(exrmultipart)
add_subdirectory(exrmultiview)
add_subdirectory(exrstdattr)
