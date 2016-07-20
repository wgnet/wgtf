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
	KeyFrameItem( float time, const char * eventValue, const char * type )
		: time_( time )
		, eventValue_( eventValue )
		, type_( type )
	{

	}

	Variant getData( int column, size_t roleId ) const override
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

		return Variant();
	}

	
	bool setData( int column,
		size_t roleId,
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

	float time_;
	std::string eventValue_;
	std::string type_;

	Signal< KeyFrameItem::DataSignature > preDataChanged_;
	Signal< KeyFrameItem::DataSignature > postDataChanged_;
};

class KeyFramesModel : public AbstractListModel
{
public:
	KeyFrameItem * addKeyFrame( float time, const char * eventValue, const char * type )
	{
		const int row = static_cast< int >( items_.size() );
		items_.emplace_back( new KeyFrameItem( time, eventValue, type ) );

		const auto pItem = items_.back().get();

		const auto & preModelDataChanged = preDataChanged_;
		const auto preData = [ row, &preModelDataChanged ]( int column,
			size_t role,
			const Variant & value )
		{
			preModelDataChanged( row, column, role, value );
		};
		const auto preDataChanged = pItem->connectPreDataChanged( preData );

		const auto & postModelDataChanged = postDataChanged_;
		const auto postData = [ row, &postModelDataChanged ]( int column,
			size_t role,
			const Variant & value )
		{
			postModelDataChanged( row, column, role, value );
		};
		const auto postDataChanged = pItem->connectPostDataChanged( postData );

		return pItem;
	}

	AbstractItem * item( int row ) const
	{
		return items_[row].get();
	}

	int index( const AbstractItem * item ) const
	{
		auto it = std::find_if( items_.begin(), items_.end(),
			[&](const std::unique_ptr< KeyFrameItem >& keyFrameItem) { return keyFrameItem.get() == item; } );
		return static_cast< int >( it - items_.begin() );
	}

	int rowCount() const
	{
		return static_cast< int >( items_.size() );
	}

	int columnCount() const
	{
		return 1;
	}

	std::vector< std::string > roles() const override
	{
		std::vector< std::string > roles;
		roles.emplace_back( ItemRole::timeName );
		roles.emplace_back( ItemRole::eventValueName );
		roles.emplace_back( ItemRole::typeName );
		return roles;
	}

	Connection connectPreItemDataChanged( DataCallback callback ) /* override */
	{
		return preDataChanged_.connect( callback );
	}


	Connection connectPostItemDataChanged( DataCallback callback ) /* override */
	{
		return postDataChanged_.connect( callback );
	}

	std::vector< std::unique_ptr< KeyFrameItem > > items_;
	Signal< KeyFramesModel::DataSignature > preDataChanged_;
	Signal< KeyFramesModel::DataSignature > postDataChanged_;
};

class TimelineItem : public AbstractListItem
{
public:
	TimelineItem( const char * name, const char * type, const char * barColor, int rowSpan )
		: name_( name )
		, type_( type )
		, barColor_( barColor )
		, rowSpan_( rowSpan )
	{

	}

	Variant getData( int column, size_t roleId ) const override
	{
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

		return AbstractListItem::getData( column, roleId );
	}

	bool setData( int column,
		size_t roleId,
		const Variant & data ) override
	{
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

	std::string name_;
	std::string type_;
	std::string barColor_;
	int rowSpan_;

	Signal< TimelineItem::DataSignature > preDataChanged_;
	Signal< TimelineItem::DataSignature > postDataChanged_;
};

class ComponentItem : public TimelineItem
{
public:
	ComponentItem( const char * name )
		: TimelineItem( name, "fullBar", "#666666", 1 )
	{
	}
};

class TextBoxItem : public TimelineItem
{
public:
	TextBoxItem( const char * name, const char * text )
		: TimelineItem( name, "textBox", "#6666DD", 1 )
		, text_( text )
	{
	}

