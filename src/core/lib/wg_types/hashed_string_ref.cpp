#include "hashed_string_ref.hpp"
#include "hash_utilities.hpp"
#include <cstring>

namespace wgt
{
//------------------------------------------------------------------------------
HashedStringRef::HashedStringRef( const char * str )
	: hash_( static_cast<size_t>( HashUtilities::compute( str ) ) )
	, pStart_( str )
	, length_( strlen( pStart_ ) )
{
}


//------------------------------------------------------------------------------
size_t HashedStringRef::hash() const
{
	return hash_;
}


//------------------------------------------------------------------------------
bool HashedStringRef::operator == ( const HashedStringRef & other ) const
{
	if (hash_ != other.hash_)
	{
		return false;
	}
	if (length_ != other.length_)
	{
		return false;
	}
	return strcmp( pStart_, other.pStart_ ) == 0;
}
} // end namespace wgt
