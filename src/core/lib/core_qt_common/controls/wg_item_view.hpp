#ifndef WG_ITEM_VIEW_HPP
#define WG_ITEM_VIEW_HPP

#include "core_qt_common/qt_new_handler.hpp"

#include <memory>
#include <QQuickItem>
#include <QQmlListProperty>

class QAbstractItemModel;
class QString;

namespace wgt
{
class IModelExtension;

/** This class forms the base for all WGItemViews, including WGListView, WGTreeView, and WGTableView.*/
class WGItemView : public QQuickItem
{
	Q_OBJECT

	/** The data model that is presented in this view.*/
	Q_PROPERTY(QAbstractItemModel* model READ getModel WRITE setModel NOTIFY modelChanged)
	/** The IModelExtension components extending the functionality of this view.*/
	Q_PROPERTY(QQmlListProperty<wgt::IModelExtension> extensions READ getExtensions)
	/** The model after imbued by the extra roles from the extensions.*/
	Q_PROPERTY(QAbstractItemModel* extendedModel READ getExtendedModel NOTIFY extendedModelChanged)
	/** HeaderData list per column. This is created from the available roles in the view.*/
	Q_PROPERTY(QList<QObject*> headerData READ getHeaderData NOTIFY headerDataChanged)
	Q_PROPERTY(int modelRowCount READ getRowCount NOTIFY rowCountChanged)
	Q_PROPERTY(int modelColumnCount READ getColumnCount NOTIFY columnCountChanged)

	DECLARE_QT_MEMORY_HANDLER

public:
	WGItemView();
	~WGItemView();

	/** Extracts the row number from an index.
		@param index The index to get the row from.
		@return The row number.*/
	Q_INVOKABLE int getRow(const QModelIndex& index) const;
	/** Extracts the column number from an index.
		@param index The index to get the column from.
		@return The column number.*/
	Q_INVOKABLE int getColumn(const QModelIndex& index) const;
	/** Determines the parent index from an index.
		@param index The index to get the parent index from.
		@return The parent index.*/
	Q_INVOKABLE QModelIndex getParent(const QModelIndex& index) const;

	Q_INVOKABLE int sourceColumn(int index) const;
	Q_INVOKABLE void hideColumn(int index);
	Q_INVOKABLE void showAllColumns();

	Q_INVOKABLE QStringList mimeTypes() const;
	Q_INVOKABLE QVariantMap mimeData(const QList<QModelIndex>& indexes);
	Q_INVOKABLE bool canDropMimeData(const QVariantMap& data, Qt::DropAction action, const QModelIndex& index) const;
	Q_INVOKABLE bool dropMimeData(const QVariantMap& data, Qt::DropAction action, const QModelIndex& index);

	Q_INVOKABLE QStringList headerMimeTypes(Qt::Orientation orientation) const;
	Q_INVOKABLE QVariantMap headerMimeData(const QList<int>& sections, Qt::Orientation orientation) const;
	Q_INVOKABLE bool canDropHeaderMimeData(const QVariantMap& data, Qt::DropAction action, int section, Qt::Orientation orientation) const;
	Q_INVOKABLE bool dropHeaderMimeData(const QVariantMap& data, Qt::DropAction action, int section, Qt::Orientation orientation);

signals:
	/** Signalled when the model has changed.*/
	void modelChanged();
	/** Signalled when the extendedModel changed.*/
	void extendedModelChanged();
	/** Signalled when the headerData changed.*/
	void headerDataChanged();
	void rowCountChanged();
	void columnCountChanged();

private:
	QAbstractItemModel* getModel() const;
	void setModel(QAbstractItemModel* model);

	QQmlListProperty<IModelExtension> getExtensions() const;
	static void appendExtension(QQmlListProperty<IModelExtension>* property, IModelExtension* value);
	static int countExtensions(QQmlListProperty<IModelExtension>* property);
	static IModelExtension* extensionAt(QQmlListProperty<IModelExtension>* property, int index);
	static void clearExtensions(QQmlListProperty<IModelExtension>* property);

	QAbstractItemModel* getExtendedModel() const;
	QList<QObject*> getHeaderData() const;

	int getRowCount() const;
	int getColumnCount() const;

	void refreshHeaderData();

	struct Impl;
	std::unique_ptr<Impl> impl_;
};
} // end namespace wgt
#endif//WG_ITEM_VIEW_HPP
