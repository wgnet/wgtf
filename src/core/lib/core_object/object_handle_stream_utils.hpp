#pragma once

#include "core_reflection/object_handle.hpp"
#include "core_serialization/text_stream.hpp"
#include "core_serialization/binary_stream.hpp"

namespace wgt
{
class ObjectHandleStreamUtils
{
public:
	template <typename Fn>
	static void metaAction(const ObjectHandle& value, Fn fn)
	{
		const MetaType* metaType = nullptr;
		void* raw = nullptr;

		if (IObjectHandleStorage* storage = value.storage().get())
		{
			metaType = MetaType::find(storage->type());
			raw = storage->data();
		}

		fn(metaType, raw);
	}

	static TextStream& stream(TextStream& stream, StrongType<const ObjectHandle&> value)
	{
		metaAction(value, [&](const MetaType* metaType, void* raw) {
			if (metaType && raw)
			{
				metaType->streamOut(stream, raw);
			}
			else
			{
				stream.setState(std::ios_base::failbit);
			}
		});

		return stream;
	}

	static TextStream& stream(TextStream& stream, ObjectHandle& value)
	{
		metaAction(value, [&](const MetaType* metaType, void* raw) {
			if (metaType && raw)
			{
				metaType->streamIn(stream, raw);
			}
			else
			{
				stream.setState(std::ios_base::failbit);
			}
		});

		return stream;
	}

	static BinaryStream& stream(BinaryStream& stream, StrongType<const ObjectHandle&> value)
	{
		metaAction(value, [&](const MetaType* metaType, void* raw) {
			if (metaType && raw)
			{
				metaType->streamOut(stream, raw);
			}
			else
			{
				stream.setState(std::ios_base::failbit);
			}
		});

		return stream;
	}

	static BinaryStream& stream(BinaryStream& stream, ObjectHandle& value)
	{
		metaAction(value, [&](const MetaType* metaType, void* raw) {
			if (metaType && raw)
			{
				metaType->streamIn(stream, raw);
			}
			else
			{
				stream.setState(std::ios_base::failbit);
			}
		});

		return stream;
	}
};

inline TextStream& operator<<(TextStream& stream, StrongType<const ObjectHandle&> value)
{
	return ObjectHandleStreamUtils::stream(stream, value);
}

inline TextStream& operator>>(TextStream& stream, ObjectHandle& value)
{
	return ObjectHandleStreamUtils::stream(stream, value);
}

inline BinaryStream& operator<<(BinaryStream& stream, StrongType<const ObjectHandle&> value)
{
	return ObjectHandleStreamUtils::stream(stream, value);
}

inline BinaryStream& operator>>(BinaryStream& stream, ObjectHandle& value)
{
	return ObjectHandleStreamUtils::stream(stream, value);
}

} // end namespace wgt
