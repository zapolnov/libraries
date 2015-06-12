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
project(freetype C)

include(../../nz/cmake/SetSourceGroups.cmake)

add_definitions(-DFT2_BUILD_LIBRARY)
if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_definitions(/wd4267)
endif()

file(GLOB all_files RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
    include/config/*.h
    include/internal/services/*.h
    include/*.h
    src/autofit/*.c
    src/autofit/*.h
    src/base/*.c
    src/base/*.h
    src/bdf/*.c
    src/bdf/*.h
    src/bzip2/*.c
    src/cache/*.c
    src/cache/*.h
    src/cff/*.c
    src/cff/*.h
    src/cid/*.c
    src/cid/*.h
    src/gxvalid/*.c
    src/gxvalid/*.h
    src/lzw/*.c
    src/lzw/*.h
    src/otvalid/*.c
    src/otvalid/*.h
    src/pcf/*.c
    src/pcf/*.h
    src/pfr/*.c
    src/pfr/*.h
    src/psaux/*.c
    src/psaux/*.h
    src/pshinter/*.c
    src/pshinter/*.h
    src/psnames/*.c
    src/psnames/*.h
    src/raster/*.c
    src/raster/*.h
    src/sfnt/*.c
    src/sfnt/*.h
    src/smooth/*.c
    src/smooth/*.h
    src/truetype/*.c
    src/truetype/*.h
    src/type1/*.c
    src/type1/*.h
    src/type42/*.c
    src/type42/*.h
    src/winfonts/*.c
    src/winfonts/*.h
)

set(source_files
    src/autofit/autofit.c
    src/base/ftadvanc.c
    src/base/ftbbox.c
    src/base/ftbdf.c
    src/base/ftbitmap.c
    src/base/ftcalc.c
    src/base/ftcid.c
    src/base/ftdbgmem.c
    src/base/ftdebug.c
    src/base/ftfntfmt.c
    src/base/ftfstype.c
    src/base/ftgasp.c
    src/base/ftgloadr.c
    src/base/ftglyph.c
    src/base/ftgxval.c
    src/base/ftinit.c
    src/base/ftlcdfil.c
    src/base/ftmm.c
    src/base/ftobjs.c
    src/base/ftotval.c
    src/base/ftoutln.c
    src/base/ftpatent.c
    src/base/ftpfr.c
    src/base/ftrfork.c
    src/base/ftsnames.c
    src/base/ftstream.c
    src/base/ftstroke.c
    src/base/ftsynth.c
    src/base/ftsystem.c
    src/base/fttrigon.c
    src/base/fttype1.c
    src/base/ftutil.c
    src/base/ftwinfnt.c
    src/bdf/bdf.c
    src/bzip2/ftbzip2.c
    src/cache/ftcache.c
    src/cff/cff.c
    src/cid/type1cid.c
    src/gzip/ftgzip.c
    src/lzw/ftlzw.c
    src/pcf/pcf.c
    src/pfr/pfr.c
    src/psaux/psaux.c
    src/pshinter/pshinter.c
    src/psnames/psmodule.c
    src/raster/raster.c
    src/sfnt/sfnt.c
    src/smooth/smooth.c
    src/truetype/truetype.c
    src/type1/type1.c
    src/type42/type42.c
    src/winfonts/winfnt.c
)

foreach(file ${all_files})
    list(FIND source_files "${file}" index)
    if(${index} LESS 0)
        set_source_files_properties("${file}" PROPERTIES HEADER_FILE_ONLY TRUE)
    endif()
endforeach()

z_set_source_groups(${all_files})
add_library(libfreetype STATIC ${EXCLUDE_FROM_ALL} ${all_files})

target_include_directories(libfreetype BEFORE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_include_directories(libfreetype BEFORE PRIVATE
    src/truetype
    src/sfnt
    src/autofit
    src/smooth
    src/raster
    src/psaux
    src/psnames
)
