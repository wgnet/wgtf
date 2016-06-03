#ifndef GENERIC_TREE_MODEL_HPP
#define GENERIC_TREE_MODEL_HPP

#include "i_tree_model.hpp"

namespace wgt
{
class GenericTreeItem;


/**
 *	Tree data model.
 *	Can add or remove GenericTreeItems using an index.
 *	Has no concept of rows and columns.
 */
class GenericTreeModel : public ITreeModel
{
public:
	GenericTreeModel( int columnCount = 1 );
	virtual void addRootItem( GenericTreeItem * item );
	virtual void removeRootItem( GenericTreeItem * item );

	IItem * item( size_t index, const IItem * parent ) const override;
	ItemIndex index( const IItem * item ) const override;

	bool empty( const IItem * item ) const override;
	size_t size( const IItem * item ) const override;
	int columnCount() const override;

private:
	GenericTreeItem * getItemInternal( size_t index, const GenericTreeItem * parent ) const;
	size_t getIndexInternal( const GenericTreeItem * item ) const;
	bool emptyInternal( const GenericTreeItem * item ) const;
	size_t getChildCountInternal( const GenericTreeItem * item ) const;

	std::vector< GenericTreeItem * > rootItems_;
	int columnCount_;
};
} // end namespace wgt
#endif
