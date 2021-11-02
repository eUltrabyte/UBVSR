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
#include <numeric>
#include <set>
#include <vector>

namespace ubv
{
	class FrameBuffer
	{
	public:
		inline explicit FrameBuffer(std::uint16_t t_width, std::uint16_t t_height) noexcept
			: m_width{ t_width }, m_height{ t_height }, m_color_buffer{ m_width, m_height }, m_ms_color_buffer{ m_width, m_height },
			m_ms_depth_buffer{ m_width, m_height }, m_ms_stencil_buffer{ m_width, m_height }, m_ms_ndc_positions{ m_width,
																											 m_height },
			m_ms_mutex_buffer{std::make_unique<std::mutex[]>(m_width * m_height)}
		{
			fill_ndc_positions();
		}

		inline void fill_ndc_positions() noexcept
		{
			for (std::uint32_t x = 0; x < m_width * m_multisample; ++x)
			{
				for (std::uint32_t y = 0; y < m_height * m_multisample; ++y)
				{
					m_ms_ndc_positions.at(x, y) = fvec2{ static_cast<float>(x) / static_cast<float>(m_width),
														static_cast<float>(y) / static_cast<float>(m_height) } /
						static_cast<float>(m_multisample) * 2.0F -
						1.0F;
				}
			}
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
			m_ms_color_buffer.clear();
			m_ms_depth_buffer.clear();
			// m_ms_stencil_buffer.clear();
		}

		inline bool zbuffer_test_and_set(std::uint16_t t_x, std::uint16_t t_y, float t_depth)
		{
			float& existing_depth = m_ms_depth_buffer.at(t_x, t_y);
			if (t_depth < existing_depth)
			{
				existing_depth = t_depth;
				return true;
			}
			return false;
		}

		constexpr bool is_point_inside_triangle(const fvec2& s, const fvec2& a, const fvec2& b,
			const fvec2& c) const noexcept
		{
			const auto as_x = s.x - a.x;
			const auto as_y = s.y - a.y;

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

		constexpr fvec2 line_intersection(const fvec2& A, const fvec2& B, const fvec2& C, const fvec2& D) const noexcept
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

			return fvec2{ x, y };
		}

		struct FogParams
		{
			float start = 2.0F;
			float end = 4.5F;
			Pixel color = Pixel{ 0, 100, 220 };
			float destiny = 0.95F;
			bool enable = false;
		} fog_params;

		constexpr bool cull_test(const std::array<Vertex, 3>& t_vertices)
		{
			if ((t_vertices[0].position.x < -t_vertices[0].position.w &&
				t_vertices[1].position.x < -t_vertices[1].position.w &&
				t_vertices[2].position.x < -t_vertices[2].position.w) ||
				(t_vertices[0].position.y < -t_vertices[0].position.w &&
					t_vertices[1].position.y < -t_vertices[1].position.w &&
					t_vertices[2].position.y < -t_vertices[2].position.w) ||
				(t_vertices[0].position.z < 0.0F && t_vertices[1].position.z < 0.0F && t_vertices[2].position.z < 0.0F) ||
				(t_vertices[0].position.x > t_vertices[0].position.w &&
					t_vertices[1].position.x > t_vertices[1].position.w &&
					t_vertices[2].position.x > t_vertices[2].position.w) ||
				(t_vertices[0].position.y > t_vertices[0].position.w &&
					t_vertices[1].position.y > t_vertices[1].position.w &&
					t_vertices[2].position.y > t_vertices[2].position.w) ||
				(t_vertices[0].position.z > t_vertices[0].position.w &&
					t_vertices[1].position.z > t_vertices[1].position.w &&
					t_vertices[2].position.z > t_vertices[2].position.w))
			{
				return false;
			}
			return true;
		}

		constexpr std::array<std::array<Vertex, 3>, 2> clip_with_one_wrong_vertex(
			const Vertex& t_wrong_vertex, const std::array<Vertex, 2>& t_correct_vertices)
		{
			const auto fraction0 =
				(-t_wrong_vertex.position.z) / (t_correct_vertices[0].position.z - t_wrong_vertex.position.z);
			const auto fraction1 =
				(-t_wrong_vertex.position.z) / (t_correct_vertices[1].position.z - t_wrong_vertex.position.z);

			const auto vertex0 = t_wrong_vertex.interpolate(t_correct_vertices[0], fraction0);
			const auto vertex1 = t_wrong_vertex.interpolate(t_correct_vertices[1], fraction1);

			return { std::array<Vertex, 3>{vertex0, t_correct_vertices[0], t_correct_vertices[1]},
					{vertex0, vertex1, t_correct_vertices[1]} };
		}

