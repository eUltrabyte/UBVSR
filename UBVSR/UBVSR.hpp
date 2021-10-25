﻿#pragma once

#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>
#include <map>
#include <unordered_map>
#include <set>

#include "Pixel.hpp"
#include "Vertex.hpp"
#include "Texture.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#if defined(_WIN32)
    #include "Window/WindowWin32.hpp"
#elif defined(__unix__)
    #include "Window/WindowX11.hpp"
#endif


namespace ubv {
    class Sandbox {
    public:
        explicit Sandbox(int t_argc, char** t_argv);
        virtual ~Sandbox() noexcept;

        virtual void start();

        Texture texture1;
        //Texture texture2;

        glm::mat4 projection;
        glm::mat4 view;
    };
};
