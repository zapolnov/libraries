
file(GLOB source_files *.cpp *.h)
source_group("Source Files" FILES ${source_files})

add_executable(exrmaketiled ${EXCLUDE_FROM_ALL} ${source_files})
target_link_libraries(exrmaketiled libopenexr)
