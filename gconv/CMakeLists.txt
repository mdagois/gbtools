cmake_minimum_required(VERSION 3.10.0)
project(clitools)

############################################################
# libraries
############################################################

add_subdirectory(gfx)
add_subdirectory(cli)
include_directories(${CLI_INCLUDE_DIR} ${GFX_INCLUDE_DIR})
add_subdirectory(conv)
add_subdirectory(phys)

