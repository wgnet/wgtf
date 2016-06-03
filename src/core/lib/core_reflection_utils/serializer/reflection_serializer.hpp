#ifndef REFLECTION_SERIALIZER_HPP
#define REFLECTION_SERIALIZER_HPP

#include "core_reflection/interfaces/i_class_definition.hpp"
#include "core_reflection/reflected_object.hpp"
#include "core_reflection/property_accessor.hpp"
#include "core_serialization/serializer/i_serializer.hpp"


namespace wgt
{
class ObjectHandle;

/**
 * reflected object Serializer
 */
class ReflectionSerializer : public ISerializer
{
public:
	ReflectionSerializer( ISerializationManager & serializationManager, 
						  IMetaTypeManager & metaTypeManager, 
						  IObjectManager & objManager,
						  IDefinitionManager & defManager );
	~ReflectionSerializer();

	std::vector<TypeId> getSupportedType();

private:
	bool write( 
		IDataStream * dataStream, const Variant & variant ) override;
	bool read( 
		IDataStream * dataStream, Variant & variant ) override;

	void writeProperties( const ObjectHandle & provider );
	void writeProperty( const PropertyAccessor & property );
	void writePropertyValue( const Variant & value );
	void writeCollection( const Collection & collection );

	void readProperties( const ObjectHandle & provider );
	void readProperty( const ObjectHandle & provider );
	void readPropertyValue( const char * valueType, PropertyAccessor & pa );
	void readCollection( const PropertyAccessor & prop );
	

	ISerializationManager & serializationManager_;
	IMetaTypeManager & metaTypeManager_;
	IObjectManager & objManager_;
	IDefinitionManager & defManager_;
	IDataStream * curDataStream_;
	typedef std::vector< std::pair< PropertyAccessor, RefObjectId > > ObjLinks;
	std::vector<TypeId> typeList;
	ObjLinks objLinks_;
};
} // end namespace wgt
#endif // REFLECTION_SERIALIZER_HPP
