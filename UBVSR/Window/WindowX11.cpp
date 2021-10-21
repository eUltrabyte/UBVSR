#include "WindowX11.hpp"

#if defined(__unix__)

namespace ubv
{
WindowX11::WindowX11(WindowProps t_win_props)
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
	m_window = XCreateSimpleWindow(m_display, RootWindow(m_display, m_screen), 10, 10, 1280, 720, 1,
								   BlackPixel(m_display, m_screen), WhitePixel(m_display, m_screen));

	XSelectInput(m_display, m_window, ExposureMask | KeyPressMask);
	XMapWindow(m_display, m_window);
}

void WindowX11::update()
{
	XNextEvent(m_display, &m_event);
	if (m_event.type == Expose)
	{
		//XSetForeground(display, DefaultGC(display, screen), red.pixel);
		//XFillRectangle(display, window, DefaultGC(display, screen), 20, 20, 10, 10);
		//XDrawPoint(display, window, DefaultGC(display, screen), 50, 50);
	}

	if (m_event.type == KeyPress)
	{
		//break;
	}
}

void WindowX11::destroy()
{
	XCloseDisplay(m_display);
}
}; // namespace ubv

#endif
