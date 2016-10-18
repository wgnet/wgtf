#ifndef QT_COMPONENT_H
#define QT_COMPONENT_H

#include <QQuickItem>
#include <set>

namespace wgt
{
class QtComponentFinder : public QQuickItem
{
	Q_OBJECT

	Q_PROPERTY(QString type READ getType WRITE addType)

public:
	const std::set<QString>& getTypes() const;

	void addType(QString type);

	QString getType() const;

private:
	std::set<QString> types_;
};
} // end namespace wgt
#endif // QT_COMPONENT_H
