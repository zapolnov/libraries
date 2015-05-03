
cmake_minimum_required(VERSION 3.1)
project(glew)

include(../../nz/cmake/OpenGL.cmake)

if(Z_OPENGL_FOUND)

    add_definitions(-DGLEW_BUILD)

    set(include_files
        include/GL/glew.h
        include/GL/glxew.h
        include/GL/wglew.h
    )

    set(source_files
        src/glew.c
    )

    source_group("Source Files\\include" FILES ${include_files})
    source_group("Source Files\\src" FILES ${source_files})

    add_library(glew STATIC ${EXCLUDE_FROM_ALL}
        ${include_files}
        ${source_files}
    )

    target_include_directories(glew PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
    target_compile_definitions(glew PUBLIC GLEW_STATIC GLEW_NO_GLU)
    z_target_link_opengl(glew)

endif()
