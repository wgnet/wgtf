#include "signal.hpp"

namespace wgt
{
Connection::Connection()
{
}

Connection::Connection(SignalHolderPtr entry, std::weak_ptr<DisconnectSig> disconnectSig)
    : entry_(entry), disconnectSig_(disconnectSig)
{
}

Connection::Connection(const Connection& other) : entry_(other.entry_), disconnectSig_(other.disconnectSig_)
{
}

Connection::Connection(Connection&& other)
    : entry_(std::move(other.entry_)), disconnectSig_(std::move(other.disconnectSig_))
{
}

Connection::~Connection()
{
}

Connection& Connection::operator=(const Connection& other)
{
	entry_ = other.entry_;
	disconnectSig_ = other.disconnectSig_;
	return *this;
}

Connection& Connection::operator=(Connection&& other)
{
	entry_ = std::move(other.entry_);
	disconnectSig_ = std::move(other.disconnectSig_);
	return *this;
}

void Connection::enable()
{
	if (entry_)
	{
		entry_->enabled_ = true;
	}
}

void Connection::disable()
{
	if (entry_)
	{
		entry_->enabled_ = false;
	}
}

void Connection::disconnect()
{
	auto disconnectSig = disconnectSig_.lock();
	if (disconnectSig)
	{
		(*disconnectSig)(*this);
	}
}

bool Connection::enabled() const
{
	if (entry_)
	{
		return entry_->enabled_;
	}
	else
	{
		return false;
	}
}

bool Connection::connected() const
{
	return entry_ != nullptr;
}

ConnectionHolder::ConnectionHolder()
{
}

ConnectionHolder::~ConnectionHolder()
{
	clear();
}

void ConnectionHolder::clear()
{
	for (auto& connection : connections_)
	{
		connection.disconnect();
	}
	connections_.clear();
}

void ConnectionHolder::add(const Connection& connection)
{
	connections_.emplace_back(connection);
}

void ConnectionHolder::operator+=(const Connection& connection)
{
	add(connection);
}
} // end namespace wgt
