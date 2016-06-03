#include "binary_stream.hpp"
#include "fixed_memory_stream.hpp"
#include "resizing_memory_stream.hpp"
#include <type_traits>
#include <cstring>


namespace wgt
{
void BinaryStream::serializeBuffer( const void* buffer, std::streamsize size )
{
	if (size < 0)
	{
		// negative size is not OK
		setState( std::ios_base::failbit );
		return;
	}

	auto s = static_cast<size_type>( size );
	if (static_cast<std::streamsize>( s ) != size)
	{
		// overflow
		setState( std::ios_base::failbit );
		return;
	}

	(*this) << s;

	FixedMemoryStream src( buffer, size );
	copyFrom( src );
}


void BinaryStream::deserializeBuffer( IDataStream& destination, std::streamsize maximumSize )
{
	size_type size = 0;
	(*this) >> size;
	if (fail())
	{
		return;
	}

	auto s = static_cast<std::streamsize>( size );
	if (s < 0)
	{
		// negative size is not OK
		setState( std::ios_base::failbit );
		return;
	}

	if (static_cast<size_type>( s ) != size)
	{
		// overflow
		setState( std::ios_base::failbit );
		return;
	}

	if (maximumSize > 0 &&
		s > maximumSize)
	{
		// size limit is exceeded
		setState( std::ios_base::failbit );
		return;
	}

	copyTo( destination, s );
}


BinaryStream& operator<<( BinaryStream& stream, const std::string& value )
{
	stream.serializeBuffer( value.c_str(), value.size() );
	return stream;
}


BinaryStream& operator<<( BinaryStream& stream, const char* value )
{
	stream.serializeBuffer( value, value ? std::strlen( value ) : 0 );
	return stream;
}


BinaryStream& operator>>( BinaryStream& stream, std::string& value )
{
	ResizingMemoryStream dst;
	stream.deserializeBuffer( dst );
	if (!stream.fail())
	{
		value = dst.takeBuffer();
	}
	return stream;
}
} // end namespace wgt
