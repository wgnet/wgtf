#ifndef SERIALIZATION_MANAGER_HPP
#define SERIALIZATION_MANAGER_HPP

#include "../serialization_dll.hpp"

#include <string>
#include <unordered_map>
#include <memory>
#include "core_common/deprecated.hpp"
#include "core_serialization/serializer/i_serialization_manager.hpp"
#include "core_dependency_system/i_interface.hpp"

namespace wgt
{
class ISerializer;
class IDataStream;
/**
 * Serialization Manager
 */
class DEPRECATED SERIALIZATION_DLL SerializationManager : public Implements<ISerializationManager>
{
public:
	SerializationManager();
	~SerializationManager();

	bool registerSerializer(const char* typeName, ISerializer* serializer) override;
	void deregisterSerializer(const char* typeName) override;

private:
	ISerializer* getSerializer(const TypeId& typeId) override;

	class Impl;
	std::unique_ptr<Impl> serializerMapImpl_;
};
} // end namespace wgt
#endif // SERIALIZATION_MANAGER_HPP
