cmake_minimum_required(VERSION 3.12)
project(uibasic C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake_modules)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

find_package(SDL2TTF REQUIRED)
include_directories(${SDL2TTF_INCLUDE_DIRS})

add_executable(uibasic main.c)
target_link_libraries(uibasic ${SDL2_LIBRARIES} ${SDL2TTF_LIBRARY})