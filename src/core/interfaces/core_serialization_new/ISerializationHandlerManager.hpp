#pragma once
#ifndef I_SERIALIZATION_HANDLER_MANAGER_HPP
#include <memory>
#include "core_variant/variant.hpp"

namespace wgt
{
class SerializationHandler;
class SerializationNode;

class ISerializationHandlerManager
{
public:
	typedef std::unique_ptr<SerializationNode> NodePtr;

public:
	virtual ~ISerializationHandlerManager()
	{
	}

	virtual void registerHandler(std::shared_ptr<SerializationHandler> handler, int64_t priority = -1) = 0;
	virtual void deregisterHandler(std::shared_ptr<SerializationHandler> handler) = 0;

	virtual size_t getHandlerCount() const = 0;

protected:
	virtual std::shared_ptr<SerializationHandler> findHandlerWrite(const Variant& v, const char* name = nullptr,
	                                                               bool writeType = true) const = 0;
	virtual std::shared_ptr<SerializationHandler> findHandlerRead(const NodePtr& node, const char* name = nullptr,
	                                                              const char* typeName = nullptr) const = 0;
};

} // end namespace wgt

#endif // I_SERIALIZATION_HANDLER_MANAGER_HPP
