cmake_minimum_required(VERSION 3.10)

# set the project name and version
project(chip8-emulator VERSION 0.1)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -std=c++17")

# set the build type
set(CMAKE_BUILD_TYPE Release)

include_directories(/opt/homebrew/include)
link_directories(/opt/homebrew/lib)

add_executable(emulator src/main.cxx src/launcher.cxx src/chip8.cxx)
target_link_libraries(emulator SDL2)