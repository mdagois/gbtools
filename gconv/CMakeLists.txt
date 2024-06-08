cmake_minimum_required(VERSION 3.10.0)
project(clitools)

############################################################
# libraries
############################################################

add_subdirectory(gfx)
add_subdirectory(cli)
include_directories(${CLI_INCLUDE_DIR} ${GFX_INCLUDE_DIR})

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
target_link_libraries(${CONV_BIN_NAME} ${CLI_LIB_NAME})
target_link_libraries(${CONV_BIN_NAME} ${GFX_LIB_NAME})

############################################################
# readinfo
############################################################

set(READINFO_BIN_NAME readinfo)

add_executable(
	${READINFO_BIN_NAME}
	readinfo.cpp)

target_compile_definitions(${READINFO_BIN_NAME} PUBLIC _CRT_SECURE_NO_WARNINGS)
target_compile_features(${READINFO_BIN_NAME} PRIVATE cxx_std_17)
target_link_libraries(${READINFO_BIN_NAME} ${GFX_LIB_NAME})

