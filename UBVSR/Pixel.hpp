#pragma once

#include <cstdint>

namespace ubv {
	struct Pixel {
		#if defined(__unix__)
			std::uint8_t b;
			std::uint8_t g;
			std::uint8_t r;
			std::uint8_t a{255};
		#else
			std::uint8_t b;
			std::uint8_t g;
			std::uint8_t r;
		#endif
		
		constexpr explicit Pixel() noexcept = default;
		constexpr explicit Pixel(std::uint8_t t_r, std::uint8_t t_g, std::uint8_t t_b) noexcept : r{ t_r }, g{ t_g }, b{ t_b } { }
	};

	constexpr Pixel interpolate(Pixel t_color1, Pixel t_color2, float t_fraction) noexcept {
			t_fraction = std::clamp(t_fraction, 0.0F, 1.0F);
			return Pixel{ std::uint8_t((float(t_color2.r) - float(t_color1.r)) * t_fraction + float(t_color1.r)),
						  std::uint8_t((float(t_color2.g) - float(t_color1.g)) * t_fraction + float(t_color1.g)),
						  std::uint8_t((float(t_color2.b) - float(t_color1.b)) * t_fraction + float(t_color1.b)) };
	}
};