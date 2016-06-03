#ifndef TEST_APPLICATION_HPP
#define TEST_APPLICATION_HPP

#include <atomic>
#include <thread>

#include "core_generic_plugin/interfaces/i_application.hpp"

namespace wgt
{
class TestApplication : public IApplication
{
public:
	TestApplication();
	virtual ~TestApplication();

	int startApplication() override;
	void quitApplication() override;

private:
	std::atomic<bool> exiting_;
	std::thread thread_;
};
} // end namespace wgt
#endif
