
cmake_minimum_required(VERSION 3.1)
project(tinyxml)

if(MSVC)
    add_definitions(/wd4458)
endif()

file(GLOB root_files *.cpp *.h)
source_group("Source Files" FILES ${root_files})

add_library(tinyxml STATIC ${EXCLUDE_FROM_ALL} ${root_files})
target_include_directories(tinyxml BEFORE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
