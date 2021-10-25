#pragma once

#if defined(_WIN32)

#include "../Window.hpp"

namespace ubv {
	class WindowWin32 final : public Window {
  	public:
		WindowWin32(WindowProps t_win_props);
		virtual ~WindowWin32() noexcept;

		void create() final;
		void update() final;
		void destroy() noexcept final;

		[[nodiscard]] inline std::vector<std::byte> create_packed_dib(const FrameBuffer &t_frame_buffer) {
			static std::vector<std::byte> buffer{ sizeof(BITMAPINFOHEADER) + t_frame_buffer.get_pixel_data().size() * 3U };
			static BITMAPINFOHEADER pbi;
			pbi.biSize = sizeof(BITMAPINFOHEADER);
			pbi.biWidth = (LONG)t_frame_buffer.get_width();
			pbi.biHeight = (LONG)t_frame_buffer.get_height();
			pbi.biPlanes = 1;
			pbi.biBitCount = 24;
			pbi.biCompression = BI_RGB;
			pbi.biSizeImage = 0;
			pbi.biXPelsPerMeter = 2835;
			pbi.biYPelsPerMeter = 2835;
			pbi.biClrUsed = 3;
			pbi.biClrImportant = 0;
			*(reinterpret_cast<BITMAPINFOHEADER*>(buffer.data())) = pbi;
			std::memcpy(buffer.data() + sizeof(BITMAPINFOHEADER), t_frame_buffer.get_pixel_data().data(),
						t_frame_buffer.get_pixel_data().size() * 3U);
			return buffer;
		}

		inline virtual void display(const FrameBuffer &t_frame_buffer) final {
			static RECT rect;
			SetRect(&rect, 0, 0, t_frame_buffer.get_width(), t_frame_buffer.get_height());
			FillRect(m_hdc, &rect, CreateDIBPatternBrushPt((void*)create_packed_dib(t_frame_buffer).data(), DIB_RGB_COLORS));
		}

		[[nodiscard]] constexpr HINSTANCE get_hinstance() const noexcept {
			return m_hinstance;
		}

		[[nodiscard]] constexpr HWND get_hwnd() const noexcept {
			return m_hwnd;
		}

		[[nodiscard]] constexpr HDC get_hdc() const noexcept {
			return m_hdc;
		}

  	private:
		HINSTANCE m_hinstance;
		HWND m_hwnd;
		HDC m_hdc;
	};
};

#endif