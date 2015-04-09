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

if(NOT __Z_OPENGL_CMAKE_INCLUDED)
    set(__Z_OPENGL_CMAKE_INCLUDED TRUE)

    set(Z_OPENGL_LIBRARIES)

    find_package(OpenGL)
    if(OPENGL_INCLUDE_DIR)
        include_directories(${OPENGL_INCLUDE_DIR})
    endif()

    if(NOT OPENGL_FOUND)
        find_package(PkgConfig)
        pkg_check_modules(OpenGL gl)
        if(${OpenGL_INCLUDE_DIRS})
            include_directories(${OpenGL_INCLUDE_DIRS})
        endif()
        if(${OpenGL_LDFLAGS})
            set(Z_OPENGL_LIBRARIES ${OpenGL_LDFLAGS})
        endif()
    endif()

    macro(z_get_opengl_libraries target_variable)
        if(Z_OPENGL_LIBRARIES)
            list(APPEND "${target_variable}" ${Z_OPENGL_LIBRARIES})
        endif()
    endmacro()

    macro(z_target_link_opengl target)
        set(opengl_libraries)
        z_get_opengl_libraries(opengl_libraries)
        target_link_libraries("${target}" ${opengl_libraries})
    endmacro()

endif()
