#include "fixed_memory_stream.hpp"
#include <cstring>
#include <algorithm>


namespace wgt
{
FixedMemoryStream::FixedMemoryStream( void* buffer, std::streamsize size ):
	buffer_( static_cast<char*>( buffer ) ),
	readOnly_( false ),
	pos_ ( 0 ),
	size_( size )
{
}


FixedMemoryStream::FixedMemoryStream( const void* buffer, std::streamsize size ):
	buffer_( static_cast<char*>( const_cast<void*>( buffer ) ) ),
	readOnly_( true ),
	pos_ ( 0 ),
	size_( size )
{
}


FixedMemoryStream::FixedMemoryStream( const char* buffer ):
	buffer_( const_cast<char*>( buffer ) ),
	readOnly_( true ),
	pos_ ( 0 ),
	size_( buffer ? std::strlen( buffer ) : 0 )
{
}


std::streamoff FixedMemoryStream::seek( std::streamoff offset, std::ios_base::seekdir dir )
{
	std::streamoff pos;
	switch (dir)
	{
	case std::ios_base::beg:
		pos = offset;
		break;

	case std::ios_base::cur:
		pos = pos_ + offset;
		break;

	case std::ios_base::end:
		pos = size_ + offset;
		break;

	default:
		return -1;

	}

	if (pos < 0 ||
		pos > size_)
	{
		return -1;
	}

	pos_ = pos;

	return pos_;
}


std::streamsize FixedMemoryStream::read( void* destination, std::streamsize size )
{
	const auto toRead = std::min< std::streamsize >( size, size_ - pos_ );
	if (toRead > 0)
	{
		std::memcpy( destination, buffer_ + pos_, static_cast< size_t >( toRead ) );
		pos_ += toRead;
	}

	return toRead;
}


std::streamsize FixedMemoryStream::write( const void* source, std::streamsize size )
{
	if (readOnly_)
	{
		return 0;
	}

	const auto toWrite = std::min< std::streamsize >( size, size_ - pos_ );
	if (toWrite > 0)
	{
		std::memcpy( buffer_ + pos_, source, static_cast< size_t >( toWrite ) );
		pos_ += toWrite;
	}

	return toWrite;
}


bool FixedMemoryStream::sync()
{
	return true;
}
} // end namespace wgt
