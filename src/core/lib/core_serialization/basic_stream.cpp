#include "basic_stream.hpp"
#include "i_datastream.hpp"
#include <cassert>
#include <cstdio> // for EOF
#include <cstring>
#include <utility>
#include <algorithm>


namespace wgt
{
BasicStream::BasicStream( IDataStream& dataStream ):
	dataStream_( dataStream ),
	state_( std::ios_base::goodbit ),
	//readBuffer_(),
	readPos_( readBuffer_ ),
	readEnd_( readBuffer_ ),
	ungetBufferSize_( 1 )
{
}


BasicStream::~BasicStream()
{
	sync();
}


void BasicStream::setState( std::ios_base::iostate state )
{
	state_ |= state;
}


void BasicStream::resetState( std::ios_base::iostate state )
{
	state_ &= ~state;
}


std::streamoff BasicStream::seek( std::streamoff offset, std::ios_base::seekdir dir )
{
	std::streamoff o = offset;
	if (dir == std::ios_base::cur)
	{
		o -= readEnd_ - readPos_; // shift pos back to a buffered chars count
	}

	// don't set IO error if seek fails
	auto r = dataStream_.seek( o, dir );
	if (r >= 0)
	{
		// reset read buffer
		readPos_ = readBuffer_;
		readEnd_ = readBuffer_;
	}

	return r;
}


bool BasicStream::sync()
{
	resetReadBuffer();
	return dataStream_.sync();
}


std::streamsize BasicStream::setUngetBufferSize( std::streamsize v )
{
	ungetBufferSize_ = std::min<std::streamsize>(
		std::max<std::streamsize>( v, 0 ),
		sizeof( readBuffer_ ) );
	return ungetBufferSize_;
}


bool BasicStream::get( char& c )
{
	auto r = read( &c, 1 );
	return r > 0;
}


int BasicStream::get()
{
	char c;
	if (get( c ))
	{
		return c;
	}

	return EOF;
}


bool BasicStream::unget( std::streamsize size, const void* source )
{
	if (size < 0 || size > readPos_ - readBuffer_)
	{
		// unget area doesn't contain requested amount of data
		setState( std::ios_base::badbit );
		return false;
	}

	if (source)
	{
		auto r = std::memcmp( readPos_ - size, source, static_cast< size_t >( size ) );
		if (r != 0)
		{
			// unget area doesn't match source
			setState( std::ios_base::badbit );
			return false;
		}
	}

	readPos_ -= size;
	return true;
}


int BasicStream::peek()
{
	if (readPos_ < readEnd_)
	{
		return *readPos_;
	}

	int r = get();
	if (r != EOF)
	{
		unget();
	}

	return r;
}


std::streamsize BasicStream::read( void* destination, std::streamsize size )
{
	if (size <= 0)
	{
		return 0;
	}

	// try read from buffer
	auto copyFromBuffer = std::min<std::streamsize>( size, readEnd_ - readPos_ );
	if (copyFromBuffer > 0)
	{
		std::memcpy( destination, readPos_, static_cast< size_t >( copyFromBuffer ) );
		readPos_ += copyFromBuffer;
		return copyFromBuffer;
	}

	// read buffer is empty, perform read from data stream
	assert( readPos_ ==  readEnd_ );

	// is unget area larger than needed?
	if (readPos_ - readBuffer_ > ungetBufferSize_)
	{
		// free some buffer space for reading
		std::memmove( readBuffer_, readPos_ - ungetBufferSize_, static_cast< size_t >( ungetBufferSize_ ) );
		readPos_ = readBuffer_ + ungetBufferSize_;
		readEnd_ = readPos_;
	}

	const std::streamsize freeBufferSize = readBuffer_ + sizeof( readBuffer_ ) - readEnd_;
	if (freeBufferSize > size)
	{
		// free buffer is larger that requested size, so read to buffer
		auto r = dataStream_.read( readEnd_, freeBufferSize );
		if (r <= 0)
		{
			setState( std::ios_base::eofbit );
			return 0;
		}
		readEnd_ += r;

		// read from buffer
		auto copyFromBuffer = std::min<std::streamsize>( size, r );
		std::memcpy( destination, readPos_, static_cast< size_t >( copyFromBuffer ) );

		readPos_ += copyFromBuffer;
		return copyFromBuffer;
	}
	else
	{
		// requested size is larger than buffer, so read directly to user buffer
		auto r = dataStream_.read( destination, size );
		if (r <= 0)
		{
			setState( std::ios_base::eofbit );
			return 0;
		}

		// copy read data tail to read buffer
		const auto toBuffer = std::min<std::streamsize>( r, sizeof( readBuffer_ ) );
		char* buf = readEnd_;

		if (toBuffer > freeBufferSize)
		{
			// replace (partially or completely) current unget area
			std::streamsize ungetBytesToKeep = sizeof( readBuffer_ ) - toBuffer;
			if (ungetBytesToKeep > 0)
			{
				// shift current unget area
				std::memmove( readBuffer_, readEnd_ - ungetBytesToKeep, static_cast< size_t >( ungetBytesToKeep ) );
			}
			buf = readBuffer_ + ungetBytesToKeep;
		}

		std::memcpy( buf, static_cast<char*>( destination ) + r - toBuffer, static_cast< size_t >( toBuffer ) );

		readPos_ = buf + toBuffer;
		readEnd_ = readPos_;

		return r;
	}
}


bool BasicStream::put( char c )
{
	auto r = write( &c, 1 );
	return r > 0;
}


std::streamsize BasicStream::write( const void* source, std::streamsize size )
{
	resetReadBuffer();

	// TODO: buffered write

	std::streamsize r = dataStream_.write( source, size );
	if (r <= 0)
	{
		setState( std::ios_base::badbit );
	}

	return r;
}


std::streamsize BasicStream::readHard( void* destination, std::streamsize size )
{
	std::streamsize total = 0;

	while (total < size)
	{
		auto r = read(
			static_cast<char*>( destination ) + total,
			size - total );
		if (r <= 0)
		{
			break;
		}

		total += r;
	}

	return total;
}


std::streamsize BasicStream::writeHard( const void* source, std::streamsize size )
{
	std::streamsize total = 0;

	while (total < size)
	{
		auto r = write(
			static_cast<const char*>( source ) + total,
			size - total );
		if (r <= 0)
		{
			break;
		}

		total += r;
	}

	return total;
}


std::streamsize BasicStream::copyFrom( IDataStream& source, std::streamsize amount )
{
	char buffer[1024];
	std::streamsize copied = 0;

	while (true)
	{
		auto toRead = static_cast<std::streamsize>( sizeof( buffer ) );
		if (amount >= 0)
		{
			auto remain = amount - copied;
			if (remain <= 0)
			{
				break;
			}

			if (toRead > remain)
			{
				toRead = remain;
			}
		}

		auto rr = source.read( buffer, toRead );
		if (rr <= 0)
		{
			break;
		}

		auto wr = writeHard( buffer, rr );
		copied += wr;
		if (wr < rr)
		{
			break;
		}
	}

	return copied;
}


std::streamsize BasicStream::copyTo( IDataStream& destination, std::streamsize amount )
{
	char buffer[1024];
	std::streamsize copied = 0;

	while (true)
	{
		auto toRead = static_cast<std::streamsize>( sizeof( buffer ) );
		if (amount >= 0)
		{
			auto remain = amount - copied;
			if (remain <= 0)
			{
				break;
			}

			if (toRead > remain)
			{
				toRead = remain;
			}
		}

		auto rr = read( buffer, toRead );
		if (rr <= 0)
		{
			break;
		}

		// write hard to data stream
		std::streamsize wr = 0;
		while (wr < rr)
		{
			auto r = destination.write(
				buffer + wr,
				rr - wr );
			if (r <= 0)
			{
				// failed to write all read data
				setState( std::ios_base::badbit );
				break;
			}

			wr += r;
		}

		copied += wr;

		if (wr < rr)
		{
			break;
		}
	}

	return copied;
}


bool BasicStream::resetReadBuffer()
{
	if (readPos_ == readEnd_)
	{
		return true;
	}

	return seek( 0, std::ios_base::cur ) >= 0;
}
} // end namespace wgt
