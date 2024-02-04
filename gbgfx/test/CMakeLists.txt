cmake_minimum_required(VERSION 3.10.0)

add_executable(conv conv.cpp)
target_compile_definitions(conv PUBLIC _CRT_SECURE_NO_WARNINGS)
target_link_libraries(conv gbgfx)

