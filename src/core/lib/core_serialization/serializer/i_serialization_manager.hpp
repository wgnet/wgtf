#ifndef I_SERIALIZATION_MANAGER_HPP
#define I_SERIALIZATION_MANAGER_HPP

#include "../serialization_dll.hpp"

#include "core_variant/type_id.hpp"
#include "core_serialization/serializer/i_serializer.hpp"
#include "core_serialization/i_datastream.hpp"
#include "core_common/deprecated.hpp"

namespace wgt
{
class IDataStream;
class DEPRECATED SERIALIZATION_DLL ISerializationManager
{
public:
	virtual ~ISerializationManager()
	{
	}
	virtual bool registerSerializer(const char* typeName, ISerializer* serializer) = 0;
	virtual void deregisterSerializer(const char* typeName) = 0;

	bool serialize(IDataStream& dataStream, const Variant& variant)
	{
		bool br = false;
		ISerializer* serializer = getSerializer(variant.type()->name());
		if (serializer)
		{
			br = serializer->write(&dataStream, variant);
		}
		else
		{
			br = dataStream.write(variant);
		}
		return br;
	}

	bool deserialize(IDataStream& dataStream, Variant& variant)
	{
		bool br = false;
		ISerializer* serializer = getSerializer(variant.type()->name());
		if (serializer)
		{
			br = serializer->read(&dataStream, variant);
		}
		else
		{
			br = dataStream.read(variant);
		}
		return br;
	}

private:
	virtual ISerializer* getSerializer(const TypeId& typeId) = 0;
};
} // end namespace wgt
#endif // I_SERIALIZATION_MANAGER_HPP
