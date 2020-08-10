cmake_minimum_required(VERSION 3.11)

project(chip8vm)

set(CMAKE_CXX_STANDART 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -O3")
set(SFML_LIBS sfml-system sfml-graphics sfml-window sfml-audio)

include_directories(include)

set(SOURCE_FILES
    ./src/main.cpp
    ./src/Chip8.cpp
    ./src/Graphics.cpp
)

find_package(SFML 2 REQUIRED COMPONENTS system graphics window audio)
if (SFML_FOUND)
    add_executable(${CMAKE_PROJECT_NAME} ${SOURCE_FILES})
    target_link_libraries(${CMAKE_PROJECT_NAME} ${SFML_LIBS})
else()
    message("\n===DEPENDENCY IS NOT SATISFIED===\nSFML library is not found! Install SFML library to build the project.\n")
endif()


