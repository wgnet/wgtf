#include "xml_reader.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_reflection/interfaces/i_base_property.hpp"
#include "core_variant/collection.hpp"
#include "core_serialization/fixed_memory_stream.hpp"
#include "core_serialization/text_stream.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/i_object_manager.hpp"
#include <utility>
#include <cstring>
#include <cassert>


namespace wgt
{
XMLReader::StackItem::StackItem( Variant value ):
	value( std::move( value ) ),
	object(),
	collection( nullptr ),
	property( nullptr ),
	pos(),
	characterData(),
	hasChildren( false ),
	assumedKey( 0 ),
	objectId( RefObjectId::zero() ),
	needResolve( false )
{
}


void XMLReader::StackItem::cast( IDefinitionManager& definitionManager )
{
	object = ObjectHandle();
	collection = nullptr;

	if( auto v = value.castPtr< ObjectHandle >() )
	{
		object = *v;
	}
	else if( auto v = value.castPtr< Collection >() )
	{
		collection = v;
	}
	else
	{
		if( auto definition = definitionManager.getDefinition( value.type()->typeId().getName() ) )
		{
			object = ObjectHandle( &value, definition );
		}
	}
}


XMLReader::XMLReader( TextStream& stream, IDefinitionManager& definitionManager, const XMLSerializer::Format& format ):
	base( stream ),
	definitionManager_( definitionManager ),
	format_( format ),
	stack_(),
	pushed_( false ),
	done_( false ),
	ignore_( 0 )
{
}


bool XMLReader::read( Variant& value )
{
	stack_.emplace_back( value );
	pushed_ = true;
	done_ = false;

	if( !parse() && !done_ )
	{
		return false;
	}

	value = std::move( stack_.back().value );
	stack_.pop_back();
	return true;
}


void XMLReader::elementStart( const char* elementName, const char* const* attributes )
{
	if( ignore_ )
	{
		ignore_ += 1;
		return;
	}

	// parse attributes
	const char* type = nullptr;
	const char* objectId = nullptr;
	const char* objectReference = nullptr;
	const char* propertyName = nullptr;
	const char* keyType = nullptr;
	const char* key = nullptr;

	for( auto attribute = attributes; *attribute; attribute += 2 )
	{
		const char* attributeName = attribute[0];
		const char* attributeValue = attribute[1];

		if( attributeName == format_.typeAttribute )
		{
			type = attributeValue;
		}
		else if( attributeName == format_.objectIdAttribute)
		{
			objectId = attributeValue;
		}
		else if( attributeName == format_.objectReferenceAttribute)
		{
			objectReference = attributeValue;
		}
		else if( attributeName == format_.propertyNameAttribute)
		{
			propertyName = attributeValue;
		}
		else if( attributeName == format_.keyTypeAttribute )
		{
			keyType = attributeValue;
		}
		else if( attributeName == format_.keyAttribute )
		{
			key = attributeValue;
		}
		else
		{
			// ignore unknown attributes
		}
	}

	// prepare stack top item
	assert( !stack_.empty() );

	if( pushed_ )
	{
		// asusme that value for the current element is already on the stack
		pushed_ = false;
	}
	else
	{
		auto& current = stack_.back();
		current.hasChildren = true;
		current.characterData.clear();

		if( current.object.storage() )
		{
			// find and push property
			const IClassDefinition* classDefinition = current.object.getDefinition( definitionManager_ );
			if( !classDefinition )
			{
				// type is not reflected
				abortParsing();
				return;
			}
			assert( propertyName != nullptr );
			IBasePropertyPtr property = classDefinition->findProperty( propertyName );
			if( !property )
			{
				// ignore unknown properties
				ignore_ = 1;
				return;
			}

			if( property->isMethod() )
			{
				// ignore method properties
				ignore_ = 1;
				return;
			}

			stack_.emplace_back( property->get( current.object, definitionManager_ ) );
			stack_.back().property = property;
		}
		else if( current.collection && current.collection->isValid() )
		{
			if( elementName != format_.collectionItemElement )
			{
				// ignore non-item properties
				ignore_ = 1;
				return;
			}

			Variant k;
			if( key )
			{
				k = key;

				if( keyType )
				{
					const MetaType* keyMetaType = Variant::findType( keyType );
					if( !keyMetaType )
					{
						// key type not found
						abortParsing();
						return;
					}

					if( !k.convert( keyMetaType ) )
					{
						// key type conversion failed
						abortParsing();
						return;
					}
				}

				intmax_t keyIndex = 0;
				if( k.tryCast( keyIndex) )
				{
					current.assumedKey = keyIndex;
				}
			}
			else
			{
				k = current.assumedKey;
			}

			current.assumedKey += 1;
			auto findIt = current.collection->find( k );
			if (findIt == current.collection->end())
			{
				findIt = current.collection->insert( k );
			}

			stack_.emplace_back( findIt.value() );
			stack_.back().pos = findIt;
		}
		else
		{
			// This may occur if we didn't find ClassDefinition for parent
			// element and assumed parent to be primitive value. So consider
			// this situation as an error.
			abortParsing();
			return;
		}
	}

	auto& current = stack_.back();
	if (objectId != nullptr)
	{
		current.objectId = objectId;
	}
	if (objectReference != nullptr)
	{
		current.needResolve = true;
	}
	// check type
	if( type )
	{
		bool isEmpty = current.value.isVoid();
		if( !isEmpty )
		{
			if( auto object = current.value.castPtr< ObjectHandle >() )
			{
				if( !object->isValid() )
				{
					isEmpty = true;
				}
			}
		}

		if( isEmpty )
		{
			// set type
			if( IClassDefinition* classDefinition = definitionManager_.getDefinition( type ) )
			{
				if (objectId == nullptr)
				{
					current.value = classDefinition->create();
				}
				else
				{
					auto objectManager = definitionManager_.getObjectManager();
					current.value = objectManager->createObject( RefObjectId(objectId), type );
				}
			}
			else if( const MetaType* metaType = Variant::findType( type ) )
			{
				current.value = Variant( metaType );
			}
			else
			{
				// type not found
				abortParsing();
				return;
			}
			current.cast( definitionManager_ );
		}
		else
		{
			// deduce actual underlying type
			current.cast( definitionManager_ );
			const char* actualType = current.value.type()->name();
			if( current.object.storage() )
			{
				if( const IClassDefinition* definition = current.object.getDefinition( definitionManager_ ) )
				{
					actualType = definition->getName();
				}
			}

			if( strcmp( actualType, type ) != 0 )
			{
				// types do not match
				abortParsing();
				return;
			}
		}
	}
	else
	{
		current.cast( definitionManager_ );
	}
}


void XMLReader::elementEnd( const char* elementName )
{
	if( ignore_ )
	{
		ignore_ -= 1;
		return;
	}

	assert( !stack_.empty() );

	// parse character data
	auto& current = stack_.back();
	if( !current.characterData.empty() )
	{
		if( !current.value.castPtr< Collection >() )
		{
			FixedMemoryStream dataStream( current.characterData.c_str(), current.characterData.size() );
			TextStream stream( dataStream );
			stream >> current.value;
			stream.skipWhiteSpace();

			if( stream.fail() || !stream.eof() )
			{
				// failed to deserialize primitive value
				abortParsing();
				return;
			}
		}
	}

	if( stack_.size() <= 1 )
	{
		// this seems to be a root value: we're done
		done_ = true;
		abortParsing();
		return;
	}

	// move current element to its parent
	auto& parent = *std::prev( stack_.end(), 2 );
	if( parent.object.storage() )
	{
		assert( current.property );
		if (current.needResolve)
		{
			auto objectManager = definitionManager_.getObjectManager();
			objectManager->addObjectLinks( current.objectId, current.property, parent.object );
		}
		else
		{
			current.property->set( parent.object, std::move( current.value ), definitionManager_ );
		}
		
		stack_.pop_back();
	}
	else if( parent.value.castPtr< Collection >() )
	{
		current.pos.setValue( current.value );
		stack_.pop_back();
	}
	else
	{
		assert( false ); // we shouldn't ever get here
	}
}


void XMLReader::characterData( const char* data, size_t length )
{
	if( ignore_ )
	{
		return;
	}

	assert( !stack_.empty() );

	auto& current = stack_.back();

	if( current.object.storage() )
	{
		if( !current.hasChildren )
		{
			current.characterData.append( data, data + length );
		}
	}
	else if( current.value.castPtr< Collection >() )
	{
		// ignore character data in collection
	}
	else
	{
		current.characterData.append( data, data + length );
	}
}
} // end namespace wgt
