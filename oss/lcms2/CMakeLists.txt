
cmake_minimum_required(VERSION 3.1)
project(lcms2 C)

file(GLOB source_files src/*.c src/*.h)
file(GLOB include_files include/*.c include/*.h)
source_group("Source Files\\src" FILES ${source_files})
source_group("Source Files\\include" FILES ${include_files})

add_library(lcms2 STATIC ${EXCLUDE_FROM_ALL}
    ${source_files}
    ${include_files}
)

target_include_directories(lcms2 BEFORE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

add_subdirectory(utils)
