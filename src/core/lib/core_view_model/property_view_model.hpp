#ifndef PROPERTY_VIEW_MODEL_HPP
#define PROPERTY_VIEW_MODEL_HPP

#include "view_model.hpp"

#include <memory>

namespace wgt
{
class AbstractItemModel;
class IComponentContext;
class ObjectHandle;

class PropertyViewModel : public ViewModel
{
	DECLARE_REFLECTED

public:
	PropertyViewModel()
	{
	}
	virtual ~PropertyViewModel()
	{
	}

	virtual AbstractItemModel* model() const = 0;
	virtual void setObject(const ObjectHandle& object) = 0;

	// TODO: Pass in something more than just the property name
	virtual void currentPropertyChanged(std::string property){};
};

template <class T>
class PropertyViewModelT : public PropertyViewModel
{
public:
	PropertyViewModelT(IComponentContext& componentContext)
	{
		model_.reset(new T(componentContext));
	}

	virtual ~PropertyViewModelT()
	{
	}

	virtual AbstractItemModel* model() const override
	{
		return model_.get();
	}

	virtual void setObject(const ObjectHandle& object) override
	{
		model_->setObject(object);
	}

private:
	std::unique_ptr<T> model_;
};

template <class T>
std::shared_ptr<PropertyViewModel> CreatePropertyViewModel(IComponentContext& componentContext)
{
	return std::make_shared<PropertyViewModelT<T>>(componentContext);
}

} // end namespace wgt
#endif // VIEW_MODEL_HPP
