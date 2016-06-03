#include "selection_helper.hpp"
#include "core_qt_common/helpers/qt_helpers.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_variant/variant.hpp"
#include "core_data_model/i_selection_handler.hpp"
#include <QVariant>
#include <QPersistentModelIndex>


namespace wgt
{
//==============================================================================
SelectionHelper::SelectionHelper( QObject * parent )
    : QObject( parent )
{
}


//==============================================================================
SelectionHelper::~SelectionHelper()
{
}


//==============================================================================
void SelectionHelper::source( SourceType* source )
{

	source_ = source;
	emit sourceChanged();
}


//==============================================================================
const SelectionHelper::SourceType* SelectionHelper::source() const
{
	return source_;
}


//==============================================================================
QVariant SelectionHelper::getSource() const
{
	Variant variant = ObjectHandle( const_cast< SourceType* >( source_ ) );
	return QtHelpers::toQVariant( variant, const_cast<SelectionHelper*>(this) );
}


//==============================================================================
bool SelectionHelper::setSource( const QVariant& source )
{
	Variant variant = QtHelpers::toVariant( source );
	if (variant.typeIs< ObjectHandle >())
	{
		ObjectHandle provider;
		if (variant.tryCast( provider ))
		{
			auto selectionHandler = provider.getBase< SourceType >();
			if (selectionHandler != nullptr)
			{
				this->source( selectionHandler );
				return true;
			}
		}
	}

	return false;
}


//==============================================================================
void SelectionHelper::select( const QList<QVariant>& selectionList )
{
	assert( source_ != nullptr );
	std::vector<IItem*> selectedItems;
	std::vector<int> selectedRows;
	for (auto & selection : selectionList)
	{
		assert( selection.canConvert<QModelIndex>() );
		QModelIndex index = selection.toModelIndex();
		int row = index.row();
		selectedRows.push_back( row );
		IItem* item = reinterpret_cast< IItem* >( index.internalId() );
		selectedItems.push_back( item );
	}
	source_->signalPreSelectionChanged();
	source_->setSelectedItems( selectedItems );
	source_->setSelectedRows( selectedRows );
	source_->signalPostSelectionChanged();
}
} // end namespace wgt
