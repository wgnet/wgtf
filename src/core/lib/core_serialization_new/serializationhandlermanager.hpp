#pragma once
#include "interfaces/core_serialization_new/ISerializationHandlerManager.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_dependency_system/i_interface.hpp"
#include <memory>
#include <forward_list>

namespace wgt
{
class SerializationHandler;

class SerializationHandlerManager : public Implements<ISerializationHandlerManager>
{
public:
	SerializationHandlerManager() = delete;
	SerializationHandlerManager(const IDefinitionManager& definitionManager);
	~SerializationHandlerManager();

	void registerHandler(std::shared_ptr<SerializationHandler> handler, int64_t priority = -1) override;
	void deregisterHandler(std::shared_ptr<SerializationHandler> handler) override;

	size_t getHandlerCount() const override;

	std::shared_ptr<SerializationHandler> findHandlerWrite(const Variant& v, const char* name = nullptr,
	                                                       bool writeType = true) const override;
	std::shared_ptr<SerializationHandler> findHandlerRead(const NodePtr& node, const char* name = nullptr,
	                                                      const char* typeName = nullptr) const override;

private:
	std::vector<std::shared_ptr<SerializationHandler>> handlers_;

	std::shared_ptr<SerializationHandler> reflectedHandler_;
	std::shared_ptr<SerializationHandler> primitiveIntHandler_;
	std::shared_ptr<SerializationHandler> primitiveUintHandler_;
	std::shared_ptr<SerializationHandler> primitiveDoubleHandler_;
	std::shared_ptr<SerializationHandler> primitiveStringHandler_;
	std::shared_ptr<SerializationHandler> variantHandler_;
	std::shared_ptr<SerializationHandler> collectionHandler_;

	const IDefinitionManager& definitionManager_;
};

} // end namespace wgt