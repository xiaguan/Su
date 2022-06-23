cmake_minimum_required(VERSION 3.6)
project(factory)

set(SRC
        Task.h
        Task.cpp
        Workflow.h)

add_library(${PROJECT_NAME} OBJECT ${SRC})