cmake_minimum_required(VERSION 3.10.0)
project(gbgfx)

add_library(
	gbgfx
	STATIC
	color.cpp
	image.cpp)
target_compile_definitions(gbgfx PUBLIC _CRT_SECURE_NO_WARNINGS)

