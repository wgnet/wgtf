#pragma once
#include "../serializationhandler.hpp"

namespace wgt
{
class NSTestSmallClassHandler : public SerializationHandler
{
public:
	NSTestSmallClassHandler() : SerializationHandler("NSTestSmallClass")
	{
	}

	bool write(const Variant& v, const NodePtr& node, bool writeType = true) override;

	bool read(Variant& v, const NodePtr& node, const char* typeName = nullptr) override;

	const char* getInternalTypeOf(const Variant& v, const NodePtr& node) const override;

private:
	bool canHandleWriteInternal(const Variant& v, bool writeType = true) override;
	bool canHandleReadInternal(const NodePtr& node, const char* typeName = nullptr) override;

	const char* smallClassName_ = "__NSTestSmallClass";
};
}