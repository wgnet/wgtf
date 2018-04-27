#ifndef QT_ITEM_DATA_HPP
#define QT_ITEM_DATA_HPP

#include <QObject>
#include <QModelIndex>

#include <memory>
#include "core_qt_common/qt_new_handler.hpp"

class QAbstractItemModel;

namespace wgt
{
class QtItemData : public QObject
{
	DECLARE_QT_MEMORY_HANDLER
public:
	struct MetaObject
	{
		MetaObject(QAbstractItemModel& model);
		~MetaObject();

		void destroy();
		QAbstractItemModel * model_;
		QList<int> roles_;
		QMetaObject* metaObject_;
	};

	QtItemData(const QModelIndex& index, std::shared_ptr<MetaObject> & metaObject);
	~QtItemData();

	const QModelIndex& index() const;

	const QMetaObject* metaObject() const override;
	int qt_metacall(QMetaObject::Call c, int id, void** argv) override;

private:
	QModelIndex index_;
	std::shared_ptr< MetaObject > metaObject_;
public:
	static std::shared_ptr<MetaObject> getMetaObject(QAbstractItemModel& model);
};
}

#endif // QT_ITEM_DATA_HPP