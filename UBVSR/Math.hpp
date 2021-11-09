#pragma once

#include <array>
#include <cmath>
#include <cstdint>
#include <type_traits>

namespace ubv
{

constexpr long double pi = 3.14159265359L;

template <typename T> [[nodiscard]] constexpr T degrees_to_radians(T t_degrees) noexcept
{
	return t_degrees / 180.0F * static_cast<T>(pi);
}

template <typename T> [[nodiscard]] constexpr T radians_to_degrees(T t_radians) noexcept
{
	return t_radians * 180.0F / static_cast<T>(pi);
}

template <typename T> [[nodiscard]] constexpr T abs(T t_value) noexcept
{
	return t_value < 0.0F ? -t_value : t_value;
}

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
	constexpr vec2<T> operator*(T t_value) const noexcept
	{
		return vec2<T>{x * t_value, y * t_value};
	}

	constexpr vec2<T> operator/(T t_value) const noexcept
	{
		return vec2<T>{x / t_value, y / t_value};
	}
	constexpr vec2<T> operator+(T t_value) const noexcept
	{
		return vec2<T>{x + t_value, y + t_value};
	}

	constexpr vec2<T> operator-(T t_value) const noexcept
	{
		return vec2<T>{x - t_value, y - t_value};
	}

	constexpr vec2<T> operator-(const vec2<T> &t_vec) const noexcept
	{
		return vec2<T>{x - t_vec.x, y - t_vec.y};
	}
	constexpr vec2<T> operator+(const vec2<T> &t_vec) const noexcept
	{
		return vec2<T>{x + t_vec.x, y + t_vec.y};
	}
};

template <typename T> [[nodiscard]] constexpr vec2<T> rotate_2d(const vec2<T>& t_point, T t_angle)
{
	const T s = std::sin(t_angle);
	const T c = std::cos(t_angle);

	const T x_new = t_point.x * c - t_point.y * s;
	const T y_new = t_point.x * s + t_point.y * c;

	return vec2<T>{x_new, y_new};
}

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

	constexpr vec3<T> operator-(const vec3<T> &t_vec) const noexcept
	{
		return vec3<T>{x - t_vec.x, y - t_vec.y, z - t_vec.z};
	}

	constexpr vec3<T> operator*(const vec3<T> &t_vec) const noexcept
	{
		return vec3<T>{x * t_vec.x, y * t_vec.y, z * t_vec.z};
	}

	constexpr vec3<T> operator/(const vec3<T> &t_vec) const noexcept
	{
		return vec3<T>{x / t_vec.x, y / t_vec.y, z / t_vec.z};
	}

	constexpr vec3<T> operator/(T t_value) const noexcept
	{
		return vec3<T>{x / t_value, y / t_value, z / t_value};
	}
};

template <typename T>
[[nodiscard]] constexpr vec3<T> rotate_3d(const vec3<T>& t_point, T t_pitch_angle, T t_yaw_angle, T t_roll_angle)
{
	vec3<T> point{ t_point };

	//preform roll rotation (X, Y)
	const auto roll_2d = rotate_2d(vec2<T>(point.x, point.y), t_roll_angle);

	//apply yaw rotation to the point (X , Y)
	point.x = roll_2d.x;
	point.y = roll_2d.y;

	//preform pitch rotation (Y , Z)
	const auto pitch_2d = rotate_2d(vec2<T>(point.y, point.z), t_pitch_angle);

	//apply pitch rotation to the point (Y , Z)
	point.y = pitch_2d.x;
	point.z = pitch_2d.y;

	//preform yaw rotation (X , Z)
	const auto yaw_2d = rotate_2d(vec2<T>(point.x, point.z), t_yaw_angle);
	
	//apply yaw rotation to the point (X , Z)
	point.x = yaw_2d.x;
	point.z = yaw_2d.y;

	//return the rotated point
	return point;
}

