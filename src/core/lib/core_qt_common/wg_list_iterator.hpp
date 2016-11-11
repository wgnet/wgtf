#ifndef WG_LIST_ITERATOR_HPP
#define WG_LIST_ITERATOR_HPP

#include <QObject>
#include <QVariant>

namespace wgt
{
class IListModel;
class IItem;

class WGListIterator : public QObject
{
	Q_OBJECT

	Q_PROPERTY(QVariant current READ getCurrent WRITE setCurrent)

public:
	WGListIterator(IListModel& listModel);
	virtual ~WGListIterator();

	Q_INVOKABLE void reset();
	Q_INVOKABLE bool moveNext();

private:
	QVariant getCurrent() const;
	void setCurrent(QVariant& value);

private:
	IListModel& listModel_;
	size_t size_;
	size_t currentIndex_;
	IItem* currentItem_;
};
} // end namespace wgt
#endif
