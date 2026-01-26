cmake_minimum_required(VERSION 3.10.0)
project(clitools)

############################################################
# libraries
############################################################

add_subdirectory(lib)
include_directories(${CONV_LIB_INCLUDE_DIR})
add_subdirectory(gfx)
add_subdirectory(phys)

