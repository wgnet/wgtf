#ifndef STD_DATA_STREAM_HPP_INCLUDED
#define STD_DATA_STREAM_HPP_INCLUDED

#include "i_datastream.hpp"
#include <streambuf>


namespace wgt
{
template< typename Char >
class BasicStdDataStream:
	public IDataStream
{
public:
	typedef Char char_type;
	typedef std::basic_streambuf< char_type > streambuf_type;

	explicit BasicStdDataStream( streambuf_type* streambuf ):
		streambuf_( streambuf )
	{
	}

	std::streamoff seek( std::streamoff offset, std::ios_base::seekdir dir = std::ios_base::beg ) override
	{
		auto r = streambuf_->pubseekoff(
			offset / sizeof( char_type ),
			dir,
			std::ios_base::in | std::ios_base::out );
		return r * sizeof( char_type );
	}

	std::streamsize read( void* destination, std::streamsize size ) override
	{
		auto r = streambuf_->sgetn(
			( char_type* )destination,
			size / sizeof( char_type ) );
		return r * sizeof( char_type );
	}

	std::streamsize write( const void* source, std::streamsize size ) override
	{
		auto r = streambuf_->sputn(
			( const char_type* )source,
			size / sizeof( char_type ) );
		return r * sizeof( char_type );
	}

	bool sync() override
	{
		return streambuf_->pubsync() == 0;
	}

private:
	streambuf_type* streambuf_;

};


typedef BasicStdDataStream< char > StdDataStream;
typedef BasicStdDataStream< wchar_t > WStdDataStream;
} // end namespace wgt
#endif // STD_DATA_STREAM_HPP_INCLUDED
