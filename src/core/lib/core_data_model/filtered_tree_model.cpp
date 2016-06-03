/*
This model uses an index map to map indices from a source model to filtered
indices exposed by this model. Both the source model and the filtered model
consist of a tree of IItem*. The map consists of IItem* with a vector of indices
pointing to the indices in the source model that are included in this model.
Only items that satisfy the filter function are included, and only indices of
items that satisfy the filter function are included. The two exceptions are that
a parent is included (even if not in the filter) if any of its descendants
satisfy the filter function, and that a child is included (even if not in the
filter) if one of its ancestors is in the filter. A position of an item is
identified by a parent item and an index of the parent's children.

An example of a model could be:
parent	parent
item	index	item	index
				AAAA	0
				BBBB	1
				CCCC	2
AAAA	0		AA01	0
AAAA	0		AA02	1
AAAA	0		AA03	2
AAAA	0		AA04	3
AA02	1		A021	0
CCCC	2		CC01	0
CCCC	2		CC02	1

Seen as a tree, it would look as follows:
root
 |-AAAA
 |  |-AA01
 |  |-AA02
 |  |  |-A021
 |  |-AA03
 |  |-AA04
 |-BBBB
 |-CCCC
    |-CC01
    |-CC02

A filtered model of that model could be:
parent	parent
item	index	item	index
				AAAA	0
				CCCC	2
AAAA	0		AA04	3
CCCC	2		CC02	1

The mapped indices would be:
parent	indices
null	0,2
AAAA	3
CCCC	1

If a filter is changed, the source model didn't change, but our mapping is
outdated. This is updated by a refresh. Refresh needs to do the following:
For each item in the source model, check if it was in the filter previously
and also if it satisfies the new filter. An item is in the filter if it matches
the filter function, or if any of its descendants match the filter function.
If the before and after checks are the same, then nothing needs to be done,
but if it's different the mapped index needs to either be removed
(if it's not in the filter anymore), or added (if it's in the filter now).
If an index is removed, the entry in the map for its mapped indices needs to be
removed as well, and recursively onwards. If an index is added, all its children
need to be added too.

If something in the source model changed, the source model indices are
different, but the filter is the same. This is handled by our event handlers
preItemsInserted, postItemsInserted, preItemsRemoved, postItemsRemoved,
preDataChanged, and postDataChanged. [1] If an item is new and matches the
filter, or one of its descendants match the filter, the item as well as all of
its ancestors need to be inserted if not present yet. All descendants need to
be inserted if the item matches the filter, and if the item doesn't match the
filter but a descendant does, then the same check needs to happen for each of
its children. For every item inserted, all indices after the insert point needs
to be adjusted by adding one, because these pointed to items that moved up when
the item was inserted into the source model. [2] For items removed, ancestors
need to be checked if they still satisfy the filter and removed if they don't.
If the removed item exists in the index map, it needs to be removed and well as
all descendants. For every item removed, all indices after the remove point need
to be adjusted by subtracting one, because these pointed to items that moved
down the list when the item was removed from the source model. [3] For items
that changed, the new value could mean that it does/doesn't satisfy the filter
anymore. If nothing changed, the index map stays the same, but the event needs
to be passed on if the item is in the filter. If not in the filter anymore, it
needs to be removed from the index map, and if now in the filter, it needs to be
added to the index map.

Events:
For every insert, a preItemsInserted needs to be fired before the change, and
a postItemsInserted after the change.
For every delete, a preItemsRemoved needs to be fired before the change, and
a postItemsRemoved after the change.
For every change in the item, a preDataChanged needs to be fired before the
change, and a postDataChange after the change.

This is to tell the view to update its data.
*/

#include "filtered_tree_model.hpp"
#include "core_variant/variant.hpp"

#include <unordered_map>
#include <vector>
#include <algorithm>
#include <mutex>
#include <thread>
#include <functional>
#include <atomic>
#include <cassert>

namespace wgt
{
struct FilteredTreeModel::Implementation
{
	enum class FilterUpdateType
	{
		UPDATE,
		INSERT,
		REMOVE,
		IGNORE
	};

	typedef std::unordered_map<const IItem*, std::vector<size_t>> IndexMap;

	Implementation( FilteredTreeModel & self );

	Implementation( FilteredTreeModel & self, const FilteredTreeModel::Implementation & rhs );

	~Implementation();

	void initialize();

	void haltRemapping();

	void setSource( ITreeModel * source );

	bool empty( const IItem* item ) const;

	std::vector<size_t>* findItemsToInsert(
		const IItem* parent, size_t index, size_t count, size_t& mappedIndex,
		std::vector<size_t>& newIndices, std::vector<bool>& inFilter );
	std::vector<size_t>* findItemsToRemove(
		const IItem* parent, size_t index, size_t count,
		size_t& mappedIndex, size_t& mappedCount );

