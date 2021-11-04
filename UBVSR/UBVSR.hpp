#pragma once

#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>
#include <map>
#include <unordered_map>
#include <set>

#include "Pixel.hpp"
#include "Model.hpp"

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
        Texture skybox1, skybox2, skybox3, skybox4, skybox_top, skybox_bottom;
        Model model_akwarium;
        Model model_rekin;
        //Texture texture2;
        Model model_dust2;
        fmat4x4 projection;
    };

};
