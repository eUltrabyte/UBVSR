cmake_minimum_required (VERSION 3.17)
project("UBVSR")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Release)

add_executable(UBVSR
    "UBVSR/UBVSR.cpp"
    "UBVSR/UBVSR.hpp"
    "UBVSR/Window.hpp"
    "UBVSR/Window/WindowWin32.cpp"
    "UBVSR/Window/WindowWin32.hpp"
    "UBVSR/Window/WindowX11.cpp"
    "UBVSR/Window/WindowX11.hpp"
    "UBVSR/Framebuffer.hpp"
    "UBVSR/FpsCounter.hpp"
)