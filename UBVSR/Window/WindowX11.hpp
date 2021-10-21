#pragma once

#include "../Window.hpp"

#if defined(__unix__)

namespace ubv
{
class WindowX11 final : public Window
{
  public:
	WindowX11(WindowProps t_win_props);
	virtual ~WindowX11() noexcept;
	void create() final;
	void update() final;
	void destroy() noexcept final;

	inline void display(FrameBuffer &t_frame_buffer)
	{
		XImage image = XCreateImage(
			m_display,
			DefaultVisual(m_display, 0),
			24,
			XYBitmap, //it can be XYPixmap, or ZPixmap.
			0,
			(char*)t_frame_buffer.get_pixel_data().data(),
			get_width(),
			get_height(),
			32, //nwm
			0
		);

		XPutImage(
		m_display,
		m_window,
		DefaultGC(m_display, m_screen),
		image,
		0, 0, 0, 0,
		get_width(), get_height()
		);

		XDestroyImage(image);
	}

  private:
	Display *m_display;
	int m_screen;
	::Window m_window;
	XEvent m_event;
};
}; // namespace ubv

#endif
