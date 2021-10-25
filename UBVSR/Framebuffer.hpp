#pragma once

#include "Pixel.hpp"
#include "Math.hpp"
#include "Vertex.hpp"
#include "Texture.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <map>
#include <set>
#include <vector>
#include <future>

namespace ubv {
	class FrameBuffer {
	public:
		inline explicit FrameBuffer(std::uint32_t t_width, std::uint32_t t_height) noexcept
			: m_width{ t_width }, m_height{ t_height }, m_pixels_data{ t_width * t_height }, m_zbuffer{ (float)t_width * t_height, std::numeric_limits<float>::infinity() } { }
	
		[[nodiscard]] constexpr std::uint32_t get_width() const noexcept {
			return m_width;
		}
	
		[[nodiscard]] constexpr std::uint32_t get_height() const noexcept {
			return m_height;
		}
	
		[[nodiscard]] inline Pixel& at(std::uint32_t t_x, std::uint32_t t_y) noexcept {
			if (t_x < m_width && t_y < m_height)
				return m_pixels_data[t_y * m_width + t_x];
			return null_pixel;
		}
	
		[[nodiscard]] inline const Pixel& at(std::uint32_t t_x, std::uint32_t t_y) const noexcept {
			if (t_x < m_width && t_y < m_height)
				return m_pixels_data[t_y * m_width + t_x];
			return null_pixel;
		}
		
		[[nodiscard]] inline float& at_zbuffer(std::uint32_t t_x, std::uint32_t t_y) noexcept {
			if (t_x < m_width && t_y < m_height)
				return m_zbuffer[t_y * m_width + t_x];
			static float inf = std::numeric_limits<float>::infinity();
			return inf;
		}
	
		[[nodiscard]] inline const std::vector<Pixel>& get_pixel_data() const noexcept {
			return m_pixels_data;
		}
	
		inline static Pixel null_pixel = Pixel();
	
		inline void clear(Pixel t_color) noexcept {
			for(auto& pixel : m_pixels_data) {
				pixel = t_color;
			}
			m_zbuffer.clear();
			m_zbuffer.resize(m_width * m_height, std::numeric_limits<float>::infinity());
		}
		
		inline bool zbuffer_test_and_set(u16vec2 t_position, float t_depth) {
			float& existing_depth = at_zbuffer(t_position.x, t_position.y);
			if (t_depth < existing_depth)
			{
				existing_depth = t_depth;
				return true;
			}
			return false;
		}

		inline void set_pixel(std::uint32_t t_x, std::uint32_t t_y, Pixel t_color) noexcept {
			at(t_x, t_y) = t_color;
		}
	
		inline void set_pixel(std::uint32_t t_x, std::uint32_t t_y, vec2<std::uint32_t> t_size, Pixel t_color) noexcept {
			for(std::uint32_t y = 0; y < (t_y + t_size.y); ++y) {
				for(std::uint32_t x = 0; x < (t_x + t_size.x); ++x) {
					at(x, y) = t_color;
				}
			}
		}
	
		[[deprecated]] inline Pixel interpolate(Pixel t_color1, Pixel t_color2, float t_fraction) const noexcept {
			t_fraction = std::clamp(t_fraction, 0.0F, 1.0F);
			return Pixel{ std::uint8_t((float(t_color2.r) - float(t_color1.r)) * t_fraction + float(t_color1.r)),
						  std::uint8_t((float(t_color2.g) - float(t_color1.g)) * t_fraction + float(t_color1.g)),
						  std::uint8_t((float(t_color2.b) - float(t_color1.b)) * t_fraction + float(t_color1.b)) };
		}
	
