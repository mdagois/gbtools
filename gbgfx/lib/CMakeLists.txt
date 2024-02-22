cmake_minimum_required(VERSION 3.10.0)

set(EXPORT_SRC
	export.h export.cpp
)
list(TRANSFORM EXPORT_SRC PREPEND export/)

set(IMPORT_SRC
	image.h image.cpp
)
list(TRANSFORM IMPORT_SRC PREPEND import/)

set(MODEL_SRC 
	color.h color.cpp
	palette.h palette.cpp
	tilemap.h tilemap.cpp
	tileset.h tileset.cpp
)
list(TRANSFORM MODEL_SRC PREPEND model/)

set(UTILS_SRC log.h log.cpp)
list(TRANSFORM UTILS_SRC PREPEND utils/)

add_library(
	gbgfx
	STATIC
#	${EXPORT_SRC}
	${IMPORT_SRC}
	${MODEL_SRC}
	${UTILS_SRC}
#	api.h api.cpp
	features.h features.cpp
)

target_compile_definitions(gbgfx PUBLIC _CRT_SECURE_NO_WARNINGS)
target_compile_features(gbgfx PRIVATE cxx_std_17)

