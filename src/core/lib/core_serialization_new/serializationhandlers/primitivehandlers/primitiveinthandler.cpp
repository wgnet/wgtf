#include "primitiveinthandler.hpp"
#include "core_serialization_new/serializationdocument.hpp"

namespace wgt
{
PrimitiveIntHandler::PrimitiveIntHandler() : SerializationHandler("__PrimitiveIntHandler")
{
}

bool PrimitiveIntHandler::write(const Variant& v, const NodePtr& node, bool writeType)
{
	intmax_t intValue;
	bool success = v.tryCast<intmax_t>(intValue);
	if (!success)
	{
		return false;
	}

	node->setValueInt(intValue, writeType);

	return true;
}

bool PrimitiveIntHandler::read(Variant& v, const NodePtr& node, const char* typeName)
{
	// Assuming that if read is being called, the type has already been confirmed though handlermanager
	v = node->getValueInt();
	return true;
}

const char* PrimitiveIntHandler::getInternalTypeOf(const Variant& v, const NodePtr& node) const
{
	return node->getDocument()->getPrimitiveNames().intName;
}

bool PrimitiveIntHandler::canHandleWriteInternal(const Variant& v, bool writeType)
{
	// Signed integral
	if (v.typeIs<intmax_t>())
	{
		return true;
	}

	return false;
}

bool PrimitiveIntHandler::canHandleReadInternal(const NodePtr& node, const char* typeName)
{
	std::string nodeTypeName = typeName != nullptr ? typeName : node->getType();
	SerializationDocument::SerializationPrimitiveNames primNames = node->getDocument()->getPrimitiveNames();
	if (nodeTypeName == primNames.intName)
	{
		return true;
	}

	const MetaType* nodeMetaType = MetaType::find(nodeTypeName.c_str());
	if (nodeMetaType == nullptr)
	{
		return false;
	}

	if (nodeMetaType->canConvertTo(MetaType::get<intmax_t>()))
	{
		return true;
	}

	return false;
}
} // end namespace wgt