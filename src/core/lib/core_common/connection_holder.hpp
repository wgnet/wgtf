#ifndef CONNECTION_HOLDER_HPP
#define CONNECTION_HOLDER_HPP

#include <memory>
#include <functional>

namespace wgt
{
template <typename T>
class ConnectionHolderT
{
public:
	typedef std::weak_ptr<std::function<void(T&)>> DisconnectSig;

	//--------------------------------------------------------------------------
	ConnectionHolderT() : object_(nullptr)
	{
	}

	//--------------------------------------------------------------------------
	ConnectionHolderT(T& object, DisconnectSig disconnectSig) : object_(&object), disconnectSig_(disconnectSig)
	{
	}

	//--------------------------------------------------------------------------
	ConnectionHolderT(ConnectionHolderT&& other)
	    : object_(other.object_), disconnectSig_(std::move(other.disconnectSig_))
	{
	}

	//--------------------------------------------------------------------------
	void disconnect()
	{
		auto disconnectSig = disconnectSig_.lock();
		if (disconnectSig)
		{
			(*disconnectSig)(*object_);
		}
	}

private:
	T* object_;
	DisconnectSig disconnectSig_;
};
}
#endif // CONNECTION_HOLDER_HPP