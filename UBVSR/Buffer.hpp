#pragma once

#include "Math.hpp"
#include "Pixel.hpp"

#include <algorithm>
#include <cstdint>
#include <limits>
#include <vector>
#include <atomic>
#include <mutex>

namespace ubv
{
template <typename Type, Type DefaultValue = Type{}> class Buffer
{
  public:
	inline explicit Buffer(std::uint16_t t_width, std::uint16_t t_height)
		: m_width{t_width}, m_height{t_height}, m_data(static_cast<std::size_t>(m_width) * m_height, DefaultValue)
	{
	}
	inline void clear() noexcept
	{
		std::fill(m_data.begin(), m_data.end(), DefaultValue);
	}
	[[nodiscard]] inline const Type &at(std::uint16_t t_x, std::uint16_t t_y) const noexcept
	{
		return m_data[static_cast<std::size_t>(t_y) * m_width + t_x];
	}
	[[nodiscard]] inline Type &at(std::uint16_t t_x, std::uint16_t t_y) noexcept
	{
		return m_data[static_cast<std::size_t>(t_y) * m_width + t_x];
		//return const_cast<Type &>(const_cast<const decltype(this)>(this)->at(t_x, t_y));
	}
	[[nodiscard]] inline const std::vector<Type>& data() const noexcept
	{
		return m_data;
	}
	[[nodiscard]] inline std::vector<Type>& data() noexcept
	{
		return m_data;
		//return const_cast<std::vector<Type>&>(const_cast<const decltype(this)>(this)->data());
	}

  private:
	std::uint16_t m_width;
	std::uint16_t m_height;
	std::vector<Type> m_data;
};

using ColorBuffer = Buffer<Pixel>;
using DepthBuffer = Buffer<float, std::numeric_limits<float>::infinity()>;
using StencilBuffer = Buffer<std::uint8_t, false>;
using NDCBuffer = Buffer<fvec2>;;

} // namespace ubv
