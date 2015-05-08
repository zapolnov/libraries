#
# Copyright (c) 2015 Nikolay Zapolnov (zapolnov@gmail.com).
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
project(python)

include(../../nz/CMake/WriteFile.cmake)
include(CMake/Configure.cmake)
include_directories(Include "${CMAKE_CURRENT_BINARY_DIR}")

#############################################################################

include(CMakeParseArguments)

set(enabled_modules)
macro(module name)

    cmake_parse_arguments(MOD "" "" "REQUIRES" ${ARGN})

    set(module_py_files_${name})
    set(module_c_files_${name})
    foreach(file ${MOD_UNPARSED_ARGUMENTS})
        get_filename_component(ext "${file}" EXT)
        if("${ext}" STREQUAL ".py")
            list(APPEND module_py_files_${name} "${file}")
        else()
            list(APPEND module_c_files_${name} "${file}")
        endif()
    endforeach()

    option(Z_PYTHON_MODULE_${name} "Enable Python module ${name}" OFF)
    if(Z_PYTHON_MODULE_${name})

        list(APPEND enabled_modules "${name}")

        foreach(module ${MOD_REQUIRES})
            if(NOT Z_PYTHON_MODULE_${module})
                message(STATUS "WARNING: Implicitly enabling module \"${module}\" (required by module \"${name}\")")
                set(Z_PYTHON_MODULE_${module} YES)
                list(APPEND enabled_modules "${module}")
            endif()
        endforeach()

    endif()
endmacro()

include(CMake/Modules.cmake)

set(module_files)
set(data)
foreach(name ${enabled_modules})
    list(APPEND module_files ${module_c_files_${name}})
    set(data "${data}#define PYTHON_MODULE_${name} 1\n")
endforeach()
z_write_file("${CMAKE_CURRENT_BINARY_DIR}/python_additional_module_list.h" "${data}")

#############################################################################

include(Python/Files.cmake)
include(Objects/Files.cmake)
include(Parser/Files.cmake)
include(Modules/Files.cmake)

add_library(pythoncore STATIC ${EXCLUDE_FROM_ALL}
    PC/config.c
    ${python_files}
    ${objects_files}
    ${parser_files}
    ${base_modules_files}
    ${module_files}
)

source_group("Source Files\\PC" FILES PC/config.c)

target_link_libraries(pythoncore zlib ${CMAKE_THREAD_LIBS_INIT})
if(LIBM)
    target_link_libraries(pythoncore ${LIBM})
endif()
if(WIN32)
    target_link_libraries(pythoncore user32 shell32 advapi32 ws2_32)
endif()

if(WIN32)
    #python_module(_subprocess PC/_subprocess.c)
    #python_module(_winreg PC/_winreg.c)
endif()

set(pythonmodules_io_files
    Modules/_io/_iomodule.c
    Modules/_io/_iomodule.h
    Modules/_io/bufferedio.c
    Modules/_io/bytesio.c
    Modules/_io/fileio.c
    Modules/_io/iobase.c
    Modules/_io/stringio.c
    Modules/_io/textio.c
)

set(pythonmodules_cjkcodecs_files
    Modules/cjkcodecs/_codecs_cn.c
    Modules/cjkcodecs/_codecs_hk.c
    Modules/cjkcodecs/_codecs_iso2022.c
    Modules/cjkcodecs/_codecs_jp.c
    Modules/cjkcodecs/_codecs_kr.c
    Modules/cjkcodecs/_codecs_tw.c
    Modules/cjkcodecs/alg_jisx0201.h
    Modules/cjkcodecs/cjkcodecs.h
    Modules/cjkcodecs/emu_jisx0213_2000.h
    Modules/cjkcodecs/mappings_cn.h
    Modules/cjkcodecs/mappings_hk.h
    Modules/cjkcodecs/mappings_jisx0213_pair.h
    Modules/cjkcodecs/mappings_jp.h
    Modules/cjkcodecs/mappings_kr.h
    Modules/cjkcodecs/mappings_tw.h
    Modules/cjkcodecs/multibytecodec.c
    Modules/cjkcodecs/multibytecodec.h
)

set(pythonmodules_files
    Modules/_bisectmodule.c
    Modules/_collectionsmodule.c
    Modules/_csv.c
    Modules/_heapqmodule.c
    Modules/_hotshot.c
    Modules/_json.c
    Modules/_lsprof.c
    Modules/_randommodule.c
    Modules/_struct.c
    Modules/imageop.c
    Modules/md5.c
    Modules/md5.h
    Modules/md5module.c
    Modules/sha256module.c
    Modules/sha512module.c
    Modules/shamodule.c
    Modules/socketmodule.c
    Modules/socketmodule.h
    Modules/symtablemodule.c
)

source_group("Source Files\\_io" FILES ${pythonmodules_io_files})
source_group("Source Files\\_cjkcodecs" FILES ${pythonmodules_cjkcodecs_files})
source_group("Source Files" FILES ${pythonmodules_files})

add_library(pythonmodules STATIC ${EXCLUDE_FROM_ALL}
    ${pythonmodules_io_files}
    ${pythonmodules_cjkcodecs_files}
    ${pythonmodules_files}
)

add_executable(python ${EXCLUDE_FROM_ALL} Modules/python.c)
source_group("Source Files" FILES Modules/python.c)
target_link_libraries(python pythoncore)
