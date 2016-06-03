#include "meta_type.hpp"
#include "core_serialization/fixed_memory_stream.hpp"
#include "core_serialization/resizing_memory_stream.hpp"
#include "core_serialization/text_stream.hpp"
#include <string>


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


MetaType::MetaType(
	const char* name,
	size_t size,
	const TypeId& typeId,
	const std::type_info& typeInfo,
	const std::type_info* pointedType,
	int flags ):

	typeId_( typeId ),
	name_( name ? name : typeInfo.name() ),
	size_( size ),
	typeInfo_( typeInfo ),
	pointedType_( pointedType ),
	flags_( flags ),
#if !FAST_RUNTIME_POINTER_CAST
	ptrCastsMutex_(),
	ptrCasts_(),
#endif // FAST_RUNTIME_POINTER_CAST
	conversionsFrom_(),
	defaultConversionFrom_( nullptr )
{
	addConversionFrom< std::string >( &convertFromString );
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
	auto conv = conversionsFrom_.find( &fromType->typeInfo() );
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
	auto conv = conversionsFrom_.find( &fromType->typeInfo() );
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

void MetaType::addConversionFrom( const std::type_info& fromType, ConversionFunc func )
{
	if( fromType != typeInfo_ )
	{
		conversionsFrom_[ &fromType ] = func;
	}
}


void MetaType::setDefaultConversionFrom( ConversionFunc func )
{
	defaultConversionFrom_ = func;
}

#if FAST_RUNTIME_POINTER_CAST

void* MetaType::castPtr( const std::type_info& type, void* value, bool const_value ) const
{
	// TODO: hack CRT structures and cast without exceptions
}

#endif // FAST_RUNTIME_POINTER_CAST
} // end namespace wgt
