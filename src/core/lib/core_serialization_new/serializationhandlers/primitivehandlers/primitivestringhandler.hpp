#pragma once
#include "../../serializationhandler.hpp"

namespace wgt
{
class PrimitiveStringHandler : public SerializationHandler
{
public:
	PrimitiveStringHandler();

	bool write(const Variant& v, const NodePtr& node, bool writeType = true) override;
	bool read(Variant& v, const NodePtr& node, const char* typeName = nullptr) override;

	const char* getInternalTypeOf(const Variant& v, const NodePtr& node) const override;

private:
	bool canHandleWriteInternal(const Variant& v, bool writeType = true) override;
	bool canHandleReadInternal(const NodePtr& node, const char* typeName = nullptr) override;
};

} // end namespace wgt