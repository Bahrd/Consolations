# CMakeList.txt : CMake project for Barricade, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)
set(CMAKE_CXX_FLAGS "/utf-8")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE debug)
# Add source to this project's executable.
add_executable (Barricade "Barricade.cpp")

# TODO: Add tests and install targets if needed.
