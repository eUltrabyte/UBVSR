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
};