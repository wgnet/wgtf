//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  signal.h
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifndef SIGNAL_H_
#define SIGNAL_H_

#pragma once

#include <functional>
#include <mutex>
#include <memory>
#include <vector>

namespace wgt
{
namespace internal
{
	// Base class of the callback function holder
	// Without the function signature so that it can be
	// used within the non-templated Connection class.
	struct SignalHolder
	{
		bool enabled_;
		bool expired_;

		explicit SignalHolder(bool enabled)
			: enabled_(enabled)
			, expired_(false)
		{
		}
	};

	// Templated callback function holder
	template <typename Signature>
	struct TemplateSignalHolder : public SignalHolder
	{
		typedef std::function<Signature> Function;

		Function function_;

		explicit TemplateSignalHolder(Function function, bool enabled = true)
			: SignalHolder(enabled)
			, function_(function)
		{
		}
	};
}

// Maintains the connection between the callback function
// owner and the signal that will call it. Ensures that
// the function is not called if the object gets destroyed.
class Connection
{
private:
	typedef std::shared_ptr<internal::SignalHolder> SignalHolderPtr;

	SignalHolderPtr entry_;

public:
	Connection();
	explicit Connection(SignalHolderPtr entry);
	Connection(const Connection & other);
	Connection(Connection && other);
	~Connection();

	Connection & operator=(const Connection & other);
	Connection & operator=(Connection && other);

	void enable();
	void disable();

	void disconnect();

	bool enabled() const;
	bool connected() const;

};

class ConnectionHolder
{
public:
	ConnectionHolder();
	~ConnectionHolder();

	void clear();
	void add(const Connection & connection);

	void operator+=(const Connection & connection);

private:
	std::vector< Connection > connections_;

	ConnectionHolder(const ConnectionHolder &);
	ConnectionHolder(ConnectionHolder && other);
	ConnectionHolder & operator=(const ConnectionHolder &);
	ConnectionHolder & operator=(ConnectionHolder && other);
};

// Maintains a list of callback functions to call when an event occurs.
// It is thread safe and does not own the callback functions so they
// can be disconnected at any time.
template <typename Signature>
class Signal
{
public:
	typedef std::function<Signature> Function;

private:
	typedef internal::TemplateSignalHolder<Signature> SignatureHolder;
	typedef std::shared_ptr<SignatureHolder> SignalHolderPtr;

	typedef std::vector<SignalHolderPtr> HolderList;

	HolderList entries_;

	mutable std::mutex mutex_;

public:
	Signal()
	{
	}

	Signal(Signal && other)
	{
		*this = std::move(other);
	}

	~Signal()
	{
		clear();
	}

	Signal & operator=(Signal && other)
	{
		using std::swap;

		HolderList temp;

		{
			std::lock_guard<std::mutex> lock(other.mutex_);
			swap(temp, other.entries_);
		}

		{
			std::lock_guard<std::mutex> lock(mutex_);
			swap(entries_, temp);
		}
		return *this;
	}

	void clear()
	{
		std::lock_guard<std::mutex> lock(mutex_);
		for (auto entry : entries_)
		{
			entry->expired_ = true;
		}
		entries_.clear();
	}

	// Connects a new callback function to this signal, returns
	// the Connection object which owns the connection between the two.
	Connection connect(Function callback)
	{
		std::lock_guard<std::mutex> lock(mutex_);

		// Clear invalid entries
		auto expired_func = [](SignalHolderPtr ptr) { return ptr->expired_; };
		auto erase_begin = std::remove_if(std::begin(entries_), std::end(entries_), expired_func);
		entries_.erase(erase_begin, std::end(entries_));

		// Create new entry
		auto entry = std::make_shared< SignatureHolder >(callback);
		entries_.push_back(entry);
		return Connection(entry);
	}

	// Helper macro for the body of a call(...) function since 
	// we can't use variadic templates just yet.
#define CALL_FUNCTION_ITERATE_ENTRIES(function_call_args) \
	std::lock_guard<std::mutex> lock(mutex_); \
	for (auto entry : entries_) \
	{ \
		if (!entry->expired_ && entry->function_ && entry->enabled_) \
		{ \
			entry->function_ function_call_args ; \
		} \
	}

	void operator()() const
	{
		CALL_FUNCTION_ITERATE_ENTRIES( () )
	}

	template <class TArg1>
	void operator()(TArg1 && arg1) const
	{
		CALL_FUNCTION_ITERATE_ENTRIES( (std::forward<TArg1>(arg1)) )
	}

	template <class TArg1, class TArg2>
	void operator()(TArg1 && arg1, TArg2 && arg2) const
	{
		CALL_FUNCTION_ITERATE_ENTRIES( (std::forward<TArg1>(arg1), std::forward<TArg2>(arg2)) )
	}

	template <class TArg1, class TArg2, class TArg3>
	void operator()(TArg1 && arg1, TArg2 && arg2, TArg3 && arg3) const
	{
		CALL_FUNCTION_ITERATE_ENTRIES( (std::forward<TArg1>(arg1), std::forward<TArg2>(arg2), std::forward<TArg3>(arg3)) )
	}

	template <class TArg1, class TArg2, class TArg3, class TArg4>
	void operator()(TArg1 && arg1, TArg2 && arg2, TArg3 && arg3, TArg4 && arg4) const
	{
		CALL_FUNCTION_ITERATE_ENTRIES( (std::forward<TArg1>(arg1), std::forward<TArg2>(arg2), std::forward<TArg3>(arg3), std::forward<TArg4>(arg4)) )
	}

#undef CALL_FUNCTION_ITERATE_ENTRIES

private:
	Signal(const Signal &);
	Signal & operator=(const Signal &);
};
} // end namespace wgt
#endif // SIGNAL_H_
