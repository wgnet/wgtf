#ifndef I_BACKGROUND_WORKER_HPP
#define I_BACKGROUND_WORKER_HPP

#include <functional>

namespace wgt
{
class IBackgroundWorker
{
public:
	typedef std::function<void()> JobFunction;
	virtual ~IBackgroundWorker() {}
	virtual void queueBackgroundJob(JobFunction backgroundJob) = 0;
	virtual void queueForegroundJob(JobFunction foregroundJob) = 0;
};

}
#endif //I_BACKGROUND_WORKER_HPP