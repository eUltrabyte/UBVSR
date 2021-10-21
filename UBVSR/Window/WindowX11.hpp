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

	inline void display(const FrameBuffer &t_frame_buffer) final
	{
        std::reverse(t_frame_buffer.get_pixel_data().begin(), t_frame_buffer.get_pixel_data().end());
		m_image = XCreateImage(
			m_display,
			DefaultVisual(m_display, 0),
			DefaultDepth(m_display, 0),
			ZPixmap, //it can be XYPixmap, or ZPixmap.
			0,
			(char*)t_frame_buffer.get_pixel_data().data(),
			get_win_width(),
			get_win_height(),
			32, //nwm
			0
		);
	}

    XEvent* get_event();

  private:
	Display *m_display;
	int m_screen;
	::Window m_window;
	XEvent m_event;
    XImage* m_image;
};
}; // namespace ubv

#endif
