#pragma once
#ifndef TYPE_CONVERTER_QUEUE_HPP
#define TYPE_CONVERTER_QUEUE_HPP

#include <string>
#include <vector>

namespace wgt
{
class ObjectHandle;
class Variant;

template <typename ITypeConverter, typename ScriptType>
class TypeConverterQueue
{
public:
	TypeConverterQueue();

	/**
	 *	Add a type converter to the list to be searched.
	 *
	 *	Search is performed from most-recently-added to first-added.
	 *
	 *	@pre converter must not already be added.
	 *
	 *	@param converter the converter to be added.
	 */
	void registerTypeConverter(ITypeConverter& converter);

	/**
	 *	Remove a type converter to the list to be searched.
	 *
	 *	@pre converter must have been added with registerTypeConverter().
	 *
	 *	@param converter the converter to be removed.
	 */
	void deregisterTypeConverter(ITypeConverter& converter);

	/**
	*	Convert the given Variant into a ScriptType by searching through the
	*	type converters.
	*
	*	Search is performed from most-recently-added to first-added.
	*
	*	@param inVariant the variant to be converted.
	*	@param outObject storage for the resulting object.
	*		Should not be modified if conversion fails.
	*	@param userData arbitrary userdata used by implementations
	*	@return true on success.
	*/
	template <typename UserDataType>
	bool toScriptType(const Variant& inVariant, ScriptType& outObject, UserDataType* userData = nullptr) const;

	/**
	 *	Convert the given ScriptType into a Variant by searching through the
	 *	type converters.
	 *
	 *	Search is performed from most-recently-added to first-added.
	 *
	 *	@param inObject the ScriptType to be converted.
	 *	@param outVariant storage for the resulting object.
	 *		Should not be modified if conversion fails.
	 *	@return true on success.
	 */
	bool toVariant(const ScriptType& inObject, Variant& outVariant) const;

	/**
	 *	Convert the given ScriptType into a Variant by searching through the
	 *	type converters.
	 *
	 *	Search is performed from most-recently-added to first-added.
	 *
	 *	@param inObject the ScriptType to be converted.
	 *	@param outVariant storage for the resulting object.
	 *		Should not be modified if conversion fails.
	 *	@param parentHandle holds a reference to the parent of inObject.
	 *	@param childPath the reflected property path from parentHandle
	 *		to the child object.
	 *		e.g. parentHandle has the path "root.child1.child2" and
	 *			inObject is "child3".
	 *	@return true on success.
	 */
	bool toVariantWithParent(const ScriptType& inObject, Variant& outVariant, const ObjectHandle& parentHandle,
	                         const std::string& childPath) const;

private:
	TypeConverterQueue(const TypeConverterQueue& other);
	TypeConverterQueue(TypeConverterQueue&& other);

	TypeConverterQueue& operator=(const TypeConverterQueue& other);
	TypeConverterQueue& operator=(TypeConverterQueue&& other);

	std::vector<ITypeConverter*> typeConverters_;
};

template <typename ITypeConverter, typename ScriptType>
TypeConverterQueue<ITypeConverter, ScriptType>::TypeConverterQueue()
{
}

template <typename ITypeConverter, typename ScriptType>
void TypeConverterQueue<ITypeConverter, ScriptType>::registerTypeConverter(ITypeConverter& converter)
{
	auto foundItr = std::find(typeConverters_.cbegin(), typeConverters_.cend(), &converter);
	const bool found = (foundItr != typeConverters_.cend());
	assert(!found);
	if (found)
	{
		return;
	}
	typeConverters_.push_back(&converter);
}

template <typename ITypeConverter, typename ScriptType>
void TypeConverterQueue<ITypeConverter, ScriptType>::deregisterTypeConverter(ITypeConverter& converter)
{
	auto foundItr = std::find(typeConverters_.cbegin(), typeConverters_.cend(), &converter);
	const bool found = (foundItr != typeConverters_.cend());
	assert(found);
	if (!found)
	{
		return;
	}
	typeConverters_.erase(foundItr);
}

template <typename ITypeConverter, typename ScriptType>
template <typename UserDataType>
bool TypeConverterQueue<ITypeConverter, ScriptType>::toScriptType(const Variant& inVariant, ScriptType& outObject,
                                                                  UserDataType* userData) const
{
	for (auto itr = typeConverters_.crbegin(); itr != typeConverters_.crend(); ++itr)
	{
		const auto& pTypeConverter = (*itr);
		assert(pTypeConverter != nullptr);
		if (pTypeConverter->toScriptType(inVariant, outObject, userData))
		{
			return true;
		}
	}

	return false;
}

template <typename ITypeConverter, typename ScriptType>
bool TypeConverterQueue<ITypeConverter, ScriptType>::toVariant(const ScriptType& inObject, Variant& outVariant) const
{
	for (auto itr = typeConverters_.crbegin(); itr != typeConverters_.crend(); ++itr)
	{
		const auto& pTypeConverter = (*itr);
		assert(pTypeConverter != nullptr);
		if (pTypeConverter->toVariant(inObject, outVariant))
		{
			return true;
		}
	}

	return false;
}

template <typename ITypeConverter, typename ScriptType>
bool TypeConverterQueue<ITypeConverter, ScriptType>::toVariantWithParent(const ScriptType& inObject,
                                                                         Variant& outVariant,
                                                                         const ObjectHandle& parentHandle,
                                                                         const std::string& childPath) const
{
	for (auto itr = typeConverters_.crbegin(); itr != typeConverters_.crend(); ++itr)
	{
		const auto& pTypeConverter = (*itr);
		assert(pTypeConverter != nullptr);
		if (pTypeConverter->toVariant(inObject, outVariant, parentHandle, childPath))
		{
			return true;
		}
	}

	return false;
}
} // end namespace wgt
#endif // TYPE_CONVERTER_QUEUE_HPP
