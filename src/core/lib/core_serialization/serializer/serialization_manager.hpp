#ifndef SERIALIZATION_MANAGER_HPP
#define SERIALIZATION_MANAGER_HPP

#include <string>
#include <unordered_map>
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
class DEPRECATED SerializationManager
	: public Implements< ISerializationManager >
{
public:
	SerializationManager();
	~SerializationManager();

	bool registerSerializer( const char * typeName, ISerializer * serializer ) override;
	void deregisterSerializer( const char * typeName ) override;

private:
	ISerializer * getSerializer( const TypeId & typeId ) override;
	typedef std::unordered_map< const TypeId , ISerializer * > SerializerMap;
	SerializerMap serializerMap_;
};
} // end namespace wgt
#endif // SERIALIZATION_MANAGER_HPP
