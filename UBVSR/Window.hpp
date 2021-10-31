#pragma once

#if defined(_WIN32)
	#include <Windows.h>
#elif defined(__unix__)
	#include <X11/Xlib.h>
	#include <X11/Xutil.h>
#endif

#include <chrono>
#include <cstdint>
#include <iostream>
#include <string>
#include <map>

#include "Framebuffer.hpp"

namespace ubv {
	
	enum Keys
	{
		Escape,
		W,
		S,
		A,
		D,
		Up,
		Down,
		Left,
		Right,
		Z,
		Enter
	};

	const static inline std::map<Keys, unsigned> keys_linux = {
		{ Keys::Escape, 0x00 },
		{ Keys::W, 0x00 },
		{ Keys::S, 0x00 },
		{ Keys::A, 0x00 },
		{ Keys::D, 0x00 },
		{ Keys::Up, 0x00 },
		{ Keys::Down, 0x00 },
		{ Keys::Left, 0x00 },
		{ Keys::Right, 0x00 },
		{ Keys::Z, 44 },
		{ Keys::Enter, 0x00 }
	};

	const static inline std::map<Keys, unsigned> keys_windows = {
		{ Keys::Escape, 0x1B },
		{ Keys::Up, 0x26 },
		{ Keys::Down, 0x28 },
		{ Keys::Left, 0x25 },
		{ Keys::Right, 0x27 },
		{ Keys::Z, 0x5A },
		{ Keys::Enter, 0x0D },
		{ Keys::W, 0x57 },
		{ Keys::S, 0x53 },
		{ Keys::A, 0x41 },
		{ Keys::D, 0x44 }
	};

	struct WindowProps {
		std::uint16_t width;
		std::uint16_t height;
		std::string title;

		inline explicit WindowProps(std::uint16_t t_width = 0, std::uint16_t t_height = 0,
									   std::string t_title = "") noexcept
			: width{ t_width }, height{ t_height }, title{ std::move(t_title) } { }
	};

	class Window {
	  	public:
			inline explicit Window(WindowProps t_win_props = WindowProps{ }) noexcept : m_win_props{ std::move(t_win_props) } { }
			virtual ~Window() noexcept = default;

			// Disable copying
			Window(const Window &) = delete;
			Window &operator=(const Window &) = delete;

			virtual void create() = 0;
			virtual void update() = 0;
			virtual void display(const FrameBuffer& t_framebuffer) = 0;
			virtual void destroy() = 0;

			virtual bool IsKeyPressed(int keycode) = 0;

			[[nodiscard]] constexpr std::uint16_t get_win_width() const noexcept {
				return m_win_props.width;
			}

			[[nodiscard]] constexpr std::uint16_t get_win_height() const noexcept {
				return m_win_props.height;
			}

			[[nodiscard]] constexpr const std::string &get_win_title() const noexcept {
				return m_win_props.title;
			}

	  	private:
			WindowProps m_win_props;
	};
};
