#include "background_worker.hpp"

#include <atomic>
#include <mutex>
#include <thread>
#include <list>


namespace wgt
{

struct BackgroundWorker::Impl
{
	void foregroundUpdate()
	{
		while (exit_ == false)
		{
			IBackgroundWorker::JobFunction job;
			{
				std::lock_guard<std::mutex> lock(foregroundJobMutex_);
				if (!foregroundJobs_.empty())
				{
					job = foregroundJobs_.front();
					foregroundJobs_.pop_front();
				}
			}
			if (!job)
			{
				break;
			}
			job();
		}
	}

	void backgroundUpdate()
	{
		while (exit_ == false)
		{
			BackgroundWorker::JobFunction  job;
			{
				std::unique_lock<std::mutex> lock(backgroundJobMutex_);
				jobWaiter_.wait(lock, [this]
				{
					return exit_ || !backgroundJobs_.empty();
				});

				if (exit_)
				{
					return;
				}

				if (!backgroundJobs_.empty())
				{
					job = backgroundJobs_.front();
					backgroundJobs_.pop_front();
				}
			}
			if (job)
			{
				job();
			}
		}
	}

	Impl(IApplication& application)
		: exit_(false)
	{
		foregroundUpdateConnection_ = application.signalUpdate.connect([this] { foregroundUpdate(); });

		//TODO: Grow / retire threads from this thread pool
		unsigned int nThreads = std::thread::hardware_concurrency() / 2;
		for (unsigned int i = 0; i < nThreads; ++i)
		{
			backgroundThreads_.emplace_back( std::make_unique< std::thread>([this]
			{
				backgroundUpdate();
			}));
		}
	}

	std::condition_variable jobWaiter_;
	std::atomic< bool > exit_;
	std::mutex backgroundJobMutex_;
	std::list< IBackgroundWorker::JobFunction > backgroundJobs_;
	std::mutex foregroundJobMutex_;
	std::list< IBackgroundWorker::JobFunction > foregroundJobs_;
	std::vector< std::unique_ptr< std::thread > > backgroundThreads_;
	Connection foregroundUpdateConnection_;
};


//------------------------------------------------------------------------------
BackgroundWorker::BackgroundWorker()
	: impl_(std::make_unique<Impl>(*get<IApplication>()))
{
}

//------------------------------------------------------------------------------
BackgroundWorker::~BackgroundWorker()
{
	{
		std::lock_guard< std::mutex > holder(impl_->backgroundJobMutex_);
		impl_->exit_ = true;
		impl_->jobWaiter_.notify_all();
	}
	for (size_t i = 0; i < impl_->backgroundThreads_.size(); ++i)
	{
		impl_->backgroundThreads_[ i ]->join();
	}
}

//------------------------------------------------------------------------------
void BackgroundWorker::queueBackgroundJob(IBackgroundWorker::JobFunction job)
{
	if (impl_->exit_)
	{
		return;
	}
	{
		std::lock_guard< std::mutex > holder(impl_->backgroundJobMutex_);
		impl_->backgroundJobs_.emplace_back(job);
	}
	impl_->jobWaiter_.notify_one();
}

//------------------------------------------------------------------------------
void BackgroundWorker::queueForegroundJob(IBackgroundWorker::JobFunction job)
{
	if (impl_->exit_)
	{
		return;
	}
	std::lock_guard< std::mutex > holder(impl_->foregroundJobMutex_);
	impl_->foregroundJobs_.emplace_back(job);
}
}