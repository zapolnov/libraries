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
project(jpeg)

if(MSVC)
    add_definitions(-D_CRT_NONSTDC_NO_DEPRECATE)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_definitions(/wd4267)
endif()

configure_file(jconfig.vc "${CMAKE_CURRENT_BINARY_DIR}/jconfig.h" COPYONLY)

set(source_files
    cderror.h
    cdjpeg.c
    cdjpeg.h
    jaricom.c
    jcapimin.c
    jcapistd.c
    jcarith.c
    jccoefct.c
    jccolor.c
    jcdctmgr.c
    jchuff.c
    jcinit.c
    jcmainct.c
    jcmarker.c
    jcmaster.c
    jcomapi.c
    jconfig.vc
    jcparam.c
    jcprepct.c
    jcsample.c
    jctrans.c
    jdapimin.c
    jdapistd.c
    jdarith.c
    jdatadst.c
    jdatasrc.c
    jdcoefct.c
    jdcolor.c
    jdct.h
    jddctmgr.c
    jdhuff.c
    jdinput.c
    jdmainct.c
    jdmarker.c
    jdmaster.c
    jdmerge.c
    jdpostct.c
    jdsample.c
    jdtrans.c
    jerror.c
    jerror.h
    jfdctflt.c
    jfdctfst.c
    jfdctint.c
    jidctflt.c
    jidctfst.c
    jidctint.c
    jinclude.h
    jmemmgr.c
    jmemnobs.c
    jmemsys.h
    jmorecfg.h
    jpegint.h
    jpeglib.h
    jquant1.c
    jquant2.c
    jutils.c
    jversion.h
    rdbmp.c
    rdcolmap.c
    rdgif.c
    rdppm.c
    rdswitch.c
    rdtarga.c
    transupp.c
    transupp.h
    wrbmp.c
    wrgif.c
    wrppm.c
    wrtarga.c
)

add_library(libjpeg STATIC ${EXCLUDE_FROM_ALL} ${source_files})

source_group("Source Files" FILES ${source_files} cjpeg.c djpeg.c jpegtran.c)
set_source_files_properties(jconfig.vc PROPERTIES HEADER_FILE_ONLY TRUE)

set_target_properties(libjpeg PROPERTIES OUTPUT_NAME "jpeg")
target_include_directories(libjpeg BEFORE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_include_directories(libjpeg BEFORE PUBLIC "${CMAKE_CURRENT_BINARY_DIR}")

add_executable(cjpeg ${EXCLUDE_FROM_ALL} cjpeg.c)
target_link_libraries(cjpeg libjpeg)

add_executable(djpeg ${EXCLUDE_FROM_ALL} djpeg.c)
target_link_libraries(djpeg libjpeg)

add_executable(jpegtran ${EXCLUDE_FROM_ALL} jpegtran.c)
target_link_libraries(jpegtran libjpeg)
