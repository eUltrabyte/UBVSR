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
		// SelectObject(m_memDC, m_bitmap);
		if (!BitBlt(m_hdc, 0, 0, get_win_width(), get_win_height(), m_memDC, 0, 0, SRCCOPY))
		{
			throw std::runtime_error("Failed to bit blt");
		}
	}

	inline void display_old(FrameBuffer &t_frame_buffer)
	{
		t_frame_buffer.clear(Pixel(200, 50, 50));
		HBITMAP map =
			CreateBitmap(t_frame_buffer.get_width(),  // width. 512 in my case
						 t_frame_buffer.get_height(), // height
						 1,     // Color Planes, unfortanutelly don't know what is it actually. Let it be 1
						 8 * 3, // Size of memory for one pixel in bits (in win32 4 bytes = 4*8 bits)
						 reinterpret_cast<const void *>(t_frame_buffer.get_pixel_data().data())); // pointer to array
		// Temp HDC to copy picture

		HDC src = CreateCompatibleDC(
			m_memDC);           // hdc - Device context for window, I've got earlier with GetDC(hWnd) or GetDC(NULL);
		SelectObject(src, map); // Inserting picture into our temp HDC
								// m_memDC = src;
		//  Copy image from temp HDC to window
		if (!BitBlt(m_memDC,                     // Destination
					0,                           // x and
					0,                           // y - upper-left corner of place, where we'd like to copy
					t_frame_buffer.get_width(),  // width of the region
					t_frame_buffer.get_height(), // height
					src,                         // source
					0,                           // x and
					0,        // y of upper left corner  of part of the source, from where we'd like to copy
					SRCCOPY)) // Defined DWORD to juct copy pixels. Watch more on msdn;
		{
			throw std::runtime_error("Failed to bit blt");
		}

		DeleteDC(src); // Deleting temp HDC
		display();
	}

	inline void display(FrameBuffer &t_frame_buffer)
	{
		SIZE sBmp = {t_frame_buffer.get_width(), t_frame_buffer.get_height()};
		LPBITMAPINFOHEADER pbi; // the bitmap header from the file, etc.
		std::memset(&pbi, 0, sizeof(LPBITMAPINFO));
		pbi.biSize = 0;
		pbi.biWidth = sBmp.cx;
		pbi.biHeight = sBmp.cy;
		pbi.biPlanes = 1; // ???
		pbi.biBitCount = ;
		pbi.biCompression;
		pbi.biSizeImage;
		pbi.biXPelsPerMeter;
		pbi.biYPelsPerMeter;
		pbi.biClrUsed;
		pbi.biClrImportant;
		LPVOID pvBits = (void *)t_frame_buffer.get_pixel_data().data(); // the raw bitmap bits

		StretchDIBits(m_hdc, 0, 0, sBmp.cx, sBmp.cy, 0, 0, sBmp.cx, sBmp.cy, pvBits, (BITMAPINFO *)&pbi, DIB_RGB_COLORS,
					  SRCCOPY);
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
