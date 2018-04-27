#include "primitivestringhandler.hpp"
#include "core_serialization_new/serializationdocument.hpp"

namespace wgt
{
PrimitiveStringHandler::PrimitiveStringHandler() : SerializationHandler("__PrimitiveStringHandler")
{
}

bool PrimitiveStringHandler::write(const Variant& v, const NodePtr& node, bool writeType)
{
	std::string stringValue;
	bool success = v.tryCast<std::string>(stringValue);
	if (!success)
	{
		return false;
	}

	node->setValueString(stringValue, writeType);

	return true;
}

bool PrimitiveStringHandler::read(Variant& v, const NodePtr& node, const char* typeName)
{
	// Assuming that if read is being called, the type has already been confirmed though handlermanager
	v = node->getValueString();
	return true;
}

const char* PrimitiveStringHandler::getInternalTypeOf(const Variant& v, const NodePtr& node) const
{
	return node->getDocument()->getPrimitiveNames().stringName;
}

bool PrimitiveStringHandler::canHandleWriteInternal(const Variant& v, bool writeType)
{
	// String
	if (v.typeIs<std::string>())
	{
		return true;
	}

	return false;
}

bool PrimitiveStringHandler::canHandleReadInternal(const NodePtr& node, const char* typeName)
{
	std::string nodeTypeName = typeName != nullptr ? typeName : node->getType();
	SerializationDocument::SerializationPrimitiveNames primNames = node->getDocument()->getPrimitiveNames();
	if (nodeTypeName == primNames.stringName)
	{
		return true;
	}

	const MetaType* nodeMetaType = MetaType::find(nodeTypeName.c_str());
	if (nodeMetaType == nullptr)
	{
		return false;
	}

	if (nodeMetaType->canConvertTo(MetaType::get<std::string>()))
	{
		return true;
	}

	return false;
}
} // end namespace wgt