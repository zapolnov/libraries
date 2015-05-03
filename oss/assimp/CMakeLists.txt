
cmake_minimum_required(VERSION 3.1)
project(assimp)

include(CheckCXXCompilerFlag)
if(MSVC)
    check_cxx_compiler_flag("/w" HAVE_CXX_DISABLE_ALL_WARNINGS)
    if(HAVE_CXX_DISABLE_ALL_WARNINGS)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /w")
    endif()
else()
    check_cxx_compiler_flag("-w" HAVE_CXX_DISABLE_ALL_WARNINGS)
    if(HAVE_CXX_DISABLE_ALL_WARNINGS)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w")
    endif()
endif()

set(GIT_BRANCH "master")
set(GIT_COMMIT_HASH 0)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/revision.h.in" "${CMAKE_CURRENT_BINARY_DIR}/revision.h")

include_directories(code/BoostWorkaround)
add_definitions(-DASSIMP_BUILD_BOOST_WORKAROUND)
add_definitions(-DASSIMP_BUILD_NO_OWN_ZLIB)

set(ASSIMP_NO_EXPORT OFF CACHE BOOL "Disable Assimp's export functionality.")
if(ASSIMP_NO_EXPORT)
    add_definitions(-DASSIMP_BUILD_NO_EXPORT)
endif(ASSIMP_NO_EXPORT)


file(GLOB code_files code/*.cpp code/*.h)
file(GLOB code_inl_files code/*.inl)
file(GLOB boost_math_files code/BoostWorkaround/boost/math/*.hpp)
file(GLOB boost_tuple_files code/BoostWorkaround/boost/tuple/*.hpp)
file(GLOB boost_files code/BoostWorkaround/boost/*.hpp)
file(GLOB cvtutf_files contrib/ConvertUTF/*.c contrib/ConvertUTF/*.h)
file(GLOB clipper_files contrib/clipper/*.cpp contrib/clipper/*.hpp)
file(GLOB irrxml_files contrib/irrXML/*.cpp contrib/irrXML/*.h)
file(GLOB poly2tri_common_files contrib/poly2tri/poly2tri/common/*.cc contrib/poly2tri/poly2tri/common/*.h)
file(GLOB poly2tri_sweep_files contrib/poly2tri/poly2tri/sweep/*.cc contrib/poly2tri/poly2tri/sweep/*.h)
file(GLOB poly2tri_files contrib/poly2tri/poly2tri/*.h)

source_group("Source Files\\code" FILES ${code_files} ${code_inl_files})
source_group("Source Files\\code\\BoostWorkaround\\boost\\math" FILES ${boost_math_files})
source_group("Source Files\\code\\BoostWorkaround\\boost\\tuple" FILES ${boost_tuple_files})
source_group("Source Files\\code\\BoostWorkaround\\boost" FILES ${boost_files})
source_group("Source Files\\contrib\\ConvertUTF" FILES ${cvtutf_files})
source_group("Source Files\\contrib\\clipper" FILES ${clipper_files})
source_group("Source Files\\contrib\\irrXML" FILES ${irrxml_files})
source_group("Source Files\\contrib\\poly2tri\\poly2tri" FILES ${poly2tri_files})
source_group("Source Files\\contrib\\poly2tri\\poly2tri\\common" FILES ${poly2tri_common_files})
source_group("Source Files\\contrib\\poly2tri\\poly2tri\\sweep" FILES ${poly2tri_sweep_files})
set_source_files_properties(${code_inl_files} PROPERTIES HEADER_FILE_ONLY TRUE)

add_library(libassimp STATIC ${EXCLUDE_FROM_ALL}
    ${code_files}
    ${code_inl_files}
    ${boost_math_files}
    ${boost_tuple_files}
    ${boost_files}
    ${cvtutf_files}
    ${clipper_files}
    ${irrxml_files}
    ${poly2tri_files}
    ${poly2tri_common_files}
    ${poly2tri_sweep_files}
)

target_include_directories(libassimp BEFORE PUBLIC "${CMAKE_CURRENT_BINARY_DIR}" include)
target_link_libraries(libassimp libminizip zlib)
set_target_properties(libassimp PROPERTIES OUTPUT_NAME "assimp")

add_subdirectory(tools/assimp_cmd)
