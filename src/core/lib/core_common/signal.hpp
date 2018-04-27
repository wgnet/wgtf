//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  signal.hpp
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifndef SIGNAL_HPP_
#define SIGNAL_HPP_

#pragma once

#include <functional>
#include <mutex>
#include <memory>
#include <vector>
#include <algorithm>
#include <cassert>

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

	explicit SignalHolder(bool enabled) : enabled_(enabled)
	{
	}
};

// Templated callback function holder
template <typename Signature>
struct TemplateSignalHolder : public SignalHolder
{
	typedef std::function<Signature> Function;

	Function function_;

	explicit TemplateSignalHolder(Function function, bool enabled = true) : SignalHolder(enabled), function_(function)
	{
	}
};
}

// Maintains the connection between the callback function
// owner and the signal that will call it. Ensures that
// the function is not called if the object gets destroyed.
class Connection
{
	typedef std::function<void(Connection&)> DisconnectSig;

private:
	typedef std::shared_ptr<internal::SignalHolder> SignalHolderPtr;

	SignalHolderPtr entry_;
	std::weak_ptr<DisconnectSig> disconnectSig_;

public:
	Connection();
	explicit Connection(SignalHolderPtr entry, std::weak_ptr<DisconnectSig> disconnectSig_);
	Connection(const Connection& other);
	Connection(Connection&& other);
	~Connection();

	Connection& operator=(const Connection& other);
	Connection& operator=(Connection&& other);

	void enable();
	void disable();

	void disconnect();

	bool enabled() const;
	bool connected() const;

	SignalHolderPtr& getEntry()
	{
		return entry_;
	}
};

class ConnectionHolder
{
public:
	ConnectionHolder();
	~ConnectionHolder();

	void clear();
	void add(const Connection& connection);

	void operator+=(const Connection& connection);

private:
	std::vector<Connection> connections_;

	ConnectionHolder(const ConnectionHolder&);
	ConnectionHolder(ConnectionHolder&& other);
	ConnectionHolder& operator=(const ConnectionHolder&);
	ConnectionHolder& operator=(ConnectionHolder&& other);
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

	typedef std::shared_ptr<std::function<void(Connection&)>> DisconnectSig;
	DisconnectSig disconnectSig_;
	mutable std::mutex mutex_;
public:
	Signal()
	{
		disconnectSig_ = std::make_shared<std::function<void(Connection&)>>([this](Connection& connection) {
			std::lock_guard<std::mutex> lock(mutex_);
			auto findIt = std::find(entries_.begin(), entries_.end(), connection.getEntry());
			// Disconnect can be called from many sources, so allow this to happen
			// Can't warn either as the signal owner can easily just call clear
			if (findIt != entries_.end())
			{
				entries_.erase(findIt);
			}
		});
	}

	Signal(Signal&& other)
	{
		*this = std::move(other);
	}

	~Signal()
	{
		clear();
	}

	Signal& operator=(Signal&& other)
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
		entries_.clear();
	}

	// Connects a new callback function to this signal, returns
	// the Connection object which owns the connection between the two.
	Connection connect(Function callback)
	{
		std::lock_guard<std::mutex> lock(mutex_);

		// Create new entry
		auto entry = std::make_shared<SignatureHolder>(callback);
		entries_.push_back(entry);
		return Connection(entry, disconnectSig_);
	}

	template<typename ...Args>
	void operator()(Args&&... args) const
	{
		HolderList copyList;
		{
			std::lock_guard<std::mutex> lock(mutex_);
			copyList = entries_;
		}
		for (auto entry : copyList)
		{
			if (entry && entry->function_ && entry->enabled_)
			{
				entry->function_(std::forward<Args>(args)...);
			}
		}
	}
private:
	Signal(const Signal&);

#if _MSC_VER < 1900
	Signal& operator=(const Signal&);
#else
	Signal& operator=(const Signal&) = delete;
#endif
};
} // end namespace wgt
#endif // SIGNAL_H_