		constexpr std::array<Vertex, 3> clip_with_two_wrong_vertices(const std::array<Vertex, 2>& t_wrong_vertices,
			const Vertex& t_correct_vertex)
		{
			const auto fraction0 =
				(-t_wrong_vertices[0].position.z) / (t_correct_vertex.position.z - t_wrong_vertices[0].position.z);
			const auto fraction1 =
				(-t_wrong_vertices[1].position.z) / (t_correct_vertex.position.z - t_wrong_vertices[1].position.z);

			const auto vertex0 = t_wrong_vertices[0].interpolate(t_correct_vertex, fraction0);
			const auto vertex1 = t_wrong_vertices[1].interpolate(t_correct_vertex, fraction1);

			return { vertex0, vertex1, t_correct_vertex };
		}

		bool stencil_test = false;
		enum class StencilFunction : std::uint8_t
		{
			LESS,
			GREATER
		} stencil_function = StencilFunction::GREATER;
		std::uint8_t stencil_value = 0x80;

		inline void draw_to_stencil_buffer() noexcept
		{
			for (std::uint32_t i = 0; i < m_ms_color_buffer.data().size(); ++i)
			{
				m_ms_stencil_buffer.data()[i] =
					(unsigned(m_ms_color_buffer.data()[i].r) + unsigned(m_ms_color_buffer.data()[i].g) +
						unsigned(m_ms_color_buffer.data()[i].b)) /
					3U;
			}
		}

