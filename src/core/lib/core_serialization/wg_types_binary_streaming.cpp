#include "wg_types_binary_streaming.hpp"
#include "resizing_memory_stream.hpp"


namespace wgt
{

	// BinaryBlock

	BinaryStream& operator<<( BinaryStream& stream, const BinaryBlock& v)
	{
		stream.serializeBuffer( v.data(), v.length() );
		return stream;
	}

	BinaryStream& operator>>( BinaryStream& stream, BinaryBlock& v)
	{
		ResizingMemoryStream dataStream;
		stream.deserializeBuffer( dataStream );
		if (!stream.fail())
		{
			v = BinaryBlock(
				dataStream.buffer().c_str(),
				dataStream.buffer().length(),
				false );
		}
		return stream;
	}

	// Vector2

	BinaryStream& operator<<( BinaryStream& stream, const Vector2& v)
	{
		stream << v.x << v.y;
		return stream;
	}

	BinaryStream& operator>>( BinaryStream& stream, Vector2& v)
	{
		stream >> v.x >> v.y;
		return stream;
	}

	// Vector3

	BinaryStream& operator<<( BinaryStream& stream, const Vector3& v)
	{
		stream << v.x << v.y << v.z;
		return stream;
	}

	BinaryStream& operator>>( BinaryStream& stream, Vector3& v)
	{
		stream >> v.x >> v.y >> v.z;
		return stream;
	}

	// Vector4

	BinaryStream& operator<<( BinaryStream& stream, const Vector4& v)
	{
		stream << v.x << v.y << v.z << v.w;
		return stream;
	}

	BinaryStream& operator>>( BinaryStream& stream, Vector4& v)
	{
		stream >> v.x >> v.y >> v.z >> v.w;
		return stream;
	}

}
