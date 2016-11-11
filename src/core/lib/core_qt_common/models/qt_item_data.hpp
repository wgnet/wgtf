#ifndef QT_ITEM_DATA_HPP
#define QT_ITEM_DATA_HPP

#include <QObject>
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

		QAbstractItemModel& model_;
		QList<int> roles_;
		QMetaObject* metaObject_;
	};

	QtItemData(const QModelIndex& index, std::weak_ptr<MetaObject> metaObject);

	const QModelIndex& index() const;

	const QMetaObject* metaObject() const override;
	int qt_metacall(QMetaObject::Call c, int id, void** argv) override;

private:
	struct Impl;
	std::unique_ptr<Impl> impl_;

public:
	static std::shared_ptr<MetaObject> getMetaObject(QAbstractItemModel& model);
};
}

#endif // QT_ITEM_DATA_HPP