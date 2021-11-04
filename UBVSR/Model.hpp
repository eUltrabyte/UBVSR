#pragma once

#include "Math.hpp"
#include "Texture.hpp"
#include "Vertex.hpp"

#include <array>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>
#include <memory>

namespace ubv
{
class Model
{
  public:
	fmat4x4 model_matrix = identity<float>();

	inline explicit Model(std::string_view t_filename) noexcept
	{
		std::vector<fvec3> vertices;
		std::vector<fvec2> uvs;
		std::string line;
		std::string current_texture;
		std::ifstream file(t_filename.data());
		while (std::getline(file, line))
		{
			static auto split_string = [](const std::string &t_string, char t_separator = ' ') {
				if (t_string.empty())
				{
					return std::vector<std::string>{};
				}
				std::vector<std::string> strings{1};
				for (const char c : t_string)
				{
					if (c == t_separator)
					{
						if (!strings.back().empty())
						{
							strings.push_back({});
						}
					}
					else
					{
						strings.back() += c;
					}
				}
				if (!strings.empty() && strings.back().empty())
				{
					strings.pop_back();
				}
				return strings;
			};
			const auto strings = split_string(line);
			//for (const auto& string_element : strings)
			//{
				//std::cout << string_element << ' ';
			//}
			//std::cout << std::endl;
			if (strings.empty())
			{
				continue;
			}
			if (strings[0] == "v")
			{
				vertices.push_back(fvec3{std::stof(strings[1]),std::stof(strings[2]),std::stof(strings[3]) });
			}
			else if (strings[0] == "vt")
			{
				uvs.push_back(fvec2{ std::stof(strings[1]),std::stof(strings[2]) });
			}
			else if (strings[0] == "usemtl")
			{
				current_texture = strings.at(1);
				std::cout << "Changing texture to: " << current_texture << "..." << std::flush;
				m_textures[current_texture] = std::make_unique<Texture>( "res/" + current_texture + ".tga", Texture::FilteringType::BILINEAR);
				std::cout << " Done!" << std::endl;
				
			}
			else if (strings[0] == "f")
			{
				std::array<Vertex, 3> face_verts;
				const int faces_n = int(strings.size()) - 1;
				const auto add_faces_to_vector = [&](std::array<std::string, 3> t_elements) {
					for (std::uint8_t i = 0; i < 3; ++i)
					{

						const auto elements = split_string(t_elements[i], '/');

						const fvec3 pos = vertices.at(std::stoull(elements[0]) - 1);

						const fvec2 uv = uvs.at(std::stoull(elements[1]) - 1);

						face_verts[i] = Vertex{ fvec4{pos, 1.0F}, uv };
					}

					m_triangles[current_texture].push_back(face_verts);
				};
				if (faces_n < 3)
				{
					std::cout << "not a triangle" << std::endl;
					std::abort();
				}
				else if (faces_n == 3)
				{
					add_faces_to_vector({ strings[1], strings[2], strings[3] });
				}
				else if (faces_n > 3)
				{
					std::cout << "clipping a polygon with " << faces_n << " vertices." << std::endl;

					add_faces_to_vector({ strings[1], strings[2], strings[3] });

					add_faces_to_vector({ strings[faces_n], strings[faces_n-1], strings[1] });

					for (unsigned i = 0; i < faces_n - 4; ++i)
					{

						add_faces_to_vector({ strings[1], strings[i+3], strings[i+4] });
					}
				}
				/*for (std::uint8_t i = 0; i < 3; ++i)
				{
					const auto elements = split_string(strings[i+1], '/');
					const fvec3 pos = vertices.at(std::stoull(elements[0]) - 1);
					const fvec2 uv = uvs.at(std::stoull(elements[1]) - 1);
					face_verts[i] = Vertex{ fvec4{pos, 1.0F}, uv };
				}*/
				
			}
		}
	}
	std::unordered_map<std::string, std::vector<std::array<Vertex, 3>>> m_triangles;
	std::unordered_map<std::string, std::unique_ptr<Texture>> m_textures;
};

}; // namespace ubv
