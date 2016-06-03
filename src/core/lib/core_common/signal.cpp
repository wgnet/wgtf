#include "signal.hpp"

namespace wgt
{
Connection::Connection()
{
}

Connection::Connection(SignalHolderPtr entry) :
	entry_(entry)
{
}

Connection::Connection(const Connection & other) : 
	entry_(other.entry_)
{
}

Connection::Connection(Connection && other) : 
	entry_(std::move(other.entry_))
{
}

Connection::~Connection()
{
}

Connection & Connection::operator=(const Connection & other)
{
	entry_ = other.entry_;
	return *this;
}

Connection & Connection::operator=(Connection && other)
{
	entry_ = std::move(other.entry_);
	return *this;
}

void Connection::enable()
{
	if( entry_ )
	{
		entry_->enabled_ = true;
	}
}

void Connection::disable()
{
	if( entry_ )
	{
		entry_->enabled_ = false;
	}
}

void Connection::disconnect()
{
	if( entry_ )
	{
		entry_->expired_ = true;
	}
}

bool Connection::enabled() const
{
	if( entry_ )
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
	if( entry_ )
	{
		return !entry_->expired_;
	}
	else
	{
		return false;
	}
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
	for (auto & connection : connections_)
	{
		connection.disconnect();
	}
	connections_.clear();
}

void ConnectionHolder::add( const Connection & connection )
{
	connections_.emplace_back( connection );
}

void ConnectionHolder::operator+=( const Connection & connection )
{
	add( connection );
}
} // end namespace wgt