	void shiftItems(
		size_t index, size_t offset, const IItem* parent,
		std::vector<size_t>& mappedIndices );
	void insertItems( size_t index, size_t offset, const IItem* parent,
		std::vector<size_t>& mappedIndices,
		std::vector<size_t>& newIndices,
		std::vector<bool>& inFilter, bool shift = true );
	void removeItems( size_t index, size_t count, size_t offset,
		const IItem* parent, std::vector<size_t>& mappedIndices,
		bool removeParent = true );

	FilterUpdateType checkUpdateType(
		const IItem* item, ItemIndex& sourceIndex, size_t& newIndex ) const;
	void updateItem(
		const IItem* item, const ItemIndex& sourceIndex,
		size_t newIndex, FilterUpdateType type );

	ItemIndex findInsertPoint( const IItem* parent, size_t index ) const;
	ItemIndex findRemovePoint( const IItem* parent, size_t index, size_t count ) const;

	size_t findMappedIndex( const IItem* parent, size_t index ) const;
	size_t getSourceIndex( const IItem* parent, size_t index ) const;
	size_t getMappedIndex( const IItem* parent, size_t index ) const;

	void removeMappedIndices( const IItem* parent );
	std::vector<size_t>& createMappedIndices( const IItem* parent );
	std::vector<size_t>* findMappedIndices( const IItem* parent );
	const std::vector<size_t>* findMappedIndices( const IItem* parent ) const;
	const std::vector<size_t>& getMappedIndices( const IItem* parent ) const;

	bool mapIndices( const IItem* parent, bool parentInFilter );
	void mapIndices();
	void remapIndices( const IItem* parent, bool parentInFilter );
	void remapIndices();
	void copyIndices( IndexMap& target ) const;

	void preItemDataChanged( const IItem * item, int column, size_t roleId, const Variant & data ); 
	void postItemDataChanged( const IItem * item, int column, size_t roleId, const Variant & data );
	void preItemsInserted( const IItem * parent, size_t index, size_t count );
	void postItemsInserted( const IItem * parent, size_t index, size_t count );
	void preItemsRemoved( const IItem * parent, size_t index, size_t count );
	void postItemsRemoved( const IItem * parent, size_t index, size_t count );
	void onDestructing();

	bool ancestorFilterMatched( const IItem* item ) const;
	bool filterMatched( const IItem* item ) const;
	bool descendantFilterMatched( const IItem* item ) const;

	struct UpdateData
	{
		UpdateData()
			: parent_( nullptr ), index_( 0 ), count_( 0 ), valid_( false )
		{}

		void set(
			const IItem* parent = nullptr,
			size_t index = 0,
			size_t count = 0,
			bool valid = false )
		{
			parent_ = parent;
			index_ = index;
			count_ = count;
			valid_ = valid;
		}

		const IItem* parent_;
		size_t index_;
		size_t count_;
		bool valid_;
	} lastUpdateData_;

	FilteredTreeModel& self_;
	ITreeModel * model_;
	IItemFilter * filter_;
	IndexMap indexMap_;
	mutable std::recursive_mutex indexMapMutex_;
	mutable std::mutex eventControlMutex_;
	std::atomic_uint_fast8_t remapping_;
	std::atomic<bool> stopRemapping_;
	ConnectionHolder connections_;

