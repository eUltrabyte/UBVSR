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
		constexpr explicit vec2(vec2<U> t_vec) noexcept : x{ }, y{ } { }
		//template<typename U>
		//constexpr explicit vec2(vec3<U> t_vec) : x{ t_vec.x }, y{ t_vec.y } {}
	};

	template<typename T> struct vec3 {
		T x;
		T y;
		T z;

		constexpr explicit vec3() noexcept = default;
		constexpr explicit vec3(T t_x, T t_y, T t_z) noexcept : x{ t_x }, y{ t_y }, z{ t_z } { }

		template<typename U>
		constexpr explicit vec3(vec2<U> t_vec, float t_z) noexcept : x{static_cast<T>(t_vec.x)}, y{static_cast<T>(t_vec.y)}, z{static_cast<T>(t_z)} { }
		template<typename U>
		constexpr explicit vec3(float t_x, vec2<U> t_vec) noexcept : x{ static_cast<T>(t_x) }, y{ static_cast<T>(t_vec.x) }, z{ static_cast<T>(t_vec.y) } { }
		template<typename U>
		constexpr explicit vec3(vec3<U> t_vec) noexcept : x{ static_cast<T>(t_vec.x) }, y{ static_cast<T>(t_vec.y) }, z{ static_cast<T>(t_vec.z) } { }

		template<typename U>
		constexpr explicit operator vec2<U>() const noexcept
		{
			return vec2<U>{ static_cast<U>(x), static_cast<U>(y) };
		}
	};

	using fvec2 = vec2<float>;
	using fvec3 = vec3<float>;

	using u16vec2 = vec2<std::uint16_t>;
	using u16vec3 = vec3<std::uint16_t>;

	using u32vec2 = vec2<std::uint32_t>;
	using u32vec3 = vec3<std::uint32_t>;

};