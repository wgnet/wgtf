#include "custom_xml_reader.hpp"
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
CustomXmlDataReader::StackItem::StackItem( Variant value )
	:value( std::move( value ) )
	,characterData()
	,hasChildren( false )
{
}


void CustomXmlDataReader::StackItem::cast()
{

	if( auto v = value.castPtr< CustomXmlData >() )
	{
		data = v;
	}
}


CustomXmlDataReader::CustomXmlDataReader( TextStream& stream )
	: base( stream ),
	stack_(),
	pushed_( false ),
	done_( false ),
	ignore_( 0 )
{
}


bool CustomXmlDataReader::read( Variant& value )
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


void CustomXmlDataReader::elementStart( const char* elementName, const char* const* attributes )
{
	if( ignore_ )
	{
		ignore_ += 1;
		return;
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

		if( current.data != nullptr )
		{
			if(strcmp( elementName, "name") ==0)
			{
				stack_.emplace_back( current.data->name_ );
			}
			else if(strcmp( elementName, "filename") ==0)
			{
				stack_.emplace_back( current.data->filename_ );
			}
			else if(strcmp( elementName, "createdBy") ==0)
			{
				stack_.emplace_back( current.data->createdBy_ );
			}
			else if(strcmp( elementName, "visibility") ==0)
			{
				stack_.emplace_back( current.data->visibility_ );
			}
			else if(strcmp( elementName, "position") ==0)
			{
				stack_.emplace_back( current.data->position_ );
			}
		}
		else
		{
			// This may occur if we didn't find CustomXmlData for parent
			// element and assumed parent to be primitive value. So consider
			// this situation as an error.
			abortParsing();
			return;
		}
	}

	auto& current = stack_.back();
	current.cast();
}


void CustomXmlDataReader::elementEnd( const char* elementName )
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
		//restore the value from stream
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

	if( stack_.size() <= 1 )
	{
		// this seems to be a root value: we're done
		done_ = true;
		abortParsing();
		return;
	}

	// move current element to its parent
	auto& parent = *std::prev( stack_.end(), 2 );
	// convert the variant value to members data here
	if( parent.data != nullptr )
	{
		bool isOk = false;
		if(strcmp( elementName, "name") ==0)
		{
			isOk = current.value.tryCast( parent.data->name_ );
		}
		else if(strcmp( elementName, "filename") ==0)
		{
			isOk = current.value.tryCast( parent.data->filename_ );
		}
		else if(strcmp( elementName, "createdBy") ==0)
		{
			isOk = current.value.tryCast( parent.data->createdBy_ );
		}
		else if(strcmp( elementName, "visibility") ==0)
		{
			isOk = current.value.tryCast( parent.data->visibility_ );
		}
		else if(strcmp( elementName, "position") ==0)
		{
			isOk = current.value.tryCast( parent.data->position_ );
		}
		assert( isOk );
		stack_.pop_back();
	}
	else
	{
		assert( false ); // we shouldn't ever get here
	}
}


void CustomXmlDataReader::characterData( const char* data, size_t length )
{
	if( ignore_ )
	{
		return;
	}

	assert( !stack_.empty() );

	auto& current = stack_.back();

	if( current.data != nullptr )
	{
		if( !current.hasChildren )
		{
			current.characterData.append( data, data + length );
		}
	}
	else
	{
		current.characterData.append( data, data + length );
	}
}
} // end namespace wgt