	Variant getData( int column, size_t roleId ) const override
	{
		if (roleId == ItemRole::textId)
		{
			return text_;
		}

		return TimelineItem::getData( column, roleId );
	}

	virtual bool setData( int column,
		size_t roleId,
		const Variant & data ) override
	{
		if (roleId == ItemRole::textId)
		{
			preDataChanged_( column, roleId, data );
			const auto result = data.tryCast( text_ );
			postDataChanged_( column, roleId, data );
			return result;
		}

		return TimelineItem::setData( column, roleId, data );
	}

	std::string text_;
};

class FrameSliderItem : public TimelineItem
{
public:
	FrameSliderItem( const char * name, const char * barColor, const char * eventProperty )
		: TimelineItem( name, "frameSlider",barColor, 1 )
		, eventProperty_( eventProperty )
		, keyFrames_( new KeyFramesModel )
	{
	}

	KeyFrameItem * addKeyFrame( float time, const char * eventValue, const char * type )
	{
		return keyFrames_->addKeyFrame( time, eventValue, type );
	}

	Variant getData( int column, size_t roleId ) const override
	{
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
			return ObjectHandleT< AbstractListModel >( keyFrames_.get() );
		}

		return TimelineItem::getData( column, roleId );
	}

	virtual bool setData( int column,
		size_t roleId,
		const Variant & data ) override
	{
		if (roleId == ItemRole::eventNameId)
		{
			return false;
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
			return false;
		}
		else if (roleId == ItemRole::keyFramesId)
		{
			return false;
		}

		return TimelineItem::setData( column, roleId, data );
	}

	std::string eventProperty_;
	std::unique_ptr< KeyFramesModel > keyFrames_;
};

class BarSliderItem : public TimelineItem
{
public:
	BarSliderItem( const char * name, float startTime, float endTime, const char * barColor, const char * eventProperty, const char * eventValue )
		: TimelineItem( name, "barSlider",barColor, 1 )
		, startTime_( startTime )
		, endTime_( endTime )
		, eventProperty_( eventProperty )
		, eventValue_( eventValue )
	{
	}

	Variant getData( int column, size_t roleId ) const override
	{
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

		return TimelineItem::getData( column, roleId );
	}

	virtual bool setData( int column,
		size_t roleId,
		const Variant & data ) override
	{
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
			return false;
		}
		else if (roleId == ItemRole::eventValueId)
		{
			preDataChanged_( column, roleId, data );
			const auto result = data.tryCast( eventValue_ );
			postDataChanged_( column, roleId, data );
			return result;
		}

		return TimelineItem::setData( column, roleId, data );
	}

	float startTime_;
	float endTime_;
	std::string eventProperty_;
	std::string eventValue_;
};


TimelineModel::TimelineModel()
{
}

TimelineModel::~TimelineModel()
{

}

AbstractItem * TimelineModel::addKeyFrame( float time, const char * eventValue, const char * type, AbstractItem * parent )
{
	auto frameSlider = dynamic_cast< FrameSliderItem * >( parent );
	if (frameSlider == nullptr)
	{
		return nullptr;
	}

	return frameSlider->addKeyFrame( time, eventValue, type );
}

AbstractItem * TimelineModel::addComponent( const char * name )
{
	const int row = static_cast< int >( items_.size() );
	items_.emplace_back( new ComponentItem( name ) );

	const auto pItem = items_.back().get();

	const auto & preModelDataChanged = preDataChanged_;
	const auto preData = [ row, &preModelDataChanged ]( int column,
		size_t role,
		const Variant & value )
	{
		preModelDataChanged( row, column, role, value );
	};
	const auto preDataChanged = pItem->connectPreDataChanged( preData );

	const auto & postModelDataChanged = postDataChanged_;
	const auto postData = [ row, &postModelDataChanged ]( int column,
		size_t role,
		const Variant & value )
	{
		postModelDataChanged( row, column, role, value );
	};
	const auto postDataChanged = pItem->connectPostDataChanged( postData );

	return pItem;
}

