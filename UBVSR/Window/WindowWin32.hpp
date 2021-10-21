#pragma once

#include "../Window.hpp"

#if defined(_WIN32)

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

	[[nodiscard]] inline std::vector<std::byte> create_packed_dib(FrameBuffer &t_frame_buffer)
	{
		static std::vector<std::byte> buffer{sizeof(BITMAPINFOHEADER) + t_frame_buffer.get_pixel_data().size() * 3U};
		static SIZE sBmp = {t_frame_buffer.get_width(), t_frame_buffer.get_height()};
		static BITMAPINFOHEADER pbi;
		pbi.biSize = sizeof(BITMAPINFOHEADER);
		pbi.biWidth = sBmp.cx;
		pbi.biHeight = sBmp.cy;
		pbi.biPlanes = 1;
		pbi.biBitCount = 24;
		pbi.biCompression = BI_RGB;
		pbi.biSizeImage = 0;
		pbi.biXPelsPerMeter = 2835;
		pbi.biYPelsPerMeter = 2835;
		pbi.biClrUsed = 3; // nie wiem w sumie
		pbi.biClrImportant = 0;
		LPVOID pvBits = (void *)t_frame_buffer.get_pixel_data().data(); // the raw bitmap bits
		*((BITMAPINFOHEADER *)buffer.data()) = pbi;
		std::memcpy(buffer.data() + sizeof(BITMAPINFOHEADER), t_frame_buffer.get_pixel_data().data(),
					t_frame_buffer.get_pixel_data().size() * 3U);
		return buffer;
	}

	inline void display(FrameBuffer &t_frame_buffer)
	{
		static RECT rect;
		HBRUSH brush = CreateDIBPatternBrushPt((void *)create_packed_dib(t_frame_buffer).data(), DIB_RGB_COLORS);
		SetRect(&rect, 0, 0, t_frame_buffer.get_width(), t_frame_buffer.get_height());
		FillRect(m_hdc, &rect, brush);
		DeleteObject(brush);
	}

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

  private:
	HINSTANCE m_hinstance;
	HWND m_hwnd;
	HDC m_hdc;
};
}; // namespace ubv

#endif
