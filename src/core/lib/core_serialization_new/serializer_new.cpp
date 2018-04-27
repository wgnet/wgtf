#include "serializer_new.hpp"
#include "serializationhandlermanager.hpp"
#include "serializationhandler.hpp"
#include "../../lib/core_variant/variant.hpp"
#include "../../lib/core_serialization/resizing_memory_stream.hpp"
#include "xmlserialization/xmlserializationdocument.hpp"
#include "serializationnode.hpp"
#include <string>
#include <cstring>

namespace wgt
{
SerializerNew::SerializerNew(SerializationHandlerManager* handlerManager) : handlerManager_(handlerManager)
{
}

std::unique_ptr<SerializationDocument> SerializerNew::getDocument(SerializationFormat format)
{
	SerializationDocument* doc = nullptr;

	switch (format)
	{
	case wgt::DEFAULT:
	case wgt::XML:
		doc = static_cast<SerializationDocument*>(new XMLSerializationDocument(this));
		break;
	case wgt::END:
		break;
	default:
		break;
	}

	return std::unique_ptr<SerializationDocument>(doc);
}

bool SerializerNew::serializeToDocument(const Variant& v, SerializationDocument* doc)
{
	if (doc == nullptr)
		return false;

	doc->clear();

	auto rootNode = doc->getRootNode();
	auto objectRootNode = rootNode->createEmptyChild("object", 6);

	// TODO: Set the name properly here once handlers are implemented
	objectRootNode->setName("object", 6);

	serializeObject(v, objectRootNode);

	return true;
}

bool SerializerNew::serializeToStream(const Variant& v, IDataStream* stream, SerializationFormat format)
{
	if (stream == nullptr)
		return false;

	auto doc = getDocument(format);

	serializeToDocument(v, doc.get());

	// Write to the stream
	bool success = doc->writeToStream(stream);
	success = stream->sync() && success;
	stream->seek(0, std::ios_base::beg);
	return success;
}

bool SerializerNew::deserializeFromDocument(Variant& v, SerializationDocument* doc)
{
	if (doc == nullptr)
		return false;

	auto rootNode = doc->getRootNode();
	auto objectRootNode = rootNode->getChildNode("object");

	return deserializeObject(v, objectRootNode);
}

bool SerializerNew::deserializeFromStream(Variant& v, IDataStream* stream, SerializationFormat format)
{
	if (stream == nullptr)
		return false;

	auto doc = getDocument(format);

	if (!doc->readFromStream(stream))
	{
		return false;
	}

	return deserializeFromDocument(v, doc.get());
}

bool SerializerNew::serializeObject(const Variant& v, const NodePtr& node, bool setType)
{
	if (node == nullptr)
		return false;

	// if (v.isPointer())
	//{
	// Cannot currently serialize pointers.
	// We actually can to an extent? If the object to be serialized at the root of the document is a pointer, we
	// should serialize it. It's when a member is a pointer that it becomes an issue. Custom classes will be
	// handling that of their own accord, but how do we deal with it properly in the reflected handler?
	// If it's a managed object then just get use the refId and stuff.
	// If it's unmanaged we'll probably have to create a managed object out of it when it goes into serialization.
	// But then what if a reflected object points at an unmanaged object that is also pointed at by custom classes.
	// We'll create our own managed object, but the custom classes would handle it their own way. When deserializing
	// they wouldn't be pointing at the same thing.
	// Managed pointer serialization will be implemented with pointer handler.
	//	return false;
	//}

	// Get handler
	auto handler = handlerManager_->findHandlerWrite(v);
	if (handler != nullptr)
	{
		if (handler->write(v, node, setType))
		{
			return true;
		}
	}

	return false;
}

bool SerializerNew::deserializeObject(Variant& v, const NodePtr& node, const char* typeName)
{
	if (node == nullptr)
		return false;

	auto handler = handlerManager_->findHandlerRead(node, node->getHandlerName().c_str(), typeName);
	if (handler != nullptr)
	{
		if (handler->read(v, node))
		{
			return true;
		}
	}

	return false;
}
}