
cmake_minimum_required(VERSION 3.1)
project(sqlite3)

find_package(Threads)

if(MSVC)
    add_definitions(/wd4456 /wd4459)
endif()

file(GLOB root_files *.c *.h)
source_group("Source Files" FILES ${root_files})

add_library(libsqlite3 STATIC ${EXCLUDE_FROM_ALL} sqlite3.c sqlite3.h sqlite3ext.h)
set_target_properties(libsqlite3 PROPERTIES OUTPUT_NAME "sqlite3")
target_include_directories(libsqlite3 BEFORE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(libsqlite3 ${CMAKE_THREAD_LIBS_INIT} ${CMAKE_DL_LIBS})

add_executable(sqlite3 ${EXCLUDE_FROM_ALL} shell.c)
target_link_libraries(sqlite3 libsqlite3)
