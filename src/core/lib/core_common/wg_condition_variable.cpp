#include "wg_condition_variable.hpp"

#if ENABLE_WG_CONDITION_VARIABLE_WORKAROUND

#include "core_common/assert.hpp"
#include "ngt_windows.hpp"

// evgenys: disabled to reduce number of false positive leak reports from allocator on app shutdown
// Each thread has its own container with at most one Waiter in it
//__declspec( thread ) wg_condition_variable::Waiters* wg_condition_variable::s_waiter = nullptr;
// std::mutex wg_condition_variable::s_allWaitersMutex;
// std::list< wg_condition_variable::Waiters > wg_condition_variable::s_allWaiters;

////////////////////////////////////////////////////////////////////////////////

namespace wgt
{
class wg_condition_variable::Waiter
{
public:
	Waiter()
	    : event_(CreateEvent(NULL, TRUE, FALSE, NULL)), // manual, not set
	      signaled_(false), priority_(0)
	{
	}

	Waiter(Waiter&& that) : event_(that.event_), signaled_(that.signaled_), priority_(that.priority_)
	{
		that.event_ = NULL;
	}

	~Waiter()
	{
		if (event_ != NULL)
		{
			CloseHandle(event_);
		}
	}

	Waiter& operator=(Waiter&& that)
	{
		if (event_ != NULL)
		{
			CloseHandle(event_);
		}

		event_ = that.event_;
		signaled_ = that.signaled_;
		priority_ = that.priority_;

		that.event_ = NULL;

		return *this;
	}

	wg_condition_variable::_cv_status wait()
	{
		return wait(INFINITE);
	}

	wg_condition_variable::_cv_status wait(wg_condition_variable::duration rel_time)
	{
		return wait(rel_time.count());
	}

	bool isSignaled() const
	{
		return signaled_;
	}

	void setSignaled(bool signaled)
	{
		if (signaled_ == signaled)
		{
			return;
		}

		signaled_ = signaled;

		if (signaled)
		{
			SetEvent(event_);
		}
		else
		{
			ResetEvent(event_);
		}
	}

	int priority() const
	{
		return priority_;
	}

	void setPriority(int priority)
	{
		priority_ = priority;
	}

private:
	Waiter(const Waiter&);
	Waiter& operator=(const Waiter&);

	wg_condition_variable::_cv_status wait(DWORD rel_time)
	{
		DWORD r = WaitForSingleObjectEx(event_, rel_time, TRUE);
		switch (r)
		{
		case WAIT_OBJECT_0:
		case WAIT_IO_COMPLETION:
			return std::cv_status::no_timeout;

		case WAIT_TIMEOUT:
		default:
			// Any error makes us to act as if timeout was occurred
			// to avoid busy wait in such case.
			return std::cv_status::timeout;
		}
	}

	HANDLE event_;
	bool signaled_;
	int priority_;
};

////////////////////////////////////////////////////////////////////////////////

wg_condition_variable::wg_condition_variable() : waitersMutex_(), waiters_()
{
	// nop
}

wg_condition_variable::~wg_condition_variable()
{
	TF_ASSERT(waiters_.empty());
}

void wg_condition_variable::notify_all()
{
	std::lock_guard<std::mutex> waitersGuard(waitersMutex_);
	for (auto& waiter : waiters_)
	{
		waiter.setSignaled(true);
	}
}

void wg_condition_variable::notify_one()
{
	std::lock_guard<std::mutex> waitersGuard(waitersMutex_);
	notify_one_impl();
}

void wg_condition_variable::wait(std::unique_lock<std::mutex>& lock)
{
	auto waiter = acquireWaiter();
	lock.unlock();
	waiter->wait();
	releaseWaiter(waiter);
	lock.lock();
}

void wg_condition_variable::notify_one_impl()
{
	for (auto& waiter : waiters_)
	{
		if (!waiter.isSignaled())
		{
			waiter.setSignaled(true);
			break;
		}
	}
}

wg_condition_variable::_cv_status wg_condition_variable::wait_for_impl(std::unique_lock<std::mutex>& lock,
                                                                       const duration& rel_time)
{
	auto waiter = acquireWaiter();
	lock.unlock();
	auto waitStatus = waiter->wait(rel_time);
	bool isSignaled = releaseWaiter(waiter);
	lock.lock();

	if (!isSignaled && waitStatus == std::cv_status::timeout)
	{
		// If waiter is signaled then consider we woke up because of signal.
		return std::cv_status::timeout;
	}
	else
	{
		return std::cv_status::no_timeout;
	}
}

wg_condition_variable::Waiters::iterator wg_condition_variable::acquireWaiter()
{
	const int currentPriority = GetThreadPriority(GetCurrentThread());
	Waiters::iterator waiter;

	std::lock_guard<std::mutex> waitersGuard(waitersMutex_);

	// find proper position for current thread in waiters list
	auto pos = waiters_.rbegin();
	while (pos != waiters_.rend())
	{
		if (pos->priority() >= currentPriority)
		{
			break;
		}

		++pos;
	}

	// try to take waiter from pool
	// if (s_waiter && !s_waiter->empty())
	//{
	//	waiter = s_waiter->begin();
	//	waiters_.splice( pos.base(), *s_waiter, waiter );

	//	// workaround: VS2012 for some reason invalidates waiter iterator (though it shouldn't)
	//	waiter = pos.base();
	//	--waiter;
	//}
	// else
	{
		// pool is empty, create a new waiter
		waiter = waiters_.emplace(pos.base());
	}

	// prepare waiter before return
	waiter->setPriority(currentPriority);
	waiter->setSignaled(false);

	return waiter;
}

bool wg_condition_variable::releaseWaiter(Waiters::iterator waiter)
{
	std::lock_guard<std::mutex> waitersGuard(waitersMutex_);

	const bool isSignaled = waiter->isSignaled();
	waiters_.erase(waiter);
	return isSignaled;

	/*if (!s_waiter)
	{
	    std::lock_guard< std::mutex > waitersGuard( s_allWaitersMutex );
	    s_allWaiters.emplace_back();
	    s_waiter = &s_allWaiters.back();
	}

	s_waiter->splice( s_waiter->begin(), waiters_, waiter );

	return isSignaled;*/
}
} // end namespace wgt

#endif // ENABLE_WG_CONDITION_VARIABLE_WORKAROUND
