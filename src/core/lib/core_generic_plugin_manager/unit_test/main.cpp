#include "pch.hpp"
#include <stdlib.h>

int main(int argc, char* argv[])
{
	using namespace wgt;
#ifdef _WIN32
	_set_error_mode(_OUT_TO_STDERR);
	_set_abort_behavior(0, _WRITE_ABORT_MSG);
#endif // _WIN32
	int result = BWUnitTest::runTest("generic_app_plugin_system test", argc, argv);
	return result;
}

// main.cpp
