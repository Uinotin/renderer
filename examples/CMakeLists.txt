cmake_minimum_required(VERSION 3.2)
add_executable(example main.c)
target_link_libraries(example glfw)
#target_include_directories(example PRIVATE ../renderer)
