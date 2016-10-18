#include "data_change_notifier.hpp"

#include "core_qt_common/helpers/qt_helpers.hpp"
#include "core_qt_common/models/qt_model_helpers.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_variant/variant.hpp"


namespace wgt
{
DataChangeNotifier::DataChangeNotifier()
	: QObject()
	, source_( nullptr )
{
}


DataChangeNotifier::~DataChangeNotifier()
{
	source( nullptr );
}


void DataChangeNotifier::source( SourceType* source )
{
	connections_.clear();
	source_ = source;
	emit sourceChanged();

	if (source_ != nullptr)
	{
		connections_ += source_->signalPreDataChanged.connect( std::bind( &DataChangeNotifier::onPreDataChanged, this ) );
		connections_ += source_->signalPostDataChanged.connect( std::bind( &DataChangeNotifier::onPostDataChanged, this ) );
		connections_ += source_->signalDestructing.connect( std::bind( &DataChangeNotifier::onDestructing, this ) );
	}
}


const DataChangeNotifier::SourceType* DataChangeNotifier::source() const
{
	return source_;
}


QVariant DataChangeNotifier::getSource() const
{
	Variant variant = source_;
	return QtHelpers::toQVariant(variant, const_cast<DataChangeNotifier*>(this) );
}


bool DataChangeNotifier::setSource( const QVariant& source )
{
	Variant variant = QtHelpers::toVariant( source );
	if (variant.typeIs<SourceType>())
	{
		auto valueChangeNotifier = const_cast<SourceType*>(variant.cast<const SourceType*>());
		if (valueChangeNotifier != nullptr)
		{
			this->source(valueChangeNotifier);
			return true;
		}
	}
	return false;
}


QVariant DataChangeNotifier::getData() const
{
	assert( source_ != nullptr );
	return QtHelpers::toQVariant( source_->variantValue(), nullptr );
}


bool DataChangeNotifier::setData( const QVariant& value )
{
	assert( source_ != nullptr );
	return source_->variantValue( QtHelpers::toVariant( value ) );
}


void DataChangeNotifier::onPreDataChanged()
{
	assert( source_ != nullptr );
	emit dataAboutToBeChanged();
}


void DataChangeNotifier::onPostDataChanged()
{
	assert( source_ != nullptr );
	emit dataChanged();
}

void DataChangeNotifier::onDestructing()
{
	source( nullptr );
}
} // end namespace wgt
