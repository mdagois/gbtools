cmake_minimum_required(VERSION 3.20.0)

set(EXPORT_SRC
	export.h
	palette.h palette.cpp
	tilemap.h tilemap.cpp
	tileset.h tileset.cpp
)
list(TRANSFORM EXPORT_SRC PREPEND export/)

set(IMPORT_SRC
	import.h
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

add_library(
	gbgfx
	STATIC
	${EXPORT_SRC}
	${IMPORT_SRC}
	${DATA_SRC}
	${UTILS_SRC}
	features.h features.cpp
	gbgfx.h gbgfx.cpp
	enums.h
)

target_compile_definitions(gbgfx PUBLIC _CRT_SECURE_NO_WARNINGS)
target_compile_features(gbgfx PRIVATE cxx_std_17)

