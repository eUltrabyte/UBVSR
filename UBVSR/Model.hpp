#pragma once

#include "Vertex.hpp"
#include "Math.hpp"
#include "Texture.hpp"

#include <string_view>
#include <vector>
#include <array>
#include <iostream>
#include <string>
#include <sstream>


namespace ubv
{
    class Model
    {
    public:
        inline explicit Model(std::string_view t_filename) noexcept
        {
            std::string line;
            std::ifstream file(std::string(t_filename).c_str());
            if(!file.is_open()) {
                std::cout << "chuj dupa cycki\n";
                std::abort();
            }

            while(std::getline(file, line))
            {
                if (line.starts_with("vn"))
                {
                    // TODO
                }
                else if (line.starts_with("vt"))
                {
                    // TODO
                }
                else if (line.starts_with("usemtl"))
                {

                }
                else if (line.starts_with("v"))
                {
                    std::stringstream file(line);
                    char c;
                    file >> c;
                    fvec3 vec;
                    file >> vec.x >> vec.y >> vec.z;
                    std::cout << "vertex: " << vec.x << ", " << vec.y << ", " << vec.z << std::endl;
                    m_vertices.push_back(vec);
                }
                else if (line.starts_with("f"))
                {
                    std::stringstream file(line);
                    char c;
                    file >> c;
                    unsigned trash;
                    u32vec3 face;
                    file >> face.x >> c >> trash >> c >> trash;
                    file >> face.y >> c >> trash >> c >> trash;
                    file >> face.z >> c >> trash >> c >> trash;
                    //std::cout << "face: " << face.x << ", " << face.y << ", " << face.z << std::endl;
                    m_faces.push_back(face);
                }
            }

            std::cout << "vertices size: " << m_vertices.size() << "\n";
            std::cout << "faces size: " << m_faces.size() << "\n";

            for(auto i = 0; i < m_faces.size(); ++i) {
                const fvec3 v0 = m_vertices[m_faces[i].x - 1];
                const fvec3 v1 = m_vertices[m_faces[i].y - 1];
                const fvec3 v2 = m_vertices[m_faces[i].z - 1];

                m_triangles.push_back({ Vertex{ fvec4{ v0, 1.0F }, fvec2{ 0, 0 }  }, Vertex{ fvec4{ v1, 1.0F }, fvec2{ 0, 0 } }, Vertex{ fvec4{ v2, 1.0F }, fvec2{ 0, 0 } } });
            }

        }

        std::vector<fvec3> m_vertices;
        std::vector<u32vec3> m_faces;

        std::vector<std::array<Vertex, 3>> m_triangles;
        std::vector<Texture> m_textures;

    };

};