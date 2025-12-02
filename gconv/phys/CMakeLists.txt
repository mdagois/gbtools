cmake_minimum_required(VERSION 3.10.0)
project(clitools)

############################################################
# gphys
############################################################

set(GPHYS_BIN_NAME gphys)

add_executable(
	${GPHYS_BIN_NAME}
	gphys.cpp
	options.h options.cpp)

target_compile_definitions(${GPHYS_BIN_NAME} PUBLIC _CRT_SECURE_NO_WARNINGS)
target_compile_features(${GPHYS_BIN_NAME} PRIVATE cxx_std_17)
target_link_libraries(${GPHYS_BIN_NAME} ${CLI_LIB_NAME})
target_link_libraries(${GPHYS_BIN_NAME} ${GFX_LIB_NAME})

