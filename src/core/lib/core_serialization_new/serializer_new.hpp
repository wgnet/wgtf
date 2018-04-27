#pragma once
#include "../../interfaces/core_serialization_new/ISerializerNew.hpp"
#include "../../lib/core_generic_plugin/interfaces/i_component_context.hpp"
#include <memory>

namespace wgt
{
/**
*The implementation of the ISerializer class. Allows de/serialization of objects using either IDataStreams or
 ISerializationDocuments.
 *Currently does not support serialization of pointers (including char*).
 *Support for pointers will be implemented with the pointer handler, once changes to ObjectHandles are completed.
*/

class SerializationHandlerManager;

class SerializerNew : public Implements<ISerializerNew>
{
	friend class SerializationNode;
	friend class ISerializationHandler;

public:
	typedef std::unique_ptr<SerializationNode> NodePtr;

public:
	SerializerNew() = delete;
	SerializerNew(SerializationHandlerManager* handlerManager);

	std::unique_ptr<SerializationDocument> getDocument(SerializationFormat format) override;

	// Variant Serialization
	bool serializeToDocument(const Variant& v, SerializationDocument* doc) override;
	bool serializeToStream(const Variant& v, IDataStream* stream, SerializationFormat format) override;
	bool deserializeFromDocument(Variant& v, SerializationDocument* doc) override;
	bool deserializeFromStream(Variant& v, IDataStream* stream, SerializationFormat format) override;

protected:
	// Handler serialization - these are the functions that will be called recursively.
	bool serializeObject(const Variant& v, const NodePtr& node, bool setType = true);
	bool deserializeObject(Variant& v, const NodePtr& node, const char* typeName = nullptr);

private:
	SerializationHandlerManager* const handlerManager_;
};

} // end namespace wgt