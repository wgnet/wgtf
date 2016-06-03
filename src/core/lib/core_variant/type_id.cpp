#include "type_id.hpp"
#include "core_reflection/object_handle.hpp"
#include "wg_types/hash_utilities.hpp"

#include <assert.h>
#include <string.h>


namespace wgt
{
//==============================================================================
TypeId::TypeId( const std::string & name )
	: name_( nullptr )
	, hashCode_( 0 )
	, owns_( false )
{
	if (name.empty())
	{
		return;
	}

	auto len = name.size();
	auto buffer = new char[len + 1];
	strncpy( buffer, name.c_str(), len + 1 );

	name_ = buffer;
	hashCode_ = HashUtilities::compute( buffer );
	owns_ = true;
}


//==============================================================================
TypeId::TypeId( const char * name )
	: name_( name )
	, hashCode_( name != nullptr ? HashUtilities::compute( name ) : 0 )
	, owns_( false )
{
}


//==============================================================================
TypeId::TypeId( const char * name, uint64_t hashCode )
	: name_( name )
	, hashCode_( hashCode )
	, owns_( false )
{
}


//==============================================================================
TypeId::TypeId( const TypeId & other )
	: name_( other.name_ )
	, hashCode_( other.hashCode_ )
	, owns_( other.owns_ )
{
	if (!owns_)
	{
		return;
	}

	auto len = strlen( name_ );
	auto buffer = new char[len + 1];
	strncpy( buffer, name_, len + 1 );
	name_ = buffer;
}

//==============================================================================
TypeId::~TypeId()
{
	if (owns_)
	{
		delete[] name_;
	}
}


//==============================================================================
bool TypeId::isPointer() const
{
	return removePointer( nullptr );
}


//==============================================================================
TypeId TypeId::removePointer() const
{
	TypeId typeId = *this;
	removePointer( &typeId );
	return typeId;
}


// Helper function for temporary hack in TypeId::removePointer.
std::string extractContentsIfTargetMatchingPattern( const char* target, const char* pattern, const char* section )
{
	size_t patternLength = strlen( pattern );
	size_t sectionLength = strlen( section );
	assert( patternLength > sectionLength );

	const char* patternFromSection = strstr( pattern, section );
	assert( patternFromSection != nullptr );

	auto patternPrefixLength = patternFromSection - pattern;
	auto patternSuffixLength = patternLength - patternPrefixLength - sectionLength;
	assert( patternPrefixLength > 0 || patternSuffixLength > 0 );

	size_t targetLength = strlen( target );

	if (targetLength <= patternPrefixLength + patternSuffixLength)
	{
		return target;
	}

	const bool prefixMatches = strncmp( target, pattern, patternPrefixLength) == 0;

	if (!prefixMatches)
	{
		return target;
	}

	const char* patternSuffix = pattern + patternLength - patternSuffixLength;
	const char* targetSuffix = target + targetLength - patternSuffixLength;

	const bool suffixMatches = strncmp( targetSuffix, patternSuffix, patternSuffixLength ) == 0;

	if (!suffixMatches)
	{
		return target;
	}

	const char* targetSection = target + patternPrefixLength;
	auto targetSectionLength = targetLength - patternPrefixLength - patternSuffixLength;
	return std::string( targetSection, targetSectionLength );
}


//==============================================================================
bool TypeId::removePointer( TypeId * typeId ) const
{
	// Temporary hack - Using string manipulation to determine if a typeId points to a pointer type (or ObjectHandle type)
	// Necessary whilst we still allow TypeIds to be constructed by string.

	auto name = getName();
	auto originalLength = strlen( name );

	auto voidType = TypeId::getType< void >().getName();
	auto voidPointerType = TypeId::getType< void * >().getName();

	// Test for T*
	auto extractedType = extractContentsIfTargetMatchingPattern( name, voidPointerType, voidType );

	if (extractedType.length() < originalLength)
	{
		if (typeId != nullptr)
		{
			*typeId = TypeId( extractedType );
		}

		return true;
	}

	auto voidObjectHandleType = TypeId::getType< ObjectHandleT< void * > >().getName();

	// Test for ObjectHandle<T>
	extractedType = extractContentsIfTargetMatchingPattern( name, voidObjectHandleType, voidPointerType );

	if (extractedType.length() < originalLength)
	{
		if (typeId != nullptr)
		{
			*typeId = TypeId( extractedType );
		}

		return true;
	}

	return false;
}


//==============================================================================
TypeId & TypeId::operator = ( const TypeId & other )
{
	if (owns_)
	{
		delete[] name_;
	}

	name_ = other.name_;
	hashCode_ = other.hashCode_;
	owns_ = other.owns_;

	if (!owns_)
	{
		return *this;
	}

	auto len = strlen( name_ );
	auto buffer = new char[len + 1];
	strncpy( buffer, name_, len + 1 );
	name_ = buffer;
	
	return *this;
}


//==============================================================================
bool TypeId::operator == ( const TypeId & other ) const
{
	return hashCode_ == other.getHashcode();
}


//==============================================================================
bool TypeId::operator != ( const TypeId & other ) const
{
	return hashCode_ != other.getHashcode();
}


//==============================================================================
bool TypeId::operator < (const TypeId & other ) const
{
	return hashCode_ < other.getHashcode();
}
} // end namespace wgt
