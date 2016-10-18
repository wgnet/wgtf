#include "wg_types_text_streaming.hpp"
#include "text_stream_manip.hpp"
#include "fixed_memory_stream.hpp"
#include "resizing_memory_stream.hpp"
#include "wg_types/base64.hpp"


namespace
{

	const char g_separator = ',';

}

namespace wgt
{

	// BinaryBlock

	TextStream& operator<<( TextStream& stream, const BinaryBlock& v )
	{
		std::string encodeValue = Base64::encode(
			static_cast< const char* >( v.data() ), 
			v.length() );
		FixedMemoryStream dataStream( encodeValue.c_str(), encodeValue.length() );
		stream.serializeString( dataStream );
		return stream;
	}

	TextStream& operator>>( TextStream& stream, BinaryBlock& v )
	{
		ResizingMemoryStream dataStream;
		stream.deserializeString( dataStream );
		if (stream.fail())
		{
			return stream;
		}

		std::string decodeValue;
		if (!Base64::decode( dataStream.buffer(), decodeValue ))
		{
			stream.setState(std::ios_base::failbit);
			return stream;
		}

		v = BinaryBlock(
			decodeValue.c_str(),
			decodeValue.length(),
			false );
		return stream;
	}

	// Vector2

	TextStream& operator<<( TextStream& stream, const Vector2& v )
	{
		stream << v.x << g_separator << v.y;
		return stream;
	}

	TextStream& operator>>( TextStream& stream, Vector2& v )
	{
		stream >> v.x >> match( g_separator ) >> v.y;
		return stream;
	}

	// Vector3

	TextStream& operator<<( TextStream& stream, const Vector3& v )
	{
		stream << v.x << g_separator << v.y << g_separator << v.z;
		return stream;
	}

	TextStream& operator>>( TextStream& stream, Vector3& v )
	{
		stream >> v.x >> match( g_separator ) >> v.y >> match( g_separator ) >> v.z;
		return stream;
	}

	// Vector4

	TextStream& operator<<( TextStream& stream, const Vector4& v )
	{
		stream << v.x << g_separator << v.y << g_separator << v.z << g_separator << v.w;
		return stream;
	}

	TextStream& operator>>( TextStream& stream, Vector4& v )
	{
		stream >> v.x >> match( g_separator ) >> v.y >> match( g_separator ) >> v.z >> match( g_separator ) >> v.w;
		return stream;
	}

}
