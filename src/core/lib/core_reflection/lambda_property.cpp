#include "lambda_property.hpp"
#include <utility>
#include <cassert>

namespace wgt
{
LambdaProperty::LambdaProperty(
const char* name,
const TypeId& type,
Getter getter,
Setter setter,
MetaHandle metaData)
    :

    base(name, type)
    ,
    getter_(std::move(getter))
    ,
    setter_(std::move(setter))
    ,
    metaData_(std::move(metaData))
{
}

    MetaHandle LambdaProperty::getMetaData() const
	{
	    return metaData_;
    }

	bool LambdaProperty::readOnly() const
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

	bool LambdaProperty::set(
		const ObjectHandle & handle,
		const Variant & value,
		const IDefinitionManager & definitionManager ) const
	{
		if(!setter_)
		{
			return false;
		}

		setter_(handle, value, definitionManager);
		return true;
	}

	Variant LambdaProperty::get(
		const ObjectHandle & handle,
		const IDefinitionManager & definitionManager ) const
	{
		assert(getter_);
		return getter_(handle, definitionManager);
	}
    }
