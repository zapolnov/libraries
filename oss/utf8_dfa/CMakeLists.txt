
cmake_minimum_required(VERSION 3.2)
project(utf8_dfa)

file(GLOB source_files *.cpp *.h)

add_library(utf8_dfa STATIC ${EXCLUDE_FROM_ALL}
    ${source_files}
)

source_group("Source Files" FILES ${source_files})
set_target_properties(utf8_dfa PROPERTIES CXX_STANDARD 11)
