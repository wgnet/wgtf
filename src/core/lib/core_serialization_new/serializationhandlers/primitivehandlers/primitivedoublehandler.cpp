#include "primitivedoublehandler.hpp"
#include "core_serialization_new/serializationdocument.hpp"

namespace wgt
{
PrimitiveDoubleHandler::PrimitiveDoubleHandler() : SerializationHandler("__PrimitiveDoubleHandler")
{
}

bool PrimitiveDoubleHandler::write(const Variant& v, const NodePtr& node, bool writeType)
{
	double doubleValue;
	bool success = v.tryCast<double>(doubleValue);
	if (!success)
	{
		return false;
	}

	node->setValueDouble(doubleValue, writeType);

	return true;
}

bool PrimitiveDoubleHandler::read(Variant& v, const NodePtr& node, const char* typeName)
{
	// Assuming that if read is being called, the type has already been confirmed though handlermanager
	v = node->getValueDouble();
	return true;
}

const char* PrimitiveDoubleHandler::getInternalTypeOf(const Variant& v, const NodePtr& node) const
{
	return node->getDocument()->getPrimitiveNames().doubleName;
}

bool PrimitiveDoubleHandler::canHandleWriteInternal(const Variant& v, bool writeType)
{
	// Double
	if (v.typeIs<double>())
	{
		return true;
	}

	return false;
}

bool PrimitiveDoubleHandler::canHandleReadInternal(const NodePtr& node, const char* typeName)
{
	std::string nodeTypeName = typeName != nullptr ? typeName : node->getType();
	SerializationDocument::SerializationPrimitiveNames primNames = node->getDocument()->getPrimitiveNames();
	if (nodeTypeName == primNames.doubleName)
	{
		return true;
	}

	const MetaType* nodeMetaType = MetaType::find(nodeTypeName.c_str());
	if (nodeMetaType == nullptr)
	{
		return false;
	}

	if (nodeMetaType->canConvertTo(MetaType::get<double>()))
	{
		return true;
	}

	return false;
}
} // end namespace wgt