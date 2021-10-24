#pragma once

#include "Pixel.hpp"
#include "Math.hpp"

namespace ubv {

    //Vertex for color

    struct Vertex {
    	fvec2 position;
    	Pixel color;
    	constexpr explicit Vertex(fvec2 t_position, Pixel t_color) : position{ t_position }, color{ t_color } { }
    };

    //Vertex for texture

    /*
    struct Vertex
    {
    	fvec2 position;
    	fvec2 texture_uv;
    }
    */
};