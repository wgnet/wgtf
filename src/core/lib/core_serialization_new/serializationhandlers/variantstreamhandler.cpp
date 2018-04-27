#include "variantstreamhandler.hpp"
#include "core_variant/variant.hpp"
#include "core_variant/meta_type.hpp"
#include "core_serialization/resizing_memory_stream.hpp"

namespace wgt
{
VariantStreamHandler::VariantStreamHandler() : SerializationHandler("__VariantStream")
{
}

bool VariantStreamHandler::write(const Variant& v, const NodePtr& node, bool writeType)
{
	// Variant only accepts binary/text streams for streamin/out, so we get to do this fun little stream dance.
	ResizingMemoryStream s;
	TextStream ts(s);
	ts << v;

	// Get the type to be used - leave it null if we shouldn't write the type, setValueRawData won't write a type
	const char* nodeTypeName = nullptr;
	size_t nodeTypeSize = 0;
	if (writeType)
	{
		nodeTypeName = variantName_;
		nodeTypeSize = strlen(nodeTypeName);
	}

	// ResizingMemoryStream is not guaranteed to be null terminated.
	node->setValueRawData(s.buffer().c_str(), s.buffer().size(), nodeTypeName, nodeTypeSize);
	node->setHandlerName(handlerName_);

	auto variantTypeName = this->getReservedNames().variantInternalType;
	auto typeNode = node->createEmptyChild(variantTypeName, strlen(variantTypeName));
	const char* internalTypeName = v.type()->typeId().getName();
	typeNode->setValueString(internalTypeName, strlen(internalTypeName));

	return true;
}

bool VariantStreamHandler::read(Variant& v, const NodePtr& node, const char* typeName)
{
	// Check that we've been given a valid Variant
	if (v.isVoid() || v.type() == nullptr)
	{
		return false;
	}

	auto variantTypeName = this->getReservedNames().variantInternalType;
	std::string typeData = node->getChildNode(variantTypeName, strlen(variantTypeName))->getValueString();
	TypeId typeId(typeData.c_str());

	// Check that the types match
	if (strcmp(v.type()->typeId().getName(), typeId.getName()) != 0)
	{
		return false;
	}

	ResizingMemoryStream rs(node->getValueString());
	TextStream ts(rs);
	ts >> v;

	return true;
}

const char* VariantStreamHandler::getInternalTypeOf(const Variant& v, const NodePtr& node) const
{
	// All variantstream nodes are called __TFSvariant
	// If a handler handles multiple types, it could check which type the Variant is and return the correct name for it.
	return variantName_;
}

bool VariantStreamHandler::canHandleWriteInternal(const Variant& v, bool writeName)
{
	// This handler is a temporary fallback for all types without a handler.
	return true;
}

bool VariantStreamHandler::canHandleReadInternal(const NodePtr& node, const char* typeName)
{
	// Accept everything, since this is the last fallback.
	return false;
}

} // end namespace wgt