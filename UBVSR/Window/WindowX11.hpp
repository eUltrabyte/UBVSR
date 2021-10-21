#pragma once

#include "../Window.hpp"

#if defined(__unix__)

namespace ubv
{
class WindowX11 final : public Window
{
	WindowX11(WindowProps t_win_props);
	virtual ~WindowX11() noexcept;

	void create() final;
	void update() final;
	void destroy() noexcept final;

	inline void display(FrameBuffer &t_frame_buffer)
	{

	}

private:
    Display* m_display;
    int m_screen;
	::Window m_window;
    XEvent m_event;
}

#endif