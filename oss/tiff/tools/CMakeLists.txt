
include(../../../nz/cmake/OpenGL.cmake)

add_executable(bmp2tiff ${EXCLUDE_FROM_ALL} bmp2tiff.c)
target_link_libraries(bmp2tiff tiff)

add_executable(fax2ps ${EXCLUDE_FROM_ALL} fax2ps.c)
target_link_libraries(fax2ps tiff)

add_executable(fax2tiff ${EXCLUDE_FROM_ALL} fax2tiff.c)
target_link_libraries(fax2tiff tiff)

add_executable(gif2tiff ${EXCLUDE_FROM_ALL} gif2tiff.c)
target_link_libraries(gif2tiff tiff)

add_executable(pal2rgb ${EXCLUDE_FROM_ALL} pal2rgb.c)
target_link_libraries(pal2rgb tiff)

add_executable(ppm2tiff ${EXCLUDE_FROM_ALL} ppm2tiff.c)
target_link_libraries(ppm2tiff tiff)

add_executable(ras2tiff ${EXCLUDE_FROM_ALL} ras2tiff.c rasterfile.h)
target_link_libraries(ras2tiff tiff)

add_executable(raw2tiff ${EXCLUDE_FROM_ALL} raw2tiff.c)
target_link_libraries(raw2tiff tiff)

add_executable(rgb2ycbcr ${EXCLUDE_FROM_ALL} rgb2ycbcr.c ycbcr.c)
set_source_files_properties(ycbcr.c PROPERTIES HEADER_FILE_ONLY TRUE)
target_link_libraries(rgb2ycbcr tiff)

add_executable(thumbnail ${EXCLUDE_FROM_ALL} thumbnail.c)
target_link_libraries(thumbnail tiff)

add_executable(tiff2bw ${EXCLUDE_FROM_ALL} tiff2bw.c)
target_link_libraries(tiff2bw tiff)

add_executable(tiff2pdf ${EXCLUDE_FROM_ALL} tiff2pdf.c)
target_link_libraries(tiff2pdf tiff)

add_executable(tiff2ps ${EXCLUDE_FROM_ALL} tiff2ps.c)
target_link_libraries(tiff2ps tiff)

add_executable(tiff2rgba ${EXCLUDE_FROM_ALL} tiff2rgba.c)
target_link_libraries(tiff2rgba tiff)

add_executable(tiffcmp ${EXCLUDE_FROM_ALL} tiffcmp.c)
target_link_libraries(tiffcmp tiff)

add_executable(tiffcp ${EXCLUDE_FROM_ALL} tiffcp.c)
target_link_libraries(tiffcp tiff)

add_executable(tiffcrop ${EXCLUDE_FROM_ALL} tiffcrop.c)
target_link_libraries(tiffcrop tiff)

add_executable(tiffdither ${EXCLUDE_FROM_ALL} tiffdither.c)
target_link_libraries(tiffdither tiff)

add_executable(tiffdump ${EXCLUDE_FROM_ALL} tiffdump.c)
target_link_libraries(tiffdump tiff)

add_executable(tiffinfo ${EXCLUDE_FROM_ALL} tiffinfo.c)
target_link_libraries(tiffinfo tiff)

add_executable(tiffmedian ${EXCLUDE_FROM_ALL} tiffmedian.c)
target_link_libraries(tiffmedian tiff)

add_executable(tiffset ${EXCLUDE_FROM_ALL} tiffset.c)
target_link_libraries(tiffset tiff)

add_executable(tiffsplit ${EXCLUDE_FROM_ALL} tiffsplit.c)
target_link_libraries(tiffsplit tiff)

if(Z_OPENGL_FOUND)
    find_library(glut glut)
    if(glut)
        add_executable(tiffgt ${EXCLUDE_FROM_ALL} tiffgt.c)
        target_link_libraries(tiffgt tiff ${glut})
        z_target_link_opengl(tiffgt)
    endif()
endif()
