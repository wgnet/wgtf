#ifndef TOOLS_TIME_UTILS_HPP
#define TOOLS_TIME_UTILS_HPP

#include <sstream>
#include <chrono>

namespace wgt
{
class TimeUtils
{
public:

	/**
	* Formats the duration as a string with hours minutes and fractions of seconds (i.e. 1h 5m 30.089s)
	*/
	static std::string formatDuration( std::chrono::steady_clock::duration duration
		, const std::string& hourLabel = "h "
		, const std::string& minuteLabel = "m "
		, const std::string& secondLabel = "s"
	)
	{
		auto durationHours = std::chrono::duration_cast<std::chrono::hours>( duration );
		duration -= durationHours;
		auto durationMinutes = std::chrono::duration_cast<std::chrono::minutes>( duration );
		duration -= durationMinutes;
		auto durationSeconds = std::chrono::duration_cast<std::chrono::milliseconds>( duration ).count() / 1000.0f;

		std::stringstream ss;
		if ( durationHours.count() > 0 )
		{
			ss << durationHours.count() << hourLabel.c_str();
		}
		if ( durationMinutes.count() > 0 )
		{
			ss << durationMinutes.count() << minuteLabel.c_str();
		}
		ss << durationSeconds << secondLabel;
		return ss.str();
	}
};

} // end namespace wgt
#endif // TOOLS_TIME_UTILS_HPP
