/*
This model uses an index map to map indices from a source model to filtered
indices exposed by this model. Both the source model and the filtered model
consist of a list of IItem*. The map is a vector of indices pointing to the
indices in the source model that are included in this model. Only indices of
items that satisfy the filter function are included.

An example of a model could be:
item   index
AAAA   0
BBBB   1
CCCC   2
DDDD   3
EEEE   4

A filtered model of that model could be:
item   index
BBBB   1
CCCC   2
EEEE   4

The mapped indices would be:
1,2,4

If a filter is changed, the source model didn't change, but our mapping is
outdated. This is updated by a refresh. Refresh needs to do the following:
For each item in the source model, check if it was in the filter previously
and also if it satisfies the new filter. An item is in the filter if it matches
the filter function. If the before and after checks are the same, then nothing
needs to be done, but if it's different the mapped index needs to either be
removed (if it's not in the filter anymore), or added (if it's in the filter
now).

If something in the source model changed, the source model indices are
different, but the filter is the same. This is handled by our event handlers
preItemsInserted, postItemsInserted, preItemsRemoved, postItemsRemoved,
preDataChanged, and postDataChanged. [1] If an item is new and matches the
filter, it needs to be added to the index map. For every item inserted, all
indices after the insert point needs to be adjusted by adding one, because
these pointed to items that moved up when the item was inserted into the source
model. [2] If an item is removed and it exists in the index map, it needs to be
removed. For every item removed, all indices after the remove point need to be
adjusted by subtracting one, because these pointed to items that moved down the
list when the item was removed from the source model. [3] For items
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


#include "filtered_list_model.hpp"
#include "core_variant/variant.hpp"
#include <algorithm>
#include <atomic>
#include <functional>
#include <mutex>
#include <thread>
#include <vector>

namespace wgt
{
struct FilteredListModel::Implementation
{
	enum class FilterUpdateType
	{
		UPDATE,
		INSERT,
		REMOVE,
		IGNORE
	};

	typedef std::vector< size_t > IndexMap;

	Implementation( FilteredListModel & self );
	Implementation( FilteredListModel & self, const FilteredListModel::Implementation & rhs );
	~Implementation();

	void initialize();
	void haltRemapping();

	void setSource( IListModel * source );

	void mapIndices();
	void remapIndices();
	void copyIndices( IndexMap& target ) const;

	void removeIndex( size_t index );
	void insertIndex( size_t index, size_t sourceIndex );
	void findItemsToRemove( size_t sourceIndex, size_t sourceCount, size_t& removeFrom, size_t& removeCount );

	FilterUpdateType checkUpdateType( size_t sourceIndex , size_t& newIndex ) const;

	void preItemDataChanged( const IItem * item, int column, size_t roleId, const Variant & data );
	void postItemDataChanged( const IItem * item, int column, size_t roleId, const Variant & data );
	void preItemsInserted( size_t index, size_t count );
	void postItemsInserted( size_t index, size_t count );
	void preItemsRemoved( size_t index, size_t count );
	void postItemsRemoved( size_t index, size_t count );
	void onDestructing();

	/// Is the item found by invoking filterFound_ function
	bool filterMatched( const IItem * item ) const;

	struct UpdateData
	{
		UpdateData()
			: index_( 0 ), count_( 0 ), valid_( false )
		{}

		void set(
			size_t index = 0,
			size_t count = 0,
			bool valid = false )
		{
			index_	= index;
			count_	= count;
			valid_	= valid;
		}

		size_t index_;
		size_t count_;
		bool valid_;
	} lastUpdateData_;

	FilteredListModel & self_;
	IListModel * model_;
	IItemFilter * listFilter_;
	mutable std::recursive_mutex indexMapMutex_;
	mutable std::mutex eventControlMutex_;
	std::atomic_uint_fast8_t remapping_;
	std::atomic<bool> stopRemapping_;
	IndexMap indexMap_;
	ConnectionHolder connections_;
};

FilteredListModel::Implementation::Implementation( FilteredListModel & self )
	: self_( self )
	, model_( nullptr )
	, listFilter_( nullptr )
{
	mapIndices();
	initialize();
}

FilteredListModel::Implementation::Implementation( FilteredListModel & self, const FilteredListModel::Implementation & rhs )
	: self_( self )
	, model_( rhs.model_ )
	, listFilter_( rhs.listFilter_ )
{
	rhs.copyIndices( indexMap_ );
	initialize();
}

FilteredListModel::Implementation::~Implementation()
{
}

void FilteredListModel::Implementation::initialize()
{
	remapping_ = 0;
	stopRemapping_ = false;
}

void FilteredListModel::Implementation::haltRemapping()
{
	stopRemapping_ = true;

	while (remapping_ > 0)
	{
		std::this_thread::yield();
	}

	setSource( nullptr );
}

void FilteredListModel::Implementation::setSource( IListModel * source )
{
	connections_.clear();
	model_ = source;
	if (model_ != nullptr)
	{
		using namespace std::placeholders;
		connections_+= model_->signalPreItemDataChanged.connect( std::bind( &FilteredListModel::Implementation::preItemDataChanged, this, _1, _2, _3, _4 ) );
		connections_+= model_->signalPostItemDataChanged.connect( std::bind( &FilteredListModel::Implementation::postItemDataChanged, this, _1, _2, _3, _4 ) );
		connections_+= model_->signalPreItemsInserted.connect( std::bind( &FilteredListModel::Implementation::preItemsInserted, this, _1, _2 ) );
		connections_+= model_->signalPostItemsInserted.connect( std::bind( &FilteredListModel::Implementation::postItemsInserted, this, _1, _2 ) );
		connections_+= model_->signalPreItemsRemoved.connect( std::bind( &FilteredListModel::Implementation::preItemsRemoved, this, _1, _2 ) );
		connections_+= model_->signalPostItemsRemoved.connect( std::bind( &FilteredListModel::Implementation::postItemsRemoved, this, _1, _2 ) );
		connections_+= model_->signalDestructing.connect( std::bind( &FilteredListModel::Implementation::onDestructing, this ) );
	}
}

void FilteredListModel::Implementation::mapIndices()
{
	if (model_ == nullptr)
	{
		return;
	}

	std::lock_guard<std::recursive_mutex> lock( indexMapMutex_ );
	indexMap_.clear();
	size_t itemCount = model_->size();

	for (size_t index = 0; index < itemCount; ++index)
	{
		const IItem * item = model_->item( index );

		if (filterMatched( item ))
		{
			indexMap_.push_back( index );
		}
	}
}

void FilteredListModel::Implementation::remapIndices()
{
	++remapping_;
	self_.onFilteringBegin();
	std::lock_guard<std::mutex> guard( eventControlMutex_ );

	size_t modelCount = model_ == nullptr ? 0 : model_->size();
	size_t index = 0;

	for (size_t i = 0; i < modelCount; ++i)
	{
		const IItem* item = model_->item( i );

		bool itemInFilter = filterMatched( item );
		bool indexInList = index < indexMap_.size() && indexMap_[index] == i;

		if (itemInFilter && indexInList)
		{
			// Do nothing and move to the next item
			++index;
		}
		else if (!itemInFilter && indexInList)
		{
			self_.signalPreItemsRemoved( index, 1 );
			removeIndex( index );
			self_.signalPostItemsRemoved( index, 1 );
		}
		else if (itemInFilter)
		{
			self_.signalPreItemsInserted( index, 1 );
			insertIndex( index, i );
			self_.signalPostItemsInserted( index, 1 );
			++index;
		}

		if (stopRemapping_)
		{
			break;
		}
	}

	if (!stopRemapping_)
	{
		indexMap_.resize( index );
	}

	--remapping_;
	if (remapping_ == 0)
	{
		self_.onFilteringEnd();
	}
}

void FilteredListModel::Implementation::copyIndices( IndexMap& target ) const
{
	std::lock( eventControlMutex_, indexMapMutex_ );
	target = indexMap_;
	indexMapMutex_.unlock();
	eventControlMutex_.unlock();
}

void FilteredListModel::Implementation::removeIndex( size_t index )
{
	std::lock_guard<std::recursive_mutex> guard( indexMapMutex_ );
	indexMap_.erase( indexMap_.begin() + index );
}

void FilteredListModel::Implementation::insertIndex( size_t index, size_t sourceIndex )
{
	std::lock_guard<std::recursive_mutex> guard( indexMapMutex_ );
	auto itr = indexMap_.begin() + index;
	indexMap_.insert( itr, sourceIndex );
}

/// Find the starting index and number of indices to remove.
void FilteredListModel::Implementation::findItemsToRemove(
	size_t sourceIndex, size_t sourceCount, size_t& removeFrom, size_t& removeCount )
{
	size_t lastSourceIndex = sourceIndex + sourceCount - 1;
	//size_t max = std::min( lastSourceIndex + 1, indexMap_.size() );
	//bool foundone = false;
	removeCount = 0;

	auto itr = std::lower_bound( indexMap_.begin(), indexMap_.end(), sourceIndex );

	if (itr != indexMap_.end() && *itr <= lastSourceIndex)
	{
		removeFrom = itr - indexMap_.begin();

		for (; itr != indexMap_.end() && *itr <= lastSourceIndex; ++itr)
		{
			++removeCount;
		}
	}
}

FilteredListModel::Implementation::FilterUpdateType FilteredListModel::Implementation::checkUpdateType(
	size_t sourceIndex, size_t& newIndex ) const
{
	auto itr = std::lower_bound( indexMap_.begin(), indexMap_.end(), sourceIndex );
	const IItem* item = model_->item( sourceIndex );
	bool wasInFilter = itr != indexMap_.end() && *itr == sourceIndex;
	bool nowInFilter = filterMatched( item );
	newIndex = itr - indexMap_.begin();

	if (nowInFilter && !wasInFilter)
	{
		return FilterUpdateType::INSERT;
	}

	if (!nowInFilter && wasInFilter)
	{
		return FilterUpdateType::REMOVE;
	}

	if (nowInFilter && wasInFilter)
	{
		return FilterUpdateType::UPDATE;
	}

	return FilterUpdateType::IGNORE;
}

void FilteredListModel::Implementation::preItemDataChanged( const IItem * item, int column, size_t roleId, const Variant & data )
{
	eventControlMutex_.lock();
	self_.signalPreItemDataChanged( item, column, roleId, data );
	indexMapMutex_.lock();
}

void FilteredListModel::Implementation::postItemDataChanged( const IItem * item, int column, size_t roleId, const Variant & data )
{
	std::lock_guard<std::mutex> blockEvents( eventControlMutex_, std::adopt_lock );
	indexMapMutex_.unlock();

	size_t newIndex;
	size_t sourceIndex = model_->index( item );
	FilterUpdateType updateType = checkUpdateType( sourceIndex, newIndex );

	self_.signalPostItemDataChanged( item, column, roleId, data );

	switch (updateType)
	{
	case FilterUpdateType::INSERT:
		self_.signalPreItemsInserted( newIndex, 1 );
		insertIndex( newIndex, sourceIndex );
		self_.signalPostItemsInserted( newIndex, 1 );
		break;

	case FilterUpdateType::REMOVE:
		self_.signalPreItemsRemoved( newIndex, 1 );
		removeIndex( newIndex );
		self_.signalPostItemsRemoved( newIndex, 1 );
		break;
	default:
		break;
	}
}

void FilteredListModel::Implementation::preItemsInserted( size_t index, size_t count )
{
	eventControlMutex_.lock();
	indexMapMutex_.lock();
}

void FilteredListModel::Implementation::postItemsInserted( size_t index, size_t count )
{
	std::lock_guard<std::mutex> blockEvents( eventControlMutex_, std::adopt_lock );
	indexMapMutex_.unlock();

	auto itr = std::lower_bound( indexMap_.begin(), indexMap_.end(), index );
	size_t newIndex = itr - indexMap_.begin();
	size_t max = index + count;
	IndexMap newIndices;

	for (size_t i = index; i < max; ++i)
	{
		const IItem* item = model_->item( i );

		if (filterMatched( item ))
		{
			newIndices.push_back( i );
		}
	}

	size_t newCount = newIndices.size();

	{
		std::lock_guard<std::recursive_mutex> guard( indexMapMutex_ );
		max = indexMap_.size();

		for (size_t i = newIndex; i < max; ++i)
		{
			indexMap_[i] += count;
		}
	}

	if (newIndices.size() > 0)
	{
		self_.signalPreItemsInserted( newIndex, newCount );
		indexMap_.resize( indexMap_.size() + newCount );

		for (size_t i = indexMap_.size() - 1; i >= newIndex + newCount; --i)
		{
			indexMap_[i] = indexMap_[i - newCount];
		}

		for (size_t i = newIndex; i < newIndex + newCount; ++i)
		{
			indexMap_[i] = newIndices[i - newIndex];
		}

		self_.signalPostItemsInserted( newIndex, newCount );
	}
}

void FilteredListModel::Implementation::preItemsRemoved( size_t index, size_t count )
{
	eventControlMutex_.lock();

	findItemsToRemove( index, count, lastUpdateData_.index_, lastUpdateData_.count_ );

	if (lastUpdateData_.count_ > 0)
	{
		lastUpdateData_.valid_ = true;
		self_.signalPreItemsRemoved( lastUpdateData_.index_, lastUpdateData_.count_ );
	}

	indexMapMutex_.lock();
	size_t max = lastUpdateData_.index_ + lastUpdateData_.count_;
	size_t newCount = lastUpdateData_.count_;

	if (max < indexMap_.size())
	{
		max = indexMap_.size() - newCount;

		for (size_t i = lastUpdateData_.index_; i < max; ++i)
		{
			indexMap_[i] = indexMap_[i + newCount] - count;
		}
	}

	if (lastUpdateData_.count_ > 0)
	{
		indexMap_.resize(indexMap_.size() - lastUpdateData_.count_);
	}
}

void FilteredListModel::Implementation::postItemsRemoved( size_t index, size_t count )
{
	std::lock_guard<std::mutex> blockEvents( eventControlMutex_, std::adopt_lock );
	indexMapMutex_.unlock();

	if (lastUpdateData_.valid_)
	{
		self_.signalPostItemsRemoved( lastUpdateData_.index_, lastUpdateData_.count_ );
		lastUpdateData_.set();
	}
}

void FilteredListModel::Implementation::onDestructing()
{
	setSource( nullptr );
}

bool FilteredListModel::Implementation::filterMatched( const IItem * item ) const
{
	bool matched = listFilter_ ? item != nullptr && listFilter_->checkFilter( item ) : true;
	return matched;
}

FilteredListModel::FilteredListModel()
	: impl_( new Implementation( *this ) )
{}

FilteredListModel::FilteredListModel( const FilteredListModel& rhs )
	: impl_( new Implementation( *this, *rhs.impl_ ) )
{}

FilteredListModel::~FilteredListModel()
{
	setSource( nullptr );
}

FilteredListModel & FilteredListModel::operator=( const FilteredListModel & rhs )
{
	if (this != &rhs)
	{
		impl_.reset( new Implementation( *this, *rhs.impl_ ) );
	}

	return *this;
}

IItem * FilteredListModel::item( size_t index ) const
{
	std::lock_guard<std::recursive_mutex> guard( impl_->indexMapMutex_ );

	if (index >= impl_->indexMap_.size())
	{
		return nullptr;
	}

	return impl_->model_->item( impl_->indexMap_[index] );
}

size_t FilteredListModel::index( const IItem* item ) const
{
	std::lock_guard<std::recursive_mutex> guard( impl_->indexMapMutex_ );
	size_t returnIndex = 0;
	size_t count = impl_->indexMap_.size();

	for (size_t i = 0; i < count; ++i)
	{
		const IItem * listItem = impl_->model_->item( impl_->indexMap_[i] );

		if (item == listItem)
		{
			returnIndex = i;
			break;
		}
	}

	return returnIndex;
}

bool FilteredListModel::empty() const
{
	std::lock_guard<std::recursive_mutex> guard( impl_->indexMapMutex_ );
	return impl_->indexMap_.empty();
}

size_t FilteredListModel::size() const
{
	std::lock_guard<std::recursive_mutex> guard( impl_->indexMapMutex_ );
	return impl_->indexMap_.size();
}

int FilteredListModel::columnCount() const
{
	if (impl_->model_ == nullptr)
	{
		return 1;
	}

	return impl_->model_->columnCount();
}

Variant FilteredListModel::getData( int column, size_t roleId ) const
{
	if (impl_->model_ == nullptr)
	{
		return Variant();
	}

	return impl_->model_->getData( column, roleId );
}

bool FilteredListModel::setData( int column, size_t roleId, const Variant & data )
{
	if (impl_->model_ == nullptr)
	{
		return false;
	}

	return impl_->model_->setData( column, roleId, data );
}

void FilteredListModel::setSource( IListModel * source )
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

void FilteredListModel::setFilter( IItemFilter * filter )
{
	{
		// wait for previous refresh to finish.
		std::lock_guard<std::mutex> blockEvents( impl_->eventControlMutex_ );
		impl_->listFilter_ = filter;
	}

	refresh();
}

IListModel* FilteredListModel::getSource()
{
	return impl_->model_;
}

const IListModel* FilteredListModel::getSource() const
{
	return impl_->model_;
}

bool FilteredListModel::isFiltering() const
{
	return impl_->remapping_ > 0;
}

void FilteredListModel::refresh( bool waitToFinish )
{
	if (impl_->model_ == nullptr)
	{
		return;
	}

	if (waitToFinish)
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

	void (FilteredListModel::Implementation::*refreshMethod)() = &FilteredListModel::Implementation::remapIndices;
	std::thread nextRefresh( std::bind( refreshMethod, impl_.get() ) );
	nextRefresh.detach();
}
} // end namespace wgt
