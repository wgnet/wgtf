#include "timeline_model.hpp"
#include "core_data_model/i_item_role.hpp"
#include "core_reflection/object_handle.hpp"

namespace wgt
{
ITEMROLE( name )
ITEMROLE( text )
ITEMROLE( type )
ITEMROLE( time )
ITEMROLE( startTime )
ITEMROLE( endTime )
ITEMROLE( barColor )
ITEMROLE( eventValue )
ITEMROLE( eventName )
ITEMROLE( eventProperty )
ITEMROLE( eventAction )
ITEMROLE( keyFrames )
ITEMROLE( rowSpan )

class KeyFrameItem : public AbstractListItem
{
public:
	KeyFrameItem()
		: time_( 0.0f )
	{
	}

	Variant getData( int column, ItemRole::Id roleId ) const override
	{
		if (roleId == ItemRole::timeId)
		{
			return time_;
		}
		else if (roleId == ItemRole::eventValueId)
		{
			return eventValue_;
		}
		else if (roleId == ItemRole::typeId)
		{
			return type_;
		}

		return AbstractListItem::getData( column, roleId );
	}

	
	bool setData( int column,
		ItemRole::Id roleId,
		const Variant & data ) override
	{
		if (roleId == ItemRole::timeId)
		{
			preDataChanged_( column, roleId, data );
			const auto result = data.tryCast( time_ );
			postDataChanged_( column, roleId, data );
			return result;
		}
		else if (roleId == ItemRole::eventValueId)
		{
			preDataChanged_( column, roleId, data );
			const auto result = data.tryCast( eventValue_ );
			postDataChanged_( column, roleId, data );
			return result;
		}
		else if (roleId == ItemRole::typeId)
		{
			preDataChanged_( column, roleId, data );
			const auto result = data.tryCast( type_ );
			postDataChanged_( column, roleId, data );
			return result;
		}

		return AbstractListItem::setData( column, roleId, data );
	}

	virtual Connection connectPreDataChanged( DataCallback callback ) override
	{
		return preDataChanged_.connect( callback );
	}

	virtual Connection connectPostDataChanged( DataCallback callback ) override
	{
		return postDataChanged_.connect( callback );
	}

	double time_;
	std::string eventValue_;
	std::string type_;

	Signal< KeyFrameItem::DataSignature > preDataChanged_;
	Signal< KeyFrameItem::DataSignature > postDataChanged_;
};

class KeyFramesModel : public AbstractListModel
{
public:
	KeyFramesModel()
	{
	}
	virtual ~KeyFramesModel()
	{
	}
	AbstractItem * item( int row ) const override
	{
		return items_[row].get();
	}

	int index( const AbstractItem * item ) const override
	{
		auto it = std::find_if( items_.begin(), items_.end(),
			[&](const std::unique_ptr< KeyFrameItem >& keyFrameItem) { return keyFrameItem.get() == item; } );
		return static_cast< int >( it - items_.begin() );
	}

	int rowCount() const override
	{
		return static_cast< int >( items_.size() );
	}

	int columnCount() const override
	{
		return 1;
	}

	bool insertRows( int row, int count ) override
	{
		preRowsInserted_( row, count );

		const auto startRow = row;
		const auto endRow = row + count;
		for (auto i = startRow; i < endRow; ++i)
		{
			const auto pItem = new KeyFrameItem();

			const auto & preModelDataChanged = preDataChanged_;
			const auto preData = [ row, &preModelDataChanged ]( int column,
				ItemRole::Id role,
				const Variant & value )
			{
				preModelDataChanged( row, column, role, value );
			};
			const auto preDataChanged = pItem->connectPreDataChanged( preData );

			const auto & postModelDataChanged = postDataChanged_;
			const auto postData = [ row, &postModelDataChanged ]( int column,
				ItemRole::Id role,
				const Variant & value )
			{
				postModelDataChanged( row, column, role, value );
			};
			const auto postDataChanged = pItem->connectPostDataChanged( postData );

			auto itr = items_.cbegin() + i;
			items_.emplace( itr, pItem );
		}

		postRowsInserted_( row, count );
		return true;
	}

	bool removeRows( int row, int count ) override
	{
		preRowsRemoved_( row, count );
		auto begin = items_.begin() + row;
		auto end = begin + count;
		items_.erase( begin, end );
		postRowsRemoved_( row, count );
		return true;
	}

