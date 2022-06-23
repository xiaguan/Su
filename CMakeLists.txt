cmake_minimum_required(VERSION 3.22)
project(su)

include_directories(${PROJECT_SOURCE_DIR})

set(CMAKE_CXX_STANDARD 20)

add_custom_target(
        LINK_HEADERS ALL
)

include(CMakeLists_Headers.txt)

add_subdirectory(Kernel)
add_subdirectory(Factory)

add_dependencies(kernel LINK_HEADERS)
add_dependencies(factory LINK_HEADERS)

aux_source_directory(${PROJECT_SOURCE_DIR}/Log/ LIB_SRCS)
aux_source_directory(${PROJECT_SOURCE_DIR}/utils/ LIB_SRCS)
aux_source_directory(${PROJECT_SOURCE_DIR}/Thread/ LIB_SRCS)

add_library(${PROJECT_NAME} OBJECT ${LIB_SRCS})

add_executable(test main.cpp)
target_link_libraries(test PUBLIC ${PROJECT_NAME})

add_subdirectory(${PROJECT_SOURCE_DIR}/tests)