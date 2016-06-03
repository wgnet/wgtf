#pragma once
#ifndef _REFLECTED_OBJECT_ITEM_NEW_HPP
#define _REFLECTED_OBJECT_ITEM_NEW_HPP

#include "reflected_tree_item_new.hpp"

#include <memory>


namespace wgt
{
/**
 *	Create an item in a ReflectedTreeModel from an ObjectHandle.
 *	Can be a root or child item.
 */
class ReflectedObjectItemNew : public ReflectedTreeItemNew
{
public:

	/**
	 *	Construct a tree data model by reflecting over the given object.
	 *	@param object the root of the tree.
	 *		@warning the parent *must* correspond to the object.
	 *	@param parent the parent model of this property.
	 *		In this case the item is the root of the model.
	 */
	ReflectedObjectItemNew( IComponentContext & contextManager,
		const ObjectHandle & object,
		const ReflectedTreeModelNew & model );
	/**
	 *	Construct a tree data model by reflecting over the given object.
	 *	@param object the root of the tree.
	 *		@warning the parent *must* correspond to the object.
	 *	@param parent the parent item of this property.
	 *		In this case the item is a child item.
	 */
	ReflectedObjectItemNew( IComponentContext & contextManager,
		const ObjectHandle & object,
		ReflectedTreeItemNew * parent,
		size_t index );
	virtual ~ReflectedObjectItemNew();

	// AbstractItem
	virtual Variant getData( int column, size_t roleId ) const override;
	virtual bool setData( int column, size_t roleId, const Variant & data ) override;

	// ReflectedTreeItemNew
	virtual const ObjectHandle & getRootObject() const override;
	virtual const ObjectHandle & getObject() const override;
	virtual const IClassDefinition * getDefinition() const override;

	virtual ReflectedTreeItemNew * getChild( size_t index ) const override;
	virtual int rowCount() const override;

	virtual bool isInPlace() const override;

	virtual bool preSetValue( const PropertyAccessor & accessor, const Variant & value ) override;
	virtual bool postSetValue( const PropertyAccessor & accessor, const Variant & value ) override;

	virtual bool preInsert( const PropertyAccessor & accessor, size_t index, size_t count ) override;
	virtual bool postInserted( const PropertyAccessor & accessor, size_t index, size_t count ) override;

	virtual bool preErase( const PropertyAccessor & accessor, size_t index, size_t count ) override;
	virtual bool postErased( const PropertyAccessor & accessor, size_t index, size_t count ) override;

private:
	class Implementation;
	std::unique_ptr< Implementation > impl_;

	typedef std::function< bool( ReflectedTreeItemNew & ) > ReflectedItemCallback;
	void enumerateChildren( const ReflectedItemCallback & callback ) const;
};
} // end namespace wgt
#endif // _REFLECTED_OBJECT_ITEM_NEW_HPP
