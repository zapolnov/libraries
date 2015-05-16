
include(../../../../nz/cmake/OpenGL.cmake)

add_executable(imgcmp ${EXCLUDE_FROM_ALL} imgcmp.c)
target_link_libraries(imgcmp libjasper)

add_executable(imginfo ${EXCLUDE_FROM_ALL} imginfo.c)
target_link_libraries(imginfo libjasper)

add_executable(jasper ${EXCLUDE_FROM_ALL} jasper.c)
target_link_libraries(jasper libjasper)

if(Z_OPENGL_FOUND)
    find_package(GLUT)
    if(GLUT_FOUND)
        include_directories(${GLUT_INCLUDE_DIR})
        if(NOT MSVC)
            add_definitions(-w)
        endif()
        add_executable(jiv ${EXCLUDE_FROM_ALL} jiv.c)
        target_link_libraries(jiv libjasper ${GLUT_LIBRARIES})
        z_target_link_opengl(jiv)
    endif()
endif()
