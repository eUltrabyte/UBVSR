#pragma once

#if defined(_WIN32)

#include "../Window.hpp"

namespace ubv
{
class WindowWin32 final : public Window
{
  public:
	WindowWin32(WindowProps t_win_props);
	virtual ~WindowWin32() noexcept;

	void create() final;
	void update() final;
	void destroy() noexcept final;

	inline void display()
	{
		if (!BitBlt(m_hdc, 0, 0, get_win_width(), get_win_height(), m_memDC, 0, 0, SRCCOPY))
		{
			throw std::runtime_error("Failed to bit blt");
		}
	}

	void set_pixel(std::uint16_t t_x, std::uint16_t t_y, std::uint8_t t_color_r, std::uint8_t t_color_g,
				   std::uint8_t t_color_b) final;

	[[nodiscard]] constexpr HINSTANCE get_hinstance() const noexcept
	{
		return m_hinstance;
	}
	[[nodiscard]] constexpr HWND get_hwnd() const noexcept
	{
		return m_hwnd;
	}
	[[nodiscard]] constexpr HDC get_hdc() const noexcept
	{
		return m_hdc;
	}

public:

	HBITMAP m_bitmap;
	HDC m_memDC;

  private:
	HINSTANCE m_hinstance;
	HWND m_hwnd;
	HDC m_hdc;
};
}; // namespace ubv

#endif