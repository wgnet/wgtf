#ifndef WG_COPY_CONTROLLER_HPP
#define WG_COPY_CONTROLLER_HPP
#include "core_copy_paste/i_copyable_object.hpp"
#include "core_qt_common/qt_new_handler.hpp"
#include "core_variant/variant.hpp"
#include <QObject>
#include <QVariant>

namespace wgt
{
class WGCopyController
	: public QObject
	, public ICopyableObject
{
	Q_OBJECT

	Q_PROPERTY( QVariant data MEMBER data_  )
	Q_PROPERTY( bool pasted MEMBER bPasted_  )

public:
	WGCopyController( QObject * parent = NULL );
	~WGCopyController();

	Q_INVOKABLE void setValue( const QVariant & data );
	// This hint is designed for 
	// searching most suitable controls which the value could paste to. 
	Q_INVOKABLE void setValueHint( const QString & hint );
	DECLARE_QT_MEMORY_HANDLER

	const char * getDataHint() const override;
	const Variant & getData() override;
	bool setData( const Variant & value ) override;

signals:
	void dataCopied();
	void dataPasted();

private:
	std::string  valueHint_;
	QVariant data_;
	Variant value_;
	bool bPasted_;
};
} // end namespace wgt
#endif //WG_COPY_CONTROLLER_HPP
