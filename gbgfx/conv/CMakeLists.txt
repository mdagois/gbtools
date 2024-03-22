cmake_minimum_required(VERSION 3.20.0)

add_executable(
	conv
	conv.cpp
	options.h options.cpp)

target_compile_definitions(conv PUBLIC _CRT_SECURE_NO_WARNINGS)
target_compile_features(conv PRIVATE cxx_std_17)
target_link_libraries(conv gbgfx)
target_link_libraries(conv cli)

add_executable(
	readinfo
	readinfo.cpp)

target_compile_definitions(readinfo PUBLIC _CRT_SECURE_NO_WARNINGS)
target_compile_features(readinfo PRIVATE cxx_std_17)
target_link_libraries(readinfo gbgfx)

