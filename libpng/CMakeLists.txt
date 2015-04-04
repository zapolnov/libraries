
cmake_minimum_required(VERSION 3.1)
project(libpng C)

configure_file(scripts/pnglibconf.h.prebuilt "${CMAKE_CURRENT_BINARY_DIR}/pnglibconf.h" COPYONLY)

add_library(libpng STATIC ${EXCLUDE_FROM_ALL}
	scripts/pnglibconf.h.prebuilt
	png.c
	png.h
	pngconf.h
	pngdebug.h
	pngerror.c
	pngget.c
	pnginfo.h
	pngmem.c
	pngpread.c
	pngpriv.h
	pngread.c
	pngrio.c
	pngrtran.c
	pngrutil.c
	pngset.c
	pngstruct.h
	pngtrans.c
	pngwio.c
	pngwrite.c
	pngwtran.c
	pngwutil.c
)

set_source_files_properties(scripts/pnglibconf.h.prebuilt PROPERTIES HEADER_FILE_ONLY TRUE)
set_target_properties(libpng PROPERTIES OUTPUT_NAME "png")
target_include_directories(libpng BEFORE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_include_directories(libpng BEFORE PUBLIC "${CMAKE_CURRENT_BINARY_DIR}")
target_link_libraries(libpng zlib)
