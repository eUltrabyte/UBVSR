#pragma once

#include <array>
#include <cstdint>
#include <vector>

struct Pixel
{
	std::uint8_t r;
	std::uint8_t g;
	std::uint8_t b;
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
		return m_pixels_data[t_y * m_width + t_x];
	}

	[[nodiscard]] inline const Pixel &at(std::uint32_t t_x, std::uint32_t t_y) const noexcept
	{
		return m_pixels_data[t_y * m_width + t_x];
	}

	[[nodiscard]] inline const std::vector<Pixel> &get_pixel_data() const noexcept
	{
		return m_pixels_data;
	}

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

  private:
	std::uint32_t m_width;
	std::uint32_t m_height;
	std::vector<Pixel> m_pixels_data;
};