	std::vector< std::string > roles() const override
	{
		std::vector< std::string > roles;
		roles.emplace_back( ItemRole::timeName );
		roles.emplace_back( ItemRole::eventValueName );
		roles.emplace_back( ItemRole::typeName );
		return roles;
	}

	Connection connectPreItemDataChanged( DataCallback callback ) override
	{
		return preDataChanged_.connect( callback );
	}

	Connection connectPostItemDataChanged( DataCallback callback ) override
	{
		return postDataChanged_.connect( callback );
	}

	Connection connectPreRowsInserted( RangeCallback callback ) override
	{
		return preRowsInserted_.connect( callback );
	}

	Connection connectPostRowsInserted( RangeCallback callback ) override
	{
		return postRowsInserted_.connect( callback );
	}

	Connection connectPreRowsRemoved( RangeCallback callback ) override
	{
		return preRowsRemoved_.connect( callback );
	}

	Connection connectPostRowsRemoved( RangeCallback callback ) override
	{
		return postRowsRemoved_.connect( callback );
	}

	std::vector< std::unique_ptr< KeyFrameItem > > items_;
	Signal< KeyFramesModel::DataSignature > preDataChanged_;
	Signal< KeyFramesModel::DataSignature > postDataChanged_;
	Signal< KeyFramesModel::RangeSignature > preRowsInserted_;
	Signal< KeyFramesModel::RangeSignature > postRowsInserted_;
	Signal< KeyFramesModel::RangeSignature > preRowsRemoved_;
	Signal< KeyFramesModel::RangeSignature > postRowsRemoved_;
};

class TimelineItem : public AbstractListItem
{
public:
	TimelineItem()
		: rowSpan_( 0 )
		, startTime_( 0.0f )
		, endTime_( 0.0f )
		, keyFrames_( std::make_shared< KeyFramesModel >() )
	{
	}

	Variant getData( int column, ItemRole::Id roleId ) const override
	{
		// General timeline item
		if (roleId == ItemRole::nameId)
		{
			return name_;
		}
		else if (roleId == ItemRole::typeId)
		{
			return type_;
		}
		else if (roleId == ItemRole::barColorId)
		{
			return barColor_;
		}
		else if (roleId == ItemRole::rowSpanId)
		{
			return rowSpan_;
		}

		// Text box item
		if (roleId == ItemRole::textId)
		{
			return text_;
		}

		// Frame slider item
		if (roleId == ItemRole::eventNameId)
		{
			return "set";
		}
		else if (roleId == ItemRole::eventPropertyId)
		{
			return eventProperty_;
		}
		else if (roleId == ItemRole::eventActionId)
		{
			return "=";
		}
		else if (roleId == ItemRole::keyFramesId)
		{
			return keyFrames_;
		}

		// Bar slider item
		if (roleId == ItemRole::startTimeId)
		{
			return startTime_;
		}
		else if (roleId == ItemRole::endTimeId)
		{
			return endTime_;
		}
		else if (roleId == ItemRole::eventPropertyId)
		{
			return eventProperty_;
		}
		else if (roleId == ItemRole::eventActionId)
		{
			return "=";
		}
		else if (roleId == ItemRole::eventValueId)
		{
			return eventValue_;
		}

		return AbstractListItem::getData( column, roleId );
	}

