
cmake_minimum_required(VERSION 2.8)

project(lasty_ld25_villain)

set(CMAKE_CXX_FLAGS "-Wall -std=gnu++11 -static-libgcc -static-libstdc++")

set(SOURCES main.cpp os_utils.cpp)


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




## main target

add_executable(lasty_ld25_villain ${SOURCES})

target_link_libraries(lasty_ld25_villain SDL2-static libgleeStatic ${OPENGL_LIBRARY})

