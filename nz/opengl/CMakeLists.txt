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

include(../cmake/OpenGL.cmake)
include(../cmake/Qt5.cmake)
include(../cmake/TargetPlatform.cmake)

file(GLOB root_sources *.cpp *.h *.def)
file(GLOB qt5_sources qt5/*.cpp qt5/*.h)
file(GLOB normal_sources normal/*.cpp normal/*.h)
file(GLOB dummy_sources dummy/*.cpp dummy/*.h)

if(Z_OPENGL_FOUND)
    add_definitions(-DZ_OPENGL_FOUND)
endif()

set(SHADERS_ZIP "${CMAKE_CURRENT_BINARY_DIR}/shaders.zip")
set(SHADERS_ZIP_CXX "${CMAKE_CURRENT_BINARY_DIR}/shaders.zip.cpp")

file(GLOB_RECURSE shaders RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" shaders/*)

add_custom_command(OUTPUT
        "${SHADERS_ZIP}"
    COMMAND
        "${CMAKE_COMMAND}" -E remove -f "${SHADERS_ZIP}"
    COMMAND
        minizip -9 "${SHADERS_ZIP}" ${shaders}
    DEPENDS
        ${shaders} minizip
    WORKING_DIRECTORY
        "${CMAKE_CURRENT_SOURCE_DIR}"
    COMMENT
        "Compressing shaders"
)

add_custom_command(OUTPUT
        "${SHADERS_ZIP_CXX}"
    COMMAND
        bin2c -o "${SHADERS_ZIP_CXX}" -i z_shaders_zip "${SHADERS_ZIP}"
    DEPENDS
        "${SHADERS_ZIP}" bin2c
    WORKING_DIRECTORY
        "${CMAKE_CURRENT_BINARY_DIR}"
    COMMENT
        "Generating shaders.zip.cpp"
)

add_library(NZOpenGL STATIC ${EXCLUDE_FROM_ALL}
    ${shaders}
    "${SHADERS_ZIP_CXX}"
    ${root_sources}
    ${qt5_sources}
    ${normal_sources}
    ${dummy_sources}
)

source_group("Source Files" FILES ${root_sources})
source_group("Source Files\\qt5" FILES ${qt5_sources})
source_group("Source Files\\normal" FILES ${normal_sources})
source_group("Source Files\\dummy" FILES ${dummy_sources})
source_group("Source Files\\shaders" FILES ${shaders})
source_group("Generated Files" FILES "${SHADERS_ZIP_CXX}")

set_source_files_properties(GLAttribute.def GLUniform.def ${shaders} PROPERTIES HEADER_FILE_ONLY TRUE)

set_target_properties(NZOpenGL PROPERTIES CXX_STANDARD 11)
target_include_directories(NZOpenGL BEFORE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/..")
target_link_libraries(NZOpenGL NZYaml NZUtility NZImage NZMesh NZZipFS)

if(Z_TARGET_QT5)
    z_target_link_qt5(NZOpenGL)
    target_link_libraries(NZOpenGL NZQt5Utility NZQt5OpenGLWindow)
else()
    z_target_link_opengl(NZOpenGL)
endif()

if(Z_OPENGL_FOUND)
    target_link_libraries(NZOpenGL glew)
endif()
