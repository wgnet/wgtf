#ifndef WG_ITEM_VIEW_HPP
#define WG_ITEM_VIEW_HPP

#include "core_qt_common/qt_new_handler.hpp"

#include <memory>
#include <QQuickItem>
#include <QQmlListProperty>

class QAbstractItemModel;
class QString;
class QItemSelection;
typedef QList<QModelIndex> QModelIndexList;

namespace wgt
{
class QtModelExtension;
class CustomModelExtension;
class ModelExtensionManager;

/** This class forms the base for all WGItemViews, including WGListView, WGTreeView, and WGTableView.*/
class WGItemView : public QQuickItem
{
	Q_OBJECT

	/** The data model that is presented in this view.*/
	Q_PROPERTY(QAbstractItemModel* model READ getModel WRITE setModel NOTIFY modelChanged)
	/** The QtModelExtension components extending the functionality of this view.*/
	Q_PROPERTY(QQmlListProperty<wgt::QtModelExtension> extensions READ getExtensions)
	/** The model after imbued by the extra roles from the extensions.*/
	Q_PROPERTY(QAbstractItemModel* extendedModel READ getExtendedModel NOTIFY extendedModelChanged)
	Q_PROPERTY(QAbstractItemModel* headerRowModel READ getHeaderRowModel NOTIFY headerRowModelChanged)
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

	Q_INVOKABLE QModelIndex sourceIndex(const QModelIndex& index) const;
	Q_INVOKABLE QModelIndex extendedIndex(const QModelIndex& index) const;
	Q_INVOKABLE QModelIndexList getExtendedIndexes(const QItemSelection& sourceSelection) const;

	/** Check if the values in the clipboard can be copied.
	    @param indexes list of selected items to be copied.
		@return true if one of the selected items can be copied. */
	Q_INVOKABLE bool canCopy(const QList<QModelIndex>& indexes) const;
	/** Copy from the value role for the given list of items.
		Copies to the clipboard as text.
	    @param indexes list of selected items to be copied. */
	Q_INVOKABLE void copy(const QList<QModelIndex>& indexes) const;
	/** Check if the values in the clipboard can be pasted.
	    @param indexes list of selected items into which the values are pasted.
		@return true if one of the selected items can be pasted. */
	Q_INVOKABLE bool canPaste(const QList<QModelIndex>& indexes) const;
	/** Paste to the value role for the given list of items.
		Pastes from the clipboard as text.
	    @param indexes list of selected items into which the values are pasted. */
	Q_INVOKABLE void paste(const QList<QModelIndex>& indexes);

	Q_INVOKABLE QStringList mimeTypes() const;
	Q_INVOKABLE QVariantMap mimeData(const QList<QModelIndex>& indexes) const;
	Q_INVOKABLE bool canDropMimeData(const QVariantMap& data, Qt::DropAction action, const QModelIndex& index) const;
	Q_INVOKABLE bool dropMimeData(const QVariantMap& data, Qt::DropAction action, const QModelIndex& index);

	Q_INVOKABLE QStringList headerMimeTypes(Qt::Orientation orientation) const;
	Q_INVOKABLE QVariantMap headerMimeData(const QList<int>& sections, Qt::Orientation orientation) const;
	Q_INVOKABLE bool canDropHeaderMimeData(const QVariantMap& data, Qt::DropAction action, int section,
	                                       Qt::Orientation orientation) const;
	Q_INVOKABLE bool dropHeaderMimeData(const QVariantMap& data, Qt::DropAction action, int section,
	                                    Qt::Orientation orientation);

	Q_INVOKABLE wgt::QtModelExtension* createExtension(const QString& name);

	Q_INVOKABLE QModelIndex toModelIndex(const QPersistentModelIndex& persistentModelIndex) const;
	Q_INVOKABLE QPersistentModelIndex toPersistentModelIndex(const QModelIndex& modelIndex) const;

	Q_INVOKABLE QModelIndexList toModelIndexList(const QModelIndex& modelIndex) const;
	Q_INVOKABLE QModelIndexList toModelIndexList(const QItemSelection& itemSelection) const;
	Q_INVOKABLE QItemSelection toItemSelection(const QModelIndexList& modelIndexList) const;

signals:
	/** Signalled when the model has changed.*/
	void modelChanged();
	/** Signalled when the extendedModel changed.*/
	void extendedModelChanged();
	void headerRowModelChanged();
	void rowCountChanged();
	void columnCountChanged();

private:
	QAbstractItemModel* getModel() const;
	void setModel(QAbstractItemModel* model);

	QQmlListProperty<QtModelExtension> getExtensions() const;
	static void appendExtension(QQmlListProperty<QtModelExtension>* property, QtModelExtension* value);
	static int countExtensions(QQmlListProperty<QtModelExtension>* property);
	static QtModelExtension* extensionAt(QQmlListProperty<QtModelExtension>* property, int index);
	static void clearExtensions(QQmlListProperty<QtModelExtension>* property);

	QAbstractItemModel* getExtendedModel() const;
	QAbstractItemModel* getHeaderRowModel() const;

	int getRowCount() const;
	int getColumnCount() const;

	struct Impl;
	std::unique_ptr<Impl> impl_;
};
} // end namespace wgt
#endif // WG_ITEM_VIEW_HPP
