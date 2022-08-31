cmake_minimum_required(VERSION 3.10)

project(chip8-emulator VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -O3")
set(CMAKE_BUILD_TYPE Release)

include_directories(/opt/homebrew/include)
link_directories(/opt/homebrew/lib)

file(GLOB SOURCES src/*.cpp)

add_executable(emulator ${SOURCES})
target_link_libraries(emulator SDL2)