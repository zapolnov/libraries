
cmake_minimum_required(VERSION 3.1)
project(byacc)

include(CheckIncludeFile)
check_include_file(fcntl.h HAVE_FCNTL_H)

include(CheckFunctionExists)
check_function_exists(mkstemp HAVE_MKSTEMP)
check_function_exists(vsnprintf HAVE_VSNPRINTF)

configure_file(config.h.in "${CMAKE_CURRENT_BINARY_DIR}/config.h")
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

add_definitions(-DHAVE_CONFIG_H)

add_executable(byacc ${EXCLUDE_FROM_ALL}
    closure.c
    defs.h
    error.c
    graph.c
    lalr.c
    lr0.c
    main.c
    mkpar.c
    mstring.c
    output.c
    reader.c
    symtab.c
    verbose.c
    warshall.c
    yaccpar.c
)
