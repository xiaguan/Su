
add_executable(logstream_test logstream_test.cpp)
target_link_libraries(logstream_test PUBLIC su)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)