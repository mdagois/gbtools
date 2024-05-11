cmake_minimum_required(VERSION 3.10.0)
project(gbgfx)

set(GB_LIB_NAME gbgfx)
set(CLI_LIB_NAME cli)

include_directories(cli lib)
add_subdirectory(lib)
add_subdirectory(cli)
add_subdirectory(conv)

