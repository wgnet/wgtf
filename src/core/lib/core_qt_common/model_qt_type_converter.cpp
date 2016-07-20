#include "model_qt_type_converter.hpp"

#include "core_data_model/abstract_item_model.hpp"
#include "core_qt_common/models/qt_item_model.hpp"
#include "core_reflection/object_handle.hpp"

#include <QQmlEngine>

namespace wgt
{


ModelQtTypeConverter::ModelQtTypeConverter( IComponentContext & context )
	: context_( context )
{
}


bool ModelQtTypeConverter::toVariant( const QVariant & qVariant, Variant & o_variant ) const
{
	if (qVariant.canConvert< QtTableModel * >())
	{
		auto model = qVariant.value< QtTableModel * >();
		o_variant = ObjectHandle( &model->source() );
		return true;
	}

	if (qVariant.canConvert< QtTreeModel * >())
	{
		auto model = qVariant.value< QtTreeModel * >();
		o_variant = ObjectHandle( &model->source() );
		return true;
	}

	if (qVariant.canConvert< QtListModel * >())
	{
		auto model = qVariant.value< QtListModel * >();
		o_variant = ObjectHandle( &model->source() );
		return true;
	}

	if (qVariant.canConvert< QtItemModel * >())
	{
		auto model = qVariant.value< QtItemModel * >();
		o_variant = ObjectHandle( &model->source() );
		return true;
	}

	return false;
}

bool ModelQtTypeConverter::toQVariant(const Variant & variant, QVariant & o_qVariant, QObject* parent) const
{
	if (!variant.typeIs< ObjectHandle >())
	{
		return false;
	}
	
	ObjectHandle provider;
	if (!variant.tryCast( provider ))
	{
		return false;
	}

	{
		auto source = provider.getBase< AbstractItemModel >();
		if (source != nullptr)
		{
			assert( parent != nullptr );
			auto model = new QtItemModel( context_, *source );
			model->setParent( parent );
			o_qVariant = QVariant::fromValue( model );
			return true;
		}
	}

	{
		auto source = provider.getBase< AbstractListModel >();
		if (source != nullptr)
		{
			assert( parent != nullptr );
			auto model = new QtListModel( context_, *source );
			model->setParent( parent );
			o_qVariant = QVariant::fromValue( model );
			return true;
		}
	}

	{
		auto source = provider.getBase< AbstractTreeModel >();
		if (source != nullptr)
		{
			assert( parent != nullptr );
			auto model = new QtTreeModel( context_, *source );
			model->setParent( parent );
			o_qVariant = QVariant::fromValue( model );
			return true;
		}
	}

	{
		auto source = provider.getBase< AbstractTableModel >();
		if (source != nullptr)
		{
			assert( parent != nullptr );
			auto model = new QtTableModel( context_, *source );
			model->setParent(parent);
			o_qVariant = QVariant::fromValue( model );
			return true;
		}
	}

	return false;
}
} // end namespace wgt
