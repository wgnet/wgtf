#ifndef REFLECTED_METHOD_HPP
#define REFLECTED_METHOD_HPP

#include "base_property.hpp"
#include "core_variant/variant.hpp"

namespace wgt
{
class ReflectedMethod : public BaseProperty
{
public:
	explicit ReflectedMethod(const char* name) : BaseProperty(name, TypeId(""))
	{
	}

	bool isMethod() const
	{
		return true;
	}

	bool isCollection() const
	{
		return false;
	}

	virtual ReflectedMethod* getUndoMethod()
	{
		return nullptr;
	}

	virtual ReflectedMethod* getRedoMethod()
	{
		return nullptr;
	}

	typedef ReflectedMethod SelfType;
};
} // end namespace wgt
#endif // REFLECTED_METHOD_HPP
