#include <cstdio>
#include "reflection_serializer.hpp"
#include "core_serialization/i_datastream.hpp"
#include "core_reflection/i_object_manager.hpp"
#include "core_reflection/reflected_object.hpp"
#include "core_reflection/property_accessor.hpp"
#include "core_reflection/property_iterator.hpp"
#include "core_reflection/interfaces/i_base_property.hpp"
#include "core_command_system/i_command_manager.hpp"
#include "core_reflection/metadata/meta_utilities.hpp"
#include "core_reflection/metadata/meta_impl.hpp"
#include "core_serialization/serializer/i_serialization_manager.hpp"

namespace wgt
{
ReflectionSerializer::ReflectionSerializer( ISerializationManager & serializationManager , 
										    IObjectManager & objManager,
											IDefinitionManager & defManager )
	: serializationManager_( serializationManager )
	, objManager_( objManager )
	, defManager_( defManager )
	, curDataStream_( nullptr )
{
	typeList.push_back( TypeId::getType<ObjectHandle>() );
}

ReflectionSerializer::~ReflectionSerializer()
{
	curDataStream_ = nullptr;
	typeList.clear();
}

std::vector<TypeId> ReflectionSerializer::getSupportedType()
{
	return typeList;
}

bool ReflectionSerializer::write( IDataStream * dataStream, const Variant & variant )
{
	TypeId type( variant.type()->name() );
	curDataStream_ = dataStream;
	if(!curDataStream_)
	{
		return false;
	}

	if(variant.canCast<ObjectHandle>())
	{
		ObjectHandle provider;
		bool isOk = variant.tryCast( provider );
		assert( isOk );
		if(provider.isValid())
		{
			provider = reflectedRoot( provider, defManager_ );
			const auto classDef = provider.getDefinition( defManager_ );
			if (classDef == nullptr)
			{
				curDataStream_->write("");
				return true;
			}
			curDataStream_->write( classDef->getName() );
			std::string stringId = "";
			RefObjectId id;
			isOk = provider.getId( id );
			if (isOk)
			{
				stringId = id.toString();
			}
			curDataStream_->write( stringId );
			writeProperties( provider );
			return true;
		}
		else
		{
			// write empty string for classdefinition name to handle nullptr
			curDataStream_->write( "" );
			return true;
		}
	}
	else
	{
		assert( false );
	}
	return false;
}

void ReflectionSerializer::writeProperties( const ObjectHandle & provider )
{
	const auto classDef = provider.getDefinition( defManager_ );
	assert( classDef );
	const PropertyIteratorRange& props = classDef->allProperties();
	std::vector< PropertyAccessor > pas;
	for (PropertyIterator pi = props.begin(), end = props.end(); 
		pi != end; ++pi)
	{
		if (pi->isMethod())
		{
			continue;
		}
		PropertyAccessor pa = classDef->bindProperty( 
			pi->getName(), provider );
		assert( pa.isValid() );
		auto metaData = findFirstMetaData<MetaNoSerializationObj>( pa, defManager_ );
		if(metaData != nullptr)
		{
			continue;
		}
		pas.push_back( pa );
	}
	size_t count = pas.size();
	curDataStream_->write( count );
	for(auto & pa : pas)
	{
		writeProperty( pa );
	}
}

void ReflectionSerializer::writeProperty( const PropertyAccessor & property )
{
	assert( property.isValid() );
	const auto & propPath = property.getFullPath();
	curDataStream_->write( propPath );

	Variant value = property.getValue();
	curDataStream_->write( value.type()->name() );
	if(!value.isVoid())
	{
		writePropertyValue( value );
	}
	
}

void ReflectionSerializer::writeCollection( const Collection & collection )
{
	size_t count = collection.size();
	curDataStream_->write( count );
	for (auto it = collection.begin(), end = collection.end();
		it != end; ++it )
	{
		unsigned int index;
		it.key().tryCast( index );
		//! TODO! how to write the key if we don't know its type?
		
		std::string strIndex = std::to_string(index);
		curDataStream_->write( strIndex );

		Collection subCollection;
		auto value = it.value();
		curDataStream_->write( value.type()->name() );
		writePropertyValue( value );
	}
}

void ReflectionSerializer::writePropertyValue( const Variant & value )
{
	if( value.typeIs< Collection >() )
	{
		Collection collection;
		bool isCollection = value.tryCast( collection );
		assert( isCollection );
		writeCollection( collection );
	}
	else
	{
		bool hasId = value.typeIs< ObjectHandle >();
		if(hasId)
		{
			ObjectHandle provider;
			bool isOk = value.tryCast( provider );
			assert( isOk );
			if(provider.isValid())
			{
				RefObjectId id;
				hasId = provider.getId( id );
				if(hasId)
				{
					curDataStream_->write( id.toString() );
				}
				else
				{	
					hasId = false;
				}
			}
			else
			{
				hasId = false;
			}
		}
		if (!hasId )
		{
			curDataStream_->write( "" );
			serializationManager_.serialize( *curDataStream_, value );
		}
	}
}

bool ReflectionSerializer::read( IDataStream * dataStream, Variant & variant )
{
	curDataStream_ = dataStream;
	if(!curDataStream_)
	{
		return false;
	}
	
	ObjectHandle provider;
	if(!variant.tryCast( provider ))
	{
		assert( false );
		return false;
	}
	std::string classDefName;
	curDataStream_->read( classDefName );
	if(!classDefName.empty())
	{
		std::string id;
		curDataStream_->read( id );
		if(id.empty())
		{
			if(provider.isValid())
			{
				readProperties( provider );
			}
			else
			{
				auto polyStruct = objManager_.create( classDefName.c_str() );
				readProperties( polyStruct );
				variant = polyStruct;
			}
		}
		else
		{
			auto object = 
				objManager_.createObject( RefObjectId( id.c_str() ), classDefName.c_str() );
			readProperties( object );
			variant = object;
		}
	}
	return true;
}

void ReflectionSerializer::readProperties( const ObjectHandle & provider )
{
	assert( provider.isValid() );
	size_t count = 0;
	curDataStream_->read( count );
	for(size_t i = 0; i < count; i++)
	{
		readProperty( provider );
	}
}

void ReflectionSerializer::readProperty( const ObjectHandle & provider )
{
	assert( provider.isValid() );
	std::string propName;
	curDataStream_->read( propName );
	
	PropertyAccessor prop = provider.getDefinition( defManager_ )->bindProperty(
		propName.c_str(), provider );
	assert( prop.isValid() );

	std::string valueType;
	curDataStream_->read( valueType );
	const MetaType * metaType = Variant::findType( valueType.c_str() );
	if(metaType == nullptr)
	{
		assert( false );
		return;
	}
	readPropertyValue( valueType.c_str(), prop );
}

void ReflectionSerializer::readCollection( const PropertyAccessor & prop )
{
	assert( prop.isValid() );
	ObjectHandle baseProvider = prop.getRootObject();
	size_t count = 0;
	curDataStream_->read( count );

	Collection collection;
	bool isCollection = prop.getValue().tryCast( collection );
	assert( isCollection );
	if (!collection.empty())
	{
		collection.erase(collection.begin(), collection.end());
	}
	
	std::string strIndex;
	std::string propName;
	std::string valueType;
	for (size_t i = 0; i < count; i++)
	{
		strIndex.clear();
		curDataStream_->read( strIndex );
		int index = atoi( strIndex.c_str() );
		auto it = collection.insert( index );
		
		propName = prop.getFullPath();
		propName += '[';
		propName += strIndex;
		propName += ']';

		//TODO: Allow iteration to next element in collection.
		PropertyAccessor pa = baseProvider.getDefinition( defManager_ )->bindProperty( 
			propName.c_str(), baseProvider);
		assert( pa.isValid() );
		valueType.clear();
		curDataStream_->read( valueType );
		const MetaType * metaType = Variant::findType( valueType.c_str() );
		if(metaType == nullptr)
		{
			assert( false );
			return;
		}
		readPropertyValue( valueType.c_str(), pa );
	}
}

void ReflectionSerializer::readPropertyValue( const char * valueType, PropertyAccessor & pa )
{
	const MetaType * metaType = Variant::findType( valueType );
	if(metaType == nullptr)
	{
		assert( false );
		return;
	}
	Variant value = pa.getValue();
	
	if (value.typeIs< Collection >())
	{
		readCollection( pa );
		return;
	}
	std::string id;
	curDataStream_->read( id );
	if (ReflectionUtilities::isStruct( pa ))
	{
		read( curDataStream_, value );
	}
	else
	{
		if (!id.empty())
		{
			auto obj = objManager_.getObject( id );
			if(obj == nullptr)
			{
				objManager_.addObjectLinks( id, pa.getProperty(), pa.getRootObject() );
			}
			else
			{
				pa.setValueWithoutNotification( obj );
			}
		}
		else
		{
			Variant variant( metaType );
			if(!variant.isVoid())
			{
				serializationManager_.deserialize( *curDataStream_, variant );
				pa.setValueWithoutNotification( variant );
			}
		}
	}
}
} // end namespace wgt
