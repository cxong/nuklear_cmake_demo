cmake_minimum_required(VERSION 2.8.2)
project(nuklear_sdl_gl2_cmake)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/build/cmake")

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(OpenGL REQUIRED)

include_directories(
  ${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIR} ${OPENGL_INCLUDE_DIRS})

set(SRC main.c nuklear.h nuklear_sdl_gl2.h)
add_executable(nuklear_sdl_gl2_cmake ${SRC})
target_link_libraries(
	nuklear_sdl_gl2_cmake
	${SDL2_LIBRARY} ${SDL2_IMAGE_LIBRARY} ${OPENGL_LIBRARIES})
