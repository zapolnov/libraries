
cmake_minimum_required(VERSION 3.1)
project(ilmbase)

find_package(Threads)

if(MSVC)
    add_definitions(/wd4018)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

if(NOT WIN32)
    add_definitions(-pthread)
endif()

file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/include")
configure_file(IlmBaseConfig.h.in "${CMAKE_CURRENT_BINARY_DIR}/include/IlmBaseConfig.h")

file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/include/OpenEXR")
include(InstallHeaders.cmake)

set(half_sources
    Half/eLut.h
    Half/half.cpp
    Half/half.h
    Half/halfExport.h
    Half/halfFunction.h
    Half/halfLimits.h
    Half/toFloat.h
)

file(GLOB iex_sources Iex/*.cpp Iex/*.h)
file(GLOB iexmath_sources IexMath/*.cpp IexMath/*.h)
file(GLOB ilmthread_sources IlmThread/*.cpp IlmThread/*.h)
file(GLOB imath_sources Imath/*.cpp Imath/*.h)

source_group("Source Files\\Half" FILES ${half_sources})
source_group("Source Files\\Iex" FILES ${iex_sources})
source_group("Source Files\\IexMath" FILES ${iexmath_sources})
source_group("Source Files\\IlmThread" FILES ${ilmthread_sources})
source_group("Source Files\\Imath" FILES ${imath_sources})

if(NOT WIN32)
    set_source_files_properties(
        IlmThread/IlmThreadMutexWin32.cpp
        IlmThread/IlmThreadSemaphoreWin32.cpp
        IlmThread/IlmThreadWin32.cpp
        PROPERTIES HEADER_FILE_ONLY TRUE)
endif()

add_library(ilmbase STATIC ${EXCLUDE_FROM_ALL}
    ${half_sources}
    ${iex_sources}
    ${iexmath_sources}
    ${ilmthread_sources}
    ${imath_sources}
)

target_include_directories(ilmbase PUBLIC BEFORE
    "${CMAKE_CURRENT_BINARY_DIR}/include"
    "${CMAKE_CURRENT_BINARY_DIR}/include/OpenEXR"
)

target_include_directories(ilmbase PRIVATE BEFORE
    "${CMAKE_CURRENT_SOURCE_DIR}/Half"
    "${CMAKE_CURRENT_SOURCE_DIR}/Iex"
    "${CMAKE_CURRENT_SOURCE_DIR}/IexMath"
    "${CMAKE_CURRENT_SOURCE_DIR}/IlmThread"
    "${CMAKE_CURRENT_SOURCE_DIR}/Imath"
)

target_link_libraries(ilmbase ${CMAKE_THREAD_LIBS_INIT})
