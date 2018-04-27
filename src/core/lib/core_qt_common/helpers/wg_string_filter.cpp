#include "wg_string_filter.hpp"
#include "core_data_model/i_item.hpp"
#include "core_data_model/filtering/string_filter.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_qt_common/interfaces/i_qt_helpers.hpp"
#include <QString>
#include <QVariant>

namespace wgt
{
struct WGStringFilter::Implementation
{
	Implementation(WGStringFilter& self);

	void setFilterText(const QString& filterText);

	WGStringFilter& self_;
	StringFilter filter_;
};

WGStringFilter::Implementation::Implementation(WGStringFilter& self) : self_(self)
{
}

void WGStringFilter::Implementation::setFilterText(const QString& filterText)
{
	std::string inputValue = filterText.toUtf8().constData();
	if (strcmp(inputValue.c_str(), filter_.getFilterText()) == 0)
	{
		return;
	}

	filter_.setFilterText(inputValue.c_str());
	filter_.signalFilterChanged();
}

WGStringFilter::WGStringFilter() : impl_(new Implementation(*this))
{
}

WGStringFilter::~WGStringFilter()
{
}

void WGStringFilter::updateInternalItemRole()
{
	impl_->filter_.setRole(roleId_);
}

QString WGStringFilter::getFilterText() const
{
	return QString::fromStdString(impl_->filter_.getFilterText());
}

void WGStringFilter::setFilterText(const QString& filterText)
{
	impl_->setFilterText(filterText);
}

IItemFilter* WGStringFilter::getFilter() const
{
	return static_cast<IItemFilter*>(&impl_->filter_);
}
} // end namespace wgt
