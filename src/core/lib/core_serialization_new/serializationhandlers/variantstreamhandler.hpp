#pragma once
#include "../serializationhandler.hpp"

namespace wgt
{
class VariantStreamHandler : public SerializationHandler
{
public:
	VariantStreamHandler();

	bool write(const Variant& v, const NodePtr& node, bool writeType = true) override;
	bool read(Variant& v, const NodePtr& node, const char* typeName = nullptr) override;

	// Allows the handler to specify what the types it handles should be called.
	const char* getInternalTypeOf(const Variant& v, const NodePtr& node) const override;

private:
	bool canHandleWriteInternal(const Variant& v, bool writeName = true) override;
	bool canHandleReadInternal(const NodePtr& node, const char* typeName = nullptr) override;

	const char* variantName_ = "__TFSvariant";
};

} // end namespace wgt