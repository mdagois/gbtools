cmake_minimum_required(VERSION 3.10.0)
project(gbgfx)

############################################################
# libraries
############################################################

set(GB_LIB_NAME gbgfx)

include_directories(cli lib)
add_subdirectory(lib)
add_subdirectory(cli)

############################################################
# gconv
############################################################

set(CONV_BIN_NAME gconv)

add_executable(
	${CONV_BIN_NAME}
	conv.cpp
	options.h options.cpp)

target_compile_definitions(${CONV_BIN_NAME} PUBLIC _CRT_SECURE_NO_WARNINGS)
target_compile_features(${CONV_BIN_NAME} PRIVATE cxx_std_17)
target_link_libraries(${CONV_BIN_NAME} ${GB_LIB_NAME})
target_link_libraries(${CONV_BIN_NAME} ${CLI_LIB_NAME})

############################################################
# readinfo
############################################################

set(READINFO_BIN_NAME readinfo)

add_executable(
	${READINFO_BIN_NAME}
	readinfo.cpp)

target_compile_definitions(${READINFO_BIN_NAME} PUBLIC _CRT_SECURE_NO_WARNINGS)
target_compile_features(${READINFO_BIN_NAME} PRIVATE cxx_std_17)
target_link_libraries(${READINFO_BIN_NAME} ${GB_LIB_NAME})

