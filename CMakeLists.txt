cmake_minimum_required(VERSION 3.16)
project(AstarVisual)

set(CMAKE_CXX_STANDARD 17)

add_executable(AstarVisual main.cpp)

target_link_libraries(${CMAKE_PROJECT_NAME} ${SDL2_LIBRARY} -lSDL2 -lOpenGL)