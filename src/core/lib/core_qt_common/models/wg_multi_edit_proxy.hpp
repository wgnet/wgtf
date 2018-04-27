#ifndef WG_MULTI_EDIT_PROXY_HPP
#define WG_MULTI_EDIT_PROXY_HPP

#include "qt_connection_holder.hpp"
#include "core_qt_common/qt_new_handler.hpp"
#include <memory>
#include <QAbstractItemModel>
#include <vector>
#include <unordered_map>
#include <map>

namespace wgt
{

/**
* Proxy layer which merges properties for multiple items.
*
* This class assumes that the input models are property lists for different items. It produces a
* union between the sets of properties for all the items (one row for each unique property).
* Properties are compared using the roles specified in the mergeKeys fields: two properties
* belonging to different items are merged if they return the same values for all the roles
* indicated in mergeKeys.
*
* The proxy exposes a data role called multipleValues which is set to true for properties which
* are shared between multiple input objects. For all other roles, the data() method behaves
* in the following way when called for shared properties:
*	- if the property has the same value for ALL the objects, the value is returned
*	- otherwise, an invalid QVariant is returned
*
* The view can use this behaviour to show multiple-valued properties appropriately.
*
* The setData() method will forward the value to all the items which share the property.
*/
class WGMultiEditProxy : public QAbstractItemModel
{
	Q_OBJECT
	DECLARE_QT_MEMORY_HANDLER

public:
	WGMultiEditProxy();
	virtual ~WGMultiEditProxy();

	Q_INVOKABLE void addModel(QAbstractItemModel* model);
	Q_INVOKABLE void removeModel(QAbstractItemModel* model);

	Q_PROPERTY(QList<QString> mergeKeys READ getMergeKeys WRITE setMergeKeys);

	Q_INVOKABLE virtual QModelIndex index(int row, int column,
	                                      const QModelIndex& parent = QModelIndex()) const override;
	Q_INVOKABLE virtual QModelIndex parent(const QModelIndex& child) const override;

	Q_INVOKABLE virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	Q_INVOKABLE virtual int columnCount(const QModelIndex& parent = QModelIndex()) const override;

	Q_INVOKABLE virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
	Q_INVOKABLE virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

	virtual QHash<int, QByteArray> roleNames() const override;

private:

	struct Entry
	{
		QModelIndex sourceParent_;
		std::map<int, int> rows_;
	};

	struct Mapping
	{
		Mapping() : mapped_(false), rowCount_(0)
		{
		}
		bool mapped_;
		int rowCount_;
		QHash<QModelIndex, Mapping*>::iterator mapIter_;
		std::map<QAbstractItemModel*, Entry> entries_;
	};

	QList<QString> getMergeKeys() const;
	void setMergeKeys(const QList<QString>& mergeKeys);

	void onSourceDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles);

	QHash<QModelIndex, Mapping*>::iterator create_index_mapping(const QModelIndex& proxy_parent) const;
	QHash<QModelIndex, Mapping*>::const_iterator index_to_iterator(const QModelIndex& proxy_index) const;
	void clearMapping();

	void create_role_mapping(QAbstractItemModel* sourceModel);

	QList<QString> mergeKeys_;
	std::vector<int> mergeKeyRoleIds_;
	QHash<int, QByteArray> roleNames_;
	std::map<QAbstractItemModel*, std::map<int, int>> roleMappings_;

	//////////////////////////////////////////////////////////////////////////
	std::vector<QAbstractItemModel*> sourceModels_;
	mutable QHash<QModelIndex, Mapping*> mappings_;
	int multipleValuesRole_;
	QtConnectionHolder connections_;
};
}

#endif // WG_MULTI_EDIT_PROXY_HPP