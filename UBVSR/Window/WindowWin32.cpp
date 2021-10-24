#if defined(_WIN32)

#include "WindowWin32.hpp"

namespace ubv {
	LRESULT CALLBACK WindowProc(HWND t_hwnd, UINT t_msg, WPARAM t_wparam, LPARAM t_lparam) {
		switch (t_msg) {
		case WM_CLOSE: {
				DestroyWindow(t_hwnd);
			}
			break;

		case WM_DESTROY: {
				DestroyWindow(t_hwnd);
				PostQuitMessage(0);
				std::quick_exit(0);
			}
			break;

		default:
			return DefWindowProc(t_hwnd, t_msg, t_wparam, t_lparam);
		}

		return 0;
	}

	WindowWin32::WindowWin32(WindowProps t_win_props) : Window{ std::move(t_win_props) } {
		create();
	}

	WindowWin32::~WindowWin32() noexcept {
		destroy();
	}

	void WindowWin32::create() {
		m_hinstance = GetModuleHandle(0);

		if(!m_hinstance) {
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

		if(!RegisterClassEx(&windowClassEx)) {
			throw std::runtime_error("Failed to register window class");
		}

		std::uint16_t x = 50;
		std::uint16_t y = 50;
		std::uint16_t w = get_win_width() + 4;
		std::uint16_t h = get_win_height() + 4;

		RECT rect;
		rect.left = x;
		rect.top = y;
		rect.right = x + w;
		rect.bottom = y + h;

		UINT styles = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

		AdjustWindowRectEx(&rect, styles, 0, 0);

		m_hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, "UBVSR Win32 Window Class", get_win_title().c_str(), styles, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, 0, 0, m_hinstance, 0);

		if(!m_hwnd) {
			UnregisterClass("UBVSR Win32 Window Class", m_hinstance);
			throw std::runtime_error("Failed to create window");
		}

		m_hdc = GetDC(m_hwnd);

		ShowWindow(m_hwnd, SW_SHOW);
	}

	void WindowWin32::update() {
		MSG message;
		if(PeekMessage(&message, 0, 0, 0, PM_REMOVE)) {
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
	}

	void WindowWin32::destroy() noexcept {
		PostQuitMessage(0);
		DestroyWindow(m_hwnd);
		UnregisterClass("UBVSR Win32 Window Class", m_hinstance);
	}
};

#endif