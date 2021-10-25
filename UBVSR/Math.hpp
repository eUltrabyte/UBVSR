#pragma once

#include <cstdint>
#include <type_traits>

namespace ubv {
	template<typename T> struct vec2 {
		static_assert(std::is_arithmetic_v<T>, "Type must be arithmetic");

		T x;
		T y;

		constexpr explicit vec2() noexcept = default;
		constexpr explicit vec2(T t_x, T t_y) noexcept : x{ t_x }, y{ t_y } { }
		template<typename U>
		constexpr explicit vec2(vec2<U> t_vec) noexcept : x{ static_cast<T>(t_vec.x) }, y{static_cast<T>(t_vec.y)} { }
	};

	template<typename T> struct vec3 {
		static_assert(std::is_arithmetic_v<T>, "Type must be arithmetic");

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

		constexpr vec3<T> operator+(const vec3<T>& t_vec) const noexcept
		{
			return vec3<T>{x + t_vec.x, y + t_vec.y, z + t_vec.z};
		}
	};

	using fvec2 = vec2<float>;
	using fvec3 = vec3<float>;

	using u16vec2 = vec2<std::uint16_t>;
	using u16vec3 = vec3<std::uint16_t>;

	using u32vec2 = vec2<std::uint32_t>;
	using u32vec3 = vec3<std::uint32_t>;

	template<typename T> struct mat4x4 {
		static_assert(std::is_arithmetic_v<T>, "Type must be arithmetic");

		std::array<std::array<T, 4>, 4> matrix;

		constexpr explicit mat4x4() noexcept = default;
		constexpr explicit mat4x4(T t_value1, T t_value2, T t_value3, T t_value4, T t_value5, T t_value6) noexcept {
			matrix[0][0] = t_value1;
			matrix[1][1] = t_value2;
			matrix[2][2] = t_value3;
			matrix[3][2] = t_value4;
			matrix[2][3] = t_value5;
			matrix[3][3] = t_value6;
		}
		
		T* get_mat() noexcept {
			return matrix[0].data();
		}

		template<typename U>
		T& get_mat_at(vec2<U> t_vec) noexcept {
			return matrix[t_vec.x][t_vec.y];
		}

		template<typename U>
		void set_mat_at(vec2<U> t_vec, T t_data) noexcept {
			matrix[t_vec.x][t_vec.y] = t_data;
		}

		fvec3 multiply(fvec3 t_input) const noexcept {
			/*fvec3 output{
				t_input.x * matrix[0][0] + t_input.y * matrix[1][0] + t_input.z * matrix[2][0] + matrix[3][0],
				t_input.x * matrix[0][1] + t_input.y * matrix[1][1] + t_input.z * matrix[2][1] + matrix[3][1],
				t_input.x * matrix[0][2] + t_input.y * matrix[1][2] + t_input.z * matrix[2][2] + matrix[3][2]
			};
			
			const float w = t_input.x * matrix[0][3] + t_input.y * matrix[1][3] + t_input.z * matrix[2][3] + matrix[3][3];
			
			if(w != 0.0F) {
				output.x /= w;
				output.y /= w;
				output.z /= w;
			}
			
			return output;*/
			const auto w{  t_input.x * matrix[0][3] + t_input.y * matrix[1][3] + t_input.z * matrix[2][3] + matrix[3][3] };
			return fvec3{ (t_input.x * matrix[0][0] + t_input.y * matrix[1][0] + t_input.z * matrix[2][0] + matrix[3][0]) / w,
				          (t_input.x * matrix[0][1] + t_input.y * matrix[1][1] + t_input.z * matrix[2][1] + matrix[3][1]) / w,
				          (t_input.x * matrix[0][2] + t_input.y * matrix[1][2] + t_input.z * matrix[2][2] + matrix[3][2]) / w};
		}
	};

	using fmat4x4 = mat4x4<float>;
};