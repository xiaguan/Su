cmake_minimum_required(VERSION 3.22)
project(Su)

set(CMAKE_CXX_STANDARD 20)

set(ALL_SRCS ${LIB_SRCS})
aux_source_directory(${PROJECT_SOURCE_DIR}/Log/ LIB_SRCS)

add_executable(untitled1 main.cpp ${LIB_SRCS})
