#pragma once

#include "core_qt_common/qt_filter_object.hpp"
#include "core_qt_common/filter_expression.hpp"
#include "core_data_model/i_item_role.hpp"
#include <regex>

namespace wgt
{
class SearchFilterObject
	: public QtFilterObject
{
public:
	Q_OBJECT
	DECLARE_QT_MEMORY_HANDLER

	SearchFilterObject();
	~SearchFilterObject();

	enum SearchFilterRole
	{
		FullPath = 0,
		IndexPath = 1
	};
	Q_ENUMS(SearchFilterRole)

	Q_PROPERTY(SearchFilterRole filterRole READ getFilterRole WRITE setFilterRole)
	Q_PROPERTY(QObject* parentFilter READ getParentFilter WRITE setParentFilter)
	Q_PROPERTY(QString filterString READ getFilterString WRITE setFilterString NOTIFY filterStringChanged)
	Q_PROPERTY(QString filterExpression READ getFilterExpression WRITE setFilterExpression NOTIFY filterExpressionChanged)
	Q_PROPERTY(QString filterName READ getFilterName WRITE setFilterName)

	bool hasFilter() const override;
	bool filterAcceptsRowValid() const override;
	bool filterAcceptsRow(const Variant& variant) const override;

signals:
	void filterStringChanged();
	void filterExpressionChanged();

protected:
	SearchFilterRole getFilterRole() const;
	void setFilterRole(SearchFilterRole filterRole);

	QString getFilterString() const;
	void setFilterString(const QString& filterString);

	QString getFilterExpression() const;
	void setFilterExpression(const QString& expression);

	QString getFilterName() const override;
	void setFilterName(const QString& filterName);

	QObject* getParentFilter() const;
	void setParentFilter(QObject* filter);

private:
	std::string getParentPath(const std::string& path) const;

	ItemRole::Id filterRoleId_;
	SearchFilterRole filterRole_;
	QtFilterObject* parentFilter_ = nullptr;
	std::regex filter_;
	std::string filterString_;
	FilterExpression filterExpression_;
	std::string filterName_;
};
}