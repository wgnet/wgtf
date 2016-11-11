#include <stdlib.h>
#include "pch.hpp"

int main(int argc, char* argv[])
{
#ifdef _WIN32
	_set_error_mode(_OUT_TO_STDERR);
	_set_abort_behavior(0, _WRITE_ABORT_MSG);
#endif // _WIN32

	int result = 0;
	result = wgt::BWUnitTest::runTest("", argc, argv);

	return result;
}

// main.cpp