		inline void draw_triangle(const std::array<Vertex, 3>& t_vertices, const Texture& t_texture,
			bool t_force_draw = false)
		{
			if (!t_force_draw)
			{
				if (!cull_test(t_vertices))
				{
					return;
				}

				std::vector<std::uint8_t> wrong_vertices_indexes;
				std::vector<std::uint8_t> correct_vertices_indexes;

				for (std::uint8_t i = 0; i < 3; ++i)
				{
					if (t_vertices[i].position.z < 0.0F)
					{
						wrong_vertices_indexes.push_back(i);
					}
					else
					{
						correct_vertices_indexes.push_back(i);
					}
				}

				if (wrong_vertices_indexes.size() == 1)
				{
					const auto triangles = clip_with_one_wrong_vertex(
						t_vertices[wrong_vertices_indexes[0]],
						{ t_vertices[correct_vertices_indexes[0]], t_vertices[correct_vertices_indexes[1]] });
					for (const auto& triangle : triangles)
					{
						draw_triangle(triangle, t_texture, true);
					}
					return;
				}

				if (wrong_vertices_indexes.size() == 2)
				{
					const auto triangle = clip_with_two_wrong_vertices(
						{ t_vertices[wrong_vertices_indexes[0]], t_vertices[wrong_vertices_indexes[1]] },
						t_vertices[correct_vertices_indexes[0]]);
					draw_triangle(triangle, t_texture, true);
					return;
				}
			}

			const std::array<fvec3, 3> ndc_vertices = {
				static_cast<fvec3>(t_vertices[0].position) / t_vertices[0].position.w,
				static_cast<fvec3>(t_vertices[1].position) / t_vertices[1].position.w,
				static_cast<fvec3>(t_vertices[2].position) / t_vertices[2].position.w };

			std::array<fvec3, 3> vertices = { fvec3((t_vertices[0].position.x / t_vertices[0].position.w + 1.0F) / 2.0F *
													   static_cast<float>(m_width) * static_cast<float>(m_multisample),
												   (t_vertices[0].position.y / t_vertices[0].position.w + 1.0F) / 2.0F *
													   static_cast<float>(m_height) * static_cast<float>(m_multisample),
												   t_vertices[0].position.z / t_vertices[0].position.w),
											 fvec3((t_vertices[1].position.x / t_vertices[1].position.w + 1.0F) / 2.0F *
													   static_cast<float>(m_width) * static_cast<float>(m_multisample),
												   (t_vertices[1].position.y / t_vertices[1].position.w + 1.0F) / 2.0F *
													   static_cast<float>(m_height) * static_cast<float>(m_multisample),
												   t_vertices[1].position.z / t_vertices[1].position.w),
											 fvec3((t_vertices[2].position.x / t_vertices[2].position.w + 1.0F) / 2.0F *
													   static_cast<float>(m_width) * static_cast<float>(m_multisample),
												   (t_vertices[2].position.y / t_vertices[2].position.w + 1.0F) / 2.0F *
													   static_cast<float>(m_height) * static_cast<float>(m_multisample),
												   t_vertices[2].position.z / t_vertices[2].position.w) };

			std::uint32_t start_x =
				std::max<float>(std::min<float>({ vertices[0].x, vertices[1].x, vertices[2].x }) - 1.0F, 0.0F);
			std::uint32_t end_x = std::min<std::uint32_t>(
				std::max<float>({ vertices[0].x, vertices[1].x, vertices[2].x }) + 1, m_width * m_multisample);

			std::uint32_t start_y =
				std::max<float>(std::min<float>({ vertices[0].y, vertices[1].y, vertices[2].y }) - 1.0F, 0.0F);
			std::uint32_t end_y = std::min<std::uint32_t>(
				std::max<float>({ vertices[0].y, vertices[1].y, vertices[2].y }) + 1, m_height * m_multisample);

			// for (std::uint32_t x = start_x; x < end_x; ++x)
			{
				for (std::uint32_t y = start_y; y < end_y; ++y)
				{
					std::uint32_t triangle_start_left_x = end_x;
					std::uint32_t triangle_end_right_x = end_x;
					for (std::uint32_t x = start_x; x < end_x; ++x)
					{
						if (is_point_inside_triangle(m_ms_ndc_positions.at(x, y), fvec2(ndc_vertices[0]),
							fvec2(ndc_vertices[1]), fvec2(ndc_vertices[2])))
						{
							triangle_start_left_x = x;
							break;
						}
					}
					if (triangle_start_left_x == end_x)
					{
						continue;
					}
					for (std::int32_t x = int(end_x) - 1; x >= start_x; --x)
					{
						if (is_point_inside_triangle(m_ms_ndc_positions.at(x, y), fvec2(ndc_vertices[0]),
							fvec2(ndc_vertices[1]), fvec2(ndc_vertices[2])))
						{
							triangle_end_right_x = x;
							break;
						}
					}
					if (triangle_end_right_x == end_x)
					{
						continue;
					}
					for (std::uint32_t x = triangle_start_left_x; x <= triangle_end_right_x; ++x)
					{
						const auto& ndc_position = m_ms_ndc_positions.at(x, y);
						if (stencil_test)
						{
							const auto stencil_pixel_value = m_ms_stencil_buffer.at(x, y);
							if (stencil_function == StencilFunction::LESS)
							{
								if (stencil_pixel_value < stencil_value)
								{
									continue;
								}
							}
							else
							{
								if (stencil_pixel_value > stencil_value)
								{
									continue;
								}
							}
						}
						std::array<float, 3> scales;
						for (int i = 0; i < 3; ++i)
						{
							const auto point =
								line_intersection(fvec2(ndc_vertices[i]), ndc_position, fvec2(ndc_vertices[(i + 1) % 3]),
									fvec2(ndc_vertices[(i + 2) % 3]));
							const auto d1 = fvec2(ndc_vertices[i]) - point;
							const auto d2 = fvec2(ndc_position) - point;
							const auto fraction =
								std::sqrt(d2.x * d2.x + d2.y * d2.y) / std::sqrt(d1.x * d1.x + d1.y * d1.y);
							scales[i] = fraction;
						}

						const auto total_scale = scales[0] + scales[1] + scales[2];

						const float z_value = 1.0F / ((1.0F / t_vertices[0].position.w * scales[0] +
							1.0F / t_vertices[1].position.w * scales[1] +
							1.0F / t_vertices[2].position.w * scales[2]) /
							total_scale);

						std::scoped_lock lock(m_ms_mutex_buffer[y * m_width * m_multisample + x]);
						if (zbuffer_test_and_set(x, y, z_value))
						{
							// m_ms_color_buffer.at(x, y)
							auto pixel = t_texture.sample(
								fvec2{ ((t_vertices[0].texture_uv.x / t_vertices[0].position.w) * scales[0] +
									   (t_vertices[1].texture_uv.x / t_vertices[1].position.w) * scales[1] +
									   (t_vertices[2].texture_uv.x / t_vertices[2].position.w) * scales[2]) /
										  total_scale * z_value,
									  ((t_vertices[0].texture_uv.y / t_vertices[0].position.w) * scales[0] +
									   (t_vertices[1].texture_uv.y / t_vertices[1].position.w) * scales[1] +
									   (t_vertices[2].texture_uv.y / t_vertices[2].position.w) * scales[2]) /
										  total_scale * z_value });
							if (fog_params.enable)
							{
								const auto fraction =
									std::clamp((z_value - fog_params.start) / (fog_params.end - fog_params.start), 0.0F,
										1.0F) *
									fog_params.destiny;
								pixel = Pixel(fast_lerp(pixel.r, fog_params.color.r, fraction),
									fast_lerp(pixel.g, fog_params.color.g, fraction),
									fast_lerp(pixel.b, fog_params.color.b, fraction));
							}
							m_ms_color_buffer.at(x, y) = pixel;
						}
					}
				}
			}
		}

