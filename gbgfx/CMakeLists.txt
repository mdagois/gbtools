cmake_minimum_required(VERSION 3.10.0)
project(gbgfx)

add_library(
	gbgfx
	STATIC
	color.h color.cpp
	image.h image.cpp
	palette.h palette.cpp)
target_compile_definitions(gbgfx PUBLIC _CRT_SECURE_NO_WARNINGS)

add_executable(conv conv.cpp)
target_compile_definitions(conv PUBLIC _CRT_SECURE_NO_WARNINGS)
target_link_libraries(conv gbgfx)

