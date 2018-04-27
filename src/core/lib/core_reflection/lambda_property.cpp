#include "lambda_property.hpp"

#include "core_common/assert.hpp"

#include <utility>

namespace wgt
{
LambdaProperty::LambdaProperty(const char* name, const TypeId& type, Getter getter, Setter setter, MetaData metaData,
                               bool isCollection)
    : base(name, type), getter_(std::move(getter)), setter_(std::move(setter)), metaData_(std::move(metaData)),
      isCollection_(isCollection)
{
}

const MetaData & LambdaProperty::getMetaData() const
{
	return metaData_;
}

bool LambdaProperty::readOnly(const ObjectHandle&) const
{
	return !setter_;
}

bool LambdaProperty::isMethod() const
{
	return false;
}

bool LambdaProperty::isValue() const
{
	return true;
}

bool LambdaProperty::isCollection() const
{
	return isCollection_;
}

bool LambdaProperty::isByReference() const
{
	return std::is_reference<Getter::result_type>::value;
}

bool LambdaProperty::set(const ObjectHandle& handle, const Variant& value,
                         const IDefinitionManager& definitionManager) const
{
	if (!setter_)
	{
		return false;
	}

	setter_(handle, value, definitionManager);
	return true;
}

Variant LambdaProperty::get(const ObjectHandle& handle, const IDefinitionManager& definitionManager) const
{
	TF_ASSERT(getter_);
	return getter_(handle, definitionManager);
}
}
