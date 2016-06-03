#ifndef WG_CONDITION_VARIABLE_HPP_INCLUDED
#define WG_CONDITION_VARIABLE_HPP_INCLUDED

#define ENABLE_WG_CONDITION_VARIABLE_WORKAROUND (_MSC_VER == 1700) // VS2012

#if !ENABLE_WG_CONDITION_VARIABLE_WORKAROUND

#include <condition_variable>

namespace wgt
{
typedef std::condition_variable wg_condition_variable;
} // end namespace wgt
#else

#include <mutex>
#include <chrono>
#include <condition_variable>
#include <list>
#include <utility>

namespace wgt
{
/**
Replacement of buggy implementation of std::condition_variable in MSVC2012.
*/
class wg_condition_variable
{
public:
	typedef std::cv_status::cv_status _cv_status;

	wg_condition_variable();
	~wg_condition_variable();

	void notify_all();
	void notify_one();

	void wait( std::unique_lock< std::mutex >& lock );

	template< class Predicate >
	void wait(
		std::unique_lock< std::mutex >& lock,
		Predicate predicate )
	{
		while (!predicate())
		{
			wait( lock );
		}
	}

	template< class Rep, class Period >
	_cv_status wait_for(
		std::unique_lock< std::mutex >& lock,
		const std::chrono::duration< Rep, Period >& rel_time)
	{
		if (rel_time.count() < 0)
		{
			return std::cv_status::timeout;
		}

		duration dur = std::chrono::duration_cast< duration >( rel_time );
		if (dur.count() == 0 && rel_time.count() != 0)
		{
			// don't do zero wait if non-zero wait was requested
			dur = duration( 1 );
		}

		return wait_for_impl( lock, dur );
	}

	template< class Rep, class Period, class Predicate >
	bool wait_for(
		std::unique_lock< std::mutex >& lock,
		const std::chrono::duration< Rep, Period >& rel_time,
		Predicate predicate)
	{
		return wait_until(
			lock,
			std::chrono::steady_clock::now() + rel_time,
			std::move( predicate ) );
	}

	template< class Clock, class Duration >
	_cv_status wait_until(
		std::unique_lock< std::mutex >& lock,
		const std::chrono::time_point< Clock, Duration >& abs_time)
	{
		return wait_for( lock, abs_time - Clock::now() );
	}

	template< class Clock, class Duration, class Predicate >
	bool wait_until(
		std::unique_lock< std::mutex >& lock,
		const std::chrono::time_point< Clock, Duration >& abs_time,
		Predicate predicate)
	{
		while (!predicate())
		{
			if (wait_until( lock, abs_time ) == std::cv_status::timeout)
			{
				return predicate();
			}
		}
		return true;
	}

private:
	// forbid copying
	wg_condition_variable( const wg_condition_variable& );
	wg_condition_variable& operator=( const wg_condition_variable& );

	void notify_one_impl();

	// duration must be compatible with WinAPI wait functions.
	typedef std::chrono::duration< unsigned long, std::milli > duration;

	_cv_status wait_for_impl(
		std::unique_lock< std::mutex >& lock,
		const duration& rel_time);

	// Implementation details

	class Waiter;
	typedef std::list< Waiter > Waiters;

	std::mutex waitersMutex_;
	Waiters waiters_; // waiting threads ordered by their priority descending

	static __declspec( thread ) Waiters* s_waiter;
	static std::mutex s_allWaitersMutex;
	static std::list< Waiters > s_allWaiters;

	Waiters::iterator acquireWaiter();
	bool releaseWaiter( Waiters::iterator waiter );

};
} // end namespace wgt
#endif // !ENABLE_WG_CONDITION_VARIABLE_WORKAROUND

#endif // WG_CONDITION_VARIABLE_HPP_INCLUDED
