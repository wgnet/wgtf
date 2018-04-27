#include "primitiveuinthandler.hpp"
#include "core_serialization_new/serializationdocument.hpp"

namespace wgt
{
PrimitiveUintHandler::PrimitiveUintHandler() : SerializationHandler("__PrimitiveUintHandler")
{
}

bool PrimitiveUintHandler::write(const Variant& v, const NodePtr& node, bool writeType)
{
	uintmax_t uintValue;
	bool success = v.tryCast<uintmax_t>(uintValue);
	if (!success)
	{
		return false;
	}

	node->setValueUint(uintValue, writeType);

	return true;
}

bool PrimitiveUintHandler::read(Variant& v, const NodePtr& node, const char* typeName)
{
	// Assuming that if read is being called, the type has already been confirmed though handlermanager
	v = node->getValueUint();
	return true;
}

const char* PrimitiveUintHandler::getInternalTypeOf(const Variant& v, const NodePtr& node) const
{
	return node->getDocument()->getPrimitiveNames().uintName;
}

bool PrimitiveUintHandler::canHandleWriteInternal(const Variant& v, bool writeType)
{
	// Unsigned integral
	if (v.typeIs<uintmax_t>())
	{
		return true;
	}

	return false;
}

bool PrimitiveUintHandler::canHandleReadInternal(const NodePtr& node, const char* typeName)
{
	std::string nodeTypeName = typeName != nullptr ? typeName : node->getType();
	SerializationDocument::SerializationPrimitiveNames primNames = node->getDocument()->getPrimitiveNames();
	if (nodeTypeName == primNames.uintName)
	{
		return true;
	}

	const MetaType* nodeMetaType = MetaType::find(nodeTypeName.c_str());
	if (nodeMetaType == nullptr)
	{
		return false;
	}

	if (nodeMetaType->canConvertTo(MetaType::get<uintmax_t>()))
	{
		return true;
	}

	return false;
}
} // end namespace wgt