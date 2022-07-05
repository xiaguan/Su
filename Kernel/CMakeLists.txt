cmake_minimum_required(VERSION 3.6)
project(kernel)

set(SRC
        thrdpool.cpp
        SubTask.cpp
        msgqueue.cpp
        Executor.cpp
        )

add_library(${PROJECT_NAME} OBJECT ${SRC})