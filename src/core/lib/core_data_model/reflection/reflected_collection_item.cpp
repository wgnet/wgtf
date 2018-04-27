#include "reflected_collection_item.hpp"
#include "wg_types/hash_utilities.hpp"
#include "core_reflection/metadata/meta_utilities.hpp"
#include "core_reflection/metadata/meta_impl.hpp"

#include <sstream>

namespace wgt
{
ReflectedIteratorProperty::ReflectedIteratorProperty(std::string&& name_, const TypeId& type_)
    : name(std::move(name_)), nameHash(HashUtilities::compute(name)), type(std::move(type_))
{
}

bool ReflectedIteratorProperty::readOnly(const ObjectHandle&) const
{
	return true;
}

bool ReflectedIteratorProperty::isValue() const
{
	return true;
}

bool ReflectedIteratorProperty::set(const ObjectHandle& handle, const Variant& value,
                                    const IDefinitionManager& definitionManager) const
{
	ReflectedIteratorValue* iterValue = handle.getBase<ReflectedIteratorValue>();
	if (iterValue == nullptr)
		return false;

	bool result = iterValue->iterator.setValue(value);
	iterValue->value = iterValue->iterator.value();
	return result;
}

Variant ReflectedIteratorProperty::get(const ObjectHandle& handle, const IDefinitionManager& definitionManager) const
{
	ReflectedIteratorValue* iterValue = handle.getBase<ReflectedIteratorValue>();
	if (iterValue == nullptr)
		return Variant();

	return iterValue->value;
}

const TypeId& ReflectedIteratorProperty::getType() const
{
	return type;
}

const char* ReflectedIteratorProperty::getName() const
{
	return name.c_str();
}

uint64_t ReflectedIteratorProperty::getNameHash() const
{
	return nameHash;
}

const MetaData & ReflectedIteratorProperty::getMetaData() const
{
	static MetaData s_Data;
	return s_Data;
}

bool ReflectedIteratorProperty::isMethod() const
{
	return false;
}

bool ReflectedIteratorProperty::isCollection() const
{
	return false;
}

Variant ReflectedIteratorProperty::invoke(const ObjectHandle&, const IDefinitionManager&,
                                          const ReflectedMethodParameters&)
{
	return Variant();
}

size_t ReflectedIteratorProperty::parameterCount() const
{
	return 0;
}

std::string BuildIteratorPropertyName(const Collection::Iterator& iterator)
{
	std::stringstream ss;
	ss << Collection::getIndexOpen();
	std::string keyName;
	iterator.key().tryCast(keyName);
	ss << keyName;
	ss << Collection::getIndexClose();

	return ss.str();
}

} // namespace wgt
