#pragma once

#include "Pixel.hpp"
#include "Math.hpp"

namespace ubv {

	//Vertex for color

	/*struct Vertex {
		fvec2 position;
		Pixel color;
		constexpr explicit Vertex(fvec2 t_position, Pixel t_color) : position{ t_position }, color{ t_color } { }
	};*/

	//Vertex for texture

	
	struct Vertex {
		fvec4 position;
		fvec2 texture_uv;
		[[nodiscard]] constexpr Vertex interpolate(const Vertex &t_vertex, float t_fraction) const noexcept
		{
			Vertex vertex;
			vertex.position = position + (t_vertex.position - position) * t_fraction;
			vertex.texture_uv = texture_uv + (t_vertex.texture_uv - texture_uv) * t_fraction;
			return vertex;
		}
	};
}