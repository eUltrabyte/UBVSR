#if defined(_WIN32)

#include "WindowWin32.hpp"

namespace ubv
{
LRESULT CALLBACK WindowProc(HWND t_hwnd, UINT t_msg, WPARAM t_wparam, LPARAM t_lparam)
{
	switch (t_msg)
	{
	case WM_CLOSE:
		DestroyWindow(t_hwnd);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(t_hwnd, t_msg, t_wparam, t_lparam);
	}
	return 0;
}

WindowWin32::WindowWin32(WindowProps t_win_props) : Window(std::move(t_win_props))
{
	create();
}

WindowWin32::~WindowWin32() noexcept
{
	destroy();
}

void WindowWin32::create()
{
	m_hinstance = GetModuleHandle(0);

	if (!m_hinstance)
	{
		throw std::runtime_error("Failed to get HINSTANCE");
	}

	WNDCLASSEX windowClassEx;
	windowClassEx.cbSize = sizeof(WNDCLASSEX);
	windowClassEx.style = 0;
	windowClassEx.lpfnWndProc = WindowProc;
	windowClassEx.cbClsExtra = 0;
	windowClassEx.cbWndExtra = 0;
	windowClassEx.hInstance = m_hinstance;
	windowClassEx.hIcon = 0;
	windowClassEx.hCursor = LoadCursor(0, IDC_ARROW);
	windowClassEx.hbrBackground = (HBRUSH)10;
	windowClassEx.lpszMenuName = 0;
	windowClassEx.lpszClassName = "UBVSR Win32 Window Class";
	windowClassEx.hIconSm = 0;

	if (!RegisterClassEx(&windowClassEx))
	{
		throw std::runtime_error("Failed to register window class");
	}

	m_hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, "UBVSR Win32 Window Class", get_win_title().c_str(),
							WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT,
							get_win_width(), get_win_height(), 0, 0, m_hinstance, 0);

	if (!m_hwnd)
	{
		UnregisterClass("UBVSR Win32 Window Class", m_hinstance);
		throw std::runtime_error("Failed to create window");
	}


	m_hdc = GetDC(m_hwnd);
	m_memDC = CreateCompatibleDC(m_hdc);
	m_bitmap = CreateCompatibleBitmap(m_hdc, get_win_width(), get_win_height());
	SelectObject(m_memDC, m_bitmap);

	ShowWindow(m_hwnd, SW_SHOW);
}

void WindowWin32::update()
{
 
	if (!BitBlt(m_hdc, 0, 0, get_win_width(), get_win_height(), m_memDC, 0, 0, SRCCOPY))
	{
		throw std::runtime_error("Failed to bit blt");
	}

	MSG message;
	if (GetMessage(&message, 0, 0, 0))
	{
		TranslateMessage(&message);
		DispatchMessage(&message);
	}
	UpdateWindow(m_hwnd);
}

void WindowWin32::destroy() noexcept
{
	PostQuitMessage(0);
	DestroyWindow(m_hwnd);
	UnregisterClass("UBVSR Win32 Window Class", m_hinstance);
}

void WindowWin32::set_pixel(std::uint16_t t_x, std::uint16_t t_y, std::uint8_t t_color_r, std::uint8_t t_color_g,
							std::uint8_t t_color_b)
{
	SetPixel(m_memDC, t_x, t_y, RGB(t_color_r, t_color_g, t_color_b));
}

}; // namespace ubv

#endif
