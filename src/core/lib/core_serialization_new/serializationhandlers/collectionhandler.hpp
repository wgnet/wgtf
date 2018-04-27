#pragma once
#include "../serializationhandler.hpp"
#include "../serializationhandlermanager.hpp"
#include "../../core_variant/collection.hpp"
#include "core_reflection/i_definition_manager.hpp"

namespace wgt
{
class Collection;

class CollectionHandler : public SerializationHandler
{
public:
	CollectionHandler() = delete;
	CollectionHandler(const SerializationHandlerManager& handlerManager);

	bool write(const Variant& v, const NodePtr& node, bool writeType = true) override;
	bool read(Variant& v, const NodePtr& node, const char* typeName = nullptr) override;

	const char* getInternalTypeOf(const Variant& v, const NodePtr& node) const override;

private:
	bool canHandleWriteInternal(const Variant& v, bool writeType = true) override;
	bool canHandleReadInternal(const NodePtr& node, const char* typeName = nullptr) override;

	const SerializationHandlerManager& handlerManager_;

	const char* collectionName_ = "__TFScollection";
	const char* collectionBaseNodeName_ = "__CollectionBase";
	const char* keyName_ = "__k";
	const char* valueName_ = "__v";
};

} // end namespace wgt