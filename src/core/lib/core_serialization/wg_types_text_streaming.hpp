#ifndef WG_TYPES_TEXT_STREAMING_HPP_INCLUDED
#define WG_TYPES_TEXT_STREAMING_HPP_INCLUDED

#include "serialization_dll.hpp"
#include "text_stream.hpp"
#include "wg_types/binary_block.hpp"
#include "wg_types/vector2.hpp"
#include "wg_types/vector3.hpp"
#include "wg_types/vector4.hpp"

namespace wgt
{
SERIALIZATION_DLL TextStream& operator<<(TextStream& stream, const BinaryBlock& v);
SERIALIZATION_DLL TextStream& operator>>(TextStream& stream, BinaryBlock& v);

SERIALIZATION_DLL TextStream& operator<<(TextStream& stream, const Vector2& v);
SERIALIZATION_DLL TextStream& operator>>(TextStream& stream, Vector2& v);

SERIALIZATION_DLL TextStream& operator<<(TextStream& stream, const Vector3& v);
SERIALIZATION_DLL TextStream& operator>>(TextStream& stream, Vector3& v);

SERIALIZATION_DLL TextStream& operator<<(TextStream& stream, const Vector4& v);
SERIALIZATION_DLL TextStream& operator>>(TextStream& stream, Vector4& v);
}

#endif
