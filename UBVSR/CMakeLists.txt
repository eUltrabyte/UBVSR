cmake_minimum_required (VERSION 3.17)

set(CMAKE_CXX_STANDARD 20)

add_executable (UBVSR "UBVSR.cpp" "UBVSR.hpp" "Window.hpp" "Window.cpp" "Window/WindowWin32.cpp" "Window/WindowWin32.hpp" "FpsCounter.hpp")