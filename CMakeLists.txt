cmake_minimum_required(VERSION 3.2)


set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(glfw)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -pedantic -std=gnu99 -Werror -Wmissing-prototypes -Wstrict-prototypes -Wno-unused-parameter -Wno-unused-result")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -save-temps -D_DEBUG -DNDEBUG")
add_subdirectory(renderer)
add_subdirectory(examples)
