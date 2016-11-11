#include "wg_filtered_tree_model.hpp"

#include "core_data_model/filtered_tree_model.hpp"
#include "core_data_model/i_item.hpp"
#include "core_data_model/filtering/i_item_filter.hpp"
#include "core_qt_common/helpers/qt_helpers.hpp"
#include "core_qt_common/helpers/wg_filter.hpp"
#include "core_qt_common/qt_connection_holder.hpp"
#include "core_reflection/object_handle.hpp"

#include <QRegExp>

namespace wgt
{
struct WGFilteredTreeModel::Implementation
{
	Implementation(WGFilteredTreeModel& self);

	void setFilter(WGFilter* filter);
	void onFilterChanged();

	WGFilteredTreeModel& self_;
	WGFilter* filter_;
	FilteredTreeModel filteredModel_;
	QtConnectionHolder connections_;
	Connection filterChangedConnection_;
};

WGFilteredTreeModel::Implementation::Implementation(WGFilteredTreeModel& self) : self_(self), filter_(nullptr)
{
}

void WGFilteredTreeModel::Implementation::setFilter(WGFilter* filter)
{
	if (filter_ == filter)
	{
		return;
	}

	filterChangedConnection_.disconnect();
	filter_ = filter;
	auto current = filter_ != nullptr ? filter_->getFilter() : nullptr;

	if (current != nullptr)
	{
		filterChangedConnection_ =
		current->signalFilterChanged.connect(std::bind(&WGFilteredTreeModel::Implementation::onFilterChanged, this));
	}

	filteredModel_.setFilter(current);
	emit self_.filterChanged();
}

void WGFilteredTreeModel::Implementation::onFilterChanged()
{
	filteredModel_.refresh();
}

WGFilteredTreeModel::WGFilteredTreeModel() : impl_(new Implementation(*this))
{
	impl_->connections_ +=
	QObject::connect(this, &WGTreeModel::sourceChanged, this, &WGFilteredTreeModel::onSourceChanged);
}

WGFilteredTreeModel::~WGFilteredTreeModel()
{
	setSource(QVariant());

	impl_->connections_.reset();

	// Temporary hack to circumvent threading deadlock
	// JIRA: NGT-227
	impl_->filteredModel_.setSource(nullptr);
	// End temporary hack

	impl_->setFilter(nullptr);

	// evgenys: reseting impl_ to null first to avoid pure virtual func call in filteredModel_ destructor
	delete impl_.release();
}

ITreeModel* WGFilteredTreeModel::getModel() const
{
	// This component will return the filtered source, not the original source.
	return impl_ ? &impl_->filteredModel_ : nullptr;
}

void WGFilteredTreeModel::onSourceChanged()
{
	ITreeModel* source = nullptr;

	Variant variant = QtHelpers::toVariant(getSource());
	if (variant.typeIs<ITreeModel>())
	{
		source = const_cast<ITreeModel*>(variant.cast<const ITreeModel*>());
	}
	else if (variant.typeIs<ObjectHandle>())
	{
		NGT_WARNING_MSG("Double wrapping found, please wrap ITreeModel pointer as Variant directly.\n");
		ObjectHandle provider;
		if (variant.tryCast(provider))
		{
			source = provider.getBase<ITreeModel>();
		}
	}

	impl_->filteredModel_.setSource(source);
}

QObject* WGFilteredTreeModel::getFilter() const
{
	return impl_->filter_;
}

void WGFilteredTreeModel::setFilter(QObject* filter)
{
	auto wgFilter = qobject_cast<WGFilter*>(filter);
	impl_->setFilter(wgFilter);
}
} // end namespace wgt
