#include "xml_writer.hpp"
#include "core_serialization/text_stream_manip.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_reflection/interfaces/i_base_property.hpp"
#include "core_variant/variant.hpp"
#include "core_variant/collection.hpp"
#include "wg_types/binary_block.hpp"
#include "wg_types/vector2.hpp"
#include "wg_types/vector3.hpp"
#include "wg_types/vector4.hpp"


namespace wgt
{
namespace
{

	const char* valueType( const Variant& value, IDefinitionManager& definitionManager )
	{
		const char* result = value.type()->name();

		value.visit<ObjectHandle>( [&]( const ObjectHandle& v )
		{
			if( const IClassDefinition* definition = v.getDefinition( definitionManager ) )
			{
				result = definition->getName();
			}
		} );

		return result;
	}

	bool writeTypeExplicitly( const TypeId& type )
	{
		return
			type == TypeId::getType<Variant>() ||
			type == TypeId::getType<ObjectHandle>() ||
            type == TypeId::getType<Vector2>() ||
            type == TypeId::getType<Vector3>() ||
            type == TypeId::getType<Vector4>() ||
			type == TypeId::getType<BinaryBlock>();
	}

}


XMLWriter::XMLWriter( TextStream& stream, IDefinitionManager& definitionManager, const XMLSerializer::Format& format ):
	stream_( stream ),
	definitionManager_( definitionManager ),
	format_( format ),
	indent_( format.indent ),
	tagOpening_( false ),
	hasChildElements_( false )
{
}


bool XMLWriter::write( const Variant& value )
{
	beginOpenTag( format_.rootName.c_str() );
	writeValue( value, true );
	closeTag( format_.rootName.c_str() );

	return !fail();
}


void XMLWriter::writeValue( const Variant& value, bool explicitType, bool isObjectReference /* = false*/ )
{
	value.visit<ObjectHandle>( [=]( const ObjectHandle& v )
	{
		if (isObjectReference)
		{
			RefObjectId id;
			bool isOk = v.getId(id);
			if (isOk)
			{
				writeAttribute( format_.objectIdAttribute, quoted( id.toString() ) );
				writeAttribute( format_.objectReferenceAttribute, quoted( "reference" ) );
				endOpenTag();
				return;
			}
		}
		writeObject( v, true );
	} ) ||
	value.visit<Collection>( [=]( const Collection& v )
	{
		writeCollection( v );
	} ) ||
	value.visit<BinaryBlock>( [=]( const BinaryBlock& v )
	{
		writeVariant( v, true );
	} ) ||
	value.visit<Variant>( [=]( const Variant& v )
	{
		if( auto definition = definitionManager_.getDefinition( v.type()->typeId().getName() ) )
		{
			writeObject( ObjectHandle( v, definition ), true );
		}
		else
		{
			writeVariant( v, explicitType );
		}
	} );
}

void XMLWriter::writeObject( const ObjectHandle& object, bool explicitType )
{
	if(!object.isValid())
	{
		// nullptr objecthandle
		return;
	}

	auto rootObject = reflectedRoot( object, definitionManager_ );

	const IClassDefinition* definition = rootObject.getDefinition( definitionManager_ );
	if( !definition )
	{
		stream_.setState( std::ios_base::failbit );
		return;
	}

	if( explicitType )
	{
		// actual object type
		writeAttribute( format_.typeAttribute, quoted( definition->getName() ) );
	}

	RefObjectId id;
	bool isOk = rootObject.getId( id );
	if (isOk)
	{
		writeAttribute( format_.objectIdAttribute, quoted( id.toString() ) );
	}

	for( IBasePropertyPtr property: definition->allProperties() )
	{
		if( property->isMethod() )
		{
			continue;
		}

		// write object property
		const char* propertyName = property->getName();

		beginOpenTag( format_.propertyName.c_str() );
		writeAttribute( format_.propertyNameAttribute, quoted( propertyName ) );
		writeValue(
			property->get( rootObject, definitionManager_ ),
			writeTypeExplicitly( property->getType() ), true );
		closeTag( format_.propertyName.c_str() );

		if( fail() )
		{
			return;
		}
	}
}


void XMLWriter::writeCollection( const Collection& collection )
{
	writeAttribute( format_.typeAttribute, quoted( Variant::findType<Collection>()->name() ) );

	intmax_t assumedKey = 0;
	for( auto it = collection.begin(), end = collection.end(); it != end; ++it, ++assumedKey )
	{
		const Variant key = it.key();

		beginOpenTag( format_.collectionItemElement.c_str() );

		// write key
		bool keyMatchesAssumed = false;
		intmax_t keyIndex = 0;
		if( key.tryCast( keyIndex ) )
		{
			if( keyIndex == assumedKey )
			{
				keyMatchesAssumed = true;
			}
			else
			{
				assumedKey = keyIndex; // assumedKey tracks actual key
			}
		}

		if( !keyMatchesAssumed )
		{
			std::string keyValue;
			if( !key.type()->testFlags( MetaType::DeducibleFromText ) )
			{
				writeAttribute( format_.keyTypeAttribute, quoted( valueType( key, definitionManager_ ) ) );
				if( !key.tryCast( keyValue ) )
				{
					// arbitrary type can be saved in attribute only as string
					stream_.setState( std::ios_base::failbit );
					return;
				}
			}
			else
			{
				if( !key.tryCast( keyValue ) )
				{
					// arbitrary type can be saved in attribute only as string
					stream_.setState( std::ios_base::failbit );
					return;
				}
			}
			writeAttribute( format_.keyAttribute, quoted( keyValue ) );
		}

		// write value
		writeValue( it.value(), writeTypeExplicitly( collection.valueType() ), true );

		closeTag( format_.collectionItemElement.c_str() );

		if( fail() )
		{
			return;
		}
	}
}


void XMLWriter::writeVariant( const Variant& variant, bool explicitType )
{
	if( variant.isVoid() )
	{
		// nop
		return;
	}

	if( explicitType &&
		!variant.type()->testFlags( MetaType::DeducibleFromText ) )
	{
		writeAttribute( format_.typeAttribute, quoted( variant.type()->name() ) );
	}

	endOpenTag();

	stream_ << format_.padding << variant << format_.padding;
}


void XMLWriter::writeIndent()
{
	if( !format_.indentString.empty() )
	{
		for( unsigned i = 0; i < indent_; i++ )
		{
			stream_ << format_.indentString;
		}
	}
}


void XMLWriter::writeNewline()
{
	if( format_.newLines )
	{
		stream_ << "\n";
	}
}


void XMLWriter::beginOpenTag( const char* tag )
{
	if( tagOpening_ )
	{
		stream_ << ">";
		writeNewline();
	}

	writeIndent();
	stream_ << "<" << tag;
	tagOpening_ = true;
	hasChildElements_ = false;
	indent_ += 1;
}


void XMLWriter::endOpenTag()
{
	if( !tagOpening_ )
	{
		return;
	}

	stream_ << ">";
	tagOpening_ = false;
}


void XMLWriter::closeTag( const char* tag )
{
	indent_ -= 1;

	if( tagOpening_ )
	{
		stream_ << "/>";
		tagOpening_ = false;
	}
	else
	{
		if( hasChildElements_ )
		{
			writeIndent();
		}
		stream_ << "</" << tag << ">";
	}

	writeNewline();

	hasChildElements_ = true;
}
} // end namespace wgt
