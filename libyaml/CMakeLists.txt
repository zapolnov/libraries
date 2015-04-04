
cmake_minimum_required(VERSION 3.1)
project(yaml C)

include_directories(win32)
add_definitions(-DHAVE_CONFIG_H)

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_definitions(/wd4457)
endif()

file(GLOB SRC src/*.c)
add_library(libyaml STATIC ${EXCLUDE_FROM_ALL} ${SRC})

set_target_properties(libyaml PROPERTIES OUTPUT_NAME "yaml")
target_compile_definitions(libyaml PUBLIC "YAML_DECLARE_STATIC")
target_include_directories(libyaml PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
