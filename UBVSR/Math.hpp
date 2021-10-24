#pragma once

#include <cstdint>
#include <type_traits>

namespace ubv {
	template<typename T> struct vec2 {
		T x;
		T y;

		constexpr explicit vec2() noexcept = default;
		constexpr explicit vec2(T t_x, T t_y) noexcept : x{ t_x }, y{ t_y } { }
		template<typename U>
		constexpr explicit vec2(vec2<U> t_vec) : x{ }, y{ } { }
	};

	template<typename T> struct vec3 {
		T x;
		T y;
		T z;

		constexpr explicit vec3() noexcept = default;
		constexpr explicit vec3(T t_x, T t_y, T t_z) noexcept : x{ t_x }, y{ t_y }, z{ t_z } { }
	};

	using fvec2 = vec2<float>;
	using fvec3 = vec3<float>;

	using u16vec2 = vec2<std::uint16_t>;
	using u16vec3 = vec3<std::uint16_t>;

	using u32vec2 = vec2<std::uint32_t>;
	using u32vec3 = vec3<std::uint32_t>;

};