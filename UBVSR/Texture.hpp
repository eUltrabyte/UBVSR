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
			if (pos_uv.x < 0.0F)
				pos_uv.x += 1.0F;
			if (pos_uv.y < 0.0F)
				pos_uv.y += 1.0F;
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
						x_texture_position2 = ((x_texture_position2) + 1) % unsigned(get_size().x);
					}
					else
					{
						x_texture_position2 = ((x_texture_position2) - 1) % unsigned(get_size().x);
					}
					if (pos_y_mod > half_section_y)
					{
						y_texture_position2 = ((y_texture_position2) + 1) % unsigned(get_size().y);
					}
					else
					{
						y_texture_position2 = ((y_texture_position2) - 1) % unsigned(get_size().y);
					}
					const auto pixel11 = m_tga.get_pixel(ubv::u16vec2( x_texture_position, y_texture_position ));
					const auto pixel21 = m_tga.get_pixel(ubv::u16vec2( x_texture_position2, y_texture_position ));
					const auto pixel12 = m_tga.get_pixel(ubv::u16vec2( x_texture_position, y_texture_position2 ));
					const auto pixel22 = m_tga.get_pixel(ubv::u16vec2( x_texture_position2, y_texture_position2 ));

					//TODO: INTERPOLATE
					const auto interpolated_pixel_r1 = std::lerp(pixel11.r, pixel21.r, 1.0F - fraction_x);
					const auto interpolated_pixel_r2 = std::lerp(pixel12.r, pixel22.r, 1.0F - fraction_x);
					const auto interpolated_pixel_r3 = std::lerp(pixel11.r, pixel12.r, 1.0F - fraction_x);
					const auto interpolated_pixel_r4 = std::lerp(pixel21.r, pixel22.r, 1.0F - fraction_x);
					const auto interpolated_pixel_rl1 = std::lerp(interpolated_pixel_r1, interpolated_pixel_r3, 1.0F - fraction_y);
					const auto interpolated_pixel_rl2 = std::lerp(interpolated_pixel_r2, interpolated_pixel_r4, 1.0F - fraction_y);
					const auto interpolated_pixel_r = std::lerp(interpolated_pixel_rl1, interpolated_pixel_rl2,  1.0F - std::abs(fraction_x - fraction_y) / 2.0F);

					const auto interpolated_pixel_g1 = std::lerp(pixel11.g, pixel21.g, 1.0F - fraction_x);
					const auto interpolated_pixel_g2 = std::lerp(pixel12.g, pixel22.g, 1.0F - fraction_x);
					const auto interpolated_pixel_g3 = std::lerp(pixel11.g, pixel12.g, 1.0F - fraction_x);
					const auto interpolated_pixel_g4 = std::lerp(pixel21.g, pixel22.g, 1.0F - fraction_x);
					const auto interpolated_pixel_gl1 = std::lerp(interpolated_pixel_g1, interpolated_pixel_g3, 1.0F - fraction_y);
					const auto interpolated_pixel_gl2 = std::lerp(interpolated_pixel_g2, interpolated_pixel_g4, 1.0F - fraction_y);
					const auto interpolated_pixel_g = std::lerp(interpolated_pixel_gl1, interpolated_pixel_gl2, 1.0F - std::abs(fraction_x - fraction_y) / 2.0F);

					const auto interpolated_pixel_b1 = std::lerp(pixel11.b, pixel21.b, 1.0F - fraction_x);
					const auto interpolated_pixel_b2 = std::lerp(pixel12.b, pixel22.b, 1.0F - fraction_x);
					const auto interpolated_pixel_b3 = std::lerp(pixel11.b, pixel12.b, 1.0F - fraction_x);
					const auto interpolated_pixel_b4 = std::lerp(pixel21.b, pixel22.b, 1.0F - fraction_x);
					const auto interpolated_pixel_bl1 = std::lerp(interpolated_pixel_b1, interpolated_pixel_b3, 1.0F - fraction_y);
					const auto interpolated_pixel_bl2 = std::lerp(interpolated_pixel_b2, interpolated_pixel_b4, 1.0F - fraction_y);
					const auto interpolated_pixel_b = std::lerp(interpolated_pixel_bl1, interpolated_pixel_bl2,  1.0F - std::abs(fraction_x - fraction_y) / 2.0F);

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

		//Texture() = default;

		inline Texture(std::string_view t_tga_filename, FilteringType t_filtering_type = FilteringType::NEAREST) 
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