#include "serialization_manager.hpp"
#include "core_serialization/serializer/i_serializer.hpp"

namespace wgt
{
class SerializationManager::Impl
{
public:
	typedef std::unordered_map<const TypeId, ISerializer*> SerializerMap;
	SerializerMap serializerMap_;
};

SerializationManager::SerializationManager() : serializerMapImpl_(new SerializationManager::Impl())
{
}

SerializationManager::~SerializationManager()
{
	// nop
}

bool SerializationManager::registerSerializer(const char* typeName, ISerializer* serializer)
{
	bool br = false;
	TypeId id(typeName);
	auto findIt = serializerMapImpl_->serializerMap_.find(id);
	if (findIt == serializerMapImpl_->serializerMap_.end())
	{
		serializerMapImpl_->serializerMap_[id] = serializer;
		br = true;
	}
	assert(br);
	return br;
}

void SerializationManager::deregisterSerializer(const char* typeName)
{
	TypeId id(typeName);
	auto findIt = serializerMapImpl_->serializerMap_.find(id);
	if (findIt != serializerMapImpl_->serializerMap_.end())
	{
		serializerMapImpl_->serializerMap_.erase(findIt);
	}
}

ISerializer* SerializationManager::getSerializer(const TypeId& typeId)
{
	ISerializer* ret = nullptr;
	auto findIt = serializerMapImpl_->serializerMap_.find(typeId);
	if (findIt != serializerMapImpl_->serializerMap_.end())
	{
		ret = findIt->second;
	}
	return ret;
}
} // end namespace wgt
