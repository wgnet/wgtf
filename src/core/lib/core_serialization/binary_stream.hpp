#ifndef BINARY_STREAM_HPP_INCLUDED
#define BINARY_STREAM_HPP_INCLUDED

#include "basic_stream.hpp"
#include "serialization_dll.hpp"
#include <type_traits>
#include <string>
#include <cstdint>


namespace wgt
{
class SERIALIZATION_DLL BinaryStream:
	public BasicStream
{
	typedef BasicStream base;

public:
	// common platform-independent types
	typedef uint64_t size_type;

	explicit BinaryStream( IDataStream& stream ):
		base( stream )
	{
	}

	/**
	Serialize given buffer.

	Serialized buffer consists of data size followed by data itself.
	*/
	void serializeBuffer( const void* buffer, std::streamsize size );

	/**
	Deserialize buffer to a data stream.

	Set @a maximumSize to a maximum size of data which is accepted. If larger
	data size was serialized then deserialization is failed and failbit is set.
	By default any buffer size is accepted.

	@warning Due to stream inconsistency, some other error or even intentional
	DoS attack, source stream can provide some insanely large data size, which
	may lead to memory issues like out-of-memory error, or drastic application
	performance degradation. Use default @a maximumSize (i.e. unlimited data
	size) only on trusted data sources.
	*/
	void deserializeBuffer( IDataStream& destination, std::streamsize maximumSize = -1 );

};


// generic simple types serialization
// TODO: ensure little-endian format for numbers

template< typename T >
typename std::enable_if< std::is_trivially_copyable< T >::value, BinaryStream& >::type
	operator<<( BinaryStream& stream, const T& value )
{
	std::streamsize r = stream.writeHard( &value, sizeof( value ) );
	if( r != static_cast<std::streamsize>( sizeof( value ) ) )
	{
		stream.setState( std::ios_base::failbit );
	}

	return stream;
}


template< typename T >
typename std::enable_if< std::is_trivially_copyable< T >::value, BinaryStream& >::type
	operator>>( BinaryStream& stream, T& value )
{
	std::streamsize r = stream.readHard( &value, sizeof( value ) );
	if( r != static_cast<std::streamsize>( sizeof( value ) ) )
	{
		stream.setState( std::ios_base::failbit );
	}

	return stream;
}


// string serialization
SERIALIZATION_DLL BinaryStream& operator<<( BinaryStream& stream, const std::string& value );
SERIALIZATION_DLL BinaryStream& operator<<( BinaryStream& stream, const char* value );
SERIALIZATION_DLL BinaryStream& operator>>( BinaryStream& stream, std::string& value );

} // end namespace wgt

#include "wg_types_binary_streaming.hpp"

#endif // BINARY_STREAM_HPP_INCLUDED