cmake_minimum_required(VERSION 3.20.0)

add_library(
	cli
	STATIC
	commandline.h commandline.cpp
)

target_compile_definitions(cli PUBLIC _CRT_SECURE_NO_WARNINGS)
target_compile_features(cli PRIVATE cxx_std_17)

