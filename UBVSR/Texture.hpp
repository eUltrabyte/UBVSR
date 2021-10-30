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
					const float section_x = 1.0F / static_cast<float>(get_size().x);
					const float section_y = 1.0F / static_cast<float>(get_size().y);
					const float half_section_x = section_x / 2.0F;
					const float half_section_y = section_y / 2.0F;
					const float pos_x_mod = std::fmod(pos_uv.x, section_x);
					const float pos_y_mod = std::fmod(pos_uv.x, section_y);
					const float fraction_x = std::abs(pos_x_mod - half_section_x) / section_x;
					const float fraction_y = std::abs(pos_y_mod - half_section_y) / section_y;
					int x_texture_position = std::clamp<std::uint16_t>(std::uint16_t(pos_uv.x * get_size().x), 0, get_size().x - 1);
					int y_texture_position = std::clamp<std::uint16_t>(std::uint16_t(pos_uv.y * get_size().y), 0, get_size().y - 1);
					int x_texture_position2 = x_texture_position;
					int y_texture_position2 = y_texture_position;
					if (pos_x_mod > half_section_x)
					{
						x_texture_position2 = ((x_texture_position2) + 1) % int(get_size().x);
					}
					else
					{
						x_texture_position2 = ((x_texture_position2) - 1) % int(get_size().x);
					}
					if (pos_y_mod > half_section_y)
					{
						y_texture_position2 = ((y_texture_position2) + 1) % int(get_size().y);
					}
					else
					{
						y_texture_position2 = ((y_texture_position2) - 1) % int(get_size().y);
					}
					const auto pixel11 = m_tga.get_pixel(ubv::u16vec2( x_texture_position, y_texture_position ));
					const auto pixel21 = m_tga.get_pixel(ubv::u16vec2( x_texture_position2, y_texture_position ));
					const auto pixel12 = m_tga.get_pixel(ubv::u16vec2( x_texture_position, y_texture_position2 ));
					const auto pixel22 = m_tga.get_pixel(ubv::u16vec2( x_texture_position2, y_texture_position2 ));

					//TODO: INTERPOLATE
					const auto interpolated_pixel_r1 = std::lerp(pixel11.r, pixel21.r, fraction_x);
					const auto interpolated_pixel_r2 = std::lerp(pixel12.r, pixel22.r, fraction_x);
					const auto interpolated_pixel_r = std::lerp(interpolated_pixel_r1, interpolated_pixel_r2, fraction_y);

					const auto interpolated_pixel_g1 = std::lerp(pixel11.g, pixel21.g, fraction_x);
					const auto interpolated_pixel_g2 = std::lerp(pixel12.g, pixel22.g, fraction_x);
					const auto interpolated_pixel_g = std::lerp(interpolated_pixel_g1, interpolated_pixel_g2, fraction_y);

					const auto interpolated_pixel_b1 = std::lerp(pixel11.b, pixel21.b, fraction_x);
					const auto interpolated_pixel_b2 = std::lerp(pixel12.b, pixel22.b, fraction_x);
					const auto interpolated_pixel_b = std::lerp(interpolated_pixel_b1, interpolated_pixel_b2, fraction_y);

					//std::abort(); // no i chuj
					return Pixel( interpolated_pixel_r, interpolated_pixel_g, interpolated_pixel_b );
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
			return u16vec2(static_cast<uint16_t>(m_tga.get_width()), static_cast<uint16_t>(m_tga.get_height()));
		}

	private:
		TGA m_tga;

	};
};