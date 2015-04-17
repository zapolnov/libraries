
cmake_minimum_required(VERSION 3.1)

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_definitions(-D_CRT_NONSTDC_NO_DEPRECATE)
    add_definitions(/wd4068 /wd4141 /wd4146 /wd4244 /wd4456 /wd4459)
endif()

include(CheckCSourceCompiles)
check_c_source_compiles("
    #define _POSIX_SOURCE 1
    #include <stdio.h>
    #include <unistd.h>
    #include <sys/types.h>
    #include <sys/mman.h>
    #include <fcntl.h>
    int main() {
        size_t map_length = 0;
        int fd = open(\"file\", O_RDONLY, 0);
        long l = lseek(fd, 0, SEEK_END);
        FILE* f = fdopen(fd, \"r\");
        void* p = mmap(0, map_length, PROT_READ, MAP_PRIVATE, fd, 0);
        if (p != MAP_FAILED)
            munmap(p, map_length);
        return 0;
    }"
    UCPP_MMAP)

file(GLOB source_files *.c *.h)
set_source_files_properties(arith.c sample.c PROPERTIES HEADER_FILE_ONLY TRUE)
source_group("Source Files" FILES ${source_files})

add_library(libucpp STATIC ${EXCLUDE_FROM_ALL} ${source_files})
set_target_properties(libucpp PROPERTIES OUTPUT_NAME "ucpp")
target_include_directories(libucpp BEFORE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(ucpp ${EXCLUDE_FROM_ALL} ${source_files})
target_compile_definitions(ucpp PRIVATE "STAND_ALONE")

if(UCPP_MMAP)
    target_compile_definitions(libucpp PUBLIC "UCPP_MMAP")
    target_compile_definitions(ucpp PRIVATE "UCPP_MMAP")
endif()
