cmake_minimum_required(VERSION 3.10)

# set the project name and version
project(chip8-emulator VERSION 0.1)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror -std=c++17")

# set the build type
set(CMAKE_BUILD_TYPE Release)

set(HOMEBREW_DIR /opt/homebrew)
include_directories(${HOMEBREW_DIR}/include)
link_directories(${HOMEBREW_DIR}/lib)

add_executable(emulator src/main.cxx src/emulator.cxx)
target_link_libraries(emulator SDL2)