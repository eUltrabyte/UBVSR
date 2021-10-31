#pragma once

#if defined(__unix__)

#include "../Window.hpp"

namespace ubv {
	class WindowX11 final : public Window {
	public:
		WindowX11(WindowProps t_win_props);
		virtual ~WindowX11() noexcept;
		void create() final;
		void update() final;
		void destroy() noexcept final;

		inline void display(const FrameBuffer &t_frame_buffer) final {
			static std::vector<Pixel> data;
			data = t_frame_buffer.get_color_buffer().data();


			for (std::uint16_t y=0;y<t_frame_buffer.get_height();++y)
			{
				for (std::uint16_t x=0;x<t_frame_buffer.get_width();++x) {
		            data[(t_frame_buffer.get_height() - y - 1) * t_frame_buffer.get_width() + x] = t_frame_buffer.get_color_buffer().data()[y * t_frame_buffer.get_width() + x];
		        }
			}

			m_image = XCreateImage(
				m_display,
				DefaultVisual(m_display, 0),
				DefaultDepth(m_display, 0),
				ZPixmap,
				0,
				reinterpret_cast<char*>(data.data()),
				get_win_width(),
				get_win_height(),
				32,
				0
			);
		}

		[[nodiscard]] inline bool IsKeyPressed(int keycode) noexcept {
			switch(m_event.type) {
				case KeyPress:
					printf( "KeyPress: %x\n", m_event.xkey.keycode );
					if (m_event.xkey.keycode == keycode) {
						return true;
					}
					break;

				case KeyRelease:
					printf( "KeyRelease: %x\n", m_event.xkey.keycode );
					if (m_event.xkey.keycode == keycode) {
						return true;
					}
					break;

				default:
					return false;
					break;
			}
		}

		XEvent* get_event();

	private:
		Display *m_display;
		int m_screen;
		::Window m_window;
		XEvent m_event;
		XImage* m_image;
	};
};

#endif
