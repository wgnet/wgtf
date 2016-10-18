#ifndef WG_ACTION_HPP
#define WG_ACTION_HPP

#include "../qt_new_handler.hpp"
#include <QQuickItem>

#include <memory>

namespace wgt
{
/**
* @ingroup wgcontrols
*/
class WGAction : public QQuickItem
{
	Q_OBJECT

	Q_PROPERTY( bool active READ getActive WRITE setActive )
	Q_PROPERTY( QString actionId READ getActionId WRITE setActionId )
	Q_PROPERTY( bool checkable READ getCheckable WRITE setCheckable )
	Q_PROPERTY( bool checked READ getChecked WRITE setChecked )
	Q_PROPERTY( bool enabled READ getEnabled WRITE setEnabled )
	Q_PROPERTY( bool visible READ getVisible WRITE setVisible )
	Q_PROPERTY( QVariant contextObject READ data WRITE setData NOTIFY dataChanged )
	DECLARE_QT_MEMORY_HANDLER

public:
	WGAction( QQuickItem * parent = nullptr );
	virtual ~WGAction();

protected:
	void componentComplete();

	bool getActive() const;
	void setActive( bool active );

	QString getActionId() const;
	void setActionId( const QString& actionId );

	bool getCheckable() const;
	void setCheckable( bool checkable );

	bool getChecked() const;
	void setChecked( bool checked );

	bool getEnabled() const;
	void setEnabled( bool enabled );

	bool getVisible() const;
	void setVisible( bool visible );

    QVariant data() const;
    void setData(const QVariant& data);

signals:
	void triggered();
	void dataChanged();

private:
	struct Implementation;
	std::unique_ptr< Implementation > impl_;
};
} // end namespace wgt
#endif //WG_ACTION_HPP
