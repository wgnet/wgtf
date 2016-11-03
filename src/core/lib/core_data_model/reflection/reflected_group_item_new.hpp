#pragma once
#ifndef _REFLECTED_GROUP_ITEM_NEW_HPP
#define _REFLECTED_GROUP_ITEM_NEW_HPP

#include "reflected_tree_item_new.hpp"

#include <memory>

namespace wgt
{
class MetaGroupObj;

/** Collects items together with the same group metadata. */
class ReflectedGroupItemNew : public ReflectedTreeItemNew
{
public:
	ReflectedGroupItemNew( IComponentContext & contextManager,
		const MetaGroupObj * groupObj,
		ReflectedTreeItemNew * parent,
		size_t index,
		const std::string & inPlacePath );
	virtual ~ReflectedGroupItemNew();

	// AbstractItem
	virtual Variant getData( int column, ItemRole::Id roleId ) const override;
	virtual bool setData( int column, ItemRole::Id roleId, const Variant & data ) override;

	// ReflectedTreeItemNew
	virtual const ObjectHandle & getRootObject() const override;
	virtual const ObjectHandle & getObject() const override;

	virtual ReflectedTreeItemNew * getChild( size_t index ) const override;
	virtual int rowCount() const override;

	virtual bool preSetValue( const PropertyAccessor & accessor, const Variant & value ) override;
	virtual bool postSetValue( const PropertyAccessor & accessor, const Variant & value ) override;

	virtual bool preInsert( const PropertyAccessor & accessor, size_t index, size_t count ) override;
	virtual bool postInserted( const PropertyAccessor & accessor, size_t index, size_t count ) override;

	virtual bool preErase( const PropertyAccessor & accessor, size_t index, size_t count ) override;
	virtual bool postErased( const PropertyAccessor & accessor, size_t index, size_t count ) override;

private:
	class Implementation;
	std::unique_ptr< Implementation > impl_;

	typedef std::vector< Variant > Variants;
	void getChildValues( Variants & outChildValues ) const;
};
} // end namespace wgt
#endif //_REFLECTED_GROUP_ITEM_NEW_HPP
