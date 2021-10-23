#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <map>
#include <set>
#include <vector>

namespace ubv
{
struct Pixel
{
#if defined(__unix__)
	std::uint8_t b;
	std::uint8_t g;
	std::uint8_t r;
	std::uint8_t a{255};
#else
	std::uint8_t b;
	std::uint8_t g;
	std::uint8_t r;
#endif
	constexpr explicit Pixel() noexcept = default;
	constexpr explicit Pixel(std::uint8_t t_r, std::uint8_t t_g, std::uint8_t t_b) noexcept : r{t_r}, g{t_g}, b{t_b}
	{
	}
	friend void swap(Pixel &p1, Pixel &p2)
	{
		using std::swap;

		std::swap(p1.r, p2.r);
		std::swap(p1.g, p2.g);
		std::swap(p1.b, p2.b);
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

// static_assert(sizeof(Pixel) == 3);

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

	inline static Pixel null_pixel = Pixel();

	inline void clear(Pixel t_color) noexcept
	{
		for (auto &pixel : m_pixels_data)
		{
			pixel = t_color;
		}
	}

	inline void set_pixel(std::uint32_t t_x, std::uint32_t t_y, Pixel t_color) noexcept
	{
		at(t_x, t_y) = t_color;
	}

	inline void set_pixel(std::uint32_t t_x, std::uint32_t t_y, vec2<std::uint32_t> t_size, Pixel t_color) noexcept
	{
		for (std::uint32_t y = 0; y < (t_y + t_size.y); ++y)
		{
			for (std::uint32_t x = 0; x < (t_x + t_size.x); ++x)
			{
				at(x, y) = t_color;
			}
		}
	}

	using u16vec2 = vec2<std::uint16_t>;
	using fvec2 = vec2<float>;

	inline Pixel interpolate(Pixel t_color1, Pixel t_color2, float t_fraction) const noexcept
	{
		t_fraction = std::clamp(t_fraction, 0.0F, 1.0F);
		return Pixel{std::uint8_t((float(t_color2.r) - float(t_color1.r)) * t_fraction + float(t_color1.r)),
					 std::uint8_t((float(t_color2.g) - float(t_color1.g)) * t_fraction + float(t_color1.g)),
					 std::uint8_t((float(t_color2.b) - float(t_color1.b)) * t_fraction + float(t_color1.b))};
	}

	inline void draw_line(fvec2 t_p1, fvec2 t_p2, Pixel t_color1, Pixel t_color2) noexcept
	{
		// set_pixel(t_p1.x, t_p1.y, t_color1);
		// set_pixel(t_p2.x, t_p2.y, t_color2);
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
			std::swap(t_color1, t_color2);
		}

		int error = 0;
		int y = t_p1.y;

		for (int x = t_p1.x; x <= t_p2.x; ++x)
		{
			const float total_distance = t_p2.x - t_p1.x;
			const float passed_distance = x - t_p1.x;
			const float fraction = passed_distance / total_distance;

			if (steep)
			{
				set_pixel(y, x, interpolate(t_color1, t_color2, fraction));
			}
			else
			{
				set_pixel(x, y, interpolate(t_color1, t_color2, fraction));
			}

			error += std::abs(t_p2.y - t_p1.y) * 2;
			if (error > (t_p2.x - t_p1.x))
			{
				y += (t_p2.y > t_p1.y ? 1 : -1);
				error -= (t_p2.x - t_p1.x) * 2;
			}
		}
	}

	inline void draw_fancy_glowy_circle(float t_x, float t_y, float t_size) noexcept
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

	inline bool is_point_inside_triangle(fvec2 s, fvec2 a, fvec2 b, fvec2 c) const noexcept
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

	/*inline void draw_triangle(fvec2 t_a, fvec2 t_b, fvec2 t_c, Pixel t_color) noexcept
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

		draw_line(t_a, t_b, Pixel{255, 0, 0}, Pixel{0, 0, 255});
		draw_line(t_b, t_c, Pixel{0, 255, 0}, Pixel{0, 255, 0});
		draw_line(t_c, t_a, Pixel{0, 0, 255}, Pixel{255, 0, 0});
	}*/

	struct Vertex
	{
		fvec2 position;
		Pixel color;
		// inline bool operator<(const Vertex& t_other) const noexcept
		//{
		//	return position.x < t_other.position.x;
		// }
	};

	inline fvec2 line_intersection(fvec2 A, fvec2 B, fvec2 C, fvec2 D)
	{
		// Line AB represented as a1x + b1y = c1
		const float a1 = B.y - A.y;
		const float b1 = A.x - B.x;
		const float c1 = a1 * (A.x) + b1 * (A.y);

		// Line CD represented as a2x + b2y = c2
		const float a2 = D.y - C.y;
		const float b2 = C.x - D.x;
		const float c2 = a2 * (C.x) + b2 * (C.y);

		const float determinant = a1 * b2 - a2 * b1;

		const float x = (b2 * c1 - b1 * c2) / determinant;
		const float y = (a1 * c2 - a2 * c1) / determinant;
		return fvec2{float(x), float(y)};
	}

