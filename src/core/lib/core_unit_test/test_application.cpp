#include "pch.hpp"
#include "test_application.hpp"

#include <chrono>

namespace wgt
{
TestApplication::TestApplication() : exiting_(false)
{
	thread_ = std::thread(&TestApplication::startApplication, this);
}

TestApplication::~TestApplication()
{
	quitApplication();
	thread_.join();
}

int TestApplication::startApplication()
{
	while (!exiting_)
	{
		// TODO: don't notifyUpdate in the unit tests as many systems such as
		// the command manager require the application thread to be the main thread

		// notifyUpdate();
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	return 0;
}

void TestApplication::quitApplication()
{
	exiting_ = true;
}
} // end namespace wgt
