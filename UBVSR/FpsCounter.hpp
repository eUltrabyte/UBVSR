#pragma once

#include <chrono>

using Number = double;

class Timepoint
{
  public:
	// create a timepoint that represents the current point in time
	Timepoint();

	// add duration to the timepoint
	Timepoint &operator+=(Number duration);
	[[nodiscard]] Timepoint operator+(Number duration) const;

	// subtract duration from the timepoint
	Timepoint &operator-=(Number duration);
	[[nodiscard]] Timepoint operator-(Number duration) const;

	// create a duration by subtracting the previous timepoint
	[[nodiscard]] Number operator-(const Timepoint &previous_timepoint) const;

	// compare with the other timepoint
	[[nodiscard]] bool operator<(const Timepoint &timepoint) const;

	[[nodiscard]] Number operator%(Number duration) const;

  private:
	std::chrono::time_point<std::chrono::steady_clock> _timepoint;
};

Timepoint::Timepoint() : _timepoint{std::chrono::steady_clock::now()}
{
}

Timepoint &Timepoint::operator+=(Number duration)
{
	_timepoint = std::chrono::time_point_cast<std::chrono::steady_clock::duration>(
		_timepoint + std::chrono::duration<Number>(duration));
	return *this;
}

Timepoint Timepoint::operator+(Number duration) const
{
	return Timepoint(*this) += duration;
}

Timepoint &Timepoint::operator-=(Number duration)
{
	return *this += -duration;
}

Timepoint Timepoint::operator-(Number duration) const
{
	return Timepoint(*this) -= duration;
}

Number Timepoint::operator-(const Timepoint &previous_timepoint) const
{
	return std::chrono::duration<Number>(_timepoint - previous_timepoint._timepoint).count();
}

bool Timepoint::operator<(const Timepoint &timepoint) const
{
	return _timepoint < timepoint._timepoint;
}

Number Timepoint::operator%(Number duration) const
{
	return std::chrono::duration<Number>(
			   _timepoint.time_since_epoch() %
			   std::chrono::duration_cast<std::chrono::steady_clock::duration>(std::chrono::duration<Number>(duration)))
		.count();
}

#include <set>

class FpsCounter
{
	std::multiset<Timepoint> timepoints;

  public:
	[[nodiscard]] inline uint32_t update(const Timepoint &timepoint = {})
	{
		timepoints.emplace(timepoint);
		timepoints.erase(timepoints.begin(), timepoints.lower_bound(timepoint - 1.0L));
		return timepoints.size();
	}
};
