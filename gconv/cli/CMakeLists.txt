cmake_minimum_required(VERSION 3.20.0)

set(LIB_NAME cli)
set(CLI_LIB_NAME ${LIB_NAME} PARENT_SCOPE)
set(CLI_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)

add_library(
	${LIB_NAME}
	STATIC
	commandline.h commandline.cpp
)

target_compile_definitions(${LIB_NAME} PUBLIC _CRT_SECURE_NO_WARNINGS)
target_compile_features(${LIB_NAME} PRIVATE cxx_std_17)

