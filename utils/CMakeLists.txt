cmake_minimum_required(VERSION 3.2)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -pedantic -std=gnu99 -Werror -Wmissing-prototypes -Wstrict-prototypes -Wno-unused-parameter -Wno-unused-result -Wno-char-subscripts")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -save-temps -D_DEBUG -DNDEBUG")
add_executable(bmpddsbmp main.c bmp.c dds.c readutils.c converter.c)
