#include "CppUnitLite2/src/CppUnitLite2.h"
#include "core_common/wg_condition_variable.hpp"

#include <thread>
#include <chrono>
#include <mutex>
#include <array>
#include <atomic>

namespace wgt
{
TEST(wg_condition_variable)
{
	std::mutex mutex;
	bool stop = false;
	size_t tasks = 0;
	size_t consumersStarted = 0;
	size_t wakeUps = 0; // total wakeups count
	size_t uniqueWakeUps = 0; // consumers count which woke up at least once
	wg_condition_variable haveWork; // tasks > 0 || stop
	wg_condition_variable allConsumersStarted; // consumersStarted == consumers.size()
	wg_condition_variable noTasks; // tasks == 0
	wg_condition_variable endOfWorld; // will not be signaled (hopefully ;-)

	// start consumers
	std::array<std::thread, 64> consumers;
	for (auto& consumer : consumers)
	{
		consumer = std::thread([&] {
			bool reported = false;

			std::unique_lock<std::mutex> lock(mutex);

			// increment started consumers counter
			consumersStarted += 1;
			if (consumersStarted == consumers.size())
			{
				// this is the last consumer, notify main thread
				allConsumersStarted.notify_one();
			}

			// consumer loop
			while (true)
			{
				haveWork.wait(lock, [&] { return tasks > 0 || stop; });

				// report we woke up
				wakeUps += 1;
				if (!reported)
				{
					uniqueWakeUps += 1;
					reported = true;
				}

				if (stop)
				{
					break;
				}

				if (tasks > 0)
				{
					// consume task
					tasks -= 1;
					if (tasks == 0)
					{
						noTasks.notify_one();
					}
				}

				// Give wakeup opportunity for other threads on monocore systems.
				// This is needed for fairness check only.
				lock.unlock();
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
				lock.lock();
			}
		});
	}

	{
		std::unique_lock<std::mutex> lock(mutex);

		// wait consumers startup
		CHECK(allConsumersStarted.wait_until(lock, std::chrono::steady_clock::now() + std::chrono::seconds(1),
		                                     [&] { return consumersStarted == consumers.size(); }));
		CHECK_EQUAL(consumers.size(), consumersStarted);

		// check wait timeout
		auto waitStartTime = std::chrono::steady_clock::now();
		endOfWorld.wait_for(lock, std::chrono::milliseconds(100));
		auto waitDelta = std::chrono::steady_clock::now() - waitStartTime;
		auto waitDeltaMS = std::chrono::duration_cast<std::chrono::milliseconds>(waitDelta);
		CHECK(waitDeltaMS.count() >= 50);

		// check if any consumer has woke up
		CHECK_EQUAL(0, wakeUps);
		CHECK_EQUAL(0, uniqueWakeUps);

		// produce tasks
		tasks += consumers.size();
		haveWork.notify_all();

		// wait consumers
		CHECK(noTasks.wait_for(lock, std::chrono::seconds(1), [&] { return tasks == 0; }));
		CHECK_EQUAL(0, tasks);

		// check fairness: consumer shouldn't take another's tasks
		CHECK_EQUAL(consumers.size(), uniqueWakeUps);
		CHECK_EQUAL(consumers.size(), wakeUps);

		// stop consumers
		stop = true;
		haveWork.notify_all();
	}

	for (auto& consumer : consumers)
	{
		consumer.join();
	}

	// ensure wakeUps variable is synced to this thread
	std::atomic_thread_fence(std::memory_order_acquire);

	// each haveWork.notify_all() must produce consumers.size() wakeups
	CHECK_EQUAL(consumers.size() * 2, wakeUps);
}
} // end namespace wgt