	bool setData( int column,
		ItemRole::Id roleId,
		const Variant & data ) override
	{
		// General timeline item
		if (roleId == ItemRole::nameId)
		{
			preDataChanged_( column, roleId, data );
			const auto result = data.tryCast( name_ );
			postDataChanged_( column, roleId, data );
			return result;
		}
		else if (roleId == ItemRole::typeId)
		{
			preDataChanged_( column, roleId, data );
			const auto result = data.tryCast( type_ );
			postDataChanged_( column, roleId, data );
			return result;
		}
		else if (roleId == ItemRole::barColorId)
		{
			preDataChanged_( column, roleId, data );
			const auto result = data.tryCast( barColor_ );
			postDataChanged_( column, roleId, data );
			return result;
		}
		else if (roleId == ItemRole::rowSpanId)
		{
			preDataChanged_( column, roleId, data );
			const auto result = data.tryCast( rowSpan_ );
			postDataChanged_( column, roleId, data );
			return result;
		}

		// Text box item
		if (roleId == ItemRole::textId)
		{
			preDataChanged_( column, roleId, data );
			const auto result = data.tryCast( text_ );
			postDataChanged_( column, roleId, data );
			return result;
		}

		// Frame slider item
		if (roleId == ItemRole::eventNameId)
		{
			// Need to return true so that undo state is restored correctly
			return true;
		}
		else if (roleId == ItemRole::keyFramesId)
		{
			preDataChanged_( column, roleId, data );
			const auto result = data.tryCast( keyFrames_ );
			postDataChanged_( column, roleId, data );
			return result;
		}

		// Bar slider item
		if (roleId == ItemRole::startTimeId)
		{
			preDataChanged_( column, roleId, data );
			const auto result = data.tryCast( startTime_ );
			postDataChanged_( column, roleId, data );
			return result;
		}
		else if (roleId == ItemRole::endTimeId)
		{
			preDataChanged_( column, roleId, data );
			const auto result = data.tryCast( endTime_ );
			postDataChanged_( column, roleId, data );
			return result;
		}
		else if (roleId == ItemRole::eventPropertyId)
		{
			preDataChanged_( column, roleId, data );
			const auto result = data.tryCast( eventProperty_ );
			postDataChanged_( column, roleId, data );
			return result;
		}
		else if (roleId == ItemRole::eventActionId)
		{
			// Need to return true so that undo state is restored correctly
			return true;
		}
		else if (roleId == ItemRole::eventValueId)
		{
			preDataChanged_( column, roleId, data );
			const auto result = data.tryCast( eventValue_ );
			postDataChanged_( column, roleId, data );
			return result;
		}

		return AbstractListItem::setData( column, roleId, data );
	}

	virtual Connection connectPreDataChanged( DataCallback callback ) override
	{
		return preDataChanged_.connect( callback );
	}

	virtual Connection connectPostDataChanged( DataCallback callback ) override
	{
		return postDataChanged_.connect( callback );
	}

	// General timeline item
	std::string name_;
	std::string type_;
	std::string barColor_;
	int rowSpan_;

	// Text box item
	std::string text_;

	// Bar slider item
	double startTime_;
	double endTime_;
	std::string eventProperty_;
	std::string eventValue_;

	// Frame slider item
	ObjectHandleT< AbstractItemModel > keyFrames_;