		inline void sample() noexcept
		{
			if (m_multisample == 1)
			{
				m_color_buffer = m_ms_color_buffer;
				return;
			}
			std::vector<std::uint32_t> r_values(std::size_t(m_multisample) * m_multisample);
			std::vector<std::uint32_t> g_values(std::size_t(m_multisample) * m_multisample);
			std::vector<std::uint32_t> b_values(std::size_t(m_multisample) * m_multisample);
			for (std::uint32_t x = 0; x < m_width; ++x)
			{
				for (std::uint32_t y = 0; y < m_height; ++y)
				{
					for (std::uint32_t mx = 0; mx < m_multisample; ++mx)
					{
						for (std::uint32_t my = 0; my < m_multisample; ++my)
						{
							r_values[my * m_multisample + mx] =
								m_ms_color_buffer.at(x * m_multisample + mx, y * m_multisample + my).r;
							g_values[my * m_multisample + mx] =
								m_ms_color_buffer.at(x * m_multisample + mx, y * m_multisample + my).g;
							b_values[my * m_multisample + mx] =
								m_ms_color_buffer.at(x * m_multisample + mx, y * m_multisample + my).b;
						}
					}
					m_color_buffer.at(x, y) =
						Pixel(std::accumulate(r_values.begin(), r_values.end(), 0) / (r_values.size()),
							std::accumulate(g_values.begin(), g_values.end(), 0) / (g_values.size()),
							std::accumulate(b_values.begin(), b_values.end(), 0) / (b_values.size()));
				}
			}
		}

		inline void render_to_file(std::string_view t_tga_filename) const
		{
			TGA t_tga(m_width, m_height);
			for (std::uint16_t x = 0; x < m_width; ++x)
			{
				for (std::uint16_t y = 0; y < m_height; ++y)
				{
					t_tga.get_pixel(u16vec2{ x, y }) = m_color_buffer.at(x, y);
				}
			}
			t_tga.to_file(t_tga_filename);
		}

		inline void draw_z_buffer() noexcept
		{
			auto& z_buffer = m_ms_depth_buffer.data();
			const auto min = *std::min_element(z_buffer.begin(), z_buffer.end());
			float max{ min };
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
				if (z_buffer[i] <= -1.0F)
				{
					m_ms_color_buffer.data()[i] = Pixel{ color_value, 0, 0 };
				}
				else if (z_buffer[i] <= 0.0F)
				{
					m_ms_color_buffer.data()[i] = Pixel{ color_value, color_value, 0 };
				}
				else if (z_buffer[i] <= 1.0F)
				{
					m_ms_color_buffer.data()[i] = Pixel{ 0, color_value, 0 };
				}
				else
				{
					m_ms_color_buffer.data()[i] = Pixel{ 0, 0, color_value };
				}
			}
		}

		[[nodiscard]] inline const ColorBuffer& get_color_buffer() const noexcept
		{
			return m_color_buffer;
		}

		[[nodiscard]] constexpr std::uint8_t get_multisample() const noexcept
		{
			return m_multisample;
		}

		void set_multisample(std::uint8_t t_multisample) noexcept
		{
			if (t_multisample == m_multisample || t_multisample == 0)
			{
				return;
			}
			m_multisample = t_multisample;
			m_ms_depth_buffer = DepthBuffer(m_width * m_multisample, m_height * m_multisample);
			m_ms_color_buffer = ColorBuffer(m_width * m_multisample, m_height * m_multisample);
			m_ms_stencil_buffer = StencilBuffer(m_width * m_multisample, m_height * m_multisample);
			m_ms_ndc_positions = NDCBuffer(m_width * m_multisample, m_height * m_multisample);
			m_ms_mutex_buffer = std::make_unique<std::mutex[]>(m_width * m_height * m_multisample * m_multisample);
			fill_ndc_positions();
		}

	private:
		// size
		std::uint16_t m_width;
		std::uint16_t m_height;

		std::uint8_t m_multisample{ 1 };

		// color buffer
		ColorBuffer m_color_buffer;

		// depth buffer
		ColorBuffer m_ms_color_buffer;

		DepthBuffer m_ms_depth_buffer;
		NDCBuffer m_ms_ndc_positions;

		std::unique_ptr<std::mutex[]> m_ms_mutex_buffer;

		// stencil buffer
		StencilBuffer m_ms_stencil_buffer;
	};
}; // namespace ubv
