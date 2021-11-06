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
		: m_width{t_width}, m_height{t_height}, m_color_buffer{m_width, m_height}, //m_ms_color_buffer{m_width, m_height},
		  m_ms_depth_buffer{m_width, m_height}, //m_ms_stencil_buffer{m_width, m_height},
	m_ms_pixel_info_buffer{ m_width, m_height  }
		  //m_ms_ndc_positions{m_width, m_height}//, m_ms_mutex_buffer{std::make_unique<std::mutex[]>(m_width * m_height)}
	{
		//fill_ndc_positions();
	}

	/*inline void fill_ndc_positions() noexcept
	{
		for (std::uint32_t x = 0; x < m_width * m_multisample; ++x)
		{
			for (std::uint32_t y = 0; y < m_height * m_multisample; ++y)
			{
				m_ms_ndc_positions.at(x, y) = fvec2{static_cast<float>(x) / static_cast<float>(m_width),
													static_cast<float>(y) / static_cast<float>(m_height)} /
												  static_cast<float>(m_multisample) * 2.0F -
											  1.0F;
			}
		}
	}*/

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
		triangles = 0;
		sampled_pixels = 0;

		//m_color_buffer.clear();
		//m_ms_color_buffer.clear();
		m_ms_depth_buffer.clear();
		m_ms_pixel_info_buffer.clear();
		// m_ms_stencil_buffer.clear();
	}

	inline bool zbuffer_test(std::uint_fast16_t t_x, std::uint_fast16_t t_y, float t_depth)
	{
		return (t_depth < m_ms_depth_buffer.at(t_x, t_y));
	}

	inline bool zbuffer_test_and_set(std::uint_fast16_t t_x, std::uint_fast16_t t_y, float t_depth)
	{
		float &existing_depth = m_ms_depth_buffer.at(t_x, t_y);
		if (t_depth < existing_depth)
		{
			existing_depth = t_depth;
			return true;
		}
		return false;
	}

	constexpr bool is_point_inside_triangle(const fvec2 &s, const fvec2 &a, const fvec2 &b,
											const fvec2 &c) const noexcept
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

	constexpr fvec2 line_intersection(const fvec2 &A, const fvec2 &B, const fvec2 &C, const fvec2 &D) const noexcept
	{
		const float a1 = B.y - A.y;
		const float b1 = A.x - B.x;
		const float c1 = a1 * (A.x) + b1 * (A.y);

		const float a2 = D.y - C.y;
		const float b2 = C.x - D.x;
		const float c2 = a2 * (C.x) + b2 * (C.y);

		const float determinant = a1 * b2 - a2 * b1;

		const auto inverted_determinant = 1.0F / determinant;

		const float x = (b2 * c1 - b1 * c2) * inverted_determinant;
		const float y = (a1 * c2 - a2 * c1) * inverted_determinant;

		return fvec2{x, y};
	}

	struct FogParams
	{
		float start = 2.0F;
		float end = 4.5F;
		Pixel color = Pixel{142, 64, 41};
		float destiny = 0.95F;
		bool enable = false;
	} fog_params;

	constexpr bool cull_test(const std::array<Vertex, 3> &t_vertices)
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
		const Vertex &t_wrong_vertex, const std::array<Vertex, 2> &t_correct_vertices)
	{
		const auto fraction0 =
			(-t_wrong_vertex.position.z) / (t_correct_vertices[0].position.z - t_wrong_vertex.position.z);
		const auto fraction1 =
			(-t_wrong_vertex.position.z) / (t_correct_vertices[1].position.z - t_wrong_vertex.position.z);

		const auto vertex0 = t_wrong_vertex.interpolate(t_correct_vertices[0], fraction0);
		const auto vertex1 = t_wrong_vertex.interpolate(t_correct_vertices[1], fraction1);

		return {std::array<Vertex, 3>{vertex0, t_correct_vertices[0], t_correct_vertices[1]},
				{vertex0, vertex1, t_correct_vertices[1]}};
	}

	constexpr std::array<Vertex, 3> clip_with_two_wrong_vertices(const std::array<Vertex, 2> &t_wrong_vertices,
																 const Vertex &t_correct_vertex)
	{
		const auto fraction0 =
			(-t_wrong_vertices[0].position.z) / (t_correct_vertex.position.z - t_wrong_vertices[0].position.z);
		const auto fraction1 =
			(-t_wrong_vertices[1].position.z) / (t_correct_vertex.position.z - t_wrong_vertices[1].position.z);

		const auto vertex0 = t_wrong_vertices[0].interpolate(t_correct_vertex, fraction0);
		const auto vertex1 = t_wrong_vertices[1].interpolate(t_correct_vertex, fraction1);

		return {vertex0, vertex1, t_correct_vertex};
	}

	constexpr float calculate_weight(const fvec2& t_delta, const float t_offset, const fvec2& t_point)
	{
		return 0.0F;
	}

	bool stencil_test = false;
	enum class StencilFunction : std::uint8_t
	{
		LESS,
		GREATER
	} stencil_function = StencilFunction::GREATER;
	std::uint8_t stencil_value = 0x80;

	/*inline void draw_to_stencil_buffer() noexcept
	{
		for (std::uint32_t i = 0; i < m_ms_color_buffer.data().size(); ++i)
		{
			m_ms_stencil_buffer.data()[i] =
				(unsigned(m_ms_color_buffer.data()[i].r) + unsigned(m_ms_color_buffer.data()[i].g) +
				 unsigned(m_ms_color_buffer.data()[i].b)) /
				3U;
		}
	}*/

	bool depth_test = true;

	unsigned triangles = 0;

	std::uint64_t sampled_pixels = 0;

	constexpr float triangle_area(const fvec2& A, const fvec2& B, const fvec2& C) const noexcept
	{
		return A.x * (B.y - C.y) + B.x * (C.y - A.y) + C.x * (A.y - B.y);
	}

	constexpr float triangle_area(float constant1, float constant2, float constant3, const fvec2& C)
	{
		return constant1 + C.y * constant2 + C.x * constant3;
	}

	template<bool ForceDraw = false>
	inline void draw_triangle(const std::array<Vertex, 3> &t_vertices, const Texture &t_texture)
	{
		if constexpr (!ForceDraw)
		{
			if (!cull_test(t_vertices))
			{
				return;
			}

			std::uint8_t wrong_vertices_indexes_size = 0;
			std::uint8_t correct_vertices_indexes_size = 0;

			std::array<std::uint8_t, 3> wrong_vertices_indexes;
			std::array<std::uint8_t, 3> correct_vertices_indexes;

			for (std::uint8_t i = 0; i < 3; ++i)
			{
				if (t_vertices[i].position.z < 0.0F)
				{
					wrong_vertices_indexes[wrong_vertices_indexes_size] = i;
					++wrong_vertices_indexes_size;
				}
				else
				{
					correct_vertices_indexes[correct_vertices_indexes_size] = i;
					++correct_vertices_indexes_size;
				}
			}

			if (wrong_vertices_indexes_size == 1)
			{
				const auto triangles = clip_with_one_wrong_vertex(
					t_vertices[wrong_vertices_indexes[0]],
					{t_vertices[correct_vertices_indexes[0]], t_vertices[correct_vertices_indexes[1]]});
				for (const auto &triangle : triangles)
				{
					draw_triangle<true>(triangle, t_texture);
				}
				return;
			}

			if (wrong_vertices_indexes_size == 2)
			{
				const auto triangle = clip_with_two_wrong_vertices(
					{t_vertices[wrong_vertices_indexes[0]], t_vertices[wrong_vertices_indexes[1]]},
					t_vertices[correct_vertices_indexes[0]]);
				return draw_triangle<true>(triangle, t_texture);
			}
		}

		//std::cout << "Rendering triangle " << triangles << '\n';
		++triangles;

		const std::array<float, 3> inverted_w_values = {
			1.0F / t_vertices[0].position.w, 1.0F / t_vertices[1].position.w, 1.0F / t_vertices[2].position.w};

		const std::array<fvec2, 3> ndc_vertices = {static_cast<fvec2>(t_vertices[0].position) * inverted_w_values[0],
												   static_cast<fvec2>(t_vertices[1].position) * inverted_w_values[1],
												   static_cast<fvec2>(t_vertices[2].position) * inverted_w_values[2]};

		const static auto ms_width = static_cast<float>(m_width) * static_cast<float>(m_multisample);
		const static auto ms_height = static_cast<float>(m_height) * static_cast<float>(m_multisample);

		const static auto inv_ms_2width = 0.5F * ms_width;
		const static auto inv_ms_2height = 0.5F * ms_height;

		const static auto inv_inv_ms_2width = 2.0F * (1.0F / ms_width);
		const static auto inv_inv_ms_2height = 2.0F * (1.0F / ms_height);

		std::array<fvec2, 3> vertices = {
			fvec2((ndc_vertices[0].x + 1.0F) * inv_ms_2width, (ndc_vertices[0].y + 1.0F) * inv_ms_2height),
			fvec2((ndc_vertices[1].x + 1.0F) * inv_ms_2width, (ndc_vertices[1].y + 1.0F) * inv_ms_2height),
			fvec2((ndc_vertices[2].x + 1.0F) * inv_ms_2width, (ndc_vertices[2].y + 1.0F) * inv_ms_2height),
		};

		std::uint32_t start_x =
			std::max<float>(std::min<float>({vertices[0].x, vertices[1].x, vertices[2].x}) - 1.0F, 0.0F);
		std::uint32_t end_x = std::min<std::uint32_t>(
			std::max<float>({vertices[0].x, vertices[1].x, vertices[2].x}) + 1, ms_width);

		std::uint32_t start_y =
			std::max<float>(std::min<float>({vertices[0].y, vertices[1].y, vertices[2].y}) - 1.0F, 0.0F);
		std::uint32_t end_y = std::min<std::uint32_t>(
			std::max<float>({vertices[0].y, vertices[1].y, vertices[2].y}) + 1, ms_height);

		const auto By = ndc_vertices[1].y;
		const auto Ay = ndc_vertices[0].y;
		const auto Cy = ndc_vertices[2].y;
		const auto Ax = ndc_vertices[0].x;
		const auto Bx = ndc_vertices[1].x;
		const auto Cx = ndc_vertices[2].x;

		const auto true_area_abc = triangle_area(ndc_vertices[0], ndc_vertices[1], ndc_vertices[2]);
		const auto inv_true_area_abc = 1.0F / true_area_abc;

		const auto constant1C = Ax * By - Bx * Ay;
		const auto constant2C = -Ax + Bx;
		const auto constant3C = Ay - By;

		const auto constant1B = Ax * Cy - Cx * Ay;
		const auto constant2B = -Ax + Cx;
		const auto constant3B = Ay - Cy;

		for (std::uint32_t y = start_y; y < end_y; ++y)
		{
			const auto ndc_position_y = float(y) * inv_inv_ms_2height - 1.0F;
			std::uint32_t triangle_start_left_x = end_x;
			std::uint32_t triangle_end_right_x = end_x;
			for (std::uint32_t x = start_x; x < end_x; ++x)
			{
				const auto ndc_position_x = float(x) * inv_inv_ms_2width - 1.0F;
				if (is_point_inside_triangle(fvec2(ndc_position_x, ndc_position_y), fvec2(ndc_vertices[0]),
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
				const auto ndc_position_x = float(x) * inv_inv_ms_2width - 1.0F;
				if (is_point_inside_triangle(fvec2(ndc_position_x, ndc_position_y), fvec2(ndc_vertices[0]),
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
			const auto y_c_scale = constant1C + ndc_position_y * constant2C;
			const auto y_b_scale = constant1B + ndc_position_y * constant2B;
			for (std::uint32_t x = triangle_start_left_x; x <= triangle_end_right_x; ++x)
			{
				const auto ndc_position = fvec2(float(x) * inv_inv_ms_2width - 1.0F, ndc_position_y);
				//if (stencil_test)
				/*if (false)
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
				}*/
				//std::array<float, 3> scales;
				//const auto c_scale = inverse_cdx * ndc_position.x + inverse_cdy * ndc_position.y + inverse_cK;
				//const auto b_scale = inverse_bdx * ndc_position.x + inverse_bdy * ndc_position.y + inverse_bK;
				const auto c_scale = std::abs((y_c_scale + ndc_position.x * constant3C) * inv_true_area_abc);
				const auto b_scale = std::abs((y_b_scale + ndc_position.x * constant3B) * inv_true_area_abc);
				const auto a_scale = 1.0F - c_scale - b_scale;
				const std::array<float, 3> scales{a_scale, b_scale, c_scale};
				/*for (int i = 0; i < 2; ++i)
				{
					const auto point =
						line_intersection(fvec2(ndc_vertices[i]), ndc_position, fvec2(ndc_vertices[i + 1]),
										  fvec2(ndc_vertices[i == 0 ? 2 : 0]));
					const auto d1 = fvec2(ndc_vertices[i]) - point;
					const auto d2 = fvec2(ndc_position) - point;
					const auto fraction = std::sqrt(d2.x * d2.x + d2.y * d2.y) / std::sqrt(d1.x * d1.x + d1.y * d1.y);
					scales[i] = fraction;
				}
				scales[2] = 1.0F - scales[0] - scales[1];*/

				const static auto aspect_ratio = float(m_width) / float(m_height);

				const auto inverted_w_value =
					(inverted_w_values[0] * scales[0] + inverted_w_values[1] * scales[1] +
						inverted_w_values[2] * scales[2]);

				const auto w_value = 1.0F / inverted_w_value;

				if (!depth_test || zbuffer_test_and_set(x, y, w_value))
				{
					float fog_fraction = 0.0F;
					if (fog_params.enable)
					{
						const float x_value =
							(ndc_vertices[0].x * scales[0] + ndc_vertices[1].x * scales[1] + ndc_vertices[2].x * scales[2]) *
							w_value * aspect_ratio;

						const float y_value =
							(ndc_vertices[0].y * scales[0] + ndc_vertices[1].y * scales[1] + ndc_vertices[2].y * scales[2]) *
							w_value;

						const float distance = std::sqrt(w_value * w_value + x_value * x_value + y_value * y_value);

						const static auto inverted_fog_disance = 1.0F / (fog_params.end - fog_params.start);
						fog_fraction =
							std::clamp((distance - fog_params.start) * inverted_fog_disance, 0.0F,
								1.0F) *
							fog_params.destiny;
					}
					m_ms_pixel_info_buffer.at(x, y) = pixel_info{ &t_texture, fog_fraction, fvec2{((t_vertices[0].texture_uv.x * inverted_w_values[0]) * scales[0] +
												(t_vertices[1].texture_uv.x * inverted_w_values[1]) * scales[1] +
												(t_vertices[2].texture_uv.x * inverted_w_values[2]) * scales[2]) *
												   w_value,
											   ((t_vertices[0].texture_uv.y * inverted_w_values[0]) * scales[0] +
												(t_vertices[1].texture_uv.y * inverted_w_values[1]) * scales[1] +
												(t_vertices[2].texture_uv.y * inverted_w_values[2]) * scales[2]) *
												   w_value} };
					/*++sampled_pixels;
					auto pixel =
						t_texture.sample(fvec2{((t_vertices[0].texture_uv.x * inverted_w_values[0]) * scales[0] +
												(t_vertices[1].texture_uv.x * inverted_w_values[1]) * scales[1] +
												(t_vertices[2].texture_uv.x * inverted_w_values[2]) * scales[2]) *
												   w_value,
											   ((t_vertices[0].texture_uv.y * inverted_w_values[0]) * scales[0] +
												(t_vertices[1].texture_uv.y * inverted_w_values[1]) * scales[1] +
												(t_vertices[2].texture_uv.y * inverted_w_values[2]) * scales[2]) *
												   w_value});
					if (fog_params.enable)
					{
						const float x_value =
							(ndc_vertices[0].x * scales[0] + ndc_vertices[1].x * scales[1] + ndc_vertices[2].x * scales[2]) *
							w_value * aspect_ratio;

						const float y_value =
							(ndc_vertices[0].y * scales[0] + ndc_vertices[1].y * scales[1] + ndc_vertices[2].y * scales[2]) *
							w_value;

						const float distance = std::sqrt(w_value * w_value + x_value * x_value + y_value * y_value);

						const static auto inverted_fog_disance = 1.0F / (fog_params.end - fog_params.start);
						const auto fraction =
							std::clamp((distance - fog_params.start) * inverted_fog_disance, 0.0F,
									   1.0F) *
							fog_params.destiny;
						pixel = Pixel(fast_lerp(pixel.r, fog_params.color.r, fraction),
									  fast_lerp(pixel.g, fog_params.color.g, fraction),
									  fast_lerp(pixel.b, fog_params.color.b, fraction));
					}
					m_ms_color_buffer.at(x, y) = pixel;
					*/

				}
			}
		}
	}

	/*constexpr std::uint8_t accumulate(const std::vector<std::uint32_t>& t_values, unsigned t_size)
	{
		unsigned t_total_value{};
		for (unsigned i = 0; i < t_size; ++i)
		{
			t_total_value += t_values[i];
		}
		return t_total_value / t_size;
	}*/

	inline void sample_simple() noexcept
	{
		for (std::uint32_t x = 0; x < m_width; ++x)
		{
			for (std::uint32_t y = 0; y < m_height; ++y)
			{
				const auto& pixel_info = m_ms_pixel_info_buffer.at(x, y);
				if (!pixel_info.texture)
				{
					//r_values[ms_index] = fog_params.color.r;
					//g_values[ms_index] = fog_params.color.g;
					//b_values[ms_index] = fog_params.color.b;
					continue;
				}
				auto pixel = pixel_info.texture->sample(pixel_info.uv);
				const auto fraction = pixel_info.fog_fraction;
				if (fraction > 0.0F)
				{
					pixel = Pixel(fast_lerp(pixel.r, fog_params.color.r, fraction),
						fast_lerp(pixel.g, fog_params.color.g, fraction),
						fast_lerp(pixel.b, fog_params.color.b, fraction));
				}
				m_color_buffer.at(x, y) = pixel;
			}
		}
	}

	inline void sample() noexcept
	{
		if (m_multisample == 1)
		{
			return sample_simple();
		}
		static const unsigned ms_times_ms = std::size_t(m_multisample) * m_multisample;
		static std::vector<std::uint32_t> r_values(ms_times_ms);
		static std::vector<std::uint32_t> g_values(ms_times_ms);
		static std::vector<std::uint32_t> b_values(ms_times_ms);
		for (std::uint32_t x = 0; x < m_width; ++x)
		{
			for (std::uint32_t y = 0; y < m_height; ++y)
			{
				const auto y_times_multisample = y * m_multisample;
				for (std::uint32_t mx = 0; mx < m_multisample; ++mx)
				{
					const auto x_times_multisample = x * m_multisample;
					for (std::uint32_t my = 0; my < m_multisample; ++my)
					{
						const auto index = u32vec2{ x_times_multisample + mx, y_times_multisample + my };
						const auto& pixel_info = m_ms_pixel_info_buffer.at(index.x, index.y);
						const auto ms_index = my * m_multisample + mx;
						if (!pixel_info.texture)
						{
							//r_values[ms_index] = fog_params.color.r;
							//g_values[ms_index] = fog_params.color.g;
							//b_values[ms_index] = fog_params.color.b;
							continue;
						}
						++sampled_pixels;
						auto pixel = pixel_info.texture->sample(pixel_info.uv);
						const auto fraction = pixel_info.fog_fraction;
						if (fraction > 0.0F)
						{
							pixel = Pixel(fast_lerp(pixel.r, fog_params.color.r, fraction),
								fast_lerp(pixel.g, fog_params.color.g, fraction),
								fast_lerp(pixel.b, fog_params.color.b, fraction));
						}
						r_values[ms_index] = pixel.r;
						g_values[ms_index] = pixel.g;
						b_values[ms_index] = pixel.b;
					}
				}
				//m_color_buffer.at(x, y) =
					//Pixel(accumulate(r_values, ms_times_ms),
					    //  accumulate(g_values, ms_times_ms),
						 // accumulate(b_values, ms_times_ms));
				m_color_buffer.at(x, y) =
					Pixel(std::accumulate(r_values.begin(), r_values.end(), 0U) / ms_times_ms,
						std::accumulate(g_values.begin(), g_values.end(), 0U) / ms_times_ms,
						std::accumulate(b_values.begin(), b_values.end(), 0U) / ms_times_ms);
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
				t_tga.get_pixel(x, y) = m_color_buffer.at(x, y);
			}
		}
		t_tga.to_file(t_tga_filename);
	}

	/*inline void draw_z_buffer() noexcept
	{
		auto &z_buffer = m_ms_depth_buffer.data();
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
			if (z_buffer[i] <= -1.0F)
			{
				m_ms_color_buffer.data()[i] = Pixel{color_value, 0, 0};
			}
			else if (z_buffer[i] <= 0.0F)
			{
				m_ms_color_buffer.data()[i] = Pixel{color_value, color_value, 0};
			}
			else if (z_buffer[i] <= 1.0F)
			{
				m_ms_color_buffer.data()[i] = Pixel{0, color_value, 0};
			}
			else
			{
				m_ms_color_buffer.data()[i] = Pixel{0, 0, color_value};
			}
		}
	}*/

	[[nodiscard]] inline const ColorBuffer &get_color_buffer() const noexcept
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
		//m_ms_color_buffer = ColorBuffer(m_width * m_multisample, m_height * m_multisample);
		m_ms_pixel_info_buffer = decltype(m_ms_pixel_info_buffer)(m_width * m_multisample, m_height * m_multisample);
		//m_ms_stencil_buffer = StencilBuffer(m_width * m_multisample, m_height * m_multisample);
		//m_ms_ndc_positions = NDCBuffer(m_width * m_multisample, m_height * m_multisample);
		//m_ms_mutex_buffer = std::make_unique<std::mutex[]>(m_width * m_height * m_multisample * m_multisample);
		//fill_ndc_positions();
	}

  private:
	// size
	std::uint16_t m_width;
	std::uint16_t m_height;

	std::uint8_t m_multisample{1};

	// color buffer
	ColorBuffer m_color_buffer;

	// depth buffer
	//ColorBuffer m_ms_color_buffer;

	DepthBuffer m_ms_depth_buffer;
	//NDCBuffer m_ms_ndc_positions;
	struct pixel_info
	{
		const Texture* texture;
		float fog_fraction;
		fvec2 uv;
	};
	Buffer < pixel_info, pixel_info{ nullptr } > m_ms_pixel_info_buffer;
	//std::unique_ptr<std::mutex[]> m_ms_mutex_buffer;

	// stencil buffer
	//StencilBuffer m_ms_stencil_buffer;
};
}; // namespace ubv
