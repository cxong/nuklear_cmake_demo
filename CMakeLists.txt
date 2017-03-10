cmake_minimum_required(VERSION 2.8.2)
project(nuklear_sdl_gl2_cmake)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/build/cmake")
set(GLEW_LOCATION $ENV{SDLDIR})

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIRS})

set(SRC main.c nuklear.h nuklear_sdl_gl2.h)
add_executable(nuklear_sdl_gl2_cmake ${SRC})
target_link_libraries(nuklear_sdl_gl2_cmake ${SDL2_LIBRARY} ${OPENGL_LIBRARIES})
