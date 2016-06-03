#include "wg_copy_controller.hpp"
#include "helpers/qt_helpers.hpp"
#include "core_copy_paste/i_copy_paste_manager.hpp"
#include <cassert>
#include <QtCore>
#include <QQmlEngine>

namespace wgt
{
//==============================================================================
WGCopyController::WGCopyController( QObject * parent )
	: QObject( parent )
	, valueHint_( "" )
	, bPasted_( false )
{
}


//==============================================================================
WGCopyController::~WGCopyController()
{
}


//==============================================================================
void WGCopyController::setValue( const QVariant & data )
{
	if (data_ == data)
	{
		return;
	}
	data_ = data;
}

//==============================================================================
void WGCopyController::setValueHint( const QString & hint )
{
	if(hint.isEmpty() || hint.isNull())
	{
		return;
	}
	valueHint_ = hint.toUtf8().constData();
}

//==============================================================================
const char * WGCopyController::getDataHint() const
{
	return valueHint_.c_str();
}

//==============================================================================
const Variant & WGCopyController::getData()
{
	emit dataCopied();
	value_ = QtHelpers::toVariant( data_ );
	return value_;
}


//==============================================================================
bool WGCopyController::setData( const Variant& value )
{

	auto data = QtHelpers::toQVariant( value, this );
	if (data_ != data)
	{
		data_ = data;
		bPasted_ = true;
		emit dataPasted();
		return bPasted_;
	}
	return false;
}
} // end namespace wgt
