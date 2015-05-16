
include_directories(common)

add_library(lcms2_utils_common ${EXCLUDE_FROM_ALL} common/utils.h common/vprf.c common/xgetopt.c)
target_link_libraries(lcms2_utils_common lcms2)

add_executable(jpgicc ${EXCLUDE_FROM_ALL} jpgicc/iccjpeg.c jpgicc/iccjpeg.h jpgicc/jpgicc.c)
target_link_libraries(jpgicc lcms2 lcms2_utils_common libjpeg)

add_executable(linkicc ${EXCLUDE_FROM_ALL} linkicc/linkicc.c)
target_link_libraries(linkicc lcms2 lcms2_utils_common)

add_executable(psicc ${EXCLUDE_FROM_ALL} psicc/psicc.c)
target_link_libraries(psicc lcms2 lcms2_utils_common)

add_executable(tificc ${EXCLUDE_FROM_ALL} tificc/tificc.c)
target_link_libraries(tificc lcms2 lcms2_utils_common libtiff)

add_executable(tifdiff ${EXCLUDE_FROM_ALL} tificc/tifdiff.c)
target_link_libraries(tifdiff lcms2 lcms2_utils_common libtiff)

add_executable(transicc ${EXCLUDE_FROM_ALL} transicc/transicc.c)
target_link_libraries(transicc lcms2 lcms2_utils_common)
