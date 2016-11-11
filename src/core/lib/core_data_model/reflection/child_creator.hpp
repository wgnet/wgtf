#ifndef CHILD_CREATOR_HPP
#define CHILD_CREATOR_HPP

#include "property_model_extensions.hpp"

#include "core_common/signal.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_dependency_system/depends.hpp"

namespace wgt
{
class ChildCreator
{
public:
	ChildCreator(IComponentContext& context);
	~ChildCreator();

	std::shared_ptr<const PropertyNode> createRoot(const ObjectHandle& handle);
	void updateSubTree(const std::shared_ptr<const PropertyNode>& parent);
	void removeNode(const std::shared_ptr<const PropertyNode>& parent);
	void clear();

	void registerExtension(const std::shared_ptr<ChildCreatorExtension>& extension);
	void unregisterExtension(const std::shared_ptr<ChildCreatorExtension>& extension);

	Signal<void(std::shared_ptr<const PropertyNode> parent, std::shared_ptr<const PropertyNode> child,
	            size_t childPosition)>
	nodeCreated;
	Signal<void(std::shared_ptr<const PropertyNode> child)> nodeRemoved;

private:
	std::shared_ptr<ChildCreatorExtension> extensions;
	Depends<IDefinitionManager> interfaceHolder;

	std::unordered_map<std::shared_ptr<const PropertyNode>, std::vector<std::shared_ptr<const PropertyNode>>>
	propertiesIndex;
	std::shared_ptr<IChildAllocator> allocator;
};

} // namespace wgt
#endif