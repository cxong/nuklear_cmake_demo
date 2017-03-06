cmake_minimum_required(VERSION 2.8.2)
project(nuklear_sdl_gl3_cmake)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/build/cmake")
set(GLEW_LOCATION $ENV{SDLDIR})

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

include_directories(
  ${SDL2_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIRS} ${GLEW_INCLUDE_DIR})

set(SRC main.c nuklear.h nuklear_sdl_gl3.h)
add_executable(nuklear_sdl_gl3_cmake ${SRC})
target_link_libraries(
  nuklear_sdl_gl3_cmake ${SDL2_LIBRARY} ${OPENGL_LIBRARIES} ${GLEW_LIBRARY})
