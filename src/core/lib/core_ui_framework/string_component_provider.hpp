#pragma once

#include "simple_component_provider.hpp"

namespace wgt
{
class StringComponentProvider : public SimpleComponentProvider
{
public:
	StringComponentProvider(const char* component) : SimpleComponentProvider(component), typeName_(component)
	{
	}

	StringComponentProvider(const char* component, const ItemRole::Id roles[], size_t count)
	    : SimpleComponentProvider(component, roles, count), typeName_(component)
	{
	}

	StringComponentProvider(const char* typeName, const char* component)
	    : SimpleComponentProvider(component), typeName_(typeName)
	{
	}

	StringComponentProvider(const char* typeName, const char* component, const ItemRole::Id roles[], size_t count)
	    : SimpleComponentProvider(component, roles, count), typeName_(typeName)
	{
	}

	const char* componentId(const TypeId& typeId, std::function<bool(const ItemRole::Id&)>& predicate) const override
	{
		if (typeId.getName() != typeName_)
		{
			return nullptr;
		}

		return SimpleComponentProvider::componentId(typeId, predicate);
	}

private:
	std::string typeName_;
};
}
