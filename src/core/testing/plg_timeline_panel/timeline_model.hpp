#pragma once
#ifndef _TIMELINE_MODEL_HPP
#define _TIMELINE_MODEL_HPP

#include "core_data_model/abstract_item_model.hpp"

#include <memory>
#include <vector>

namespace wgt
{

class TimelineItem;

class TimelineModel : public AbstractListModel
{
public:
	TimelineModel();
	~TimelineModel();

	AbstractItem * addKeyFrame( double time, const char * eventValue, const char * type, AbstractItem * parent );
	AbstractItem * addComponent( const char * name );
	AbstractItem * addTextBox( const char * name, const char * text );
	AbstractItem * addFrameSlider( const char * name, const char * barColor, const char * eventProperty );
	AbstractItem * addBarSlider(const char * name, double startTime, double endTime, const char * barColor, const char * eventProperty, const char * eventValue);

	AbstractItem * item( int row ) const override;
	int index( const AbstractItem * item ) const override;

	int rowCount() const override;
	int columnCount() const override;

	bool insertRows( int row, int count ) override;
	bool removeRows( int row, int count ) override;

	std::vector< std::string > roles() const override;

	virtual Connection connectPreItemDataChanged( DataCallback callback ) override;
	virtual Connection connectPostItemDataChanged( DataCallback callback ) override;

	Connection connectPreRowsInserted( RangeCallback callback ) override;
	Connection connectPostRowsInserted( RangeCallback callback ) override;
	Connection connectPreRowsRemoved( RangeCallback callback ) override;
	Connection connectPostRowsRemoved( RangeCallback callback ) override;

private:
	// Disable copy and move
	TimelineModel( const TimelineModel & );
	TimelineModel( TimelineModel && );
	TimelineModel & operator=( const TimelineModel & );
	TimelineModel & operator=( TimelineModel && );

	std::vector< std::unique_ptr< TimelineItem > > items_;
	Signal< TimelineModel::DataSignature > preDataChanged_;
	Signal< TimelineModel::DataSignature > postDataChanged_;
	Signal< TimelineModel::RangeSignature > preRowsInserted_;
	Signal< TimelineModel::RangeSignature > postRowsInserted_;
	Signal< TimelineModel::RangeSignature > preRowsRemoved_;
	Signal< TimelineModel::RangeSignature > postRowsRemoved_;
};
} // namespace wgt

#endif