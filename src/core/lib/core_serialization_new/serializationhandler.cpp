#include "serializationhandler.hpp"

namespace wgt
{
SerializationHandler::SerializationHandler(const char* handlerName) : handlerName_(handlerName), reservedNames_()
{
}

const char* SerializationHandler::getName() const
{
	return handlerName_.c_str();
}

const SerializationHandler::ReservedNames& SerializationHandler::getReservedNames() const
{
	return reservedNames_;
}

bool SerializationHandler::canHandleWrite(const Variant& v, const char* handlerName, bool writeType)
{
	if (handlerName != nullptr)
	{
		if (handlerName == handlerName_)
		{
			// Names don't match, so this probably isn't the right handler
			return false;
		}
	}

	// Have to check if we can *actually* handle the given Variant
	return this->canHandleWriteInternal(v, writeType);
}

bool SerializationHandler::canHandleRead(const NodePtr& node, const char* handlerName, const char* typeName)
{
	if (handlerName != nullptr)
	{
		if (handlerName != handlerName_)
		{
			// Names don't match, so this probably isn't the right handler
			return false;
		}
	}

	// Have to check if we can *actually* handle the given Variant
	return this->canHandleReadInternal(node, typeName);
}
}