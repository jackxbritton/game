cmake_minimum_required(VERSION 3.5.1)

include_directories(src /usr/include/freetype2)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_C_FLAGS "-g -Wall")

set(BIN game)

project(${BIN})
add_executable(${BIN} src/main.c
                      src/misc.c
                      src/window.c
                      src/shader_program.c
                      src/texture.c
                      src/catalog.c
                      src/font.c
                      src/text.c
                      src/draw.c)
target_link_libraries(${BIN} m SDL2 GL GLEW freetype harfbuzz)
