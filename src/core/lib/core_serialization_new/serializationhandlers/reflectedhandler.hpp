#pragma once
#include "../serializationhandler.hpp"
#include "core_reflection/i_definition_manager.hpp"

namespace wgt
{
class ReflectedHandler : public SerializationHandler
{
public:
	ReflectedHandler() = delete;
	ReflectedHandler(const IDefinitionManager& definitionManager);

	bool write(const Variant& v, const NodePtr& node, bool writeType = true) override;
	bool read(Variant& v, const NodePtr& node, const char* typeName = nullptr) override;

	const char* getInternalTypeOf(const Variant& v, const NodePtr& node) const override;

private:
	bool canHandleWriteInternal(const Variant& v, bool writeType = true) override;
	bool canHandleReadInternal(const NodePtr& node, const char* typeName = nullptr) override;

	const IDefinitionManager& definitionManager_;

	const char* reflectedName_ = "__TFSreflected";
};

} // end namespace wgt