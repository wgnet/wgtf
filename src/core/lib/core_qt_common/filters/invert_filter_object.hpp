#pragma once

#include "core_qt_common/qt_filter_object.hpp"

namespace wgt
{
class InvertFilterObject
	: public QtFilterObject
{
public:
	Q_OBJECT
	DECLARE_QT_MEMORY_HANDLER

	Q_PROPERTY(QObject* subFilter READ getSubFilter WRITE setSubFilter)

	InvertFilterObject();
	~InvertFilterObject();

	bool hasFilter() const override;
	bool filterAcceptsRowValid() const override;
	bool filterAcceptsRow(const Variant& variant) const override;

protected:
	QObject* getSubFilter() const;
	void setSubFilter(QObject* subFilter);

private:
	QtFilterObject* pSubFilter_ = nullptr;
};
}