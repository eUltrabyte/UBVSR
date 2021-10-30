#pragma once

#include <cstdint>
#include <type_traits>

namespace ubv
{
template <typename T> struct vec2
{
	static_assert(std::is_arithmetic_v<T>, "Type must be arithmetic");

	T x;
	T y;

	constexpr explicit vec2() noexcept = default;
	constexpr explicit vec2(T t_x, T t_y) noexcept : x{t_x}, y{t_y}
	{
	}
	template <typename U>
	constexpr explicit vec2(const vec2<U> &t_vec) noexcept : x{static_cast<T>(t_vec.x)}, y{static_cast<T>(t_vec.y)}
	{
	}
};

template <typename T> struct vec3
{
	static_assert(std::is_arithmetic_v<T>, "Type must be arithmetic");

	T x;
	T y;
	T z;

	constexpr explicit vec3() noexcept = default;
	constexpr explicit vec3(T t_x, T t_y, T t_z) noexcept : x{t_x}, y{t_y}, z{t_z}
	{
	}

	template <typename U>
	constexpr explicit vec3(const vec2<U> &t_vec, U t_z) noexcept
		: x{static_cast<T>(t_vec.x)}, y{static_cast<T>(t_vec.y)}, z{static_cast<T>(t_z)}
	{
	}
	template <typename U>
	constexpr explicit vec3(U t_x, const vec2<U> &t_vec) noexcept
		: x{static_cast<T>(t_x)}, y{static_cast<T>(t_vec.x)}, z{static_cast<T>(t_vec.y)}
	{
	}
	template <typename U>
	constexpr explicit vec3(const vec3<U> &t_vec) noexcept
		: x{static_cast<T>(t_vec.x)}, y{static_cast<T>(t_vec.y)}, z{static_cast<T>(t_vec.z)}
	{
	}

	template <typename U> constexpr explicit operator vec2<U>() const noexcept
	{
		return vec2<U>{static_cast<U>(x), static_cast<U>(y)};
	}

	constexpr vec3<T> operator+(const vec3<T> &t_vec) const noexcept
	{
		return vec3<T>{x + t_vec.x, y + t_vec.y, z + t_vec.z};
	}

	constexpr vec3<T> operator-(const vec3<T>& t_vec) const noexcept
	{
		return vec3<T>{x - t_vec.x, y - t_vec.y, z - t_vec.z};
	}

	constexpr vec3<T> operator*(const vec3<T>& t_vec) const noexcept
	{
		return vec3<T>{x * t_vec.x, y * t_vec.y, z * t_vec.z};
	}

	constexpr vec3<T> operator/(const vec3<T>& t_vec) const noexcept
	{
		return vec3<T>{x / t_vec.x, y / t_vec.y, z / t_vec.z};
	}
};

template <typename T> struct vec4
{
	static_assert(std::is_arithmetic_v<T>, "Type must be arithmetic");

	T x;
	T y;
	T z;
	T w;

	constexpr explicit vec4() noexcept = default;
	constexpr explicit vec4(T t_x, T t_y, T t_z, T t_w) noexcept : x{t_x}, y{t_y}, z{t_z}, w{t_w}
	{
	}

	template <typename U>
	constexpr explicit vec4(const vec3<U> &t_vec, U t_w) noexcept
		: x{static_cast<T>(t_vec.x)}, y{static_cast<T>(t_vec.y)}, z{static_cast<T>(t_vec.z)}, w{static_cast<T>(t_w)}
	{
	}
	template <typename U>
	constexpr explicit vec4(const vec2<U> &t_vec, U t_z, U t_w) noexcept
		: x{static_cast<T>(t_vec.x)}, y{static_cast<T>(t_vec.y)}, z{static_cast<T>(t_z)}, w{static_cast<T>(t_w)}
	{
	}
	template <typename U>
	constexpr explicit vec4(const vec2<U> &t_vec1, const vec2<U> &t_vec2) noexcept
		: x{static_cast<T>(t_vec1.x)}, y{static_cast<T>(t_vec1.y)}, z{static_cast<T>(t_vec2.x)}, w{static_cast<T>(
																									 t_vec2.y)}
	{
	}
	template <typename U>
	constexpr explicit vec4(const vec4<U> &t_vec) noexcept
		: x{static_cast<T>(t_vec.x)}, y{static_cast<T>(t_vec.y)}, z{static_cast<T>(t_vec.z)}, w{static_cast<T>(t_vec.w)}
	{
	}

	constexpr vec4<T> operator+(const vec4<T> &t_vec) const noexcept
	{
		return vec4<T>{x + t_vec.x, y + t_vec.y, z + t_vec.z, w + t_vec.w};
	}

	constexpr vec4<T> operator-(const vec4<T> &t_vec) const noexcept
	{
		return vec4<T>{x - t_vec.x, y - t_vec.y, z - t_vec.z, w - t_vec.w};
	}

	constexpr vec4<T> operator*(const vec4<T> &t_vec) const noexcept
	{
		return vec4<T>{x * t_vec.x, y * t_vec.y, z * t_vec.z, w * t_vec.w};
	}

	constexpr vec4<T> operator/(const vec4<T> &t_vec) const noexcept
	{
		return vec4<T>{x / t_vec.x, y / t_vec.y, z / t_vec.z, w / t_vec.w};
	}

	constexpr vec4<T> operator*(T t_value) const noexcept
	{
		return vec4<T>{x * t_value, y * t_value, z * t_value, w * t_value};
	}

	constexpr vec4<T> operator/(T t_value) const noexcept
	{
		return vec4<T>{x / t_value, y / t_value, z / t_value, w / t_value};
	}

	template <typename U> constexpr explicit operator vec3<U>() const noexcept
	{
		return vec3<U>{static_cast<U>(x), static_cast<U>(y), static_cast<U>(z)};
	}

	template <typename U> constexpr explicit operator vec2<U>() const noexcept
	{
		return vec2<U>{static_cast<U>(x), static_cast<U>(y)};
	}
};

using fvec2 = vec2<float>;
using fvec3 = vec3<float>;
using fvec4 = vec4<float>;

using u16vec2 = vec2<std::uint16_t>;
using u16vec3 = vec3<std::uint16_t>;
using u16vec4 = vec4<std::uint16_t>;

using u32vec2 = vec2<std::uint32_t>;
using u32vec3 = vec3<std::uint32_t>;
using u32vec4 = vec4<std::uint32_t>;

template <typename T> struct mat4x4
{
	static_assert(std::is_arithmetic_v<T>, "Type must be arithmetic");

	std::array<std::array<T, 4>, 4> matrix = {std::array<T, 4>{0, 0, 0, 0}, std::array<T, 4>{0, 0, 0, 0},
											  std::array<T, 4>{0, 0, 0, 0}, std::array<T, 4>{0, 0, 0, 0}};

	constexpr explicit mat4x4() noexcept = default;

	// perspective matrix
	inline explicit mat4x4(T t_fov, T t_aspect_ratio, T t_near_z, T t_far_z) noexcept
	{
		const T tan_half_fov_radians = 1.0F / std::tan(t_fov * 0.5F / 180.0F * 3.14159F);
		matrix[0][0] = 1.0F / t_aspect_ratio * tan_half_fov_radians;
		matrix[1][1] = 1.0F / tan_half_fov_radians;
		matrix[2][2] = t_far_z / (t_far_z - t_near_z);
		matrix[2][3] = 1.0F;
		matrix[3][2] = -(2.0F * t_far_z * t_near_z) / (t_far_z - t_near_z);
	}

	// ortho matrix
	constexpr explicit mat4x4(T t_left, T t_right, T t_bottom, T t_top, T t_near_z, T t_far_z) noexcept
	{
		matrix[0][0] = 2.0F / (t_right - t_left);
		matrix[1][1] = 2.0F / (t_top - t_bottom);
		matrix[2][2] = -2.0F / (t_far_z - t_near_z);
		matrix[3][0] = -(t_right + t_left) / (t_right - t_left);
		matrix[3][1] = -(t_top + t_bottom) / (t_top - t_bottom);
		matrix[3][2] = -(t_far_z + t_near_z) / (t_far_z - t_near_z);
	}

	[[nodiscard]] constexpr mat4x4<T> operator*(const mat4x4<T>& t_matrix) const noexcept
	{
		//TODO: Does it work correctly???
		mat4x4<T> result;
		for (std::uint_fast8_t x = 0; x < 4; ++x)
		{
			for (std::uint_fast8_t y = 0; y < 4; ++y)
			{
				for (std::uint_fast8_t z = 0; z < 4; ++z)
				{
					result.matrix[x][y] += matrix[x][z] * t_matrix.matrix[z][y];
				}
			}
		}
		return result;
	}

	[[nodiscard]] constexpr vec4<T> operator*(const vec4<T> &t_vec) const noexcept
	{
		return vec4<T>{
			t_vec.x * matrix[0][0] + t_vec.y * matrix[1][0] + t_vec.z * matrix[2][0] + t_vec.w * matrix[3][0],
			t_vec.x * matrix[0][1] + t_vec.y * matrix[1][1] + t_vec.z * matrix[2][1] + t_vec.w * matrix[3][1],
			t_vec.x * matrix[0][2] + t_vec.y * matrix[1][2] + t_vec.z * matrix[2][2] + t_vec.w * matrix[3][2],
			t_vec.x * matrix[0][3] + t_vec.y * matrix[1][3] + t_vec.z * matrix[2][3] + t_vec.w * matrix[3][3]};
	}

	constexpr void translate(const vec3<T>& t_vec) noexcept = delete;
		/* {
			//TODO: Verify it's correct...
			matrix[3][0] = matrix[0][0] * t_vec.x + matrix[1][0] * t_vec.y + matrix[2][0] * t_vec.z + matrix[3][0];
			matrix[3][1] = matrix[0][1] * t_vec.x + matrix[1][1] * t_vec.y + matrix[2][1] * t_vec.z + matrix[3][1];
			matrix[3][2] = matrix[0][2] * t_vec.x + matrix[1][2] * t_vec.y + matrix[2][2] * t_vec.z + matrix[3][2];
			matrix[3][3] = matrix[0][3] * t_vec.x + matrix[1][3] * t_vec.y + matrix[2][3] * t_vec.z + matrix[3][3];
		}*/

	constexpr void scale(const vec3<T>&t_vec) noexcept = delete;
		/* {
			std::abort();
		}*/

	constexpr void rotate(T t_angle, const vec3<T>&t_vec) noexcept = delete;
	/* {
		std::abort();
	}*/

	/*constexpr explicit mat4x4(T t_value1, T t_value2, T t_value3, T t_value4, T t_value5, T t_value6) noexcept {
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
		fvec3 output{
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

		return output;
		const auto w{  t_input.x * matrix[0][3] + t_input.y * matrix[1][3] + t_input.z * matrix[2][3] + matrix[3][3] };
		return fvec3{ (t_input.x * matrix[0][0] + t_input.y * matrix[1][0] + t_input.z * matrix[2][0] + matrix[3][0]) /
	w, (t_input.x * matrix[0][1] + t_input.y * matrix[1][1] + t_input.z * matrix[2][1] + matrix[3][1]) / w, w};
		*/
};

using fmat4x4 = mat4x4<float>;
using u16mat4x4 = mat4x4<std::uint16_t>;
using u32mat4x4 = mat4x4<std::uint32_t>;

template <typename T>
[[nodiscard]] inline mat4x4<T> projection(T t_fov, T t_aspect_ratio, T t_near_z, T t_far_z) noexcept
{
	return mat4x4<T>{t_fov, t_aspect_ratio, t_near_z, t_far_z};
	/*fmat4x4 matrix;

	const T fov_radians = 1.0F / std::tan(t_fov * 0.5F / 180.0F * 3.14159F);
	matrix[0][0] = 1.0F / t_aspect_ratio * fov_radians;
	matrix[1][1] = 1.0F / fov_radians;
	matrix[2][2] = t_far_z / (t_far_z - t_near_z);
	matrix[2][3] = 1.0F;
	matrix[3][2] = -(2.0F * t_far_z * t_near_z) / (t_far_z - t_near_z);

	return matrix;*/
}

template <typename T>
[[nodiscard]] constexpr mat4x4<T> ortho2d(T t_left, T t_right, T t_bottom, T t_top, T t_near_z, T t_far_z) noexcept
{
	return mat4x4<T>{t_left, t_right, t_bottom, t_top, t_near_z, t_far_z};
	/*fmat4x4 matrix;

	matrix[0][0] = 2.0F / (t_right - t_left);
	matrix[1][1] = 2.0F / (t_top - t_bottom);
	matrix[2][2] = -2.0F / (t_far_z - t_near_z);
	matrix[3][0] = -(t_right + t_left) / (t_right - t_left);
	matrix[3][1] = -(t_top + t_bottom) / (t_top - t_bottom);
	matrix[3][2] = -(t_far_z + t_near_z) / (t_far_z - t_near_z);

	return matrix;*/
}

template <typename T>
[[nodiscard]] inline mat4x4<T> look_at(const vec3<T>& t_eye, const vec3<T>& t_center, const vec3<T>& t_up) noexcept
{
	const vec3<T> f(normalize(t_center - t_eye));
	const vec3<T> s(normalize(cross_product(f, t_up)));
	const vec3<T> u(cross_product(s, f));

	mat4x4<T> matrix{identity<T>()};
	matrix.matrix[0][0] = s.x;
	matrix.matrix[1][0] = s.y;
	matrix.matrix[2][0] = s.z;
	matrix.matrix[0][1] = u.x;
	matrix.matrix[1][1] = u.y;
	matrix.matrix[2][1] = u.z;
	matrix.matrix[0][2] = -f.x;
	matrix.matrix[1][2] = -f.y;
	matrix.matrix[2][2] = -f.z;
	matrix.matrix[3][0] = -dot_product(s, t_eye);
	matrix.matrix[3][1] = -dot_product(u, t_eye);
	matrix.matrix[3][2] = dot_product(f, t_eye);
	return matrix;
}

template <typename T> [[nodiscard]] constexpr T dot_product(const vec3<T> &t_vec1, const vec3<T> &t_vec2) noexcept
{
	return t_vec1.x * t_vec2.x + t_vec1.y * t_vec2.y + t_vec1.z * t_vec2.z;
}

template <typename T>
[[nodiscard]] constexpr vec3<T> cross_product(const vec3<T> &t_vec1, const vec3<T> &t_vec2) noexcept
{
	return vec3<T>{t_vec1.y * t_vec2.z - t_vec1.z * t_vec2.y, t_vec1.z * t_vec2.x - t_vec1.x * t_vec2.z,
				   t_vec1.x * t_vec2.y - t_vec1.y * t_vec2.x};
}

template <typename T>
[[nodiscard]] inline T length(const vec3<T>& t_vec) noexcept
{
	return std::sqrt((t_vec.x * t_vec.x) + (t_vec.y * t_vec.y) + (t_vec.z * t_vec.z));
}

template <typename T>
[[nodiscard]] inline vec3<T> normalize(const vec3<T>& t_vec) noexcept
{
	const T vec_length{ length(t_vec) };
	return vec3<T>{t_vec.x / vec_length, t_vec.y / vec_length, t_vec.z / vec_length};
}

template<typename T>
[[nodiscard]] constexpr mat4x4<T> identity() noexcept
{
	mat4x4<T> matrix;
	matrix.matrix[0][0] = 1.0F;
	matrix.matrix[1][1] = 1.0F;
	matrix.matrix[2][2] = 1.0F;
	matrix.matrix[3][3] = 1.0F;
	return matrix;
}

}; // namespace ubv
