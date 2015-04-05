
cmake_minimum_required(VERSION 3.1)

include(CheckTypeSize)
check_type_size(char SIZEOF_CHAR)
check_type_size(short SIZEOF_SHORT)
check_type_size(int SIZEOF_INT)
check_type_size(long SIZEOF_LONG)

include(CheckFunctionExists)
check_function_exists(strdup HAVE_STRDUP)
check_function_exists(strndup HAVE_STRNDUP)

configure_file(re2c_config.h.in "${CMAKE_CURRENT_BINARY_DIR}/re2c_config.h")
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

file(GLOB source_files *.cc *.h)

add_executable(re2c ${EXCLUDE_FROM_ALL}
    ${source_files}
)

source_group("Source Files" FILES ${source_files})
