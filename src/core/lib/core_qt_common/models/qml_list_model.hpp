#pragma once

#include "qt_abstract_item_model.hpp"
#include "qml_model_data.hpp"
#include "qml_model_row.hpp"
#include "core_qt_common/qt_new_handler.hpp"

#include <QObject>
#include <QVector>

#include <memory>

namespace wgt
{
class QmlListModel : public QtListModel
{
	Q_OBJECT

	Q_PROPERTY(QmlModelRow* header READ header WRITE setHeader NOTIFY headerChanged)
	Q_PROPERTY(QQmlListProperty<wgt::QmlModelRow> rows READ rows)
	Q_CLASSINFO("DefaultProperty", "rows")

	DECLARE_QT_MEMORY_HANDLER

public:
	QmlListModel();
	virtual ~QmlListModel();

	/** Converts a location in the model into an index.
	@note An invalid index implies the top level of the model.
	@param row The row number for the index.
	@param column The column number for the index.
	@param parent The index this index falls under.
	@return A model index containing the location information. */
	QModelIndex index(int row, int column, const QModelIndex& parent) const override;

	/** Finds the parent index for an index.
	@param child The index to find the parent for.
	@return The index's parent index. */
	QModelIndex parent(const QModelIndex& child) const override;

	/** Gets number of rows in the model, relating to a parent.
	@note An invalid index implies the top level of the model.
	@param parent The parent index.
	@return The number of rows. */
	int rowCount(const QModelIndex& parent) const override;

	/** Gets number of columns in the model, relating to a parent.
	@note An invalid index implies the top level of the model.
	@param parent The parent index.
	@return The number of columns. */
	int columnCount(const QModelIndex& parent) const override;

	/** Determines if there are any indices under a parent index.
	@note An invalid index implies the top level of the model.
	@param parent The parent index to check under.
	@return The number of indices under the parent index. */
	bool hasChildren(const QModelIndex& parent) const override;

	/** Gets role data from an item at an index position.
	This converts the index and role to the format of the source model, calls
	the source model's getData, converts the value to a Qt compatible value,
	and returns that converted value.
	@param index The location of the item.
	@param role The role identifier of the value to get.
	@return The value of the role. */
	QVariant data(const QModelIndex& index, int role) const override;

	/** Changes role data for an item at an index position.
	This converts the index, value, and role to the format of the source model, calls
	the source model's setData, and returns whether if was successful or not.
	@param index The location of the item.
	@param value The new value of the role.
	@param role The role identifier of the value to get.
	@return True if successful. */
	bool setData(const QModelIndex& index, const QVariant& value, int role) override;

	/** Gets role header data at a position in the header.
	This converts the role to the source model's format, calls the source model's getHeaderData,
	converts the value to a Qt compatible value, and returns that converted value.
	@param section The location of the item, either column or row depending on orientation.
	@param orientation The layout of the header, either horizontal or vertical.
	@param role The role identifier of the value to get.
	@return The value of the role. */
	QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

	/** Changes role header data at a position in the header.
	This converts the role and value to the source model's format,
	calls the source model's setHeaderData,	and returns whether it was successful or not.
	@param section The location of the item, either column or row depending on orientation.
	@param orientation The layout of the header, either horizontal or vertical.
	@param value The new value of the role.
	@param role The role identifier of the value to get.
	@return True if successful. */
	bool setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int role) override;

	/** Returns a map of supported roles to be used in QML.
	These are mapped from role identifier to role name.
	@return The roles map. */
	QHash<int, QByteArray> roleNames() const override;

signals:
	void headerChanged();

private:
	QmlModelRow* header() const;
	void setHeader(QmlModelRow* row);

	QQmlListProperty<QmlModelRow> rows() const;
	static void appendRows(QQmlListProperty<QmlModelRow>* property, QmlModelRow* row);
	static int countRows(QQmlListProperty<QmlModelRow>* property);
	static QmlModelRow* rowAt(QQmlListProperty<QmlModelRow>* property, int index);
	static void clearRows(QQmlListProperty<QmlModelRow>* property);

	struct Implementation;
	friend Implementation;
	std::unique_ptr<Implementation> impl_;
};
} // end namespace wgt

QML_DECLARE_TYPE(wgt::QmlListModel)
