
cmake_minimum_required(VERSION 3.1)

include(CheckTypeSize)
check_type_size(char SIZEOF_CHAR)
check_type_size(short SIZEOF_SHORT)
check_type_size(int SIZEOF_INT)
check_type_size(long SIZEOF_LONG)

include(CheckFunctionExists)
check_function_exists(strdup HAVE_STRDUP)
check_function_exists(strndup HAVE_STRNDUP)

configure_file(re2c_config.h.in "${CMAKE_CURRENT_BINARY_DIR}/re2c_config.h")
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

add_executable(re2c ${EXCLUDE_FROM_ALL}
	actions.cc
	basics.h
	code.cc
	code.h
	code_names.h
	dfa.cc
	dfa.h
	enc.cc
	enc.h
	free_list.h
	globals.h
	indent.h
	input_api.cc
	input_api.h
	ins.h
	main.cc
	mbo_getopt.cc
	mbo_getopt.h
	parser.cc
	parser.h
	parser.y
	print.cc
	print.h
	range.cc
	range.h
	range_suffix.cc
	range_suffix.h
	re.h
	scanner.cc
	scanner.h
	scanner.re
	smart_ptr.h
	stream_lc.h
	substr.cc
	substr.h
	token.h
	translate.cc
	utf16.cc
	utf16.h
	utf16_range.cc
	utf16_range.h
	utf16_regexp.cc
	utf16_regexp.h
	utf8.cc
	utf8.h
	utf8_range.cc
	utf8_range.h
	utf8_regexp.cc
	utf8_regexp.h
	y.tab.h
)
