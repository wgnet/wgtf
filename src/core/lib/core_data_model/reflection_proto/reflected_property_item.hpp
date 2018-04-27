#ifndef PROTO_REFLECTED_PROPERTY_ITEM_HPP
#define PROTO_REFLECTED_PROPERTY_ITEM_HPP

#include "core_data_model/abstract_item_model.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_dependency_system/depends.hpp"

namespace wgt
{
class IDefinitionManager;
class IReflectionController;
class IClassDefinition;
class IAssetManager;
class IPropertyPath;

namespace proto
{
class ReflectedTreeModel;

class ReflectedPropertyItem : public AbstractTreeItem, Depends<IReflectionController, IDefinitionManager, IAssetManager>
{
public:
	ReflectedPropertyItem(const ReflectedTreeModel& model, const std::shared_ptr< const IPropertyPath > & path, bool recordHistory = true);
	virtual ~ReflectedPropertyItem();

	const std::shared_ptr< const IPropertyPath > & getPath() const;
	void setPath(const std::shared_ptr< const IPropertyPath > & path);

	// AbstractItem
	virtual Variant getData(int column, ItemRole::Id roleId) const override;
	virtual bool setData(int column, ItemRole::Id roleId, const Variant& data) override;

	virtual bool hasController() const override
	{
		return true;
	}

private:
	const std::string getPathName() const;
	bool isReadOnly(const PropertyAccessor& propertyAccessor) const;
	PropertyAccessor parentCollectionPropertyAccessor(const PropertyAccessor& propertyAccessor, IClassDefinition* definition) const;

	const ReflectedTreeModel& model_;
	ObjectHandle object_;

	std::shared_ptr< const IPropertyPath > path_;
	bool recordHistory_;
	mutable std::unique_ptr<AbstractListModel> enumModel_;
	mutable std::unique_ptr<AbstractListModel> definitionModel_;

	// TEMP
public:
	ObjectHandle objectReference_;
	std::string referencePath_;
};

typedef std::unique_ptr<ReflectedPropertyItem> ReflectedPropertyItemPtr;
}
}

#endif // PROTO_REFLECTED_PROPERTY_ITEM_HPP