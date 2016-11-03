#ifndef SPREADSHEET_EXTENSION_HPP
#define SPREADSHEET_EXTENSION_HPP

#include "i_model_extension.hpp"
#include <QMap>
#include <QTime>
#include <QSet>

namespace wgt
{
class SpreadsheetExtension : public IModelExtension
{
	Q_OBJECT

public:
	SpreadsheetExtension();
	virtual ~SpreadsheetExtension();

	QVariant data( const QModelIndex &index, ItemRole::Id roleId ) const override;

	void onDataChanged( const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles ) override;

	Q_INVOKABLE QTime currentTime();
	Q_INVOKABLE void commitData();

private:
	QMap< QPersistentModelIndex, QTime > lastEdits_;
	QTime commitTime_;
};
} // end namespace wgt
#endif // SPREADSHEET_EXTENSION_HPP
