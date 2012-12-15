
cmake_minimum_required(VERSION 2.8)

project(lasty_ld25_villain)

set(CMAKE_CXX_FLAGS "-Wall -std=gnu++11 -static-libgcc -static-libstdc++")

set(SOURCES main.cpp os_utils.cpp vertex.cpp program.cpp camera.cpp prim.cpp image.cpp obj.cpp game.cpp)


##SDL2

set(SDL_ATOMIC OFF CACHE BOOL "")
set(SDL_RENDER OFF CACHE BOOL "")
set(SDL_POWER OFF CACHE BOOL "")
set(SDL_THREADS OFF CACHE BOOL "")
set(SDL_FILE OFF CACHE BOOL "")
set(SDL_CPUINFO OFF CACHE BOOL "")
set(SDL_HAPTIC OFF CACHE BOOL "")
set(SDL_JOYSTICK OFF CACHE BOOL "")
set(ENV{DXSDK_DIR} /)  #msys/mingw has these already
set(SDL_SHARED OFF CACHE BOOL "")  #dont build a dll, i will link static
set(SDL_INCLUDE libs/SDL/include)

add_subdirectory(libs/SDL)

include_directories(${SDL_INCLUDE})



## Opengl and GLee

find_package(OpenGL)

add_subdirectory(libs/glee)
include_directories(libs/glee/include)

## glm maths library

include_directories(libs/glm-0.9.4.0)


## webp library

include_directories(libs/libwebp-0.2.0/src/webp)
SET(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} ${CMAKE_CURRENT_LIST_DIR}/libs/libwebp-0.2.0/src/.libs)

find_library(WEBP_LIBRARY NAMES webp)
message(${CMAKE_LIBRARY_PATH})

## main target

add_executable(lasty_ld25_villain ${SOURCES})

target_link_libraries(lasty_ld25_villain SDL2-static libgleeStatic ${OPENGL_LIBRARY} ${WEBP_LIBRARY})

