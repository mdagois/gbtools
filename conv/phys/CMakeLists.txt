cmake_minimum_required(VERSION 3.10.0)
project(clitools)

############################################################
# pconv
############################################################

set(PCONV_BIN_NAME pconv)

add_executable(
	${PCONV_BIN_NAME}
	pconv.cpp
	options.h options.cpp)

target_compile_definitions(${PCONV_BIN_NAME} PUBLIC _CRT_SECURE_NO_WARNINGS)
target_compile_features(${PCONV_BIN_NAME} PRIVATE cxx_std_17)
target_link_libraries(${PCONV_BIN_NAME} ${CONV_LIB_NAME})

