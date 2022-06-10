cmake_minimum_required(VERSION 3.22)
project(Su)

set(CMAKE_CXX_STANDARD 20)

set(ALL_SRCS ${LIB_SRCS})

aux_source_directory(${PROJECT_SOURCE_DIR}/Log/ LIB_SRCS)
aux_source_directory(${PROJECT_SOURCE_DIR}/utils/ LIB_SRCS)
aux_source_directory(${PROJECT_SOURCE_DIR}/Thread/ LIB_SRCS)
if(UNIX)
    aux_source_directory(${PROJECT_SOURCE_DIR}/Net/ LIB_SRCS)
    aux_source_directory(${PROJECT_SOURCE_DIR}/kernel/ LIB_SRCS)
endif()


add_library(su OBJECT ${LIB_SRCS})

include_directories(${PROJECT_SOURCE_DIR})

add_executable(test main.cpp)
target_link_libraries(test PUBLIC su)

add_subdirectory(${PROJECT_SOURCE_DIR}/tests)