#ifndef SCOPED_STOP_WATCH_HPP
#define SCOPED_STOP_WATCH_HPP

#include <chrono>

namespace wgt
{

class ScopedStopwatch
{
public:
	ScopedStopwatch(const char* name);
	~ScopedStopwatch();
private:
	const char* name_;
	std::chrono::high_resolution_clock::time_point start_;
};

#define SCOPE_TAG ScopedStopwatch sw( __FUNCTION__ );
}
#endif //SCOPED_STOP_WATCH_HPP