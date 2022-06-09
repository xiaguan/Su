project(Zipper)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

add_executable(zipper main.cpp Encoder.cpp)
target_link_libraries(zipper PUBLIC su)