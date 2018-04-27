#include "reflected_list.hpp"

#include "core_reflection/property_accessor.hpp"
#include "core_reflection/property_accessor_listener.hpp"
#include "core_reflection/definition_manager.hpp"
#include "core_reflection/interfaces/i_class_definition_details.hpp"
#include "core_data_model/i_item_role.hpp"

namespace wgt
{
class ReflectedListListener : public PropertyAccessorListener
{
public:
	ReflectedListListener(const ReflectedList& list) : list_(list)
	{
	}

	// PropertyAccessorListener
	void preSetValue(const PropertyAccessor& accessor, const Variant& value) override;
	void postSetValue(const PropertyAccessor& accessor, const Variant& value) override;

private:
	size_t findIndex(const PropertyAccessor& accessor);

	const ReflectedList& list_;
};

ReflectedList::ReflectedList(IDefinitionManager* defManager)
    : listener_(new ReflectedListListener(*this)), defManager_(defManager)
{
	setSource(Collection(data_));
	defManager_->registerPropertyAccessorListener(listener_);
}

ReflectedList::~ReflectedList()
{
	defManager_->deregisterPropertyAccessorListener(listener_);
}

size_t ReflectedListListener::findIndex(const PropertyAccessor& accessor)
{
	const Variant obj = accessor.getRootObject();
	const Collection& collection = list_.getSource();
	const auto data = collection.container<std::vector<Variant>>();

	auto callback = [&](const Variant& item) { return obj == item; };

	auto it = std::find_if(data->cbegin(), data->cend(), callback);
	return (it != data->cend()) ? it - data->cbegin() : -1;
}

void ReflectedListListener::preSetValue(const PropertyAccessor& accessor, const Variant& value)
{
	if (auto row = findIndex(accessor))
	{
		// int row, int column, ItemRole::Id role, const Variant& newValue
		list_.preItemDataChanged_(int(row), 0, DefinitionRole::roleId_, value);
	}
}

void ReflectedListListener::postSetValue(const PropertyAccessor& accessor, const Variant& value)
{
	if (auto row = findIndex(accessor))
	{
		list_.postItemDataChanged_(int(row), 0, DefinitionRole::roleId_, value);
	}
}
} // end namespace wgt
