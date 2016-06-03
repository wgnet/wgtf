#ifndef REFLECTED_TREE_MODEL_HPP
#define REFLECTED_TREE_MODEL_HPP

#include "core_data_model/generic_tree_model.hpp"
#include "core_data_model/reflection/reflected_object_item.hpp"


namespace wgt
{
class IDefinitionManager;
class IReflectionController;
class PropertyAccessorListener;


/**
 *	Construct a tree data model by reflecting over the given object.
 */
class ReflectedTreeModel
	: public GenericTreeModel
{
	typedef GenericTreeModel base;
public:

	/**
	 *	Construct a tree data model by reflecting over the given object.
	 *	@param object the root of the tree.
	 *	@param definitionManager goes with the object.
	 *	@param controller used to get/set/invoke reflection data on the object.
	 */
	ReflectedTreeModel( const ObjectHandle & object,
		IDefinitionManager & definitionManager,
		IReflectionController * controller );
	virtual ~ReflectedTreeModel();
	
	virtual void addRootItem( GenericTreeItem * item ) override;
	virtual void removeRootItem( GenericTreeItem * item ) override;
private:
	ReflectedObjectItem rootItem_;
	IDefinitionManager & definitionManager_;
	std::shared_ptr< PropertyAccessorListener > listener_;
};
} // end namespace wgt
#endif
