
cmake_minimum_required(VERSION 3.1)
project(ode)

if(APPLE)
    include(CheckCXXCompilerFlag)
    check_cxx_compiler_flag("-Wno-deprecated-declarations" HAVE_WNO_DEPRECATED_DECLARATIONS)
    if(HAVE_WNO_DEPRECATED_DECLARATIONS)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")
    endif()
endif()

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/GIMPACT/include")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/OPCODE")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/libccd/src")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/build")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/ode/src")
add_definitions(-DHAVE_CONFIG_H)

file(GLOB build_files build/*.h)
file(GLOB gimpact_include_files GIMPACT/include/GIMPACT/*.h)
file(GLOB gimpact_src_files GIMPACT/src/*.cpp)
file(GLOB opcode_ice_files OPCODE/Ice/*.cpp OPCODE/Ice/*.h)
file(GLOB opcode_files OPCODE/*.cpp OPCODE/*.h)
file(GLOB include_ode_files include/ode/*.h)
file(GLOB libccd_files libccd/src/*.c)
file(GLOB libccd_header_files libccd/src/ccd/*.h)
file(GLOB ode_src_files ode/src/*.c ode/src/*.cpp ode/src/*.h)
file(GLOB ode_src_joints_files ode/src/joints/*.cpp ode/src/joints/*.h)

source_group("Source Files\\GIMPACT\\include\\GIMPACT" FILES ${gimpact_include_files})
source_group("Source Files\\GIMPACT\\src" FILES ${gimpact_src_files})
source_group("Source Files\\OPCODE\\Ice" FILES ${opcode_ice_files})
source_group("Source Files\\OPCODE" FILES ${opcode_files})
source_group("Source Files\\include\\ode" FILES ${include_ode_files})
source_group("Source Files\\libccd\\src" FILES ${libccd_files})
source_group("Source Files\\libccd\\src\\ccd" FILES ${libccd_header_files})
source_group("Source Files\\ode\\src" FILES ${ode_src_files})
source_group("Source Files\\ode\\src\\joints" FILES ${ode_src_joints_files})
source_group("Source Files\\build" FILES ${build_files})

add_library(ode STATIC ${EXCLUDE_FROM_ALL}
    ${build_files}
    ${gimpact_include_files}
    ${gimpact_src_files}
    ${opcode_ice_files}
    ${opcode_files}
    ${include_ode_files}
    ${libccd_files}
    ${libccd_hedaer_files}
    ${ode_src_files}
    ${ode_src_joints_files}
)

target_include_directories(ode BEFORE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
