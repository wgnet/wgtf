#include "alert_models.hpp"

#include "core_common/assert.hpp"
#include "core_data_model/collection_model.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_object/managed_object.hpp"
#include "core_dependency_system/depends.hpp"

namespace wgt
{
struct AlertObjectModel::Implementation
{
	Implementation(AlertObjectModel& self, const char* message);

	AlertObjectModel& self_;
	std::string message_;
};

AlertObjectModel::Implementation::Implementation(AlertObjectModel& self, const char* message)
    : self_(self)
    , message_(message)
{
}

AlertObjectModel::AlertObjectModel(const char* message)
    : impl_(new Implementation(*this, message))
{
}

const char* AlertObjectModel::getMessage() const
{
	return impl_->message_.c_str();
}

struct AlertPageModel::Implementation : Depends<IDefinitionManager>
{
	Implementation(AlertPageModel& self);

	AlertPageModel& self_;
    std::vector<Variant> alerts_;
	std::vector<ManagedObjectPtr> alertObjs_;
	CollectionModel alertsModel_;
	int currentSelectedRowIndex_;
};

AlertPageModel::Implementation::Implementation(AlertPageModel& self)
    : self_(self)
    , currentSelectedRowIndex_(-1)
{
}

AlertPageModel::AlertPageModel() 
    : impl_(new Implementation(*this))
{
	impl_->alertsModel_.setSource(Collection(impl_->alerts_));
}

void AlertPageModel::addAlert(const char* message)
{
    impl_->alertObjs_.push_back(ManagedObject<AlertObjectModel>::make_unique(message));
	Collection& collection = impl_->alertsModel_.getSource();
	collection.insertValue(collection.size(), impl_->alertObjs_.back()->getHandle());
}

const AbstractListModel* AlertPageModel::getAlerts() const
{
	return &impl_->alertsModel_;
}

ObjectHandle AlertPageModel::removeAlert() const
{
	if (impl_->currentSelectedRowIndex_ == -1)
	{
		return nullptr;
	}

	const int selectedIndex = impl_->currentSelectedRowIndex_;

    Collection& collection = impl_->alertsModel_.getSource();
    TF_ASSERT(selectedIndex < (int)collection.size());
    TF_ASSERT(collection.size() == impl_->alertObjs_.size());
    TF_ASSERT(collection.size() == impl_->alerts_.size());

    auto collectionItr = collection.begin();
    std::advance(collectionItr, selectedIndex);
    collection.erase(collectionItr);

    auto objIter = impl_->alertObjs_.begin();
    std::advance(objIter, selectedIndex);
    impl_->alertObjs_.erase(objIter);

    TF_ASSERT(collection.size() == impl_->alertObjs_.size());
    TF_ASSERT(collection.size() == impl_->alerts_.size());
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
