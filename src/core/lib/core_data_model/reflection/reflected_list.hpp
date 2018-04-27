#ifndef REFLECTED_LIST_HPP
#define REFLECTED_LIST_HPP

#include "core_data_model/collection_model.hpp"

namespace wgt
{
class IDefinitionManager;
class PropertyAccessorListener;

class ReflectedList : public CollectionModel
{
public:
	ReflectedList(IDefinitionManager* defManager);
	virtual ~ReflectedList();

private:
	std::shared_ptr<PropertyAccessorListener> listener_;
	IDefinitionManager* defManager_;
	std::vector<Variant> data_;

	friend class ReflectedListListener;
};
} // end namespace wgt
#endif // REFLECTED_LIST_HPP
