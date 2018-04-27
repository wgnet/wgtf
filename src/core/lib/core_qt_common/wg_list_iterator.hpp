#ifndef WG_LIST_ITERATOR_HPP
#define WG_LIST_ITERATOR_HPP

#include <QObject>
#include <QVariant>
#include "core_qt_common/interfaces/i_qt_helpers.hpp"
#include "core_dependency_system/depends.hpp"

namespace wgt
{
class AbstractListModel;
class AbstractItem;

class WGListIterator : public QObject, Depends<IQtHelpers>
{
	Q_OBJECT

	Q_PROPERTY(QVariant current READ getCurrent WRITE setCurrent)

public:
	WGListIterator(AbstractListModel& listModel);
	virtual ~WGListIterator();

	Q_INVOKABLE void reset();
	Q_INVOKABLE bool moveNext();

private:
	QVariant getCurrent() const;
	void setCurrent(QVariant& value);

private:
	AbstractListModel& listModel_;
	int size_;
	int currentIndex_;
	AbstractItem* currentItem_;
};
} // end namespace wgt
#endif
