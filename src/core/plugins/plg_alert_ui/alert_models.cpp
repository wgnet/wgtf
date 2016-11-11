#include "alert_models.hpp"
#include "core_data_model/variant_list.hpp"
#include "core_reflection/i_definition_manager.hpp"

namespace wgt
{
struct AlertObjectModel::Implementation
{
	Implementation(AlertObjectModel& self, const char* message);

	AlertObjectModel& self_;
	std::string message_;
};

AlertObjectModel::Implementation::Implementation(AlertObjectModel& self, const char* message)
    : self_(self), message_(message)
{
}

AlertObjectModel::AlertObjectModel() : impl_(new Implementation(*this, ""))
{
}

AlertObjectModel::AlertObjectModel(const AlertObjectModel& rhs)
    : impl_(new Implementation(*this, rhs.impl_->message_.c_str()))
{
}

AlertObjectModel::AlertObjectModel(const char* message) : impl_(new Implementation(*this, message))
{
}

AlertObjectModel::~AlertObjectModel()
{
}

void AlertObjectModel::init(const char* message)
{
	impl_->message_ = message;
}

const char* AlertObjectModel::getMessage() const
{
	return impl_->message_.c_str();
}

struct AlertPageModel::Implementation
{
	Implementation(AlertPageModel& self);

	AlertPageModel& self_;
	IDefinitionManager* definitionManager_;
	VariantList alerts_;
	int currentSelectedRowIndex_;
};

AlertPageModel::Implementation::Implementation(AlertPageModel& self)
    : self_(self), definitionManager_(nullptr), currentSelectedRowIndex_(-1)
{
}

AlertPageModel::AlertPageModel() : impl_(new Implementation(*this))
{
}

AlertPageModel::AlertPageModel(const AlertPageModel& rhs) : impl_(new Implementation(*this))
{
	impl_->definitionManager_ = rhs.impl_->definitionManager_;

	for (auto itr = rhs.impl_->alerts_.cbegin(); itr != rhs.impl_->alerts_.cend(); ++itr)
	{
		impl_->alerts_.push_back(*itr);
	}
}

AlertPageModel::~AlertPageModel()
{
}

void AlertPageModel::init(IComponentContext& contextManager)
{
	impl_->definitionManager_ = contextManager.queryInterface<IDefinitionManager>();
	assert(impl_->definitionManager_ != nullptr);
}

void AlertPageModel::addAlert(const char* message)
{
	ObjectHandle object = impl_->definitionManager_->create<AlertObjectModel>(false);

	object.getBase<AlertObjectModel>()->init(message);

	impl_->alerts_.push_back(object);
}

const IListModel* AlertPageModel::getAlerts() const
{
	return &impl_->alerts_;
}

ObjectHandle AlertPageModel::removeAlert() const
{
	if (impl_->currentSelectedRowIndex_ == -1)
	{
		return nullptr;
	}

	int selectedIndex = impl_->currentSelectedRowIndex_;

	auto selectedItem = impl_->alerts_[selectedIndex];

	for (auto alertIter = impl_->alerts_.begin(); alertIter != impl_->alerts_.end(); alertIter++)
	{
		auto tempItem = *alertIter;
		if (tempItem == selectedItem)
		{
			impl_->alerts_.erase(alertIter);
			break;
		}
	}

	return nullptr;
}

const int& AlertPageModel::currentSelectedRowIndex() const
{
	return impl_->currentSelectedRowIndex_;
}

void AlertPageModel::currentSelectedRowIndex(const int& index)
{
	impl_->currentSelectedRowIndex_ = index;
}
} // end namespace wgt
