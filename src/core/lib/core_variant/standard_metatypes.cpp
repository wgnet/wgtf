#include "standard_metatypes.hpp"
#include "core_serialization/resizing_memory_stream.hpp"
#include "core_serialization/text_stream_manip.hpp"

namespace
{
using namespace wgt;

bool convertToString(std::string* to, const MetaType* fromType, const void* from)
{
	if (!to || !from)
	{
		return true;
	}

	ResizingMemoryStream dataStream;
	TextStream stream(dataStream);
	fromType->streamOut(stream, from);
	if (stream.fail())
	{
		return false;
	}

	stream.sync();
	*to = dataStream.takeBuffer();
	return true;
}

template <typename To, typename From>
bool straightConvert(To* to, const MetaType* fromType, const void* from)
{
	if (fromType->typeId() != TypeId::getType<From>())
	{
		return false;
	}

	if (!to || !from)
	{
		return true;
	}

	const From* f = reinterpret_cast<const From*>(from);

	*to = static_cast<To>(*f);

	return true;
}
}

namespace wgt
{
MetaTypeImpl<void>::MetaTypeImpl() : base("void", DeducibleFromText)
{
}

void MetaTypeImpl<void>::init(void* value) const
{
	// nop
}

void MetaTypeImpl<void>::copy(void* dest, const void* src) const
{
	// nop
}

void MetaTypeImpl<void>::move(void* dest, void* src) const
{
	// nop
}

void MetaTypeImpl<void>::destroy(void* value) const
{
	// nop
}

bool MetaTypeImpl<void>::lessThan(const void* lhs, const void* rhs) const
{
	return false;
}

bool MetaTypeImpl<void>::equal(const void* lhs, const void* rhs) const
{
	return true;
}

void MetaTypeImpl<void>::streamOut(TextStream& stream, const void* value) const
{
	stream << "void";
}

void MetaTypeImpl<void>::streamIn(TextStream& stream, void* value) const
{
	stream >> match("void");
}

void MetaTypeImpl<void>::streamOut(BinaryStream& stream, const void* value) const
{
	// nop
}

void MetaTypeImpl<void>::streamIn(BinaryStream& stream, void* value) const
{
	// nop
}

bool MetaTypeImpl<void>::convertFrom(void* to, const MetaType* fromType, const void* from) const
{
	return base::convertFrom(to, fromType, from) || true;
}

uint64_t MetaTypeImpl<void>::hashCode(const void * value) const
{
	std::hash<uint64_t> hash_fn;
	return hash_fn(reinterpret_cast<uint64_t> (value));
}


////////////////////////////////////////////////////////////////////////////

MetaTypeImpl<uint32_t>::MetaTypeImpl() : base("uint", DeducibleFromText)
{
}

bool MetaTypeImpl<uint32_t>::convertFrom(void* to, const MetaType* fromType, const void* from) const
{
	return base::convertFrom(to, fromType, from) ||
	straightConvert<uint32_t, int32_t>(&base::cast(to), fromType, from) ||
	straightConvert<uint32_t, uint64_t>(&base::cast(to), fromType, from) ||
	straightConvert<uint32_t, int64_t>(&base::cast(to), fromType, from) ||
	straightConvert<uint32_t, double>(&base::cast(to), fromType, from);
}

////////////////////////////////////////////////////////////////////////////

MetaTypeImpl<int32_t>::MetaTypeImpl() : base("int", DeducibleFromText)
{
}

bool MetaTypeImpl<int32_t>::convertFrom(void* to, const MetaType* fromType, const void* from) const
{
	return base::convertFrom(to, fromType, from) ||
	straightConvert<int32_t, uint32_t>(&base::cast(to), fromType, from) ||
	straightConvert<int32_t, int64_t>(&base::cast(to), fromType, from) ||
	straightConvert<int32_t, uint64_t>(&base::cast(to), fromType, from) ||
	straightConvert<int32_t, double>(&base::cast(to), fromType, from);
}

////////////////////////////////////////////////////////////////////////////

MetaTypeImpl<uint64_t>::MetaTypeImpl() : base("uint64", DeducibleFromText)
{
}

bool MetaTypeImpl<uint64_t>::convertFrom(void* to, const MetaType* fromType, const void* from) const
{
	return base::convertFrom(to, fromType, from) ||
	straightConvert<uint64_t, int64_t>(&base::cast(to), fromType, from) ||
	straightConvert<uint64_t, uint32_t>(&base::cast(to), fromType, from) ||
	straightConvert<uint64_t, int32_t>(&base::cast(to), fromType, from) ||
	straightConvert<uint64_t, double>(&base::cast(to), fromType, from);
}

////////////////////////////////////////////////////////////////////////////

MetaTypeImpl<int64_t>::MetaTypeImpl() : base("int64", DeducibleFromText)
{
}

bool MetaTypeImpl<int64_t>::convertFrom(void* to, const MetaType* fromType, const void* from) const
{
	return base::convertFrom(to, fromType, from) ||
	straightConvert<int64_t, uint64_t>(&base::cast(to), fromType, from) ||
	straightConvert<int64_t, uint32_t>(&base::cast(to), fromType, from) ||
	straightConvert<int64_t, int32_t>(&base::cast(to), fromType, from) ||
	straightConvert<int64_t, double>(&base::cast(to), fromType, from);
}

////////////////////////////////////////////////////////////////////////////

MetaTypeImpl<double>::MetaTypeImpl() : base("real", DeducibleFromText)
{
}

bool MetaTypeImpl<double>::convertFrom(void* to, const MetaType* fromType, const void* from) const
{
	return base::convertFrom(to, fromType, from) ||
	straightConvert<double, uint64_t>(&base::cast(to), fromType, from) ||
	straightConvert<double, int64_t>(&base::cast(to), fromType, from) ||
	straightConvert<double, uint32_t>(&base::cast(to), fromType, from) ||
	straightConvert<double, int32_t>(&base::cast(to), fromType, from);
}

////////////////////////////////////////////////////////////////////////////

MetaTypeImpl<std::string>::MetaTypeImpl() : base("string", ForceShared | DeducibleFromText)
{
}

void MetaTypeImpl<std::string>::streamOut(TextStream& stream, const void* value) const
{
	stream << quoted(base::cast(value));
}

void MetaTypeImpl<std::string>::streamIn(TextStream& stream, void* value) const
{
	stream >> quoted(base::cast(value));
}

void MetaTypeImpl<std::string>::streamOut(BinaryStream& stream, const void* value) const
{
	stream << base::cast(value);
}

void MetaTypeImpl<std::string>::streamIn(BinaryStream& stream, void* value) const
{
	stream >> base::cast(value);
}

bool MetaTypeImpl<std::string>::convertFrom(void* to, const MetaType* fromType, const void* from) const
{
	return base::convertFrom(to, fromType, from) || convertToString(&base::cast(to), fromType, from);
}
}