	static const size_t INVALID_INDEX = SIZE_MAX;
};

FilteredTreeModel::Implementation::Implementation( FilteredTreeModel & self )
	: self_( self )
	, model_( nullptr )
	, filter_( nullptr )
{
	mapIndices();
	initialize();
}

FilteredTreeModel::Implementation::Implementation(
	FilteredTreeModel& self,
	const FilteredTreeModel::Implementation& rhs )
	: self_( self )
	, model_( rhs.model_ )
	, filter_( rhs.filter_ )
{
	rhs.copyIndices( indexMap_ );
	initialize();
}

FilteredTreeModel::Implementation::~Implementation()
{
}

void FilteredTreeModel::Implementation::haltRemapping()
{	
	stopRemapping_ = true;

	while (remapping_ > 0)
	{
		std::this_thread::yield();
	}
}

void FilteredTreeModel::Implementation::initialize()
{
	remapping_ = 0;
	stopRemapping_ = false;
}

void FilteredTreeModel::Implementation::setSource( ITreeModel * source )
{
	connections_.clear();
	model_ = source;
	if (model_ != nullptr)
	{
		using namespace std::placeholders;
		connections_ += model_->signalPreItemDataChanged.connect( std::bind( &FilteredTreeModel::Implementation::preItemDataChanged, this, _1, _2, _3, _4 ) );
		connections_ += model_->signalPostItemDataChanged.connect( std::bind( &FilteredTreeModel::Implementation::postItemDataChanged, this, _1, _2, _3, _4 ) );
		connections_ += model_->signalPreItemsInserted.connect( std::bind( &FilteredTreeModel::Implementation::preItemsInserted, this, _1, _2, _3 ) );
		connections_ += model_->signalPostItemsInserted.connect( std::bind( &FilteredTreeModel::Implementation::postItemsInserted, this, _1, _2, _3 ) );
		connections_ += model_->signalPreItemsRemoved.connect( std::bind( &FilteredTreeModel::Implementation::preItemsRemoved, this, _1, _2, _3 ) );
		connections_ += model_->signalPostItemsRemoved.connect( std::bind( &FilteredTreeModel::Implementation::postItemsRemoved, this, _1, _2, _3 ) );
		connections_ += model_->signalDestructing.connect( std::bind( &FilteredTreeModel::Implementation::onDestructing, this ) );
	}
}

bool FilteredTreeModel::Implementation::empty( const IItem* item ) const
{
	if (model_ == nullptr || model_->empty( item ))
	{
		return true;
	}

	if (this->ancestorFilterMatched( item ))
	{
		return false;
	}

	const size_t max = model_->size( item );
	for (size_t index = 0; index < max; ++index)
	{
		const IItem* child = model_->item( index, item );

		if (this->filterMatched( child ) ||
			this->descendantFilterMatched( child ))
		{
			return false;
		}
	}

	return true;
}

std::vector<size_t>* FilteredTreeModel::Implementation::findItemsToInsert(
	const IItem* parent, size_t index, size_t count, size_t& mappedIndex,
	std::vector<size_t>& newIndices, std::vector<bool>& inFilter )
{
	std::vector<size_t>* mappedIndicesPointer =
		findMappedIndices( parent );

	if (mappedIndicesPointer == nullptr)
	{
		mappedIndex = 0;
		// Optimization, delay load until getChildCount is called.
		return nullptr;
	}
	else
	{
		auto itr = std::lower_bound(
			mappedIndicesPointer->begin(), mappedIndicesPointer->end(), index );
		mappedIndex = itr - mappedIndicesPointer->begin();
	}

	bool ancestorInFilter =
		filterMatched( parent ) || ancestorFilterMatched( parent );
	bool includeDueToAncestor =
		ancestorInFilter && !filter_->filterDescendantsOfMatchingItems();
	size_t max = index + count;

	for (size_t i = index; i < max; ++i)
	{
		IItem* item = model_->item( i, parent );
		bool itemInFilter = includeDueToAncestor || filterMatched( item );

		if (itemInFilter || descendantFilterMatched( item ))
		{
			newIndices.push_back( i );
			inFilter.push_back( itemInFilter );
		}
	}

	return mappedIndicesPointer;
}

std::vector<size_t>* FilteredTreeModel::Implementation::findItemsToRemove(
	const IItem* parent, size_t index, size_t count,
	size_t& mappedIndex, size_t& mappedCount )
{
	mappedCount = 0;
	std::vector<size_t>* mappedIndicesPointer = findMappedIndices( parent );

	if (mappedIndicesPointer == nullptr)
	{
		mappedIndex = 0;
		return nullptr;
	}

	std::vector<size_t>& mappedIndices = *mappedIndicesPointer;
	auto itr = std::lower_bound(
		mappedIndices.begin(), mappedIndices.end(), index );

	mappedIndex = itr - mappedIndices.begin();
	size_t max = index + count;
	size_t position = mappedIndex;

	// i tracks the index position of the source model.
	// position tracks the index position of the filter model.

	for (size_t i = index; i < max; ++i)
	{
		// move position in line with the source model position.
		while (position < mappedIndices.size() && mappedIndices[position] < i)
		{
			++position;
		}

		if (position == mappedIndices.size())
		{
			break;
		}

		if (mappedIndices[position] == i)
		{
			++mappedCount;
			++position;
		}
	}

	return mappedIndicesPointer;
}

void FilteredTreeModel::Implementation::shiftItems(
	size_t index, size_t offset, const IItem* parent,
	std::vector<size_t>& mappedIndices )
{
	std::lock_guard<std::recursive_mutex> guard( indexMapMutex_ );

	size_t max = mappedIndices.size();

	for (size_t i = index; i < max; ++i)
	{
		mappedIndices[i] += offset;
	}
}

void FilteredTreeModel::Implementation::insertItems(
	size_t index, size_t offset, const IItem* parent, 
	std::vector<size_t>& mappedIndices,
	std::vector<size_t>& newIndices,
	std::vector<bool>& inFilter, bool shift )
{
	std::lock_guard<std::recursive_mutex> guard( indexMapMutex_ );

	size_t count = newIndices.size();
	size_t max = index + count;

	if (mappedIndices.size() == 0)
	{
		mappedIndices = std::move( newIndices );
	}
	else
	{
		mappedIndices.resize( mappedIndices.size() + count );

		if (!shift)
		{
			offset = 0;
		}

		for (size_t i = mappedIndices.size() - 1; i >= max; --i)
		{
			mappedIndices[i] = mappedIndices[i - count] + offset;
		}

		for (size_t i = index; i < max; ++i)
		{
			mappedIndices[i] = newIndices[i - index];
		}
	}
}

void FilteredTreeModel::Implementation::removeItems(
	size_t index, size_t count, size_t offset, const IItem* parent,
	std::vector<size_t>& mappedIndices, bool removeParent )
{
	std::lock_guard<std::recursive_mutex> guard( indexMapMutex_ );

	size_t newSize = mappedIndices.size() - count;
	size_t stopRemoving = index + count;

	if (newSize == 0 && removeParent)
	{
		removeMappedIndices( parent );
		return;
	}

	for (size_t i = index; i < stopRemoving; ++i)
	{
		size_t sourceIndex = mappedIndices[i];
		const IItem* child = model_->item( sourceIndex, parent );

		if (child != nullptr)
		{
			removeMappedIndices( child );
		}
	}

	for (size_t i = index; i < newSize; ++i)
	{
		mappedIndices[i] = mappedIndices[i + count] - offset;
	}

	mappedIndices.resize( newSize );
}

FilteredTreeModel::Implementation::FilterUpdateType FilteredTreeModel::Implementation::checkUpdateType(
	const IItem* item, ItemIndex& itemIndex, size_t& mappedIndex ) const
{
	itemIndex = model_->index( item );
	bool includeDueToAncestor =
		ancestorFilterMatched( item ) &&
		!filter_->filterDescendantsOfMatchingItems();
	bool wasFilteredOut;
	bool nowFilteredOut =
		!includeDueToAncestor &&
		!filterMatched( item ) &&
		!descendantFilterMatched( item );
	const std::vector<size_t>* mappedIndicesPointer =
		findMappedIndices( itemIndex.second );

	if (mappedIndicesPointer == nullptr)
	{
		wasFilteredOut = true;
		nowFilteredOut = true;
		mappedIndex = 0;
	}
	else
	{
		auto itr = std::lower_bound(
			mappedIndicesPointer->begin(), mappedIndicesPointer->end(),
			itemIndex.first );

		wasFilteredOut = itr == mappedIndicesPointer->end();
		mappedIndex = itr - mappedIndicesPointer->begin();
	}

	if (nowFilteredOut && !wasFilteredOut)
	{
		return FilterUpdateType::REMOVE;
	}

	if (!nowFilteredOut && wasFilteredOut)
	{
		return FilterUpdateType::INSERT;
	}

	if (nowFilteredOut && wasFilteredOut)
	{
		return FilterUpdateType::IGNORE;
	}

	return FilterUpdateType::UPDATE;
}

void FilteredTreeModel::Implementation::updateItem(
	const IItem* item, const ItemIndex& itemIndex,
	size_t mappedIndex, FilterUpdateType type )
{
	std::vector<size_t>& mappedIndices =
		indexMap_[itemIndex.second];

	switch (type)
	{
	case FilterUpdateType::INSERT:
		{
			std::vector<size_t> newIndices( 1, itemIndex.first );
			std::vector<bool> inFilter ( 1, ancestorFilterMatched( item ) );

			insertItems(
				mappedIndex, 1, item, mappedIndices,
				newIndices, inFilter );
			break;
		}

	case FilterUpdateType::REMOVE:
		{			
			const IItem* removedItemParent = item;
			std::vector<size_t>* mappedIndicesPointer = findMappedIndices( removedItemParent );
			ItemIndex removePointIndex = findRemovePoint( itemIndex.second, itemIndex.first, 1 );

			if (removePointIndex.second != removedItemParent)
			{
				removedItemParent = removePointIndex.second;
				mappedIndicesPointer = findMappedIndices( removedItemParent );
			}

			if (mappedIndicesPointer != nullptr)
			{
				auto itr = std::lower_bound(
					mappedIndicesPointer->begin(), mappedIndicesPointer->end(),
					removePointIndex.first );

				size_t mappedRemovedIndex = itr - mappedIndicesPointer->begin();

				self_.signalPreItemsRemoved( removedItemParent, removePointIndex.first, 1 );
				removeItems( mappedRemovedIndex, 1, 0, removedItemParent, *mappedIndicesPointer, false );
				self_.signalPostItemsRemoved( removedItemParent, removePointIndex.first, 1 );
			}

			break;
		}
	default:
			break;
	};
}

ITreeModel::ItemIndex FilteredTreeModel::Implementation::findInsertPoint(
	const IItem* parent, size_t index ) const
{
	ItemIndex itemIndex;
	size_t max = self_.size( parent );
	bool insertParent = max == 0 && parent != nullptr;

	if (insertParent)
	{
		itemIndex = model_->index( parent );
		itemIndex = findInsertPoint( itemIndex.second, itemIndex.first );
	}

	if (itemIndex.second == nullptr)
	{
		itemIndex.first = index;
		itemIndex.second = parent;
	}

	return itemIndex;
}

ITreeModel::ItemIndex FilteredTreeModel::Implementation::findRemovePoint(
	const IItem* parent, size_t index, size_t count ) const
{
	ItemIndex itemIndex;
	size_t max = self_.size( parent );
	bool deleteParent = max == count && parent != nullptr;

	if (deleteParent)
	{
		itemIndex = self_.index( parent );
		itemIndex = findRemovePoint( itemIndex.second, itemIndex.first, 1 );
	}

	if (itemIndex.second == nullptr)
	{
		itemIndex.first = index;
		itemIndex.second = parent;
	}

	return itemIndex;
}

size_t FilteredTreeModel::Implementation::findMappedIndex(
	const IItem* parent, size_t index ) const
{
	const std::vector<size_t>* mappedIndices = findMappedIndices( parent );

	if (mappedIndices == nullptr)
	{
		return 0;
	}

	auto itr = std::lower_bound(
		mappedIndices->begin(), mappedIndices->end(), index );
	return itr - mappedIndices->begin();
}

size_t FilteredTreeModel::Implementation::getSourceIndex(
	const IItem* parent, size_t index ) const
{
	auto itr = indexMap_.find( parent );

	if (itr == indexMap_.end())
	{
		return INVALID_INDEX;
	}

	assert( index < itr->second.size() );
	return itr->second[index];
}

size_t FilteredTreeModel::Implementation::getMappedIndex( const IItem* parent, size_t index ) const
{
	const std::vector<size_t>& mappedIndices = getMappedIndices( parent );
	auto itr = std::lower_bound( mappedIndices.begin(), mappedIndices.end(), index );
	assert( itr != mappedIndices.end() );
	return itr - mappedIndices.begin();
}

void FilteredTreeModel::Implementation::removeMappedIndices( const IItem* parent )
{
	std::lock_guard<std::recursive_mutex> guard( indexMapMutex_ );
	auto itr = indexMap_.find( parent );

	if (itr != indexMap_.end())
	{
		const std::vector<size_t>& mappedIndices = itr->second;
		size_t max = mappedIndices.size();

		for (size_t i = 0; i < max; ++i)
		{
			size_t index = mappedIndices[i];
			const IItem* child = model_->item( index, parent );

			if (child != nullptr)
			{
				removeMappedIndices( child );
			}
		}

		indexMap_.erase( itr );
	}
}

std::vector<size_t>& FilteredTreeModel::Implementation::createMappedIndices( const IItem* parent )
{
	return indexMap_.emplace( parent, std::vector<size_t>() ).first->second;
}

std::vector<size_t>* FilteredTreeModel::Implementation::findMappedIndices( const IItem* parent )
{
	auto itr = indexMap_.find( parent );
	return itr != indexMap_.end() ? &itr->second : nullptr;
}

const std::vector<size_t>* FilteredTreeModel::Implementation::findMappedIndices( const IItem* parent ) const
{
	auto itr = indexMap_.find( parent );
	return itr != indexMap_.end() ? &itr->second : nullptr;
}

const std::vector<size_t>& FilteredTreeModel::Implementation::getMappedIndices( const IItem* parent ) const
{
	auto itr = indexMap_.find( parent );
	assert( itr != indexMap_.end() );
	return itr->second;
}

bool FilteredTreeModel::Implementation::mapIndices(	const IItem* parent, bool parentInFilter )
{
	if (model_ == nullptr)
	{
		if (parent != nullptr)
		{
			return false;
		}

		indexMap_[nullptr];
		return true;
	}

	bool includeDueToAncestor =
		(ancestorFilterMatched( parent ) || filterMatched( parent )) &&
		!filter_->filterDescendantsOfMatchingItems();
	bool indexFound = includeDueToAncestor;
	size_t max = model_->size( parent );
	std::vector<size_t> newIndices;

	for (size_t i = 0; i < max; ++i)
	{
		const IItem* item = model_->item( i, parent );

		if (item != nullptr)
		{
			if (includeDueToAncestor ||
				filterMatched( item ) ||
				descendantFilterMatched( item ))
			{
				indexFound = true;
				newIndices.push_back( i );
			}
		}
	}

	indexMap_.emplace( parent, std::move( newIndices ) );
	return indexFound;
}

void FilteredTreeModel::Implementation::mapIndices()
{
	std::lock_guard<std::recursive_mutex> guard( indexMapMutex_ );
	indexMap_.clear();
	mapIndices( nullptr, false );
}

void FilteredTreeModel::Implementation::remapIndices( const IItem* parent, bool parentInFilter )
{
	std::vector<size_t>* mappedIndicesPointer = findMappedIndices( parent );

	if (mappedIndicesPointer == nullptr || stopRemapping_ || model_ == nullptr)
	{
		return;
	}

	std::vector<size_t>& mappedIndices = *mappedIndicesPointer;
	size_t modelCount = model_->size( parent );
	size_t index = 0;
	bool includeDueToAncestor =
		parentInFilter && !filter_->filterDescendantsOfMatchingItems();

	for (size_t i = 0; i < modelCount; ++i)
	{
		if (stopRemapping_)
		{
			return;
		}

		const IItem* item = model_->item( i, parent );

		bool itemInFilter =
			item == nullptr ? false :
			includeDueToAncestor || filterMatched( item );
		bool wasInFilter =
			index < mappedIndices.size() &&
			mappedIndices[index] == i;
		bool nowInFilter =
			itemInFilter || descendantFilterMatched( item );

		if (wasInFilter && nowInFilter)
		{
			remapIndices( item, itemInFilter );
			++index;
		}
		else if (wasInFilter && !nowInFilter)
		{
			self_.signalPreItemsRemoved( parent, index, 1 );
			removeItems( index, 1, 0, parent, mappedIndices, false );
			self_.signalPostItemsRemoved( parent, index, 1 );
		}
		else if (nowInFilter)
		{
			self_.signalPreItemsInserted( parent, index, 1 );

			std::vector<size_t> newIndices( 1, i );
			std::vector<bool> newInFilter( 1, itemInFilter );
			insertItems(
				index, 0, parent, mappedIndices, newIndices, newInFilter );

			self_.signalPostItemsInserted( parent, index, 1 );
			++index;
		}
	}
}

void FilteredTreeModel::Implementation::remapIndices()
{
	++remapping_;
	std::lock_guard<std::mutex> blockEvents( eventControlMutex_ );
	remapIndices( nullptr, false );
	--remapping_;
}

void FilteredTreeModel::Implementation::copyIndices( IndexMap& target ) const
{
	std::lock( eventControlMutex_, indexMapMutex_ );
	target = indexMap_;
	indexMapMutex_.unlock();
	eventControlMutex_.unlock();
}

void FilteredTreeModel::Implementation::preItemDataChanged( const IItem * item, int column, size_t roleId, const Variant & data )
{
	eventControlMutex_.lock();
	self_.signalPreItemDataChanged( item, column, roleId, data );
	indexMapMutex_.lock();
}

void FilteredTreeModel::Implementation::postItemDataChanged( const IItem * item, int column, size_t roleId, const Variant & data  )
{
	indexMapMutex_.unlock();
	std::lock_guard<std::mutex> blockEvents( eventControlMutex_, std::adopt_lock );

	ItemIndex sourceIndex;
	size_t newIndex;
	FilterUpdateType updateType = checkUpdateType( item, sourceIndex, newIndex );

	if (updateType == FilterUpdateType::UPDATE)
	{
		if (item != nullptr)
		{
			bool parentInFilter =
				ancestorFilterMatched( item ) ||
				filterMatched( item );

			removeMappedIndices( item );
			mapIndices( item, parentInFilter );
		}
	}

	self_.signalPostItemDataChanged( item, column, roleId, data );

	switch (updateType)
	{
	case FilterUpdateType::INSERT:
		self_.signalPreItemsInserted( sourceIndex.second, sourceIndex.first, 1 );
		updateItem( item, sourceIndex, newIndex, updateType );
		self_.signalPostItemsInserted( sourceIndex.second, sourceIndex.first, 1 );
		break;

	case FilterUpdateType::REMOVE: 
		self_.signalPreItemsRemoved( sourceIndex.second, sourceIndex.first, 1 );
		updateItem( item, sourceIndex, newIndex, updateType );
		self_.signalPostItemsRemoved( sourceIndex.second, sourceIndex.first, 1 );
		break;
	default:
		break;
	};
}

void FilteredTreeModel::Implementation::preItemsInserted( const IItem * parent, size_t index, size_t count )
{
	std::lock( eventControlMutex_, indexMapMutex_ );
}

void FilteredTreeModel::Implementation::postItemsInserted( const IItem * parent, size_t index, size_t count )
{
	std::lock_guard<std::mutex> blockEvents( eventControlMutex_, std::adopt_lock );

	// Optimization, delay inserting until getChildCount has been called.
	// ItemIndex itemIndex = findInsertPoint( args.item_, args.index_ );
	ItemIndex itemIndex( index, parent );
	const IItem* item = itemIndex.second;
	size_t sourceIndex = itemIndex.first;
	size_t sourceCount = item == parent ? count : 1;

	size_t mappedIndex;
	std::vector<size_t> newIndices;
	std::vector<bool> inFilter;
	std::vector<size_t>* mappedIndicesPointer = findItemsToInsert(
		item, sourceIndex, sourceCount, mappedIndex, newIndices, inFilter );

	if (mappedIndicesPointer != nullptr)
	{
		shiftItems( mappedIndex, sourceCount, item,
			*mappedIndicesPointer );
	}

	indexMapMutex_.unlock();
	// Possible unstable tree state between this and insertItems.
	// Currently the notify needs access to the data from another thread.

	if (newIndices.size() > 0)
	{
		assert( mappedIndicesPointer != nullptr );
		self_.signalPreItemsInserted( item, mappedIndex, newIndices.size() );
		insertItems( 
			mappedIndex, sourceCount, item, 
			*mappedIndicesPointer, newIndices, inFilter, false );
		self_.signalPostItemsInserted( item, mappedIndex, newIndices.size() );
	}
}

void FilteredTreeModel::Implementation::preItemsRemoved( const IItem * parent, size_t index, size_t count )
{
	eventControlMutex_.lock();

	lastUpdateData_.set();
	size_t mappedIndex;
	size_t mappedCount;
	size_t sourceCount = count;
	std::vector<size_t>* mappedIndicesPointer = findItemsToRemove(
		parent, index, sourceCount, mappedIndex, mappedCount );

	if (mappedIndicesPointer != nullptr)
	{
		const IItem* item = parent;

		if (mappedIndex == 0 && mappedCount == mappedIndicesPointer->size())
		{
			ItemIndex itemIndex = findRemovePoint( item, index, sourceCount );

			if (itemIndex.second != item)
			{
				item = itemIndex.second;
				mappedIndex = itemIndex.first;
				mappedCount = sourceCount = 1;
				mappedIndicesPointer = findMappedIndices( item );
			}
		}

		if (mappedIndicesPointer != nullptr)
		{
			if (mappedCount > 0)
			{
				self_.signalPreItemsRemoved( item, mappedIndex, mappedCount );
			}

			indexMapMutex_.lock();
			const bool removeParent = item != nullptr;

			removeItems(
				mappedIndex, mappedCount, sourceCount,
				item, *mappedIndicesPointer, removeParent );

			lastUpdateData_.set( item, mappedIndex, mappedCount, true );
		}
	}
}

void FilteredTreeModel::Implementation::postItemsRemoved( const IItem * parent, size_t index, size_t count )
{
	std::lock_guard<std::mutex> blockEvents( eventControlMutex_, std::adopt_lock );

	if (lastUpdateData_.valid_)
	{
		indexMapMutex_.unlock();

		if (lastUpdateData_.count_ > 0)
		{
			self_.signalPostItemsRemoved(
				lastUpdateData_.parent_,
				lastUpdateData_.index_,
				lastUpdateData_.count_ );
		}

		lastUpdateData_.set();
	}
}

void FilteredTreeModel::Implementation::onDestructing()
{
	setSource(nullptr);
}

bool FilteredTreeModel::Implementation::ancestorFilterMatched( const IItem* item ) const
{
	if (item == nullptr || filter_ == nullptr)
	{
		return false;
	}

	const IItem* parentItem = model_->index( item ).second;

	if (parentItem == nullptr)
	{
		return false;
	}

	if (filter_->checkFilter( parentItem ))
	{
		return true;
	}

	return ancestorFilterMatched( parentItem );
}

bool FilteredTreeModel::Implementation::filterMatched( const IItem* item ) const
{
	return item != nullptr && filter_ != nullptr && filter_->checkFilter( item );
}

bool FilteredTreeModel::Implementation::descendantFilterMatched( const IItem* item ) const
{
	if (item == nullptr || filter_ == nullptr)
	{
		return false;
	}

	const size_t max = model_->size( item );

	for (size_t i = 0; i < max; ++i)
	{
		const IItem* child = model_->item( i, item );

		if (child == nullptr)
		{
			continue;
		}

		if (filter_->checkFilter( child ) || descendantFilterMatched( child ))
		{
			return true;
		}
	}

	return false;
}


FilteredTreeModel::FilteredTreeModel()
	: impl_( new Implementation( *this ) )
{}

FilteredTreeModel::FilteredTreeModel( const FilteredTreeModel& rhs )
	: impl_( new Implementation(
		*this, *rhs.impl_ ) )
{}

FilteredTreeModel::~FilteredTreeModel()
{	
	setSource( nullptr );
}

FilteredTreeModel& FilteredTreeModel::operator=( const FilteredTreeModel& rhs )
{
	if (this != &rhs)
	{
		impl_.reset( new Implementation( *this, *rhs.impl_ ) );
	}

	return *this;
}

IItem* FilteredTreeModel::item( size_t index, const IItem* parent ) const
{
	std::lock_guard<std::recursive_mutex> guard( impl_->indexMapMutex_ );
	size_t sourceIndex = impl_->getSourceIndex( parent, index );
	return sourceIndex == Implementation::INVALID_INDEX ?
		nullptr : impl_->model_->item( sourceIndex, parent );
}

ITreeModel::ItemIndex FilteredTreeModel::index( const IItem* item ) const
{
	std::lock_guard<std::recursive_mutex> guard( impl_->indexMapMutex_ );
	assert( impl_->model_ != nullptr );
	ItemIndex itemIndex = impl_->model_->index( item );
	itemIndex.first =
		impl_->getMappedIndex( itemIndex.second, itemIndex.first );
	return itemIndex;
}

bool FilteredTreeModel::empty( const IItem* item ) const
{
	std::lock_guard<std::recursive_mutex> guard( impl_->indexMapMutex_ );
	auto childIndices = impl_->findMappedIndices( item );

	if (childIndices == nullptr)
	{
		return impl_->empty( item );
	}

	return childIndices->empty();
}

size_t FilteredTreeModel::size( const IItem* item ) const
{
	std::lock_guard<std::recursive_mutex> guard( impl_->indexMapMutex_ );
	auto childIndices = impl_->findMappedIndices( item );

	if (childIndices == nullptr)
	{
		bool parentInFilter =
			impl_->ancestorFilterMatched( item ) || impl_->filterMatched( item );

		if (!impl_->mapIndices( item, parentInFilter ))
		{
			bool needsToBeInTheFilter = false;
			assert( needsToBeInTheFilter );
		}

		childIndices = impl_->findMappedIndices( item );
	}

	return childIndices->size();
}

int FilteredTreeModel::columnCount() const
{
	if (impl_->model_ != nullptr)
	{
		return impl_->model_->columnCount();
	}

	return 1;
}

Variant FilteredTreeModel::getData( int column, size_t roleId ) const
{
	if (impl_->model_ == nullptr)
	{
		return Variant();
	}

	return impl_->model_->getData( column, roleId );
}

bool FilteredTreeModel::setData( int column, size_t roleId, const Variant & data )
{
	if (impl_->model_ == nullptr)
	{
		return false;
	}

	return impl_->model_->setData( column, roleId, data );
}

void FilteredTreeModel::setSource( ITreeModel * source )
{
	// Kill any current remapping going on in the background
	impl_->haltRemapping();

	// Initialize and remap the indices based on the new source
	std::lock_guard<std::mutex> blockEvents( impl_->eventControlMutex_ );

	// Set the new source
	impl_->setSource( source );

	impl_->mapIndices();
	impl_->initialize();
}

void FilteredTreeModel::setFilter( IItemFilter * filter )
{
	{
		// wait for previous refresh to finish.
		std::lock_guard<std::mutex> blockEvents( impl_->eventControlMutex_ );
		impl_->filter_ = filter;
	}

	refresh();
}

ITreeModel* FilteredTreeModel::getSource()
{
	return impl_->model_;
}

const ITreeModel* FilteredTreeModel::getSource() const
{
	return impl_->model_;
}

void FilteredTreeModel::refresh( bool wait )
{
	if (impl_->model_ == nullptr)
	{
		return;
	}
	
	if (wait)
	{
		impl_->remapIndices();
		return;
	}

	// if one refresh is finishing and another is waiting, then there's no
	// point in queuing another refresh operation. (2 = two refreshes)
	if (impl_->remapping_ > 1)
	{
		return;
	}

	void (FilteredTreeModel::Implementation::*refreshMethod)() = &FilteredTreeModel::Implementation::remapIndices;
	std::thread nextRefresh( std::bind( refreshMethod, impl_.get() ) );
	nextRefresh.detach();
}
} // end namespace wgt