AbstractItem * TimelineModel::addTextBox( const char * name, const char * text )
{
	const int row = static_cast< int >( items_.size() );
	items_.emplace_back( new TextBoxItem( name, text ) );

	const auto pItem = items_.back().get();

	const auto & preModelDataChanged = preDataChanged_;
	const auto preData = [ row, &preModelDataChanged ]( int column,
		size_t role,
		const Variant & value )
	{
		preModelDataChanged( row, column, role, value );
	};
	const auto preDataChanged = pItem->connectPreDataChanged( preData );

	const auto & postModelDataChanged = postDataChanged_;
	const auto postData = [ row, &postModelDataChanged ]( int column,
		size_t role,
		const Variant & value )
	{
		postModelDataChanged( row, column, role, value );
	};
	const auto postDataChanged = pItem->connectPostDataChanged( postData );

	return pItem;
}

AbstractItem * TimelineModel::addFrameSlider( const char * name, const char * barColor, const char * eventProperty )
{
	const int itemRow = static_cast< int >( items_.size() );

	const auto pItem = new FrameSliderItem( name,
		barColor,
		eventProperty );

	// Item
	{
		const auto & preModelDataChanged = preDataChanged_;
		const auto preData = [ itemRow, &preModelDataChanged ]( int column,
			size_t role,
			const Variant & value )
		{
			preModelDataChanged( itemRow, column, role, value );
		};
		const auto preDataChanged = pItem->connectPreDataChanged( preData );

		const auto & postModelDataChanged = postDataChanged_;
		const auto postData = [ itemRow, &postModelDataChanged ]( int column,
			size_t role,
			const Variant & value )
		{
			postModelDataChanged( itemRow, column, role, value );
		};
		const auto postDataChanged = pItem->connectPostDataChanged( postData );
	}

	// Model inside item
	{
		const auto & preModelDataChanged = preDataChanged_;
		const auto preData = [ &preModelDataChanged ]( int row,
			int column,
			size_t role,
			const Variant & value )
		{
			preModelDataChanged( row, column, role, value );
		};
		const auto preDataChanged =
			pItem->keyFrames_->connectPreItemDataChanged( preData );

		const auto & postModelDataChanged = postDataChanged_;
		const auto postData = [ &postModelDataChanged ]( int row,
			int column,
			size_t role,
			const Variant & value )
		{
			postModelDataChanged( row, column, role, value );
		};
		const auto postDataChanged =
			pItem->keyFrames_->connectPostItemDataChanged( postData );
	}

	items_.emplace_back( pItem );
	return items_.back().get();
}

AbstractItem * TimelineModel::addBarSlider( const char * name, float startTime, float endTime, const char * barColor, const char * eventProperty, const char * eventValue )
{
	const int row = static_cast< int >( items_.size() );
	items_.emplace_back( new BarSliderItem( name,
		startTime,
		endTime,
		barColor,
		eventProperty,
		eventValue ) );

	const auto pItem = items_.back().get();

	const auto & preModelDataChanged = preDataChanged_;
	const auto preData = [ row, &preModelDataChanged ]( int column,
		size_t role,
		const Variant & value )
	{
		preModelDataChanged( row, column, role, value );
	};
	const auto preDataChanged = pItem->connectPreDataChanged( preData );

	const auto & postModelDataChanged = postDataChanged_;
	const auto postData = [ row, &postModelDataChanged ]( int column,
		size_t role,
		const Variant & value )
	{
		postModelDataChanged( row, column, role, value );
	};
	const auto postDataChanged = pItem->connectPostDataChanged( postData );

	return items_.back().get();
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

Connection TimelineModel::connectPreRowsRemoved( RangeCallback callback )
{
	return preRowsRemoved_.connect( callback );
}

Connection TimelineModel::connectPostRowsRemoved( RangeCallback callback )
{
	return postRowsRemoved_.connect( callback );
}
} // namespace wgt