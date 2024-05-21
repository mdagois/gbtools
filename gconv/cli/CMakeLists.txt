cmake_minimum_required(VERSION 3.20.0)

add_library(
	${CLI_LIB_NAME}
	STATIC
	commandline.h commandline.cpp
)

target_compile_definitions(${CLI_LIB_NAME} PUBLIC _CRT_SECURE_NO_WARNINGS)
target_compile_features(${CLI_LIB_NAME} PRIVATE cxx_std_17)

