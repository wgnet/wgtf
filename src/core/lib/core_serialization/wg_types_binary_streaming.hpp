#ifndef WG_TYPES_BINARY_STREAMING_HPP_INCLUDED
#define WG_TYPES_BINARY_STREAMING_HPP_INCLUDED


#include "serialization_dll.hpp"
#include "binary_stream.hpp"
#include "wg_types/binary_block.hpp"
#include "wg_types/vector2.hpp"
#include "wg_types/vector3.hpp"
#include "wg_types/vector4.hpp"


namespace wgt
{

	SERIALIZATION_DLL BinaryStream& operator<<( BinaryStream& stream, const BinaryBlock& v);
	SERIALIZATION_DLL BinaryStream& operator>>( BinaryStream& stream, BinaryBlock& v);

	SERIALIZATION_DLL BinaryStream& operator<<( BinaryStream& stream, const Vector2& v);
	SERIALIZATION_DLL BinaryStream& operator>>( BinaryStream& stream, Vector2& v);

	SERIALIZATION_DLL BinaryStream& operator<<( BinaryStream& stream, const Vector3& v);
	SERIALIZATION_DLL BinaryStream& operator>>( BinaryStream& stream, Vector3& v);

	SERIALIZATION_DLL BinaryStream& operator<<( BinaryStream& stream, const Vector4& v);
	SERIALIZATION_DLL BinaryStream& operator>>( BinaryStream& stream, Vector4& v);

}

#endif