	Signal< TimelineItem::DataSignature > preDataChanged_;
	Signal< TimelineItem::DataSignature > postDataChanged_;
};


TimelineModel::TimelineModel()
{
}

TimelineModel::~TimelineModel()
{

}

AbstractItem * TimelineModel::addKeyFrame( double time,
	const char * eventValue,
	const char * type,
	AbstractItem * parent )
{
	const int modelRow = 0;
	const int modelColumn = 0;
	auto keyFramesVariant = parent->getData( modelRow, modelColumn, ItemRole::keyFramesId );

	ObjectHandleT< AbstractItemModel > keyFramesHandle;
	const auto isHandle = keyFramesVariant.tryCast( keyFramesHandle );
	assert( isHandle );
	auto pKeyFramesModel = keyFramesHandle.get();
	assert( pKeyFramesModel != nullptr );

	const auto & preModelRowsInserted = preRowsInserted_;
	const AbstractItemModel::RangeCallback preRowsInserted =
		[ &preModelRowsInserted ]( const AbstractItemModel::ItemIndex & parentIndex,
			int startRow,
			int count )
	{
		preModelRowsInserted( startRow, count );
	};
	pKeyFramesModel->connectPreRowsInserted( preRowsInserted );

	const auto & postModelRowsInserted = postRowsInserted_;
	const AbstractItemModel::RangeCallback postRowsInserted =
		[ &postModelRowsInserted ]( const AbstractItemModel::ItemIndex & parentIndex,
			int startRow,
			int count )
	{
		postModelRowsInserted( startRow, count );
	};
	pKeyFramesModel->connectPostRowsInserted( postRowsInserted );

	const auto & preModelRowsRemoved = preRowsRemoved_;
	const AbstractItemModel::RangeCallback preRowsRemoved =
		[ &preModelRowsRemoved ]( const AbstractItemModel::ItemIndex & parentIndex,
			int startRow,
			int count )
	{
		preModelRowsRemoved( startRow, count );
	};
	pKeyFramesModel->connectPreRowsRemoved( preRowsRemoved );

	const auto & postModelRowsRemoved = postRowsRemoved_;
	const AbstractItemModel::RangeCallback postRowsRemoved =
		[ &postModelRowsRemoved ]( const AbstractItemModel::ItemIndex & parentIndex,
			int startRow,
			int count )
	{
		postModelRowsRemoved( startRow, count );
	};
	pKeyFramesModel->connectPostRowsRemoved( postRowsRemoved );

	const int keyFrameRow = pKeyFramesModel->rowCount( nullptr /* parent */ );
	const int keyFrameCount = 1;
	const auto inserted = pKeyFramesModel->insertRows( keyFrameRow,
		keyFrameCount,
		nullptr /* parent */ );
	assert( inserted );

	const AbstractItemModel::ItemIndex keyFrameIndex( keyFrameRow,
		0, /* column */
		nullptr /* parent */ );
	const auto pItem = pKeyFramesModel->item( keyFrameIndex );
	assert( pItem != nullptr );

	const int itemRow = 0;
	const int itemColumn = 0;
	bool setOk = true;
	setOk &= pItem->setData( itemRow, itemColumn, ItemRole::timeId, time );
	setOk &= pItem->setData( itemRow, itemColumn, ItemRole::eventValueId, eventValue );
	setOk &= pItem->setData( itemRow, itemColumn, ItemRole::typeId, type );
	assert( setOk );

	return pItem;
}

AbstractItem * TimelineModel::addComponent( const char * name )
{
	const int row = this->rowCount();
	const int count = 1;
	const auto inserted = this->insertRows( row, count );
	assert( inserted );

	const auto pItem = items_.back().get();
	assert( pItem != nullptr );

	const int column = 0;
	bool setOk = true;
	setOk &= pItem->setData( column, ItemRole::nameId, name );
	setOk &= pItem->setData( column, ItemRole::typeId, "fullBar" );
	setOk &= pItem->setData( column, ItemRole::barColorId, "#666666" );
	setOk &= pItem->setData( column, ItemRole::rowSpanId, 1 );
	assert( setOk );

	return pItem;
}

AbstractItem * TimelineModel::addTextBox( const char * name, const char * text )
{
	const int row = this->rowCount();
	const int count = 1;
	const auto inserted = this->insertRows( row, count );
	assert( inserted );

	const auto pItem = items_.back().get();
	assert( pItem != nullptr );

	const int column = 0;
	bool setOk = true;
	setOk &= pItem->setData( column, ItemRole::nameId, name );
	setOk &= pItem->setData( column, ItemRole::textId, text );
	setOk &= pItem->setData( column, ItemRole::typeId, "textBox" );
	setOk &= pItem->setData( column, ItemRole::barColorId, "#6666DD" );
	setOk &= pItem->setData( column, ItemRole::rowSpanId, 1 );
	assert( setOk );

	return pItem;
}

AbstractItem * TimelineModel::addFrameSlider( const char * name, const char * barColor, const char * eventProperty )
{
	const int row = this->rowCount();
	const int count = 1;
	const auto inserted = this->insertRows( row, count );
	assert( inserted );

	const auto pItem = items_.back().get();
	assert( pItem != nullptr );

	const int column = 0;
	bool setOk = true;
	setOk &= pItem->setData( column, ItemRole::nameId, name );
	setOk &= pItem->setData( column, ItemRole::typeId, "frameSlider" );
	setOk &= pItem->setData( column, ItemRole::barColorId, barColor );
	setOk &= pItem->setData( column, ItemRole::rowSpanId, 1 );
	setOk &= pItem->setData( column, ItemRole::eventPropertyId, eventProperty );

	return pItem;
}

AbstractItem * TimelineModel::addBarSlider( const char * name,
	double startTime,
	double endTime,
	const char * barColor,
	const char * eventProperty,
	const char * eventValue )
{
	const int row = this->rowCount();
	const int count = 1;
	const auto inserted = this->insertRows( row, count );
	assert( inserted );

	const auto pItem = items_.back().get();
	assert( pItem != nullptr );

	const int column = 0;
	bool setOk = true;
	setOk &= pItem->setData( column, ItemRole::nameId, name );
	setOk &= pItem->setData( column, ItemRole::typeId, "barSlider" );
	setOk &= pItem->setData( column, ItemRole::barColorId, barColor );
	setOk &= pItem->setData( column, ItemRole::rowSpanId, 1 );
	setOk &= pItem->setData( column, ItemRole::startTimeId, startTime );
	setOk &= pItem->setData( column, ItemRole::endTimeId, endTime );
	setOk &= pItem->setData( column, ItemRole::eventPropertyId, eventProperty );
	setOk &= pItem->setData( column, ItemRole::eventValueId, eventValue );

	return pItem;
}

AbstractItem * TimelineModel::item( int row ) const
{
	return items_[row].get();
}

int TimelineModel::index( const AbstractItem * item ) const
{
	auto it = std::find_if( items_.begin(), items_.end(),
		[&]( const std::unique_ptr< TimelineItem > & timelineItem ) { return timelineItem.get() == item; } );
	return static_cast< int >( it - items_.begin() );
}

int TimelineModel::rowCount() const
{
	return static_cast< int >( items_.size() );
}

int TimelineModel::columnCount() const
{
	return 1;
}

bool TimelineModel::insertRows( int row, int count ) /* override */
{
	assert( count > 0 );
	preRowsInserted_( row, count );

	const auto startRow = row;
	const auto endRow = row + count;
	for (auto i = startRow; i < endRow; ++i)
	{
		const auto pEmptyItem = new TimelineItem();
		const auto & preModelDataChanged = preDataChanged_;
		const auto preData = [ row, &preModelDataChanged ]( int column,
			ItemRole::Id role,
			const Variant & value )
		{
			preModelDataChanged( row, column, role, value );
		};
		const auto preDataChanged = pEmptyItem->connectPreDataChanged( preData );

		const auto & postModelDataChanged = postDataChanged_;
		const auto postData = [ row, &postModelDataChanged ]( int column,
			ItemRole::Id role,
			const Variant & value )
		{
			postModelDataChanged( row, column, role, value );
		};
		const auto postDataChanged = pEmptyItem->connectPostDataChanged( postData );

		auto itr = items_.cbegin() + i;
		items_.emplace( itr, pEmptyItem );
	}

	postRowsInserted_( row, count );
	return true;
}

bool TimelineModel::removeRows( int row, int count )
{
	preRowsRemoved_( row, count );
	auto begin = items_.begin() + row;
	auto end = begin + count;
	items_.erase( begin, end );
	postRowsRemoved_( row, count );
	return true;
}

std::vector< std::string > TimelineModel::roles() const
{
	std::vector< std::string > roles;
	roles.emplace_back( ItemRole::nameName );
	roles.emplace_back( ItemRole::textName );
	roles.emplace_back( ItemRole::typeName );
	roles.emplace_back( ItemRole::startTimeName );
	roles.emplace_back( ItemRole::endTimeName );
	roles.emplace_back( ItemRole::barColorName );
	roles.emplace_back( ItemRole::eventValueName );
	roles.emplace_back( ItemRole::eventNameName );
	roles.emplace_back( ItemRole::eventPropertyName );
	roles.emplace_back( ItemRole::eventActionName );
	roles.emplace_back( ItemRole::keyFramesName );
	roles.emplace_back( ItemRole::rowSpanName );
	return roles;
}

Connection TimelineModel::connectPreItemDataChanged( DataCallback callback ) /* override */
{
	return preDataChanged_.connect( callback );
}


Connection TimelineModel::connectPostItemDataChanged( DataCallback callback ) /* override */
{
	return postDataChanged_.connect( callback );
}

Connection TimelineModel::connectPreRowsInserted( RangeCallback callback ) /* override */
{
	return preRowsInserted_.connect( callback );
}

Connection TimelineModel::connectPostRowsInserted( RangeCallback callback ) /* override */
{
	return postRowsInserted_.connect( callback );
}

Connection TimelineModel::connectPreRowsRemoved( RangeCallback callback )
{
	return preRowsRemoved_.connect( callback );
}

Connection TimelineModel::connectPostRowsRemoved( RangeCallback callback )
{
	return postRowsRemoved_.connect( callback );
}
} // namespace wgt