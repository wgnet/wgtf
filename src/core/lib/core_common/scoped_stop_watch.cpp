#include "scoped_stop_watch.hpp"
#include "core_logging/logging.hpp"

namespace wgt
{

//------------------------------------------------------------------------------
ScopedStopwatch::ScopedStopwatch(const char* name)
	: name_(name)
	, start_( std::chrono::high_resolution_clock::now() )
{}


//------------------------------------------------------------------------------
ScopedStopwatch::~ScopedStopwatch()
{
	auto now = std::chrono::high_resolution_clock::now();
	auto difference = 
		std::chrono::duration_cast<std::chrono::milliseconds>(now - start_);

	NGT_DEBUG_MSG("%s: %llu ms\n", name_, difference.count());
}

}
