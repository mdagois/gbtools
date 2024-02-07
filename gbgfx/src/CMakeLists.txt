cmake_minimum_required(VERSION 3.10.0)

add_library(
	gbgfx
	STATIC
	color.h color.cpp
	image.h image.cpp
	palette.h palette.cpp
	tileset.h tileset.cpp
	utils.h utils.cpp)
target_compile_definitions(gbgfx PUBLIC _CRT_SECURE_NO_WARNINGS)

