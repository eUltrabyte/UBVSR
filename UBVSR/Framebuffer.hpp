#pragma once

#include "Buffer.hpp"
#include "Math.hpp"
#include "Pixel.hpp"
#include "Texture.hpp"
#include "Vertex.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <future>
#include <map>
#include <set>
#include <vector>

namespace ubv
{
class FrameBuffer
{
  public:
	inline explicit FrameBuffer(std::uint16_t t_width, std::uint16_t t_height) noexcept
		: m_width{t_width}, m_height{t_height}, m_color_buffer{m_width, m_height}, m_depth_buffer{m_width, m_height},
		  m_stencil_buffer{m_width, m_height}
	{
	}

	[[nodiscard]] constexpr std::uint16_t get_width() const noexcept
	{
		return m_width;
	}

	[[nodiscard]] constexpr std::uint16_t get_height() const noexcept
	{
		return m_height;
	}

	inline void clear() noexcept
	{
		m_color_buffer.clear();
		m_depth_buffer.clear();
		m_stencil_buffer.clear();
	}

	inline bool zbuffer_test_and_set(std::uint16_t t_x, std::uint16_t t_y, float t_depth)
	{
		float &existing_depth = m_depth_buffer.at(t_x, t_y);
		if (t_depth < existing_depth)
		{
			existing_depth = t_depth;
			return true;
		}
		return false;
	}

	inline bool is_point_inside_triangle(fvec2 s, fvec2 a, fvec2 b, fvec2 c) const noexcept
	{
		auto as_x = s.x - a.x;
		auto as_y = s.y - a.y;

		bool s_ab = (b.x - a.x) * as_y - (b.y - a.y) * as_x > 0.0F;

		if ((c.x - a.x) * as_y - (c.y - a.y) * as_x > 0.0F == s_ab)
		{
			return false;
		}

		if ((c.x - b.x) * (s.y - b.y) - (c.y - b.y) * (s.x - b.x) > 0.0F != s_ab)
		{
			return false;
		}

		return true;
	}

	inline fvec2 line_intersection(fvec2 A, fvec2 B, fvec2 C, fvec2 D) const noexcept
	{
		const float a1 = B.y - A.y;
		const float b1 = A.x - B.x;
		const float c1 = a1 * (A.x) + b1 * (A.y);

		const float a2 = D.y - C.y;
		const float b2 = C.x - D.x;
		const float c2 = a2 * (C.x) + b2 * (C.y);

		const float determinant = a1 * b2 - a2 * b1;

		const float x = (b2 * c1 - b1 * c2) / determinant;
		const float y = (a1 * c2 - a2 * c1) / determinant;

		return fvec2{float(x), float(y)};
	}

	inline void draw_triangle(const std::array<Vertex, 3> &t_vertices, const Texture &t_texture)
	{

		std::array<fvec3, 3> vertices = {
			fvec3((t_vertices[0].position.x + 1.0F) / 2.0F * static_cast<float>(m_width),
				  (t_vertices[0].position.y + 1.0F) / 2.0F * static_cast<float>(m_height), t_vertices[0].position.z),
			fvec3((t_vertices[1].position.x + 1.0F) / 2.0F * static_cast<float>(m_width),
				  (t_vertices[1].position.y + 1.0F) / 2.0F * static_cast<float>(m_height), t_vertices[1].position.z),
			fvec3((t_vertices[2].position.x + 1.0F) / 2.0F * static_cast<float>(m_width),
				  (t_vertices[2].position.y + 1.0F) / 2.0F * static_cast<float>(m_height), t_vertices[2].position.z)};

		const std::uint32_t start_x =
			std::max<float>(std::min<float>({vertices[0].x, vertices[1].x, vertices[2].x}) - 1.0F, 0.0F);
		const std::uint32_t end_x =
			std::min<std::uint32_t>(std::max<float>({vertices[0].x, vertices[1].x, vertices[2].x}) + 1, m_width);

		const std::uint32_t start_y =
			std::max<float>(std::min<float>({vertices[0].y, vertices[1].y, vertices[2].y}) - 1.0F, 0.0F);
		const std::uint32_t end_y =
			std::min<std::uint32_t>(std::max<float>({vertices[0].y, vertices[1].y, vertices[2].y}) + 1, m_height);

		for (std::uint32_t x = start_x; x < end_x; ++x)
		{
			for (std::uint32_t y = start_y; y < end_y; ++y)
			{
				const fvec2 p = fvec2{static_cast<float>(x), static_cast<float>(y)};
				if (is_point_inside_triangle(p, fvec2(vertices[0]), fvec2(vertices[1]), fvec2(vertices[2])))
				{
					std::array<float, 3> scales;
					for (int i = 0; i < 3; ++i)
					{
						auto point = line_intersection(fvec2(vertices[i]), p, fvec2(vertices[(i + 1) % 3]),
													   fvec2(vertices[(i + 2) % 3]));
						const auto dx = vertices[i].x - point.x;
						const auto dy = vertices[i].y - point.y;
						const auto d2x = p.x - point.x;
						const auto d2y = p.y - point.y;
						auto total_distance = std::sqrt(dx * dx + dy * dy);
						auto distance = std::sqrt(d2x * d2x + d2y * d2y);
						auto fraction = distance / total_distance;
						scales[i] = fraction;
					}

					const auto total_scale = scales[0] + scales[1] + scales[2];

					const float z_value = 1.0F / ((1.0F / vertices[0].z * scales[0] + 1.0F / vertices[1].z * scales[1] +
												   1.0F / vertices[2].z * scales[2]) /
												  total_scale);

					const float zv = z_value;
					if (zbuffer_test_and_set(x, y, zv))
					{
						m_color_buffer.at(x, y) =
							t_texture.sample(fvec2{((t_vertices[0].texture_uv.x / vertices[0].z) * scales[0] +
													(t_vertices[1].texture_uv.x / vertices[1].z) * scales[1] +
													(t_vertices[2].texture_uv.x / vertices[2].z) * scales[2]) /
													   total_scale / (1.0F / zv),
												   ((t_vertices[0].texture_uv.y / vertices[0].z) * scales[0] +
													(t_vertices[1].texture_uv.y / vertices[1].z) * scales[1] +
													(t_vertices[2].texture_uv.y / vertices[2].z) * scales[2]) /
													   total_scale / (1.0F / zv)});
					}
				}
			}
		}
	}

	inline void draw_z_buffer() noexcept
	{
		auto& z_buffer = m_depth_buffer.data();
		const auto min = *std::min_element(z_buffer.begin(), z_buffer.end());
		float max{min};
		for (const float z_value : z_buffer)
		{
			if (z_value > max && z_value != std::numeric_limits<float>::infinity())
			{
				max = z_value;
			}
		}
		const auto max_distance = max - min;
		for (std::uint32_t i = 0; i < z_buffer.size(); ++i)
		{
			std::uint8_t color_value = std::clamp((z_buffer[i] - min) / max_distance * 255.0F, 0.0F, 255.0F);
			m_color_buffer.data()[i] = Pixel{color_value, color_value, color_value};
		}
	}

	[[nodiscard]] inline const ColorBuffer& get_color_buffer() const noexcept
	{
		return m_color_buffer;
	}

  private:
	// size
	std::uint16_t m_width;
	std::uint16_t m_height;

	// color buffer
	ColorBuffer m_color_buffer;

	// depth buffer
	DepthBuffer m_depth_buffer;

	// stencil buffer
	StencilBuffer m_stencil_buffer;
};
}; // namespace ubv
