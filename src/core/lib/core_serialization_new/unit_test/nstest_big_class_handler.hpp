#pragma once
#include "../serializationhandler.hpp"

namespace wgt
{
class NSTestBigClassHandler : public SerializationHandler
{
public:
	NSTestBigClassHandler() : SerializationHandler("NSTestBigClass")
	{
	}

	bool write(const Variant& v, const NodePtr& node, bool writeType = true) override;
	bool read(Variant& v, const NodePtr& node, const char* typeName = nullptr) override;

	const char* getInternalTypeOf(const Variant& v, const NodePtr& node) const override;

private:
	bool canHandleWriteInternal(const Variant& v, bool writeType = true) override;
	bool canHandleReadInternal(const NodePtr& node, const char* typeName = nullptr) override;

	const char* bigClassName_ = "__NSTestBigClass";
};
}