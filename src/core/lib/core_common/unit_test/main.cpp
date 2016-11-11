#include <stdlib.h>
#include "core_unit_test/unit_test.hpp"

int main(int argc, char* argv[])
{
	using namespace wgt;
#ifdef _WIN32
	_set_error_mode(_OUT_TO_STDERR);
	_set_abort_behavior(0, _WRITE_ABORT_MSG);
#endif // _WIN32
	return BWUnitTest::runTest("core_common_unit_test", argc, argv);
}

// main.cpp