/*template <typename T> [[nodiscard]] constexpr vec2<T> rotate(const vec2<T>& t_point, T t_angle)
{
	const T s = std::sin(t_angle);
	const T c = std::cos(t_angle);

	const T new_x = t_point.x * c - t_point.y * s;
	const T new_y = t_point.x * s + t_point.y * c;

	return vec2<T>(-new_x, -new_y);
}

template <typename T>
[[nodiscard]] constexpr vec3<T> rotate(const vec3<T> &t_point, T t_pitch_angle, T t_yaw_angle, T t_roll_angle)
{
	const auto yaw_result = rotate(vec2<T>(t_point.x, t_point.z), t_yaw_angle);
	const auto pitch_result = rotate(vec2<T>(t_point.y, yaw_result.y), t_pitch_angle);
	const auto roll_result = rotate(vec2<T>(yaw_result.x, pitch_result.x), t_roll_angle);
	return vec3<T>{roll_result.x, roll_result.y, pitch_result.y};
}*/

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
		const T tan_half_fov_radians = 1.0F / std::tan(0.5F * degrees_to_radians(t_fov));
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

	// rotation matrix
	constexpr explicit mat4x4(const vec3<T>& t_position, T t_pitch, T t_yaw, T t_roll)
	{
		//vec3<T> right = ubv::rotate(vec3<T>(1, 0, 0), t_pitch, t_yaw, t_roll);
		//vec3<T> up = ubv::rotate(vec3<T>(0, 1, 0), t_pitch, t_yaw, t_roll);
		//vec3<T> forward = ubv::rotate(vec3<T>(0, 0, 1), t_pitch, t_yaw, t_roll);

		const T a = t_pitch;
		const T b = t_yaw;
		const T y = t_roll;

		const T cos_a = std::cos(a);
		const T cos_b = std::cos(b);
		const T cos_y = std::cos(y);

		const T sin_a = std::sin(a);
		const T sin_b = std::sin(b);
		const T sin_y = std::sin(y);

		const vec3<T> r1{cos_a * cos_b, cos_a * sin_b * sin_y - sin_a * cos_y, cos_a * sin_b * cos_y + sin_a * sin_y};
		const vec3<T> r2{sin_a * cos_b, sin_a * sin_b * sin_y + cos_a * cos_y, sin_a * sin_b * cos_y - cos_a * sin_y};
		const vec3<T> r3{-sin_b, cos_b * sin_y, cos_b * cos_y};

		matrix[0][0] = r1.x;
		matrix[0][1] = r1.y;
		matrix[0][2] = r1.z;
		matrix[0][3] = 0;

		matrix[1][0] = r2.x;
		matrix[1][1] = r2.y;
		matrix[1][2] = r2.z;
		matrix[1][3] = 0;

		matrix[2][0] = r3.x;
		matrix[2][1] = r3.y;
		matrix[2][2] = r3.z;
		matrix[2][3] = 0;

		matrix[3][0] = 0;
		matrix[3][1] = 0;
		matrix[3][2] = 0;
		matrix[3][3] = 1;
	}

	constexpr void translate(const vec3<T> &t_vec) noexcept
	{
		matrix[3][0] = matrix[0][0] * t_vec.x + matrix[1][0] * t_vec.y + matrix[2][0] * t_vec.z + matrix[3][0];
		matrix[3][1] = matrix[0][1] * t_vec.x + matrix[1][1] * t_vec.y + matrix[2][1] * t_vec.z + matrix[3][1];
		matrix[3][2] = matrix[0][2] * t_vec.x + matrix[1][2] * t_vec.y + matrix[2][2] * t_vec.z + matrix[3][2];
		matrix[3][3] = matrix[0][3] * t_vec.x + matrix[1][3] * t_vec.y + matrix[2][3] * t_vec.z + matrix[3][3];
	}

	constexpr void rotate(T t_angle, const vec3<T> &t_vec) noexcept
	{
		const T a = t_angle;
		const T c = std::cos(a);
		const T s = std::sin(a);
		mat4x4<T> result;

		const vec3<T> axis = normalize(t_vec);

		result.matrix[0][0] = c + (1.0F - c) * axis.x * axis.x;
		result.matrix[0][1] = (1.0F - c) * axis.x * axis.y + s * axis.z;
		result.matrix[0][2] = (1.0F - c) * axis.x * axis.z - s * axis.y;
		result.matrix[0][3] = 0.0F;

		result.matrix[1][0] = (1.0F - c) * axis.y * axis.x - s * axis.z;
		result.matrix[1][1] = c + (1.0F - c) * axis.y * axis.y;
		result.matrix[1][2] = (1.0F - c) * axis.y * axis.z + s * axis.x;
		result.matrix[1][3] = 0.0F;

		result.matrix[2][0] = (1.0F - c) * axis.z * axis.x + s * axis.y;
		result.matrix[2][1] = (1.0F - c) * axis.z * axis.y - s * axis.x;
		result.matrix[2][2] = c + (1.0F - c) * axis.z * axis.z;
		result.matrix[2][3] = 0.0F;

		result.matrix[3][0] = 0.0F;
		result.matrix[3][1] = 0.0F;
		result.matrix[3][2] = 0.0F;
		result.matrix[3][3] = 1.0F;
		*this = *this * result;
	}

	constexpr void scale(const vec3<T> &t_vec) noexcept
	{
		mat4x4<T> result = identity<T>();
		result.matrix[0][0] = t_vec.x;
		result.matrix[1][1] = t_vec.y;
		result.matrix[2][2] = t_vec.z;
		*this = *this * result;
	}

	[[nodiscard]] constexpr mat4x4<T> operator*(const mat4x4<T> &t_matrix) const noexcept
	{
		// TODO: Does it work correctly???
		mat4x4<T> result;
		for (std::uint_fast8_t x = 0; x < 4; ++x)
		{
			for (std::uint_fast8_t y = 0; y < 4; ++y)
			{
				for (std::uint_fast8_t z = 0; z < 4; ++z)
				{
					// result.matrix[x][y] += matrix[x][z] * t_matrix.matrix[z][y];
					result.matrix[x][y] += t_matrix.matrix[x][z] * matrix[z][y];
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

template <typename T> [[nodiscard]] constexpr mat4x4<T> identity() noexcept
{
	mat4x4<T> matrix;
	matrix.matrix[0][0] = 1.0F;
	matrix.matrix[1][1] = 1.0F;
	matrix.matrix[2][2] = 1.0F;
	matrix.matrix[3][3] = 1.0F;
	return matrix;
}

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
[[nodiscard]] inline mat4x4<T> look_at(const vec3<T> &t_eye, const vec3<T> &t_center, const vec3<T> &t_up) noexcept
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
	matrix.matrix[0][2] = f.x;
	matrix.matrix[1][2] = f.y;
	matrix.matrix[2][2] = f.z;
	matrix.matrix[3][0] = -dot_product(s, t_eye);
	matrix.matrix[3][1] = -dot_product(u, t_eye);
	matrix.matrix[3][2] = -dot_product(f, t_eye);
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

template <typename T> [[nodiscard]] inline T length(const vec3<T> &t_vec) noexcept
{
	return std::sqrt((t_vec.x * t_vec.x) + (t_vec.y * t_vec.y) + (t_vec.z * t_vec.z));
}

template <typename T> [[nodiscard]] inline vec3<T> normalize(const vec3<T> &t_vec) noexcept
{
	const T vec_length{length(t_vec)};
	return vec3<T>{t_vec.x / vec_length, t_vec.y / vec_length, t_vec.z / vec_length};
}

[[nodiscard]] constexpr float fast_lerp(float t_value_a, float t_value_b, float t_fraction)
{
	return t_value_a + (t_value_b - t_value_a) * t_fraction;
}

[[nodiscard]] constexpr float fast_blerp(const fvec4 &t_vec, const fvec2 &t_fraction_vec)
{
	return fast_lerp(fast_lerp(t_vec.x, t_vec.y, t_fraction_vec.x), fast_lerp(t_vec.z, t_vec.w, t_fraction_vec.x),
					 t_fraction_vec.y);
}

/*template <typename T> [[nodiscard]] constexpr vec3<T> intersect_plane(const vec3<T>& t_plane_p, const vec3<T>&
t_plane_n, const vec3<T>& t_line_start, const vec3<T>& t_line_end)
{
	t_plane_n = normalize(plane_n);
	const T plane_d = -dot_product(plane_n, plane_p);
	const T ad = -dot_product(t_line_start, t_);
	const T bd = -dot_product(t_line_end, plane_n);
	const T t = (-plane_d - ad) / (bd - ad);
	const vec3<T> line_start_to_end = t_line_end - t_line_start;
	const vec3<T> line_to_intersect = line_start_to_end * t;
	return t_line_start + line_to_intersect;
}

template<typename T> [[nodiscard]] constexpr std::uint8_t triangle_clip_against_plane(vec3<T> t_plane_p, vec3<T>
t_plane_n, const std::array<Vertex, 3>& t_input_triangle, const std::array<Vertex, 3>& t_output_triangle1, const
std::array<Vertex, 3>& t_output_triangle2)
{
	t_plane_n = normalize(t_plane_n);

	auto dist = [&](vec3<T>& t_point)
	{
		vec3<T> normal = normalize(t_point);
		return (t_plane_n.x * t_point.x + t_plane_n.y * t_point.y + t_plane_n.z * t_point.z - dot_product(t_plane_n,
t_plane_p));
	};

	vec3<T>* inside_points[3] = { };
	std::uint8_t inside_point_count = 0;
	vec3<T>* outside_points[3] = { };
	std::uint8_t outside_point_count = 0;

	T d0 = dist(t_input_triangle[0]);
	T d1 = dist(t_input_triangle[1]);
	T d2 = dist(t_input_triangle[2]);

	if (d0 >= 0) { inside_points[++inside_point_count] = &t_input_triangle[0]; }
	else { outside_points[++outside_point_count] = &t_input_triangle[0]; }
	if (d1 >= 0) { inside_points[++inside_point_count] = &t_input_triangle[1]; }
	else { outside_points[++outside_point_count] = &t_input_triangle[1]; }
	if (d2 >= 0) { inside_points[++inside_point_count] = &t_input_triangle[2]; }
	else { outside_points[++outside_point_count] = &t_input_triangle[2]; }

	if (inside_point_count == 0)
	{
		return 0;
	}

	if (inside_point_count == 3)
	{
		t_output_triangle1 = t_input_triangle;
		return 1;
	}

	if (inside_point_count == 1 && outside_point_count == 2)
	{
		t_output_triangle1[0] = t_input_triangle;

		t_output_triangle1[0].position = *inside_points[0];

		t_output_triangle1[1].position = intersect_plane(t_plane_p, t_plane_n, *inside_points[0], *outside_points[0]);
		t_output_triangle1[2].position = intersect_plane(t_plane_p, t_plane_n, *inside_points[0], *outside_points[0]);

		return 1;
	}

	if (inside_point_count == 2 && outside_point_count == 1)
	{
		t_output_triangle1 = t_input_triangle;
		t_output_triangle2 = t_input_triangle;

		t_output_triangle1[0].position = *inside_points[0];
		t_output_triangle1[1].position = *inside_points[1];
		t_output_triangle1[2].position = intersect_plane(t_plane_p, t_plane_n, *inside_points[0], *outside_points[0]);

		t_output_triangle2[0].position = *inside_points[1];
		t_output_triangle2[1].position = t_output_triangle1[2].position;
		t_output_triangle2[2].position = intersect_plane(t_plane_p, t_plane_n, *inside_points[1], *outside_points[1]);

		return 2;
	}
}*/

}; // namespace ubv