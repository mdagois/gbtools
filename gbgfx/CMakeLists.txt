cmake_minimum_required(VERSION 3.10.0)
project(gbgfx)

include_directories(cli lib)
add_subdirectory(lib)
add_subdirectory(cli)
add_subdirectory(conv)

