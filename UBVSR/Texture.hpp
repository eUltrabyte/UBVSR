#pragma once

#include "TGA.hpp"

#include <cmath>

namespace ubv {
	class Texture {
	public:
		enum class FilteringType : std::uint8_t {
			NEAREST,
			LINEAR,
            BILINEAR
		} m_filtering_type;

		bool clamp_x = false;
		bool clamp_y = false;

		Pixel sample(fvec2 pos_uv) const noexcept {
			pos_uv.x = std::fmod(pos_uv.x, 1.0F);
			pos_uv.y = std::fmod(pos_uv.y, 1.0F);
			switch (m_filtering_type) {
				case FilteringType::NEAREST:
				{
					std::uint16_t x_texture_position = std::clamp<std::uint16_t>(std::uint16_t(pos_uv.x * get_size().x), 0, get_size().x - 1);
					std::uint16_t y_texture_position = std::clamp<std::uint16_t>(std::uint16_t(pos_uv.y * get_size().y), 0, get_size().y - 1);
					return m_tga.get_pixel(ubv::u16vec2{x_texture_position, y_texture_position});
				}
				break;
				case FilteringType::LINEAR:
				{
					std::abort(); // no i chuj
                    return Pixel{ };
				}
				break;
                case FilteringType::BILINEAR:
				{
					std::abort(); // no i chuj v2
                    return Pixel{ };
				}
				break;
			}
            return Pixel{ };
		}

		inline Texture(std::string_view t_tga_filename, FilteringType t_filtering_type = FilteringType::LINEAR) 
		    : m_tga{ t_tga_filename }, m_filtering_type{ t_filtering_type } {
			// cos
		}
     
		constexpr u16vec2 get_size() const noexcept {
			return u16vec2((uint16_t)m_tga.get_width(), (uint16_t)m_tga.get_height());
		}

	private:
		TGA m_tga;

	};
};