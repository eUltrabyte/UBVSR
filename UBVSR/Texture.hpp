#pragma once

#include "TGA.hpp"

#include <cmath>

/*namespace std {
	template<typename T>
	[[nodiscard]] constexpr float blerp(ubv::vec2<T> t_vec0, ubv::vec2<T> t_vec1, ubv::vec2<T> t_fraction_vec) {
		return lerp(lerp(t_vec0.x, t_vec0.y, t_fraction_vec.x), lerp(t_vec1.x, t_vec1.y, t_fraction_vec.x), t_fraction_vec.y);
	}

	template<typename T>
	[[nodiscard]] constexpr float blerp(ubv::vec4<T> t_vec, ubv::vec2<T> t_fraction_vec) {
		return lerp(lerp(t_vec.x, t_vec.y, t_fraction_vec.x), lerp(t_vec.z, t_vec.w, t_fraction_vec.x), t_fraction_vec.y);
	}

	template<typename T, typename U>
	[[nodiscard]] constexpr uint32_t get_byte(const T& t_value, U t_offset) {
		if(is_arithmetic_v<decltype(t_offset)>) {
			return uint32_t(t_value & 255);
		} else {
			abort();
		}
	}
};*/

namespace ubv {
	class Texture {
	public:
		enum class FilteringType : std::uint8_t {
			NEAREST,
			BILINEAR
		} m_filtering_type;

		bool clamp = false;

		Pixel sample(fvec2 pos_uv) const noexcept {
			if (!clamp) [[likely]]
			{
				pos_uv.x = std::fmod(pos_uv.x, 1.0F);
				pos_uv.y = std::fmod(pos_uv.y, 1.0F);
				if (pos_uv.x < 0.0F)
					pos_uv.x += 1.0F;
				if (pos_uv.y < 0.0F)
					pos_uv.y += 1.0F;
			}
			const auto  m_filtering_type = FilteringType::NEAREST;
			switch (m_filtering_type) {
				case FilteringType::NEAREST:
				{
					std::uint16_t x_texture_position = std::clamp<std::uint16_t>(std::uint16_t(pos_uv.x * get_size().x), 0, get_size().x - 1);
					std::uint16_t y_texture_position = std::clamp<std::uint16_t>(std::uint16_t(pos_uv.y * get_size().y), 0, get_size().y - 1);
					return m_tga.get_pixel(ubv::u16vec2{x_texture_position, y_texture_position});
				}
				break;
				case FilteringType::BILINEAR:
				{
					const fvec2 section{ 1.0F / static_cast<float>(get_size().x), 1.0F / static_cast<float>(get_size().y) };
					const fvec2 half_section{ section / 2.0F };
					const fvec2 pos_mod{ std::fmod(pos_uv.x, section.x), std::fmod(pos_uv.y, section.y) };
					const fvec2 fraction{ std::abs(pos_mod.x - half_section.x) / section.x , std::abs(pos_mod.y - half_section.y) / section.y };
					const int x_texture_position = std::clamp<std::uint16_t>(std::uint16_t(pos_uv.x * get_size().x), 0, get_size().x - 1);
					const int y_texture_position = std::clamp<std::uint16_t>(std::uint16_t(pos_uv.y * get_size().y), 0, get_size().y - 1);
					int x_texture_position2 = x_texture_position;
					int y_texture_position2 = y_texture_position;
					if (pos_mod.x > half_section.x)
					{
						x_texture_position2 = (x_texture_position2)+1;
						if (x_texture_position2 == get_size().x) [[unlikely]]
						{
							x_texture_position2 = clamp ? get_size().x - 1 : 0;
						}
					}
					else
					{
						x_texture_position2 = (x_texture_position2)-1;
						if (x_texture_position2 < 0) [[unlikely]]
						{
							x_texture_position2 = clamp ? 0 : get_size().x - 1;
						}
					}
					if (pos_mod.y > half_section.y)
					{
						y_texture_position2 = (y_texture_position2)+1;
						if (y_texture_position2 == get_size().y) [[unlikely]]
						{
							y_texture_position2 = clamp ? get_size().y - 1 : 0;
						}
					}
					else
					{
						y_texture_position2 = (y_texture_position2-1);
						if (y_texture_position2 < 0) [[unlikely]]
						{
							y_texture_position2 = clamp ? 0 : get_size().y - 1;
						}
					}
					const auto pixel11 = m_tga.get_pixel(ubv::u16vec2( x_texture_position, y_texture_position ));
					const auto pixel21 = m_tga.get_pixel(ubv::u16vec2( x_texture_position2, y_texture_position ));
					const auto pixel12 = m_tga.get_pixel(ubv::u16vec2( x_texture_position, y_texture_position2 ));
					const auto pixel22 = m_tga.get_pixel(ubv::u16vec2( x_texture_position2, y_texture_position2 ));

					Pixel p(fast_blerp(fvec4(pixel11.r, pixel21.r, pixel12.r, pixel22.r), fraction),
						fast_blerp(fvec4(pixel11.g, pixel21.g, pixel12.g, pixel22.g), fraction),
						fast_blerp(fvec4(pixel11.b, pixel21.b, pixel12.b, pixel22.b), fraction));

					return p;
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