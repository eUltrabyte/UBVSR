#pragma once

#include "Math.hpp"
#include "TGA.hpp"

#include <cmath>

namespace ubv
{
class Texture
{
  public:
	enum class FilteringType : std::uint8_t
	{
		NEAREST,
		BILINEAR
	} m_filtering_type;

	bool clamp = false;

	constexpr vec2<int> fix_position(vec2<int> t_position) const noexcept
	{
		if (t_position.x < 0)
			t_position.x = clamp ? 0 : size_x_m1;
		else if (t_position.x > size_x_m1)
			t_position.x = clamp ? size_x_m1 : 0;

		if (t_position.y < 0)
			t_position.y = clamp ? 0 : size_y_m1;
		else if (t_position.y > size_y_m1)
			t_position.y = clamp ? size_y_m1 : 0;

		return t_position;
	}

	constexpr static float fast_fmod_1(float t_float) noexcept
	{
		const int whole = t_float;
		float fraction = t_float - static_cast<float>(whole);
		if (fraction < 0.0F)
			fraction += 1.0F;
		return fraction;
	}

	inline Pixel sample_bilinear(const fvec2& pos_uv) const noexcept
	{
		const int x_pos = std::floor(pos_uv.x - 0.5F);
		const int y_pos = std::floor(pos_uv.y - 0.5F);

		const auto pos_top_left = fix_position(vec2<int>{x_pos, y_pos});
		const auto pos_bottom_right = fix_position(vec2<int>{x_pos + 1, y_pos + 1});

		const auto pixel11 = m_tga.get_pixel(pos_top_left.x, pos_top_left.y);
		const auto pixel21 = m_tga.get_pixel(pos_bottom_right.x, pos_top_left.y);
		const auto pixel12 = m_tga.get_pixel(pos_top_left.x, pos_bottom_right.y);
		const auto pixel22 = m_tga.get_pixel(pos_bottom_right.x, pos_bottom_right.y);

		const fvec2 fraction{pos_uv.x - static_cast<float>(x_pos) - 0.5F, pos_uv.y - static_cast<float>(y_pos) - 0.5F};

		Pixel pixel(fast_blerp(fvec4(pixel11.r, pixel21.r, pixel12.r, pixel22.r), fraction),
					fast_blerp(fvec4(pixel11.g, pixel21.g, pixel12.g, pixel22.g), fraction),
					fast_blerp(fvec4(pixel11.b, pixel21.b, pixel12.b, pixel22.b), fraction));

		return pixel;
	}

	inline Pixel sample(fvec2 pos_uv) const noexcept
	{
		if (!clamp)
		{
			pos_uv.x = fast_fmod_1(pos_uv.x);
			pos_uv.y = fast_fmod_1(pos_uv.y);
		}
		pos_uv.x *= size_x;
		pos_uv.y *= size_y;
		switch (m_filtering_type)
		{
		case FilteringType::NEAREST: {
			auto position = fix_position(vec2<int>(pos_uv.x, pos_uv.y));
			return m_tga.get_pixel(position.x, position.y);
		}
		break;
		case FilteringType::BILINEAR: {
			return sample_bilinear(pos_uv);
		}
		break;
		}
	}

	inline Texture(std::string_view t_tga_filename, FilteringType t_filtering_type = FilteringType::NEAREST)
		: m_tga{t_tga_filename}, m_filtering_type{t_filtering_type}, inverse_size_x{1.0F / float(m_tga.get_width())},
		  inverse_size_y{1.0F / float(m_tga.get_height())}, size_x{float(m_tga.get_width())}, size_y{float(
																								  m_tga.get_height())},
		  size_x_m1(m_tga.get_width() - 1), size_y_m1(m_tga.get_height() - 1)
	{
	}

	constexpr u16vec2 get_size() const noexcept
	{
		return u16vec2(static_cast<uint16_t>(m_tga.get_width()), static_cast<uint16_t>(m_tga.get_height()));
	}

  private:
	TGA m_tga;

	float inverse_size_x;
	float inverse_size_y;

	float size_x;
	float size_y;

	unsigned size_x_m1;
	unsigned size_y_m1;
};
}; // namespace ubv
