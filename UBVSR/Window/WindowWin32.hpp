#pragma once

#if defined(_WIN32)

#include "../Window.hpp"

namespace ubv
{
class WindowWin32 : public Window
{
  public:
	WindowWin32(WindowProps t_win_props);
	virtual ~WindowWin32();

	void create() final;
	void update() final;
	void destroy() final;

	void set_pixel(std::uint16_t t_x, std::uint16_t t_y, std::uint8_t t_color_r, std::uint8_t t_color_g,
				   std::uint8_t t_color_b) final;

	HINSTANCE get_hinstance() const;
	HWND get_hwnd() const;
	HDC get_hdc() const;

  private:
	HINSTANCE m_hinstance;
	HWND m_hwnd;
	HDC m_hdc;
};
}; // namespace ubv

#endif
