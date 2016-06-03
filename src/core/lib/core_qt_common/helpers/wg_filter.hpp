#ifndef WG_FILTER_HPP
#define WG_FILTER_HPP

#include "../qt_new_handler.hpp"
#include "core_data_model/i_item_role.hpp"
#include "wg_types/hash_utilities.hpp"
#include <QObject>
#include <QtQml>
#include <QString>
#include <memory>

namespace wgt
{
class IItemFilter;

/**
 *	WGFilter
 *	The parent of all filter components. Grants access to the filter.
 */
class WGFilter : public QObject
{
	Q_OBJECT

	Q_PROPERTY( QString itemRole
				READ getItemRole
				WRITE setItemRole
				NOTIFY itemRoleChanged )

	DECLARE_QT_MEMORY_HANDLER

public:
	WGFilter() : roleId_( 0 ) {};
	virtual ~WGFilter() {};
	
	virtual IItemFilter * getFilter() const { return nullptr; }
	
	QString getItemRole() const { return roleName_; }
	void setItemRole( const QString & itemRole ) 
	{
		roleName_ = itemRole;
		roleId_ = ItemRole::compute( itemRole.toUtf8().constData() ); 
		updateInternalItemRole();
		emit itemRoleChanged();
	}

protected:
	virtual void updateInternalItemRole() {}

signals:
	void itemRoleChanged();

protected:
	QString roleName_;
	unsigned int roleId_;
};
} // end namespace wgt

QML_DECLARE_TYPE( wgt::WGFilter )
#endif // WG_FILTER_HPP
