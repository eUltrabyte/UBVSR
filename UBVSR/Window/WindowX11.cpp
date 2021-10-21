#include "WindowX11.hpp"

#if defined(__unix__)

namespace ubv
{
WindowX11::WindowX11(WindowProps t_win_props) : Window(std::move(t_win_props))
{
	m_display = XOpenDisplay(0);
	m_screen = DefaultScreen(m_display);
	create();
}

WindowX11::~WindowX11()
{
	destroy();
}

void WindowX11::create()
{
	m_window = XCreateSimpleWindow(m_display, RootWindow(m_display, m_screen), 10, 10, get_win_width(), get_win_height(), 1,
								   BlackPixel(m_display, m_screen), WhitePixel(m_display, m_screen));
	XStoreName(m_display, m_window, get_win_title().c_str());
	XSelectInput(m_display, m_window, ExposureMask | KeyPressMask);
	XMapWindow(m_display, m_window);
	XFlush(m_display);
}

void WindowX11::update()
{
	XNextEvent(m_display, &m_event);

    if (m_event.type == Expose)
	{
		XPutImage(
		    m_display,
		    m_window,
		    DefaultGC(m_display, m_screen),
		    m_image,
		    0, 0, 0, 0,
		    get_win_width(), get_win_height()
		);
		XSync(m_display, 1);
		XFlush(m_display);
	}

	if (m_event.type == KeyPress)
	{
		//std::quick_exit(0);
	}
}

void WindowX11::destroy() noexcept
{
    XDestroyImage(m_image);
	XCloseDisplay(m_display);
}

XEvent* WindowX11::get_event() {
    return &m_event;
}

}; // namespace ubv

#endif
