
include(../../../nz/cmake/OpenGL.cmake)

add_executable(bmp2tiff ${EXCLUDE_FROM_ALL} bmp2tiff.c)
target_link_libraries(bmp2tiff libtiff)

add_executable(fax2ps ${EXCLUDE_FROM_ALL} fax2ps.c)
target_link_libraries(fax2ps libtiff)

add_executable(fax2tiff ${EXCLUDE_FROM_ALL} fax2tiff.c)
target_link_libraries(fax2tiff libtiff)

add_executable(gif2tiff ${EXCLUDE_FROM_ALL} gif2tiff.c)
target_link_libraries(gif2tiff libtiff)

add_executable(pal2rgb ${EXCLUDE_FROM_ALL} pal2rgb.c)
target_link_libraries(pal2rgb libtiff)

add_executable(ppm2tiff ${EXCLUDE_FROM_ALL} ppm2tiff.c)
target_link_libraries(ppm2tiff libtiff)

add_executable(ras2tiff ${EXCLUDE_FROM_ALL} ras2tiff.c rasterfile.h)
target_link_libraries(ras2tiff libtiff)

add_executable(raw2tiff ${EXCLUDE_FROM_ALL} raw2tiff.c)
target_link_libraries(raw2tiff libtiff)

add_executable(rgb2ycbcr ${EXCLUDE_FROM_ALL} rgb2ycbcr.c ycbcr.c)
set_source_files_properties(ycbcr.c PROPERTIES HEADER_FILE_ONLY TRUE)
target_link_libraries(rgb2ycbcr libtiff)

add_executable(thumbnail ${EXCLUDE_FROM_ALL} thumbnail.c)
target_link_libraries(thumbnail libtiff)

add_executable(tiff2bw ${EXCLUDE_FROM_ALL} tiff2bw.c)
target_link_libraries(tiff2bw libtiff)

add_executable(tiff2pdf ${EXCLUDE_FROM_ALL} tiff2pdf.c)
target_link_libraries(tiff2pdf libtiff)

add_executable(tiff2ps ${EXCLUDE_FROM_ALL} tiff2ps.c)
target_link_libraries(tiff2ps libtiff)

add_executable(tiff2rgba ${EXCLUDE_FROM_ALL} tiff2rgba.c)
target_link_libraries(tiff2rgba libtiff)

add_executable(tiffcmp ${EXCLUDE_FROM_ALL} tiffcmp.c)
target_link_libraries(tiffcmp libtiff)

add_executable(tiffcp ${EXCLUDE_FROM_ALL} tiffcp.c)
target_link_libraries(tiffcp libtiff)

add_executable(tiffcrop ${EXCLUDE_FROM_ALL} tiffcrop.c)
target_link_libraries(tiffcrop libtiff)

add_executable(tiffdither ${EXCLUDE_FROM_ALL} tiffdither.c)
target_link_libraries(tiffdither libtiff)

add_executable(tiffdump ${EXCLUDE_FROM_ALL} tiffdump.c)
target_link_libraries(tiffdump libtiff)

add_executable(tiffinfo ${EXCLUDE_FROM_ALL} tiffinfo.c)
target_link_libraries(tiffinfo libtiff)

add_executable(tiffmedian ${EXCLUDE_FROM_ALL} tiffmedian.c)
target_link_libraries(tiffmedian libtiff)

add_executable(tiffset ${EXCLUDE_FROM_ALL} tiffset.c)
target_link_libraries(tiffset libtiff)

add_executable(tiffsplit ${EXCLUDE_FROM_ALL} tiffsplit.c)
target_link_libraries(tiffsplit libtiff)

if(Z_OPENGL_FOUND)
    find_package(GLUT)
    if(GLUT_FOUND)
        include_directories(${GLUT_INCLUDE_DIR})
        if(NOT MSVC)
            add_definitions(-w)
        endif()
        add_executable(tiffgt ${EXCLUDE_FROM_ALL} tiffgt.c)
        target_link_libraries(tiffgt libtiff ${GLUT_LIBRARIES})
        z_target_link_opengl(tiffgt)
    endif()
endif()
