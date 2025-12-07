cmake_minimum_required(VERSION 3.31)
project(data_generator)

add_subdirectory(src)

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/data)