		[[deprecated]] inline void draw_line(fvec2 t_p1, fvec2 t_p2, Pixel t_color1, Pixel t_color2) noexcept {
			bool steep = false;
			if(std::abs(t_p1.x - t_p2.x) < std::abs(t_p1.y - t_p2.y)) {
				std::swap(t_p1.x, t_p1.y);
				std::swap(t_p2.x, t_p2.y);
				steep = true;
			}
	
			if(t_p1.x > t_p2.x) {
				std::swap(t_p1.x, t_p2.x);
				std::swap(t_p1.y, t_p2.y);
				std::swap(t_color1, t_color2);
			}
	
			int error = 0;
			int y = t_p1.y;
	
			for(int x = t_p1.x; x <= t_p2.x; ++x) {
				const float total_distance = t_p2.x - t_p1.x;
				const float passed_distance = x - t_p1.x;
				const float fraction = passed_distance / total_distance;
	
				if(steep) {
					set_pixel(y, x, interpolate(t_color1, t_color2, fraction));
				} else {
					set_pixel(x, y, interpolate(t_color1, t_color2, fraction));
				}
	
				error += std::abs(t_p2.y - t_p1.y) * 2;
				if(error > (t_p2.x - t_p1.x)) {
					y += (t_p2.y > t_p1.y ? 1 : -1);
					error -= (t_p2.x - t_p1.x) * 2;
				}
			}
		}
	
