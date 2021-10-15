#include "Window.hpp"

namespace ubv
{
Window::Window(WindowProps t_win_props) : m_win_props{std::move(t_win_props)}
{
}

std::uint16_t Window::get_win_width() const noexcept
{
	return m_win_props.width;
}

std::uint16_t Window::get_win_height() const noexcept
{
	return m_win_props.height;
}

const std::string &Window::get_win_title() const noexcept
{
	return m_win_props.title;
}

} // namespace ubv
