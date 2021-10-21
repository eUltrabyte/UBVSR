#pragma once

#include <cstdint>
#include <future>
#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>
#include <map>
#include <unordered_map>
#include <set>

#if defined(_WIN32)
#include "Window/WindowWin32.hpp"
#elif defined(__unix__)
#include "Window/WindowX11.hpp"
#endif
