cmake_minimum_required(VERSION 3.6)
project(manager)

set(SRC
        WFGlobal.cpp)

add_library(${PROJECT_NAME} OBJECT ${SRC})