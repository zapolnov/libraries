
cmake_minimum_required(VERSION 3.2)
project(libyaml C)

include_directories(win32)
add_definitions(-DHAVE_CONFIG_H)

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_definitions(-D_CRT_NONSTDC_NO_DEPRECATE)
    add_definitions(/wd4244 /wd4267 /wd4456 /wd4457)
endif()

file(GLOB include_files include/*)
file(GLOB src_files src/*)
file(GLOB win32_files win32/*)

add_library(libyaml STATIC ${EXCLUDE_FROM_ALL}
    ${include_files}
    ${src_files}
    ${win32_files}
)

source_group("Source Files\\include" FILES ${include_files})
source_group("Source Files\\src" FILES ${src_files})
source_group("Source Files\\win32" FILES ${win32_files})

set_target_properties(libyaml PROPERTIES OUTPUT_NAME "yaml")
target_compile_definitions(libyaml PUBLIC "YAML_DECLARE_STATIC")
target_include_directories(libyaml PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
