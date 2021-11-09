#pragma once

#include "Framebuffer.hpp"
#include "Texture.hpp"
#include "Vertex.hpp"

#include <string>
#include <string_view>

namespace ubv
{

class TextRenderer
{
  public:
    inline TextRenderer() : m_bitmap_chars{"res/charmap.tga", Texture::FilteringType::NEAREST}
    {
    }

    static inline constexpr u32vec2 char_size{7, 9};
    static inline constexpr u32vec2 charmap_size{126, 54};
    static inline constexpr u32vec2 chars_number{charmap_size.x / char_size.x, charmap_size.y / char_size.y};
    static inline constexpr fvec2 chars_size_uv{1.0F / float(chars_number.x), 1.0F / float(chars_number.y)};
    // static inline constexpr fvec2 char_size_uv{ 7.0F / 126.0F, 9.0F / 54.0F};

    inline void RenderTextLine(FrameBuffer &t_frame_buffer, std::string_view t_text, unsigned t_x_offset,
                               unsigned t_y_offset) const
    {
        // draw invidual chars
        const auto starting_x = float(t_x_offset) / float(t_frame_buffer.get_width()) * 2.0F - 1.0F;
        const auto starting_y = - (float(t_y_offset) / float(t_frame_buffer.get_height()) * 2.0F - 1.0F);

        const auto char_offset_x = float(char_size.x) / float(t_frame_buffer.get_width()) * 4.0F;
        const auto char_offset_y = float(char_size.y) / float(t_frame_buffer.get_height()) * 4.0F;

        for (unsigned i = 0; i < t_text.size(); ++i)
        {
            const char c = t_text[i];

            // const float ndc_x_pos = float(x_pos) * t_frame_buffer.get_width();

            if (c >= ' '+1 && c <= '~')
            {
                // calculate char position
                const unsigned char_index = c - ' ';

                const unsigned x_pos = char_index % chars_number.x;
                const unsigned y_pos = char_index / chars_number.x;

                const Vertex vertex_a{fvec4(starting_x + char_offset_x * float(i), starting_y, 0.1F, 1.0F),
                                      fvec2(chars_size_uv.x * float(x_pos), chars_size_uv.y * -float(y_pos))};
                const Vertex vertex_b{
                    fvec4(starting_x + char_offset_x + char_offset_x * float(i), starting_y, 0.1F, 1.0F),
                    fvec2(chars_size_uv.x * float(x_pos) + chars_size_uv.x, chars_size_uv.y * -float(y_pos))};
                const Vertex vertex_c{
                    fvec4(starting_x + char_offset_x + char_offset_x * float(i), starting_y - char_offset_y, 0.1F, 1.0F),
                    fvec2(chars_size_uv.x * float(x_pos) + chars_size_uv.x,
                          chars_size_uv.y * -float(y_pos) - chars_size_uv.y)};
                const Vertex vertex_d{
                    fvec4(starting_x +char_offset_x * float(i), starting_y - char_offset_y, 0.1F, 1.0F),
                    fvec2(chars_size_uv.x * float(x_pos), chars_size_uv.y * -float(y_pos) - chars_size_uv.y)};
                const std::array<Vertex, 3> triangle_a{vertex_a, vertex_b, vertex_c};
                const std::array<Vertex, 3> triangle_b{vertex_a, vertex_d, vertex_c};

                t_frame_buffer.prepare_triangle(triangle_a, m_bitmap_chars);
                t_frame_buffer.prepare_triangle(triangle_b, m_bitmap_chars);
            }
        }
    }

  private:
    Texture m_bitmap_chars;
};

} // namespace ubv
