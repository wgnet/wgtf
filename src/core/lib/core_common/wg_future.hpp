#ifndef WG_FUTURE_HPP_INCLUDED
#define WG_FUTURE_HPP_INCLUDED

#define ENABLE_WG_FUTURE_WORKAROUND (_MSC_VER == 1700) // VS2012
#include <mutex>
#include <chrono>
#include <future>
#include <utility>

namespace wgt
{
/************************************************************************
 * Regular futures do not pump the Qt UI thread queue and will cause the*
 * application to deadlock if called directly.                          *
 ************************************************************************/
#if !ENABLE_WG_FUTURE_WORKAROUND
typedef std::future_status wg_future_status;
#else
typedef std::future_status::future_status wg_future_status;
#endif // !ENABLE_WG_FUTURE_WORKAROUND

template <typename T>
class wg_shared_future;

template <typename T>
class wg_future final
{
public:
	wg_future(std::function<void()> callback = []() {}) _NOEXCEPT : callback_(callback)
	{
	}
	~wg_future() _NOEXCEPT
	{
	}

	wg_future(wg_future&& _Other) _NOEXCEPT : future_(std::move(_Other.future_)), callback_(std::move(_Other.callback_))
	{
	}

	wg_future(std::future<T>&& _Other, std::function<void()> callback = []() {}) _NOEXCEPT : future_(std::move(_Other)),
	                                                                                         callback_(callback)
	{
	}

	wg_future& operator=(wg_future&& _Right) _NOEXCEPT
	{
		future_ = std::move(_Right.future_);
		callback_ = std::move(_Right.callback_);
		return (*this);
	}

	wg_future& operator=(std::future<T>&& _Right) _NOEXCEPT
	{
		future_ = std::move(_Right);
		return (*this);
	}

	T get()
	{
		wait();
		return std::move(future_.get());
	}

	wg_shared_future<T> share()
	{
		return wg_shared_future<T>(std::move(*this));
	}

	bool valid() const _NOEXCEPT
	{
		return future_.valid();
	}

	void wait() const
	{
		wg_future_status status;
		do
		{
			status = wait_for(std::chrono::microseconds(1));
			callback_();
		} while (status != std::future_status::ready);
	}

	template <class _Rep, class _Per>
	wg_future_status wait_for(const std::chrono::duration<_Rep, _Per>& _Rel_time) const
	{
#if ENABLE_WG_FUTURE_WORKAROUND
		if (future_._Is_ready())
		{
			return std::future_status::ready;
		}
#endif // ENABLE_WG_FUTURE_WORKAROUND
		return future_.wait_for(_Rel_time);
	}

	template <class _Clock, class _Dur>
	wg_future_status wait_until(const std::chrono::time_point<_Clock, _Dur>& _Abs_time) const
	{
#if ENABLE_WG_FUTURE_WORKAROUND
		if (future_._Is_ready())
		{
			return std::future_status::ready;
		}
#endif // ENABLE_WG_FUTURE_WORKAROUND
		return future_.wait_until(_Abs_time);
	}

	void setCallback(std::function<void()> callback = []() {})
	{
		callback_ = callback;
	}

private:
	// forbid copying
	wg_future(const wg_future&);
	wg_future& operator=(const wg_future&);
	std::future<T> future_;
	std::function<void()> callback_;
};

template <typename T>
class wg_shared_future final
{
public:
	wg_shared_future(std::function<void()> callback = []() {}) _NOEXCEPT
	{
	}

	wg_shared_future(const wg_shared_future& _Other)
	    : shared_future_(_Other.shared_future_), callback_(_Other.callback_)
	{
	}

	wg_shared_future(const std::shared_future<T>& _Other, std::function<void()> callback = []() {})
	    : shared_future_(_Other), callback_(callback)
	{
	}

	wg_shared_future& operator=(const wg_shared_future& _Right)
	{
		shared_future_ = _Right.shared_future_;
		return (*this);
	}

	wg_shared_future& operator=(const std::shared_future<T>& _Right)
	{
		shared_future_ = _Right;
		return (*this);
	}

	wg_shared_future(std::future<T>&& _Other) _NOEXCEPT : shared_future_(std::forward<std::future<T>>(_Other))
	{
	}

	wg_shared_future(wg_future<T>&& _Other) _NOEXCEPT : shared_future_(std::move(_Other.future_)),
	                                                    callback_(std::move(_Other.callback_))
	{
	}

	wg_shared_future(std::shared_future<T>&& _Other, std::function<void()> callback = []() {}) _NOEXCEPT
	: shared_future_(std::move(_Other)),
	  callback_(callback)
	{
	}

	wg_shared_future(wg_shared_future&& _Other) _NOEXCEPT : shared_future_(std::move(_Other.shared_future_)),
	                                                        callback_(std::move(_Other.callback_))
	{
	}

	wg_shared_future& operator=(wg_shared_future&& _Right) _NOEXCEPT
	{
		shared_future_ = std::move(_Right.shared_future_);
		callback_ = std::move(_Right.callback_);
		return (*this);
	}

	wg_shared_future& operator=(std::shared_future<T>&& _Right) _NOEXCEPT
	{
		shared_future_ = std::move(_Right);
		return (*this);
	}

	~wg_shared_future() _NOEXCEPT
	{
	}

	const T& get() const
	{
		wait();
		return shared_future_.get();
	}

	bool valid() const _NOEXCEPT
	{
		return shared_future_.valid();
	}

	void wait() const
	{
		wg_future_status status;
		do
		{
			status = wait_for(std::chrono::microseconds(1));
			callback_();
		} while (status != std::future_status::ready);
	}

	template <class _Rep, class _Per>
	wg_future_status wait_for(const std::chrono::duration<_Rep, _Per>& _Rel_time) const
	{
#if ENABLE_WG_FUTURE_WORKAROUND
		if (shared_future_._Is_ready())
		{
			return std::future_status::ready;
		}
#endif // ENABLE_WG_FUTURE_WORKAROUND
		return shared_future_.wait_for(_Rel_time);
	}

	template <class _Clock, class _Dur>
	wg_future_status wait_until(const std::chrono::time_point<_Clock, _Dur>& _Abs_time) const
	{
#if ENABLE_WG_FUTURE_WORKAROUND
		if (shared_future_._Is_ready())
		{
			return std::future_status::ready;
		}
#endif // ENABLE_WG_FUTURE_WORKAROUND
		return shared_future_.wait_until(_Abs_time);
	}

	void setCallback(std::function<void()> callback = []() {})
	{
		callback_ = callback;
	}

private:
	std::shared_future<T> shared_future_;
	std::function<void()> callback_;
};
} // end namespace wgt

#endif // WG_FUTURE_HPP_INCLUDED
