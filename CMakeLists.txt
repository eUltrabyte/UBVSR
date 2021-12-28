cmake_minimum_required (VERSION 3.17)
project("UBVSR")

set(CMAKE_CXX_STANDARD 20)
#set(CMAKE_BUILD_TYPE Release)

if(UNIX)
    find_package(X11 REQUIRED)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
endif(UNIX)

add_executable(UBVSR
    "UBVSR/UBVSR.cpp"
    "UBVSR/UBVSR.hpp"
    "UBVSR/Math.hpp"
    "UBVSR/Vertex.hpp"
    "UBVSR/Pixel.hpp"
    "UBVSR/TGA.hpp"
    "UBVSR/Texture.hpp"
    "UBVSR/Window.hpp"
    "UBVSR/Window/WindowWin32.cpp"
    "UBVSR/Window/WindowWin32.hpp"
    "UBVSR/Window/WindowX11.cpp"
    "UBVSR/Window/WindowX11.hpp"
    "UBVSR/Framebuffer.hpp"
    "UBVSR/FpsCounter.hpp"
    "UBVSR/Buffer.hpp"
    "UBVSR/Model.hpp"
 "UBVSR/TextRenderer.hpp"  "UBVSR/TextRenderer.cpp")

#enable LTO
include(CheckIPOSupported)
check_ipo_supported(RESULT supported OUTPUT error)

if (supported)
    message(STATUS "IPO / LTO enabled")
    set_property(TARGET UBVSR PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
else ()
    message(STATUS "IPO / LTO not supported: <${error}>")
endif ()

if (UNIX)
    target_link_libraries(UBVSR X11 Threads::Threads)
endif (UNIX)

file(COPY res/ DESTINATION res/)