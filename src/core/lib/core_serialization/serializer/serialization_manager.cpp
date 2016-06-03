#include "serialization_manager.hpp"
#include "core_serialization/serializer/i_serializer.hpp"

namespace wgt
{
SerializationManager::SerializationManager()
{
	serializerMap_.clear();
}

SerializationManager::~SerializationManager()
{
	serializerMap_.clear();
}

bool SerializationManager::registerSerializer( const char * typeName, ISerializer * serializer )
{
	bool br = false;
	TypeId id(typeName);
	auto findIt = serializerMap_.find( id );
	if (findIt == serializerMap_.end())
	{
		serializerMap_[id] = serializer;
		br = true;
	}
	assert( br );
	return br;
}

void SerializationManager::deregisterSerializer( const char * typeName )
{
	TypeId id(typeName);
	auto findIt = serializerMap_.find( id );
	if (findIt != serializerMap_.end())
	{
		serializerMap_.erase( findIt );
	}
}

ISerializer * SerializationManager::getSerializer( const TypeId & typeId )
{
	ISerializer * ret = nullptr;
	auto findIt = serializerMap_.find( typeId );
	if (findIt != serializerMap_.end())
	{
		ret =  findIt->second;
	}
	return ret;
}
} // end namespace wgt
