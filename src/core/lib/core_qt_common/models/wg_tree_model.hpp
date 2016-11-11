#ifndef WG_TREE_MODEL_HPP
#define WG_TREE_MODEL_HPP

#include <QAbstractItemModel>
#include <QQmlListProperty>

#include "core_data_model/i_tree_model.hpp"
#include "core_qt_common/qt_new_handler.hpp"
#include "qt_model_macros.hpp"
#include "core_data_model/i_item_role.hpp"

namespace wgt
{
class IModelExtensionOld;

/**
 *	This is the Qt data model which Qt views will use to display data.
 */
class WGTreeModel : public QAbstractItemModel
{
	Q_OBJECT

	Q_PROPERTY(QVariant source READ getSource WRITE setSource NOTIFY sourceChanged)
	Q_PROPERTY(QQmlListProperty<wgt::IModelExtensionOld> extensions READ getExtensions)
	Q_CLASSINFO("DefaultProperty", "extensions")

	DECLARE_QT_MEMORY_HANDLER

public:
	WGTreeModel();
	virtual ~WGTreeModel();

	Q_INVOKABLE QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;
	Q_INVOKABLE QModelIndex parent(const QModelIndex& child) const Q_DECL_OVERRIDE;
	Q_INVOKABLE QModelIndex convertItemToIndex(const QVariant& item) const;
	Q_INVOKABLE int rowCount(const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;
	Q_INVOKABLE int columnCount(const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;
	Q_INVOKABLE QVariant headerData(int section, Qt::Orientation orientation,
	                                int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
	Q_INVOKABLE QVariant headerData(int column, QString roleName) const;

	template <typename T>
	void registerExtension()
	{
		auto extension = new T();
		extension->setParent(this);
		registerExtension(extension);
	}

	// Used to retrieve the underlying data model. In WGTreeModel this will be the source, but it could be
	// a filtered or altered
	virtual ITreeModel* getModel() const;

	const QVariant& getSource() const;
	void setSource(const QVariant& source);

private:
	void registerExtension(IModelExtensionOld* extension);
	bool decodeRole(int role, ItemRole::Id& o_RoleId) const;

	// QAbstractItemModel Start
	QHash<int, QByteArray> roleNames() const Q_DECL_OVERRIDE;

	bool hasChildren(const QModelIndex& parent) const Q_DECL_OVERRIDE;

	QVariant data(const QModelIndex& index, int role) const Q_DECL_OVERRIDE;
	bool setData(const QModelIndex& index, const QVariant& value, int role) Q_DECL_OVERRIDE;
	// QAbstractItemModel End

	void onSourceChanged();

	QQmlListProperty<IModelExtensionOld> getExtensions() const;

	static void appendExtension(QQmlListProperty<IModelExtensionOld>* property, IModelExtensionOld* value);
	static IModelExtensionOld* extensionAt(QQmlListProperty<IModelExtensionOld>* property, int index);
	static void clearExtensions(QQmlListProperty<IModelExtensionOld>* property);
	static int countExtensions(QQmlListProperty<IModelExtensionOld>* property);

signals:
	void sourceChanged();

private:
	void onModelDataChanged(int column, ItemRole::Id roleId, const Variant& data);
	void onPreItemDataChanged(const IItem* item, int column, ItemRole::Id roleId, const Variant& data);
	void onPostItemDataChanged(const IItem* item, int column, ItemRole::Id roleId, const Variant& data);
	void onPreItemsInserted(const IItem* parent, size_t index, size_t count);
	void onPostItemsInserted(const IItem* parent, size_t index, size_t count);
	void onPreItemsRemoved(const IItem* parent, size_t index, size_t count);
	void onPostItemsRemoved(const IItem* parent, size_t index, size_t count);
	void onDestructing();

	void changeHeaderData(Qt::Orientation orientation, int first, int last);
	void beginChangeData(const QModelIndex& index, int role, const QVariant& value);
	void endChangeData(const QModelIndex& index, int role, const QVariant& value);
	void beginInsertRows(const QModelIndex& parent, int first, int last);
	void endInsertRows(const QModelIndex& parent, int first, int last);
	void beginRemoveRows(const QModelIndex& parent, int first, int last);
	void endRemoveRows(const QModelIndex& parent, int first, int last);

signals:
	void headerDataChanged(Qt::Orientation orientation, int first, int last);
	void itemDataAboutToBeChanged(const QModelIndex& index, int role, const QVariant& value);
	void itemDataChanged(const QModelIndex& index, int role, const QVariant& value);
	// private signals
	void headerDataChangedThread(Qt::Orientation orientation, int first, int last, QPrivateSignal);
	void itemDataAboutToBeChangedThread(const QModelIndex& index, int role, const QVariant& value, QPrivateSignal);
	void itemDataChangedThread(const QModelIndex& index, int role, const QVariant& value, QPrivateSignal);
	void rowsAboutToBeInsertedThread(const QModelIndex& parent, int first, int last, QPrivateSignal);
	void rowsInsertedThread(const QModelIndex& parent, int first, int last, QPrivateSignal);
	void rowsAboutToBeRemovedThread(const QModelIndex& parent, int first, int last, QPrivateSignal);
	void rowsRemovedThread(const QModelIndex& parent, int first, int last, QPrivateSignal);

private:
	class Impl;
	std::unique_ptr<Impl> impl_;
};
} // end namespace wgt
#endif // QT_TREE_MODEL_HPP