	inline void draw_triangle(const std::array<Vertex, 3> &t_vertices)
	{
		std::uint32_t start_x = std::max<float>(std::min<float>({t_vertices[0].position.x, t_vertices[1].position.x , t_vertices[2].position.x })-1, 0);
		std::uint32_t end_x = std::min<float>(std::max<float>({ t_vertices[0].position.x, t_vertices[1].position.x , t_vertices[2].position.x })+1, m_width);

		std::uint32_t start_y = std::max<float>(std::min<float>({ t_vertices[0].position.y, t_vertices[1].position.y , t_vertices[2].position.y })-1, 0);
		std::uint32_t end_y = std::min<float>(std::max<float>({ t_vertices[0].position.y, t_vertices[1].position.y , t_vertices[2].position.y })+1, m_height);

		for (std::uint32_t x = start_x; x < end_x; ++x)
		{
			for (std::uint32_t y = start_y; y < end_y; ++y)
			{
				fvec2 p = fvec2{(float)x, (float)y};
				if (is_point_inside_triangle(fvec2{(float)x, (float)y}, t_vertices[0].position, t_vertices[1].position,
											 t_vertices[2].position))
				{
					Pixel pixel;
					std::array<float, 3> scales;
					for (int i = 0; i < 3; ++i)
					{
						auto point = line_intersection(t_vertices[i].position, p, t_vertices[(i + 1) % 3].position,
													   t_vertices[(i + 2) % 3].position);
						auto total_distance = std::sqrt(std::pow(std::abs(t_vertices[i].position.x - point.x), 2.0F) +
														std::pow(std::abs(t_vertices[i].position.y - point.y), 2.0F));
						auto distance = std::sqrt(std::pow(std::abs(p.x - point.x), 2.0F) +
												  std::pow(std::abs(p.y - point.y), 2.0F));
						auto fraction = std::clamp(distance / total_distance, 0.0F, 1.0F);
						scales[i] = fraction;
					}

					const auto total_scale = scales[0] + scales[1] + scales[2];
					pixel.r = std::clamp((float(t_vertices[0].color.r) * scales[0] +
										  float(t_vertices[1].color.r) * scales[1] +
										  float(t_vertices[2].color.r) * scales[2]) /
											 (total_scale),
										 0.0F, 255.0F);
					pixel.g = std::clamp((float(t_vertices[0].color.g) * scales[0] +
										  float(t_vertices[1].color.g) * scales[1] +
										  float(t_vertices[2].color.g) * scales[2]) /
											 (total_scale),
										 0.0F, 255.0F);
					pixel.b = std::clamp((float(t_vertices[0].color.b) * scales[0] +
										  float(t_vertices[1].color.b) * scales[1] +
										  float(t_vertices[2].color.b) * scales[2]) /
											 (total_scale),
										 0.0F, 255.0F);

					set_pixel(x, y, pixel);
				}
			}
		}
	}

	/*inline void draw_colored_triangle(fvec2 t_a, fvec2 t_b, fvec2 t_c, Pixel t_color_a, Pixel t_color_b, Pixel
	t_color_c) noexcept
	{
		std::map<std::uint16_t, std::set<Vertex>> vertices_lines;
		for (int line = 0; line < 3; ++line)
		{
			fvec2 t_p1, t_p2;
			vec2<Pixel> colors;
			switch (line)
			{
			case 0:
			{
				t_p1 = t_a;
				t_p2 = t_b;
				colors.x = t_color_a;
				colors.y = t_color_b;
			}
				break;
			case 1:
			{
				t_p1 = t_b;
				t_p2 = t_c;
				colors.x = t_color_b;
				colors.y = t_color_c;
			}
				break;
			case 2:
			{
				t_p1 = t_c;
				t_p2 = t_a;
				colors.x = t_color_c;
				colors.y = t_color_a;
			}
				break;
			}

			vertices_lines[t_p1.y].emplace(Vertex{ u16vec2{std::uint16_t(t_p1.x),std::uint16_t(t_p1.y) }, colors.x});
			vertices_lines[t_p2.y].emplace(Vertex{ u16vec2{std::uint16_t(t_p2.x),std::uint16_t(t_p2.y) }, colors.y});

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
				std::swap(colors.x, colors.y);
			}

			int error = 0;
			int y = t_p1.y;
			std::set<std::uint16_t> xs;
			for (int x = t_p1.x; x <= t_p2.x; ++x)
			{
				const float total_distance = t_p2.x - t_p1.x;
				const float passed_distance = x - t_p1.x;
				const float fraction = passed_distance / total_distance;

				const auto vertex_color = interpolate(colors.x, colors.y, fraction);
				u16vec2 vertex_pos{ std::uint16_t(x),std::uint16_t(y) };

				if (steep)
				{
					std::swap(vertex_pos.x, vertex_pos.y);
				}

				vertices_lines[vertex_pos.y].emplace(Vertex{ vertex_pos , vertex_color });

				error += std::abs(t_p2.y - t_p1.y) * 2;
				if (error > (t_p2.x - t_p1.x))
				{
					y += (t_p2.y > t_p1.y ? 1 : -1);
					error -= (t_p2.x - t_p1.x) * 2;
				}
			}
		}
		for (const auto& [y, vertices] : vertices_lines)
		{
			if (vertices.size() < 2)
				continue;
			const auto& vertex_a = *vertices.begin();
			const auto& vertex_b = *vertices.rbegin();
			draw_line(fvec2{ float(vertex_a.position.x),float(vertex_a.position.y) }, fvec2{
	float(vertex_b.position.x),float(vertex_b.position.y) }, vertex_a.color, vertex_b.color);
		}

	}*/

  private:
	std::uint32_t m_width;
	std::uint32_t m_height;
	std::vector<Pixel> m_pixels_data;
};
}; // namespace ubv
