
include_directories("${CMAKE_CURRENT_SOURCE_DIR}")

file(GLOB source_files *.cpp *.h)
source_group("Source Files" FILES ${source_files})

add_executable(exrenvmap ${EXCLUDE_FROM_ALL} ${source_files})
target_link_libraries(exrenvmap libopenexr)
