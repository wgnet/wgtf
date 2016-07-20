#include "meta_type.hpp"
#include "core_serialization/fixed_memory_stream.hpp"
#include "core_serialization/resizing_memory_stream.hpp"
#include "core_serialization/text_stream.hpp"
#include <string>
#include <cassert>
#include <cstdint>


namespace wgt
{
namespace
{

	bool convertFromString( const MetaType* toType, void* to, const MetaType* fromType, const void* from )
	{
		const std::string& fromStr = *reinterpret_cast<const std::string*>( from );
		FixedMemoryStream dataStream( fromStr.c_str(), fromStr.size() );
		TextStream stream( dataStream );
		toType->streamIn( stream, to );
		return
			!stream.fail() && // conversion succeeded
			stream.peek() == EOF; // whole string was consumed
	}

}


MetaType::MetaType( const char* name, const Data& data ):
	data_( data ),
	name_( name ? name : data.typeId_.getName() ),
	conversionsFrom_(),
	defaultConversionFrom_( nullptr )
{
	for( int i = 0; i <= QualifiersMask; ++i )
	{
		// Qualified instance must be at least 4 bytes aligned (Variant uses 2 lower bits for storage type)
		assert( ( reinterpret_cast< uintptr_t >( qualified_ + i ) & 0x03 ) == 0 );
		qualified_[ i ].type_ = this;
	}

	addConversionFrom< std::string >( &convertFromString );
}
	

const MetaType::Qualified* MetaType::qualified( int qualifiers ) const
{
	assert( qualifiers >= 0 );
	assert( qualifiers < QualifiersMask );

	return qualified_ + qualifiers;
}


bool MetaType::convertFrom( void* to, const MetaType* fromType, const void* from ) const
{
	// identity conversion
	if( fromType == this )
	{
		copy( to, from );
		return true;
	}

	// custom conversion
	auto conv = conversionsFrom_.find( fromType->typeId() );
	if( conv != conversionsFrom_.end() )
	{
		return conv->second( this, to, fromType, from );
	}

	// default conversion
	if( defaultConversionFrom_ )
	{
		return defaultConversionFrom_( this, to, fromType, from );
	}

	return false;
}

bool MetaType::canConvertFrom( const MetaType* fromType ) const
{
	// identity conversion
	if( fromType == this )
	{
		return true;
	}

	// custom conversion
	auto conv = conversionsFrom_.find( fromType->typeId() );
	if( conv != conversionsFrom_.end() )
	{
		return true;
	}

	// default conversion
	if( defaultConversionFrom_ )
	{
		return true;
	}

	return false;
}


bool MetaType::convertTo( const MetaType* toType, void* to, const void* from ) const
{
	return toType->convertFrom( to, this, from );
}

bool MetaType::canConvertTo( const MetaType* toType ) const
{
	return toType->canConvertFrom( this );
}

void MetaType::addConversionFrom( const TypeId& fromType, ConversionFunc func )
{
	if( fromType != data_.typeId_ )
	{
		conversionsFrom_[ fromType ] = func;
	}
}


void MetaType::setDefaultConversionFrom( ConversionFunc func )
{
	defaultConversionFrom_ = func;
}


bool MetaType::castPtr( const TypeId& destType, void** dest, void* src ) const
{
	if( destType != data_.typeId_ )
	{
		return false;
	}

	if( dest )
	{
		*dest = src;
	}

	return true;
}

} // end namespace wgt
