
cmake_minimum_required(VERSION 3.1)
project(glew)

include(../../nz/cmake/OpenGL.cmake)

if(Z_OPENGL_FOUND)

    add_definitions(-DGLEW_BUILD)

    add_library(glew STATIC ${EXCLUDE_FROM_ALL}
        include/GL/glew.h
        include/GL/glxew.h
        include/GL/wglew.h
        src/glew.c
    )

    target_include_directories(glew PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
    target_compile_definitions(glew PUBLIC GLEW_STATIC GLEW_NO_GLU)
    z_target_link_opengl(glew)

endif()
