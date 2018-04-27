#include "property_model_extensions.hpp"

#include "core_reflection/metadata/meta_types.hpp"
#include "core_variant/type_id.hpp"

namespace wgt
{
namespace PMEDetails
{
class SelfProperty : public IBaseProperty
{
public:
	const TypeId& getType() const override
	{
		return TypeId::getType<ObjectHandle>();
	}

	const char* getName() const override
	{
		return "Self";
	}

	uint64_t getNameHash() const override
	{
		static uint64_t hash = HashUtilities::compute(getName());
		return hash;
	}

	MetaHandle getMetaData() const override
	{
		return MetaNone();
	}

	bool readOnly() const override
	{
		return true;
	}

	bool isMethod() const override
	{
		return false;
	}

	bool isValue() const override
	{
		return true;
	}

	bool isCollection() const override
	{
		return false;
	}

	bool set(const ObjectHandle&, const Variant&, const IDefinitionManager&) const override
	{
		return false;
	}

	Variant get(const ObjectHandle& handle, const IDefinitionManager&) const override
	{
		return Variant(handle);
	}

	size_t parameterCount() const override
	{
		return 0;
	}

	Variant invoke(const ObjectHandle&, const IDefinitionManager&, const ReflectedMethodParameters&) override
	{
		return Variant();
	}
};

class DummyChildCreator : public ChildCreatorExtension
{
public:
	void exposeChildren(const std::shared_ptr<const PropertyNode>&, std::vector<std::shared_ptr<const PropertyNode>>&,
	                    IDefinitionManager&) const override
	{
	}
};

class DummySetterGetter : public SetterGetterExtension
{
public:
	Variant getValue(const RefPropertyItem*, int, ItemRole::Id, IDefinitionManager&) const override
	{
		return Variant();
	}

	bool setValue(RefPropertyItem*, int, ItemRole::Id, const Variant&, IDefinitionManager&,
	              ICommandManager&) const override
	{
		return false;
	}
};

class DummyMerger : public MergeValuesExtension
{
public:
	RefPropertyItem* lookUpItem(const std::shared_ptr<const PropertyNode>& node,
	                            const std::vector<std::unique_ptr<RefPropertyItem>>& items,
	                            IDefinitionManager& definitionManager) const override
	{
		return nullptr;
	}
};

class DummyInjector : public InjectDataExtension
{
public:
	void inject(RefPropertyItem* item) override
	{
	}
	void updateInjection(RefPropertyItem* item) override
	{
	}
};

IBasePropertyPtr selfRootProperty = std::make_shared<SelfProperty>();
}

PropertyNode::PropertyNode() : propertyType(-1)
{
}

bool PropertyNode::operator==(const PropertyNode& other) const
{
	return propertyType == other.propertyType && propertyInstance == other.propertyInstance && object == other.object;
}

bool PropertyNode::operator!=(const PropertyNode& other) const
{
	return propertyType != other.propertyType || propertyInstance != other.propertyInstance || object != other.object;
}

bool PropertyNode::operator<(const PropertyNode& other) const
{
	if (propertyType != other.propertyType)
		return propertyType < other.propertyType;
	if (propertyInstance != other.propertyInstance)
		return propertyInstance < other.propertyInstance;

	return object < other.object;
}

std::shared_ptr<const PropertyNode> MakeRootNode(ObjectHandle handle, IChildAllocator& allocator)
{
	return allocator.createPropertyNode(PMEDetails::selfRootProperty, handle, PropertyNode::SelfRoot);
}

ChildCreatorExtension::ChildCreatorExtension() : ExtensionChain(TypeId::getType<ChildCreatorExtension>())
{
}

void ChildCreatorExtension::exposeChildren(const std::shared_ptr<const PropertyNode>& node,
                                           std::vector<std::shared_ptr<const PropertyNode>>& children,
                                           IDefinitionManager& defMng) const
{
	getNext<ChildCreatorExtension>()->exposeChildren(node, children, defMng);
}

std::shared_ptr<ChildCreatorExtension> ChildCreatorExtension::createDummy()
{
	return std::make_shared<PMEDetails::DummyChildCreator>();
}

void ChildCreatorExtension::setAllocator(std::shared_ptr<IChildAllocator> allocator_)
{
	allocator = allocator_;
}

SetterGetterExtension::SetterGetterExtension() : ExtensionChain(TypeId::getType<SetterGetterExtension>())
{
}

Variant SetterGetterExtension::getValue(const RefPropertyItem* item, int column, ItemRole::Id roleId,
                                        IDefinitionManager& definitionManager) const
{
	return getNext<SetterGetterExtension>()->getValue(item, column, roleId, definitionManager);
}

bool SetterGetterExtension::setValue(RefPropertyItem* item, int column, ItemRole::Id roleId, const Variant& data,
                                     IDefinitionManager& definitionManager, ICommandManager& commandManager) const
{
	return getNext<SetterGetterExtension>()->setValue(item, column, roleId, data, definitionManager, commandManager);
}

std::shared_ptr<SetterGetterExtension> SetterGetterExtension::createDummy()
{
	return std::make_shared<PMEDetails::DummySetterGetter>();
}

MergeValuesExtension::MergeValuesExtension() : ExtensionChain(TypeId::getType<MergeValuesExtension>())
{
}

RefPropertyItem* MergeValuesExtension::lookUpItem(const std::shared_ptr<const PropertyNode>& node,
                                                  const std::vector<std::unique_ptr<RefPropertyItem>>& items,
                                                  IDefinitionManager& definitionManager) const
{
	return getNext<MergeValuesExtension>()->lookUpItem(node, items, definitionManager);
}

std::shared_ptr<MergeValuesExtension> MergeValuesExtension::createDummy()
{
	return std::make_shared<PMEDetails::DummyMerger>();
}

InjectDataExtension::InjectDataExtension() : ExtensionChain(TypeId::getType<InjectDataExtension>())
{
}

void InjectDataExtension::inject(RefPropertyItem* item)
{
	return getNext<InjectDataExtension>()->inject(item);
}

void InjectDataExtension::updateInjection(RefPropertyItem* item)
{
	return getNext<InjectDataExtension>()->updateInjection(item);
}

std::shared_ptr<InjectDataExtension> InjectDataExtension::createDummy()
{
	return std::make_shared<PMEDetails::DummyInjector>();
}

} // namespace wgt