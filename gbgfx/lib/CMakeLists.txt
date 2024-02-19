cmake_minimum_required(VERSION 3.10.0)

add_library(
	gbgfx
	STATIC
	api.h api.cpp
	color.h color.cpp
	export.h export.cpp
	image.h image.cpp
	log.h log.cpp
	palette.h palette.cpp
	profile.h profile.cpp
	tilemap.h tilemap.cpp
	tileset.h tileset.cpp)
target_compile_definitions(gbgfx PUBLIC _CRT_SECURE_NO_WARNINGS)
target_compile_features(gbgfx PRIVATE cxx_std_17)

