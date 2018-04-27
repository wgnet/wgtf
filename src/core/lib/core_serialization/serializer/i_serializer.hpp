#ifndef I_SERIALIZER_HPP
#define I_SERIALIZER_HPP

#include "core_variant/variant.hpp"
#include "core_common/deprecated.hpp"
#include "core_serialization/serialization_dll.hpp"
#include "core_object/managed_object.hpp"
#include <utility>

namespace wgt
{
class IDataStream;
class ISerializationManager;

/**
 * Serializer interface
 */

class SERIALIZATION_DLL ISerializer
{
public:
	virtual ~ISerializer();

	virtual bool serialize(const Variant& value);
	virtual bool deserialize(Variant& value);

	virtual bool serialize(const IManagedObject& object);
	virtual bool deserialize(IManagedObject& object);

	template<class T>
	bool serialize(const ManagedObject<T>& object)
	{
		return this->serialize(static_cast<const IManagedObject&>(object));
	}

	template<class T>
	bool deserialize(ManagedObject<T>& object)
	{
		return this->deserialize(static_cast<IManagedObject&>(object));
	}

	template <typename T>
	bool deserialize(T& value)
	{
		Variant tmp(std::move(value));

		if (!deserialize(tmp))
		{
			return false;
		}

		if (auto ptr = tmp.value<T*>())
		{
			value = std::move(*ptr);
		}
		else if (!tmp.tryCast(value))
		{
			return false;
		}

		return true;
	}

private:
	friend class ISerializationManager;

	DEPRECATED virtual bool write(IDataStream* dataStream, const Variant& variant);
	DEPRECATED virtual bool read(IDataStream* dataStream, Variant& variant);
};
} // end namespace wgt
#endif // I_SERIALIZER_HPP
