cmake_minimum_required(VERSION 3.2)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -pedantic -std=gnu99 -Werror -Wmissing-prototypes -Wstrict-prototypes -Wno-unused-parameter")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -save-temps -D_DEBUG -DNDEBUG -fsanitize=address -fsanitize=undefined -fsanitize=float-divide-by-zero -fno-sanitize-recover")

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(glfw)
add_subdirectory(examples)
