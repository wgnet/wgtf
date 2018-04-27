#pragma once
#include "core_qt_common/qt_new_handler.hpp"
#include "core_variant/variant.hpp"
#include <QQmlEngine>

namespace wgt
{
class QtFilterObject : public QObject
{
	Q_OBJECT
	DECLARE_QT_MEMORY_HANDLER

public:
	explicit QtFilterObject(bool unmanaged = true)
	{
		if (unmanaged)
		{
			QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
		}
	}

	virtual ~QtFilterObject() = default;

	virtual QString getFilterName() const { return "QtFilterObject"; }
	virtual bool hasFilter() const { return true; }

	virtual bool filterAcceptsItemValid() const { return false; }
	virtual bool filterAcceptsItem(const Variant&) const { return false; }

	virtual bool filterAcceptsRowValid() const { return false; }
	virtual bool filterAcceptsRow(const Variant&) const { return false; }

	virtual bool filterAcceptsColumnValid() const { return false; }
	virtual bool filterAcceptsColumn(const Variant&) const { return false; }
};
}