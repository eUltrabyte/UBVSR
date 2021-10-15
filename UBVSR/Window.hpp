#pragma once

#if defined(_WIN32)
#include <Windows.h>
#elif defined(__unix__)
#include <X11/Xlib.h>
#endif

#include <cstdint>
#include <iostream>
#include <string>
#include <chrono>

namespace ubv
{
struct WindowProps
{
	std::uint16_t width;
	std::uint16_t height;
	std::string title;

	explicit inline WindowProps(std::uint16_t t_width = 0, std::uint16_t t_height = 0,
								std::string t_title = "") noexcept
		: width{t_width}, height{t_height}, title{std::move(t_title)}
	{
	}
};

class Window
{
  public:
	explicit Window(WindowProps t_win_props = WindowProps{});
	virtual ~Window() noexcept = default;

	// Disable copying
	Window(const Window &) = delete;
	Window &operator=(const Window &) = delete;

	virtual void create() = 0;
	virtual void update() = 0;
	virtual void destroy() = 0;

	virtual void set_pixel(std::uint16_t t_x, std::uint16_t t_y, std::uint8_t t_color_r, std::uint8_t t_color_g,
						   std::uint8_t t_color_b) = 0;

	std::uint16_t get_win_width() const noexcept;
	std::uint16_t get_win_height() const noexcept;
	const std::string &get_win_title() const noexcept;

  private:
	WindowProps m_win_props;
};
}; // namespace ubv
