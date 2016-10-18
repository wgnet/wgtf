#include "generic_tree_model.hpp"
#include "generic_tree_item.hpp"


namespace wgt
{
GenericTreeModel::GenericTreeModel( int columnCount/* = 1 */ )
	:columnCount_( columnCount )
{
}

void GenericTreeModel::addRootItem( GenericTreeItem * item )
{
	assert( item->getParent() == nullptr );
	assert( item->model_ == nullptr );
	item->model_ = this;

    size_t index = rootItems_.size();
    signalPreItemsInserted(nullptr, index, 1);
	rootItems_.push_back( item );
    signalPostItemsInserted(nullptr, index, 1);
}

void GenericTreeModel::removeRootItem( GenericTreeItem * item )
{
	assert( item != nullptr );
	assert( item->model_ == this );
	const auto foundItr =
		std::find( rootItems_.cbegin(), rootItems_.cend(), item );
	assert( foundItr != rootItems_.cend() );

	item->model_ = nullptr;
    auto findIter = std::find(rootItems_.begin(), rootItems_.end(), item);
    if (findIter == rootItems_.end())
        return;

    size_t index = std::distance(findIter, rootItems_.begin());
    signalPreItemsRemoved(nullptr, index, 1);
	rootItems_.erase( foundItr );
    signalPostItemsRemoved(nullptr, index, 1);
}

IItem * GenericTreeModel::item( size_t index, const IItem * parent ) const
{
	auto genericParent = static_cast< const GenericTreeItem * >( parent );
	assert( parent == nullptr || genericParent != nullptr );

	auto itemCount = getChildCountInternal( genericParent );
	for (size_t i = 0; i < itemCount; ++i)
	{
		auto item = getItemInternal( i, genericParent );
		if (item != nullptr && item->hidden())
		{
			auto childItemCount = getChildCountInternal( item );
			if (index < childItemCount)
			{
				return this->item( index, item );
			}
			index -= childItemCount;
		}
		else
		{
			if (index == 0)
			{
				return item;
			}
			--index;
		}
	}
	return nullptr;
}

ITreeModel::ItemIndex GenericTreeModel::index( const IItem * item ) const
{
	if (item == nullptr)
	{
		return std::make_pair< size_t, IItem * >( 0, nullptr );
	}

	auto genericItem = static_cast< const GenericTreeItem * >( item );
	assert( genericItem != nullptr );

	size_t index = 0;
	auto parent = genericItem->getParent();
	auto indexInternal = getIndexInternal( genericItem );
	for (size_t i = 0; i < indexInternal; ++i)
	{
		auto itemInternal = getItemInternal( i, parent );
		index += itemInternal != nullptr && itemInternal->hidden() ? 
			this->size( itemInternal ) : 1;
	}

	if (parent != nullptr && parent->hidden())
	{
		auto parentIndex = this->index( parent );
		index += parentIndex.first;
		parent = const_cast<GenericTreeItem *>(
			static_cast< const GenericTreeItem * >( parentIndex.second ) );
	}
	return std::make_pair( index, parent );
}


/**
 *	Check if tree has child items, excluding hidden items, including null items
 *	E.g.
 *	> group1
 *	>> group2 <- hidden
 *	>>> item1 - count
 *	>>> item2 - count
 *	>>> item3 - count
 *	hasChildren( group1 ) == true
 *	E.g.
 *	> group1
 *	>> group2 <- hidden
 *	>> group3 <- hidden
 *	hasChildren( group1 ) == false
 */
bool GenericTreeModel::empty( const IItem * item ) const
{
	auto genericItem = static_cast< const GenericTreeItem * >( item );
	assert( item == nullptr || genericItem != nullptr );

	// No children
	if (this->emptyInternal( genericItem ))
	{
		return true;
	}

	// Has children, but they might be hidden
	auto childCount = getChildCountInternal( genericItem );
	for (size_t i = 0; i < childCount; ++i)
	{
		auto childItem = getItemInternal( i, genericItem );
		if (childItem == nullptr || !childItem->hidden() || !this->empty( childItem ))
		{
			return false;
		}
	}

	// All children are hidden
	return true;
}


/**
 *	Get the number of child items, excluding hidden items, including null items
 *	E.g.
 *	> group1
 *	>> group2 <- hidden
 *	>>> item1 - count
 *	>>> item2 - count
 *	>>> item3 - count
 *	>> item4 - count
 *	getChildCount( group1 ) == 4
 *	E.g.
 *	> group1
 *	>> group2 <- hidden
 *	>> group3 <- hidden
 *	getChildCount( group1 ) == 0
 */
size_t GenericTreeModel::size( const IItem * item ) const
{
	auto genericItem = static_cast< const GenericTreeItem * >( item );
	assert( item == nullptr || genericItem != nullptr );

	size_t count = 0;
	auto childCount = getChildCountInternal( genericItem );
	for (size_t i = 0; i < childCount; ++i)
	{
		auto childItem = getItemInternal( i, genericItem );
		count += childItem != nullptr && childItem->hidden() ? this->size( childItem ) : 1;
	}
	return count;
}

int GenericTreeModel::columnCount() const
{
	return columnCount_;
}

GenericTreeItem * GenericTreeModel::getItemInternal( size_t index, const GenericTreeItem * parent ) const
{
	if (parent == nullptr)
	{
		return index < rootItems_.size() ? rootItems_[index] : nullptr;
	}

	return parent->getChild( index );
}

size_t GenericTreeModel::getIndexInternal( const GenericTreeItem * item ) const
{
	if (item == nullptr)
	{
		return 0;
	}

	auto parent = item->getParent();
	if (parent != nullptr)
	{
		const auto count = parent->size();
		for (size_t i = 0; i < count; ++i)
		{
			if (parent->getChild( i ) == item)
			{
				return i;
			}
		}
	}

	auto findIt = std::find( rootItems_.begin(), rootItems_.end(), item );
	assert( findIt != rootItems_.end() );
	return findIt - rootItems_.begin();
}


/**
 *	Check if there are children in tree, at one level, under the given item.
 *	Including hidden items.
 *	E.g.
 *	> group1
 *	>> group2 <- hidden
 *	hasChildrenInternal( group1 ) == true
 */
bool GenericTreeModel::emptyInternal(
	const GenericTreeItem * item ) const
{
	if (item == nullptr)
	{
		return rootItems_.empty();
	}

	return item->empty();
}


/**
 *	Get number of children in tree, at one level, under the given item.
 *	Including hidden items.
 *	E.g.
 *	> group1
 *	>> group2 <- hidden
 *	>>> item1 - count
 *	>>> item2 - count
 *	>>> item3 - count
 *	>> item4 - count
 *	getChildCountInternal( group1 ) == 2 // group2 and item4
 */
size_t GenericTreeModel::getChildCountInternal( const GenericTreeItem * item ) const
{
	if (item == nullptr)
	{
		return rootItems_.size();
	}

	return item->size();
}
} // end namespace wgt
