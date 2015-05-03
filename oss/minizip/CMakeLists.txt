
cmake_minimum_required(VERSION 3.1)
project(minizip C)

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
    add_definitions(-D_CRT_NONSTDC_NO_DEPRECATE)
endif()

add_definitions(-DHAVE_AES)
add_definitions(-DHAVE_BZIP2)

file(GLOB source_files
    crypt.h
    ioapi.c
    ioapi.h
    ioapi_buf.c
    ioapi_buf.h
    ioapi_mem.c
    ioapi_mem.h
    iowin32.c
    iowin32.h
    unzip.c
    unzip.h
    zip.c
    zip.h
)

source_group("Source Files" FILES ${source_files})

file(GLOB aes_files aes/*.c aes/*.h)
source_group("Source Files\\aes" FILES ${aes_files})

add_library(libminizip STATIC ${EXCLUDE_FROM_ALL}
    ${source_files}
    ${aes_files}
)

if(NOT WIN32)
    set_source_files_properties(iowin32.c PROPERTIES HEADER_FILE_ONLY TRUE)
endif()

set_target_properties(libminizip PROPERTIES OUTPUT_NAME "minizip")
target_include_directories(libminizip BEFORE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(libminizip zlib libbzip2)

add_executable(minizip ${EXCLUDE_FROM_ALL} minizip.c)
target_link_libraries(minizip libminizip)

add_executable(miniunz ${EXCLUDE_FROM_ALL} miniunz.c)
target_link_libraries(miniunz libminizip)
