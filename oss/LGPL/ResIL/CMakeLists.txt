
cmake_minimum_required(VERSION 3.1)
project(ResIL)

include(../../../nz/cmake/OpenGL.cmake)
if(Z_OPENGL_FOUND)
    set(ILUT_USE_OPENGL TRUE)
endif()

if(MSVC)
    add_definitions(/wd4018)
    add_definitions(/wd4244)
endif()

include(CheckIncludeFile)
check_include_file(mm_malloc.h HAVE_MM_MALLOC_H)

include(CheckFunctionExists)
check_function_exists(memalign HAVE_MEMALIGN)
check_function_exists(posix_memalign HAVE_POSIX_MEMALIGN)
check_function_exists(_mm_malloc HAVE__MM_MALLOC)
check_function_exists(valloc HAVE_VALLOC)

include(TestBigEndian)
test_big_endian(BIGENDIAN)
if(BIGENDIAN)
    set(WORDS_BIGENDIAN TRUE)
else()
    unset(WORDS_BIGENDIAN)
endif()

include(CheckCXXSourceCompiles)
check_cxx_source_compiles("
    void iSwapUShort(unsigned short *s)  {
        asm(\"ror $8,%0\"
        : \"=r\" (*s)
        : \"0\" (*s));
    }
    int main() { return 0; }"
    GCC_X86_ASM)

file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/include")
configure_file(config.h.in "${CMAKE_CURRENT_BINARY_DIR}/include/ilut_config.h")

configure_file(config.h.in "${CMAKE_CURRENT_BINARY_DIR}/config.h")
add_definitions(-DHAVE_CONFIG_H)

file(GLOB include_files include/IL/*.h include/IL/*.hpp)
file(GLOB il_include_files src-IL/include/*.h)
file(GLOB il_source_files src-IL/src/*.c*)
file(GLOB ilu_include_files src-ILU/include/*.h)
file(GLOB ilu_include_error_files src-ILU/include/ilu_error/*.h)
file(GLOB ilu_source_files src-ILU/src/*.c*)
file(GLOB ilut_include_files src-ILUT/include/*.h)
file(GLOB ilut_source_files src-ILUT/src/*.c*)

source_group("Source Files\\include\\IL" FILES ${include_files})
source_group("Source Files\\src-IL\\include" FILES ${il_include_files})
source_group("Source Files\\src-IL\\src" FILES ${il_source_files})
source_group("Source Files\\src-ILU\\include" FILES ${ilu_include_files})
source_group("Source Files\\src-ILU\\include\\ilu_error" FILES ${ilu_include_error_files})
source_group("Source Files\\src-ILU\\src" FILES ${ilu_source_files})
source_group("Source Files\\src-ILUT\\include" FILES ${ilut_include_files})
source_group("Source Files\\src-ILUT\\src" FILES ${ilut_source_files})

add_library(ResIL STATIC ${EXCLUDE_FROM_ALL}
    ${include_files}
    ${il_include_files}
    ${il_source_files}
    ${ilu_include_files}
    ${ilu_include_error_files}
    ${ilu_source_files}
#    ${ilut_include_files}
#    ${ilut_source_files}
)

set_target_properties(ResIL PROPERTIES CXX_STANDARD 11)

target_include_directories(ResIL BEFORE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_include_directories(ResIL BEFORE PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/include")
target_include_directories(ResIL BEFORE PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
target_include_directories(ResIL BEFORE PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src-IL/include")
target_include_directories(ResIL BEFORE PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src-ILU/include")
target_include_directories(ResIL BEFORE PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src-ILUT/include")
z_target_link_opengl(ResIL)

target_link_libraries(ResIL libopenexr libjasper libtiff libmng libpng libjpeg lcms2)
target_compile_definitions(ResIL PUBLIC IL_STATIC_LIB)
