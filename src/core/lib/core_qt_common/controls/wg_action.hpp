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

	Q_PROPERTY(bool active READ getActive WRITE setActive)
	Q_PROPERTY(QString actionId READ getActionId WRITE setActionId)
	Q_PROPERTY(QString actionText READ getActionText WRITE setActionText)
	Q_PROPERTY(QString actionPath READ getActionPath WRITE setActionPath)
	Q_PROPERTY(bool checkable READ getCheckable WRITE setCheckable)
	Q_PROPERTY(bool checked READ getChecked WRITE setChecked)
	Q_PROPERTY(bool enabled READ getEnabled WRITE setEnabled)
	Q_PROPERTY(bool visible READ getVisible WRITE setVisible)
	Q_PROPERTY(bool separator READ getSeparator WRITE setSeparator)
	DECLARE_QT_MEMORY_HANDLER

public:
	WGAction(QQuickItem* parent = nullptr);
	virtual ~WGAction();

protected:
	void componentComplete();

	bool getActive() const;
	void setActive(bool active);

	QString getActionId() const;
	void setActionId(const QString& actionId);

	QString getActionText() const;
	void setActionText(const QString& actionText);

	QString getActionPath() const;
	void setActionPath(const QString& actionPath);

	bool getCheckable() const;
	void setCheckable(bool checkable);

	bool getChecked() const;
	void setChecked(bool checked);

	bool getEnabled() const;
	void setEnabled(bool enabled);

	bool getVisible() const;
	void setVisible(bool visible);

	bool getSeparator() const;
	void setSeparator(bool separator);

	QVariant data() const;
	void setData(const QVariant& data);

signals:
	void triggered();
	void dataChanged();

private:
	struct Implementation;
	std::unique_ptr<Implementation> impl_;
};
} // end namespace wgt
#endif // WG_ACTION_HPP
