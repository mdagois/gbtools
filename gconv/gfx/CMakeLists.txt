cmake_minimum_required(VERSION 3.20.0)

############################################################
# sources
############################################################

set(EXPORT_SRC
	export.h
	capabilities.h capabilities.cpp
	color.h color.cpp
	enums.h
	palette.h palette.cpp
	tilemap.h tilemap.cpp
	tileset.h tileset.cpp
)
list(TRANSFORM EXPORT_SRC PREPEND export/)

set(IMPORT_SRC
	import.h
	division.h division.cpp
	image.h image.cpp
)
list(TRANSFORM IMPORT_SRC PREPEND import/)

set(DATA_SRC
	data.h
	color.h color.cpp
	palette.h palette.cpp
	tilemap.h tilemap.cpp
	tileset.h tileset.cpp
)
list(TRANSFORM DATA_SRC PREPEND data/)

set(UTILS_SRC
	utils.h
	log.h log.cpp
)
list(TRANSFORM UTILS_SRC PREPEND utils/)

set(THIRD_PARTY_SRC
	third_party.h
	stb_image.h stb_image.cpp
	stb_image_write.h stb_image_write.cpp
)
list(TRANSFORM THIRD_PARTY_SRC PREPEND third_party/)

############################################################
# library
############################################################

set(LIB_NAME gfx)
set(GFX_LIB_NAME ${LIB_NAME} PARENT_SCOPE)

add_library(
	${LIB_NAME}
	STATIC
	${EXPORT_SRC}
	${IMPORT_SRC}
	${DATA_SRC}
	${UTILS_SRC}
	${THIRD_PARTY_SRC}
	gfx.h gfx.cpp
)

target_compile_definitions(${LIB_NAME} PUBLIC _CRT_SECURE_NO_WARNINGS)
target_compile_features(${LIB_NAME} PRIVATE cxx_std_17)

############################################################
# include directory
############################################################

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
set(GFX_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)

