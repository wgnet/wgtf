#include "pch.hpp"

#include "CppUnitLite2/src/CppUnitLite2.h"
#include "core_serialization/datastreambuf.hpp"
#include "core_serialization/resizing_memory_stream.hpp"
#include "core_serialization/fixed_memory_stream.hpp"
#include <iostream>
#include <string>


namespace wgt
{
TEST( datastreambuf_raw_in )
{
	FixedMemoryStream dataStream( "abcdef01234567" );
	BasicStream basicStream( dataStream );

	DataStreamBuf dataStreamBuf( basicStream );
	std::istream stream( &dataStreamBuf );

	char buf[10] = {};
	stream.read( buf, 1 );
	CHECK_EQUAL( std::string( "a" ), buf );

	stream.putback( 'a' );
	stream.read( buf, 3 );
	CHECK_EQUAL( std::string( "abc" ), buf );

	stream.putback( 'c' );
	auto c = stream.get();
	CHECK_EQUAL( 'c', c );

	c = stream.get();
	CHECK_EQUAL( 'd', c );

	stream.unget();
	stream.read( buf, 3 );
	CHECK_EQUAL( std::string( "def"), buf );

	stream.unget();
	stream.read( buf, 9 );
	CHECK_EQUAL( std::string( "f01234567"), buf );
}


TEST( datastreambuf_formatted_io )
{
	ResizingMemoryStream dataStream;
	BasicStream basicStream( dataStream );

	DataStreamBuf dataStreamBuf( basicStream );
	std::iostream stream( &dataStreamBuf );

	// out
	CHECK( dataStream.buffer().empty() );

	stream << "Hello0123456789 " << 42 << ' ' << 1.234;

	CHECK_EQUAL( std::string("Hello0123456789 42 1.234"), dataStream.buffer().c_str() );

	// rewind
	stream.seekg( 0, std::ios_base::beg );

	// in
	std::string s;
	stream >> s;
	CHECK_EQUAL( "Hello0123456789", s );

	int i;
	stream >> i;
	CHECK_EQUAL( 42, i );

	float f;
	stream >> f;
	CHECK_EQUAL( 1.234f, f );

	CHECK( stream.eof() );
}
} // end namespace wgt
