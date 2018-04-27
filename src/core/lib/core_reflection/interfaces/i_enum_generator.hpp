#ifndef I_ENUM_GENERATOR_HPP
#define I_ENUM_GENERATOR_HPP

#include <map>
#include <memory>
#include "../reflection_dll.hpp"
#include "core_variant/collection.hpp"
#include "core_reflection/object_handle.hpp"

namespace wgt
{
class IDefinitionManager;
class ObjectHandle;

class REFLECTION_DLL IEnumGenerator
{
public:
	virtual ~IEnumGenerator()
	{
	}
	virtual Collection getCollection(const ObjectHandle& provider) = 0;
};

template <class T, bool isEnum = std::is_enum<T>::value>
class EnumGenerator : public IEnumGenerator
{
public:
	typedef void (T::*DeprecatedFunction)(std::map<int, std::wstring>*) const;
	typedef void (T::*Function)(std::map<int, Variant>*) const;

	EnumGenerator(const Function& func) 
		: func_(func)
	{
	}

	virtual Collection getCollection(const ObjectHandle& provider) override
	{
		auto object = provider.getBase<T>();
		if(func_ != nullptr)
		{
			(object->*func_)(&collection_);
			return Collection(collection_);
		}
		return Collection();
	}

private:
	Function func_;
	std::map<int, Variant> collection_;
};

template <class TEnum>
class EnumGenerator<TEnum, true> : public IEnumGenerator
{
public:
	virtual Collection getCollection(const ObjectHandle&) override
	{
		return Collection(enums_);
	}

private:
	static const std::map<typename std::underlying_type<TEnum>::type, std::string> enums_;
};

typedef std::unique_ptr<IEnumGenerator> IEnumGeneratorPtr;
} // end namespace wgt
#endif // I_ENUM_GENERATOR_HPP
