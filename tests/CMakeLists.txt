project(test)

add_executable(logstream_test logstream_test.cpp)
target_link_libraries(logstream_test PUBLIC su)

add_executable(BlockQueueTest BlockQueue_test.cpp thread_task_test.cpp exeuctor_test.cpp timestamp_test.cpp)
target_link_libraries(BlockQueueTest PUBLIC su)

add_executable(thrdpool_test thrdpool_test.cpp)
target_link_libraries(thrdpool_test PUBLIC su)

add_executable(msgqueue_test msgqueue_test.cpp)
target_link_libraries(msgqueue_test PUBLIC su)

add_executable(workflow_test workflow_test.cpp)
target_link_libraries(workflow_test PUBLIC su)

add_executable(thread_task_test thread_task_test.cpp)
target_link_libraries(thread_task_test PUBLIC su)

add_executable(exeuctor_test exeuctor_test.cpp)
target_link_libraries(exeuctor_test PUBLIC su)

add_subdirectory(${PROJECT_SOURCE_DIR}/Zipper)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)