#ifndef BACKGROUND_WORKER_HPP
#define BACKGROUND_WORKER_HPP

#include <memory>
#include "interfaces/editor_interaction/i_background_worker.hpp"
#include "core_dependency_system/depends.hpp"
#include "core_dependency_system/i_interface.hpp"
#include "core_generic_plugin/interfaces/i_application.hpp"

namespace wgt
{

class BackgroundWorker
	: public Implements< IBackgroundWorker >
	, public Depends< IApplication >
{
public:
	BackgroundWorker();
	~BackgroundWorker();
	void queueBackgroundJob(IBackgroundWorker::JobFunction job) override;
	void queueForegroundJob(IBackgroundWorker::JobFunction job) override;

private:
	struct Impl;
	std::unique_ptr< Impl > impl_;
};

}
#endif //BACKGROUND_WORKER_HPP