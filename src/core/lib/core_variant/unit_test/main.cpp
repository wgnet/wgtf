#include "pch.hpp"
#include <stdlib.h>

int main(int argc, char* argv[])
{
	using namespace wgt;
#ifdef _WIN32
	_set_error_mode(_OUT_TO_STDERR);
	_set_abort_behavior(0, _WRITE_ABORT_MSG);
#endif // _WIN32

	int ret = BWUnitTest::runTest("variant", argc, argv);

	return ret;
}

// main.cpp
