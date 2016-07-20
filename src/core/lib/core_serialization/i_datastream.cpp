#include "i_datastream.hpp"

// TODO: this whole file contains deprecated content; non-deprecated interface should be pure virtual


namespace wgt
{
IDataStream::~IDataStream()
{
}


std::streamsize IDataStream::read( void* destination, std::streamsize size )
{
	// warning: infinite recursion will happen if neither read or readRaw was overriden
	return readRaw( destination, static_cast< size_t >( size ) );
}


std::streamsize IDataStream::write( const void* source, std::streamsize size )
{
	// warning: infinite recursion will happen if neither write or writeRaw was overriden
	return writeRaw( source, static_cast< size_t >( size ) );
}


size_t IDataStream::pos() const
{
	const auto pos = const_cast<IDataStream*>( this )->seek( 0, std::ios_base::cur );
	if (pos < 0)
	{
		return 0;
	}

	return static_cast< size_t >( pos );
}


size_t IDataStream::size() const
{
	IDataStream* s = const_cast<IDataStream*>( this );
	const auto pos = s->seek( 0, std::ios_base::cur );
	if (pos < 0)
	{
		return 0;
	}

	const auto size = s->seek( 0, std::ios_base::end );
	s->seek( pos );
	return static_cast< size_t >( size );
}


const void * IDataStream::rawBuffer() const
{
	return nullptr;
}


size_t IDataStream::readRaw( void * o_Data, size_t length )
{
	auto result = read( o_Data, length );
	return static_cast< size_t >( result );
}


size_t IDataStream::writeRaw( const void * data, size_t length )
{
	auto result = write( data, length );
	return static_cast< size_t >( result );
}


bool IDataStream::eof() const
{
	IDataStream* s = const_cast<IDataStream*>( this );
	const auto pos = s->seek( 0, std::ios_base::cur );
	if (pos < 0)
	{
		return false;
	}

	const auto size = s->seek( 0, std::ios_base::end );
	s->seek( pos );
	return pos >= size;
}


bool IDataStream::read( Variant & variant )
{
	return readValue( variant );
}


bool IDataStream::write( const Variant & variant )
{
	return writeValue( variant );
}


bool IDataStream::writeValue( const Variant & variant )
{
	return false;
}


bool IDataStream::readValue( Variant & variant )
{
	return false;
}
} // end namespace wgt
