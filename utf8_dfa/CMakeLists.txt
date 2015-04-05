
cmake_minimum_required(VERSION 3.1)
project(utf8_dfa)

add_library(utf8_dfa STATIC ${EXCLUDE_FROM_ALL}
    utf8_dfa.cpp
    utf8_dfa.h
)