		[[deprecated]] inline void draw_fancy_glowy_circle(float t_x, float t_y, float t_size) noexcept {
			for(unsigned x = 0; x < m_width; ++x) {
				for(unsigned y = 0; y < m_height; ++y) {
					float distance = std::sqrt((std::abs(t_x - float(x)) * std::abs(t_x - float(x))) + (std::abs(t_y - float(y)) * std::abs(t_y - float(y))));
					if(distance < t_size) {
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
	
		inline bool is_point_inside_triangle(fvec2 s, fvec2 a, fvec2 b, fvec2 c) const noexcept {
			auto as_x = s.x - a.x;
			auto as_y = s.y - a.y;
	
			bool s_ab = (b.x - a.x) * as_y - (b.y - a.y) * as_x > 0.0F;
	
			if((c.x - a.x) * as_y - (c.y - a.y) * as_x > 0.0F == s_ab) {
				return false;
			}
	
			if((c.x - b.x) * (s.y - b.y) - (c.y - b.y) * (s.x - b.x) > 0.0F != s_ab) {
				return false;
			}
	
			return true;
		}
	
		inline fvec2 line_intersection(fvec2 A, fvec2 B, fvec2 C, fvec2 D) {
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
		
		inline float interpolate_z_value(float t_z1, float t_z2, float t_fraction)
		{
			return 1.0F / (1.0F / t_z1 + t_fraction * (1.0F / t_z2 - 1.0F / t_z1));
		}

		inline void draw_triangle(const std::array<Vertex, 3> &t_vertices, const Texture& t_texture) {

			std::array<fvec3, 3> vertices = { fvec3((t_vertices[0].position.x + 1.0F) / 2.0 * float(get_width()), (t_vertices[0].position.y + 1.0F) / 2.0 * float(get_height()), 1.0F / t_vertices[0].position.z),
											  fvec3((t_vertices[1].position.x + 1.0F) / 2.0 * float(get_width()), (t_vertices[1].position.y + 1.0F) / 2.0 * float(get_height()), 1.0F / t_vertices[1].position.z),
											  fvec3((t_vertices[2].position.x + 1.0F) / 2.0 * float(get_width()), (t_vertices[2].position.y + 1.0F) / 2.0 * float(get_height()), 1.0F / t_vertices[2].position.z)
			};

			std::cout << t_vertices[0].position.x << ", " << t_vertices[0].position.y << std::endl;

			const std::uint32_t start_x = std::max<float>(std::min<float>({vertices[0].x, vertices[1].x, vertices[2].x}) - 1, 0);
			const std::uint32_t end_x = std::min<std::uint32_t>(std::max<float>({ vertices[0].x, vertices[1].x , vertices[2].x })+1, m_width);

			const std::uint32_t start_y = std::max<float>(std::min<float>({ vertices[0].y, vertices[1].y , vertices[2].y })-1, 0);
			const std::uint32_t end_y = std::min<std::uint32_t>(std::max<float>({ vertices[0].y, vertices[1].y , vertices[2].y })+1, m_height);

			for(std::uint32_t x = start_x; x < end_x; ++x) {
				for(std::uint32_t y = start_y; y < end_y; ++y) {
					const fvec2 p = fvec2{ static_cast<float>(x), static_cast<float>(y) };
					if(is_point_inside_triangle(p, fvec2(vertices[0]), fvec2(vertices[1]), fvec2(vertices[2]))) {
						std::array<float, 3> scales;
						// float z_value;
						for(int i = 0; i < 3; ++i) {
							auto point = line_intersection(fvec2(vertices[i]), p, fvec2(vertices[(i + 1) % 3]), fvec2(vertices[(i + 2) % 3]));
							const auto dx = vertices[i].x - point.x;
							const auto dy = vertices[i].y - point.y;
							const auto d2x = p.x - point.x;
							const auto d2y = p.y - point.y;
							auto total_distance = std::sqrt(dx * dx + dy * dy);
							auto distance = std::sqrt(d2x * d2x + d2y * d2y);
							auto fraction = distance / total_distance;
							scales[i] = fraction;

							//Z value interpolation
							//if(i == 0) {
								//auto dx = vertices[1].x - vertices[2].x;
								//auto dy = vertices[1].y - vertices[2].y;

								//auto dx2 = vertices[1].y - point.x;
								//auto dy2 = vertices[1].y - point.y;

								//auto d1 = std::sqrt(dx * dx + dy * dy);
								//auto d2 = std::sqrt(dx2 * dx2 + dy2 * dy2);

								//1 mozliwosc - nie dziala
								//auto z_value1 = interpolate_z_value(vertices[1].z, vertices[2].z, d2 / d1);
								//z_value = interpolate_z_value(z_value1, vertices[0].z, fraction);

								//2 mozliwosc - nie dziala
								//auto z_value1 = interpolate_z_value(vertices[2].z, vertices[1].z, d2 / d1);
								//z_value = interpolate_z_value(z_value1, vertices[0].z, fraction);

								//3 mozliwosc - nie dziala
								//auto z_value1 = interpolate_z_value(vertices[1].z, vertices[2].z, d2 / d1);
								//z_value = interpolate_z_value(vertices[0].z, z_value1, fraction);

								//4 mozliwosc - 
								//auto z_value1 = interpolate_z_value(vertices[2].z, vertices[1].z, d2 / d1);
								//z_value = interpolate_z_value(vertices[0].z, z_value1, fraction);
							//}
						}

						const auto total_scale = scales[0] + scales[1] + scales[2];

						const float z_value = 1.0F / ((1.0F / vertices[0].z * scales[0] +
								   					   1.0F / vertices[1].z * scales[1] +
								   					   1.0F / vertices[2].z * scales[2]) / total_scale);
	
						if(zbuffer_test_and_set(u16vec2(x, y), z_value)) {
							set_pixel(x, y, t_texture.sample(fvec2{ (
							    (t_vertices[0].texture_uv.x / vertices[0].z) * scales[0] +
								(t_vertices[1].texture_uv.x / vertices[1].z) * scales[1] +
								(t_vertices[2].texture_uv.x / vertices[2].z) * scales[2]) / total_scale / (1.0F / z_value), (
								(t_vertices[0].texture_uv.y / vertices[0].z) * scales[0] +
								(t_vertices[1].texture_uv.y / vertices[1].z) * scales[1] +
								(t_vertices[2].texture_uv.y / vertices[2].z) * scales[2]) / total_scale / (1.0F / z_value) } ));
						}
					}
				}
			}

		}

		inline void draw_z_buffer() noexcept
		{
			const auto min = *std::min_element(m_zbuffer.begin(), m_zbuffer.end());
			float max{min};
			for (const float z_value : m_zbuffer)
			{
				if (z_value > max && z_value != std::numeric_limits<float>::infinity())
				{
					max = z_value;
				}
			}
			const auto max_distance = max - min;
			for (std::uint32_t i = 0; i < m_zbuffer.size(); ++i)
			{
				std::uint8_t color_value = std::clamp((m_zbuffer[i] - min) / max_distance * 255.0F, 0.0F, 255.0F);
				m_pixels_data[i] = Pixel{ color_value, color_value, color_value };
			}
		}
	
	private:
		std::uint32_t m_width;
		std::uint32_t m_height;

		//color buffer
		std::vector<Pixel> m_pixels_data;

		//z buffer
		std::vector<float> m_zbuffer;
	};
};