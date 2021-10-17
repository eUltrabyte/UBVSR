#pragma once

#include <array>
#include <cstdint>
#include <vector>

namespace ubv {
struct Pixel
{
	std::uint8_t b;
	std::uint8_t g;
	std::uint8_t r;
	constexpr explicit Pixel() noexcept = default;
	constexpr explicit Pixel(std::uint8_t t_r, std::uint8_t t_g, std::uint8_t t_b) noexcept : r{t_r}, g{t_g}, b{t_b}
	{
	}
};

template <typename T> struct vec2
{
	T x;
	T y;
	constexpr vec2() noexcept = default;
	constexpr vec2(T &&t_x, T &&t_y) noexcept : x{t_x}, y{t_y}
	{
	}
};

static_assert(sizeof(Pixel) == 3);

class FrameBuffer
{
  public:
	inline FrameBuffer(std::uint32_t t_width, std::uint32_t t_height) noexcept
		: m_width{t_width}, m_height{t_height}, m_pixels_data{t_width * t_height}
	{
	}

	[[nodiscard]] constexpr std::uint32_t get_width() const noexcept
	{
		return m_width;
	}

	[[nodiscard]] constexpr std::uint32_t get_height() const noexcept
	{
		return m_height;
	}

	[[nodiscard]] inline Pixel &at(std::uint32_t t_x, std::uint32_t t_y) noexcept
	{
		if (t_x < m_width && t_y < m_height)
			return m_pixels_data[t_y * m_width + t_x];
		return null_pixel;
	}

	[[nodiscard]] inline const Pixel &at(std::uint32_t t_x, std::uint32_t t_y) const noexcept
	{
		if (t_x < m_width && t_y < m_height)
			return m_pixels_data[t_y * m_width + t_x];
		return null_pixel;
	}

	[[nodiscard]] inline const std::vector<Pixel> &get_pixel_data() const noexcept
	{
		return m_pixels_data;
	}

	inline static Pixel null_pixel;

	inline void clear(Pixel t_color) noexcept
	{
		for (auto &pixel : m_pixels_data)
		{
			pixel = t_color;
		}
	}

	inline void set_pixel(std::uint32_t t_x, std::uint32_t t_y, Pixel t_color)
	{
		at(t_x, t_y) = t_color;
	}

	using u16vec2 = vec2<std::uint16_t>;
	using fvec2 = vec2<float>;

	inline void draw_line(fvec2 t_p1, fvec2 t_p2, Pixel t_color)
	{
		bool steep = false;
		if (std::abs(t_p1.x - t_p2.x) < std::abs(t_p1.y - t_p2.y))
		{
			std::swap(t_p1.x, t_p1.y);
			std::swap(t_p2.x, t_p2.y);
			steep = true;
		}
		
		if (t_p1.x > t_p2.x)
		{
			std::swap(t_p1.x, t_p2.x);
			std::swap(t_p1.y, t_p2.y);
		}
		
		int dx = t_p2.x - t_p1.x;
		int dy = t_p2.y - t_p1.y;
		int derror2 = std::abs(dy) * 2;
		int error2 = 0;
		int y = t_p1.y;
		
		for (int x = t_p1.x; x <= t_p2.x; ++x)
		{
			if (steep)
			{
				set_pixel(y, x, t_color);
			}
			else
			{
				set_pixel(x, y, t_color);
			}
			
			error2 += derror2;
			if (error2 > dx)
			{
				y += (t_p2.y > t_p1.y ? 1 : -1);
				error2 -= dx * 2;
			}
		}
	}

	inline void draw_fancy_glowy_circle(float t_x, float t_y, float t_size)
	{
		for (unsigned x = 0; x < m_width; ++x)
		{
			for (unsigned y = 0; y < m_height; ++y)
			{
				float distance =
					std::sqrt(std::pow(std::abs(t_x - float(x)), 2.0F) + std::pow(std::abs(t_y - float(y)), 2.0F));
				if (distance < t_size)
				{
					float brightness = distance / t_size;
					const auto op = at(x, y);
					unsigned v = 255 - uint8_t(brightness * 255.0F);
					std::uint8_t r = std::clamp<unsigned>(unsigned(op.r) + v, 0, 255);
					std::uint8_t g = std::clamp<unsigned>(unsigned(op.g) + v, 0, 255);
					std::uint8_t b = std::clamp<unsigned>(unsigned(op.b) + v, 0, 255);
					set_pixel(x, y, Pixel(r, g, b));
				}
			}
		}
	}

	inline bool is_point_inside_triangle(fvec2 s, fvec2 a, fvec2 b, fvec2 c)
	{
		auto as_x = s.x - a.x;
		auto as_y = s.y - a.y;

		bool s_ab = (b.x - a.x) * as_y - (b.y - a.y) * as_x > 0.0F;

		if ((c.x - a.x) * as_y - (c.y - a.y) * as_x > 0.0F == s_ab)
			return false;

		if ((c.x - b.x) * (s.y - b.y) - (c.y - b.y) * (s.x - b.x) > 0.0F != s_ab)
			return false;

		return true;
	}

	inline void draw_triangle(fvec2 t_a, fvec2 t_b, fvec2 t_c, Pixel t_color)
	{
		for (unsigned x = 0; x < m_width; ++x)
		{
			for (unsigned y = 0; y < m_height; ++y)
			{
				if (is_point_inside_triangle(fvec2{(float)x,(float)y},t_a,t_b,t_c))
				{
					set_pixel(x, y, t_color);
				}
			}
		}

		draw_line(t_a, t_b, Pixel{255, 0, 0});
		draw_line(t_b, t_c, Pixel{0, 255, 0});
		draw_line(t_c, t_a, Pixel{0, 0, 255});
	}

  private:
	std::uint32_t m_width;
	std::uint32_t m_height;
	std::vector<Pixel> m_pixels_data;
};
};