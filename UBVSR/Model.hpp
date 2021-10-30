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
				return strings;
			};
			auto strings = split_string(line);
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
				m_textures[current_texture] = std::make_unique<Texture>( current_texture + ".tga");
				std::cout << " Done!" << std::endl;
				
			}
			else if (strings[0] == "f")
			{
				std::cout << strings.size() << std::endl;
				std::array<Vertex, 3> face_verts;
				for (std::uint8_t i = 0; i < 3; ++i)
				{
					const auto elements = split_string(strings[i+1], '/');
					const fvec3 pos = vertices.at(std::stoull(elements[0]) - 1);
					const fvec2 uv = uvs.at(std::stoull(elements[1]) - 1);
					face_verts[i] = Vertex{ fvec4{pos, 1.0F}, uv };
				}
				m_triangles[current_texture].push_back(face_verts);
			}
		}
	}
	std::unordered_map<std::string, std::vector<std::array<Vertex, 3>>> m_triangles;
	std::unordered_map<std::string, std::unique_ptr<Texture>> m_textures;
};

}; // namespace ubv
