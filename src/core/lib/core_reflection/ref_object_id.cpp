#include "ref_object_id.hpp"

#include <stdarg.h>
#include <stdio.h>
#include <wchar.h>

#ifdef _WIN32
#include "objbase.h"
#elif __APPLE__
#include <uuid/uuid.h>
#endif
#include "core_common/assert.hpp"
#include "core_serialization/resizing_memory_stream.hpp"
#include "core_serialization/fixed_memory_stream.hpp"
#include "wg_types/hash_utilities.hpp"

namespace
{
wgt::RefObjectId s_zeroRefObjectId(0, 0, 0, 0);
}

namespace wgt
{
//==============================================================================
RefObjectId::RefObjectId() : a_(0), b_(0), c_(0), d_(0)
{
}

//==============================================================================
RefObjectId::RefObjectId(const RefObjectId& other) : a_(other.a_), b_(other.b_), c_(other.c_), d_(other.d_)
{
}

//==============================================================================
RefObjectId::RefObjectId(const std::string& s) : a_(0), b_(0), c_(0), d_(0)
{
	if (!s.empty())
	{
		unsigned int data[4];
		if (fromString(s, &data[0]))
		{
			a_ = data[0];
			b_ = data[1];
			c_ = data[2];
			d_ = data[3];
		}
		else
		{
			TF_ASSERT(false && "Error parsing RefObjectId");
		}
	}
}

//==============================================================================
RefObjectId::operator std::string() const
{
	char buf[80];
	sprintf(buf, "%08X.%08X.%08X.%08X", a_, b_, c_, d_);
	return std::string(buf);
}

//==============================================================================
bool RefObjectId::operator==(const RefObjectId& rhs) const
{
	return (a_ == rhs.a_) && (b_ == rhs.b_) && (c_ == rhs.c_) && (d_ == rhs.d_);
}

//==============================================================================
bool RefObjectId::operator!=(const RefObjectId& rhs) const
{
	return !(*this == rhs);
}

std::wstring RefObjectId::toWString() const
{
	wchar_t buf[80];
	swprintf(buf, 80, L"%08X.%08X.%08X.%08X", a_, b_, c_, d_);
	return std::wstring(buf);
}

//==============================================================================
bool RefObjectId::operator<(const RefObjectId& rhs) const
{
	if (a_ < rhs.a_)
		return true;
	else if (a_ > rhs.a_)
		return false;

	if (b_ < rhs.b_)
		return true;
	else if (b_ > rhs.b_)
		return false;

	if (c_ < rhs.c_)
		return true;
	else if (c_ > rhs.c_)
		return false;

	if (d_ < rhs.d_)
		return true;
	else if (d_ > rhs.d_)
		return false;

	return false;
}

//==============================================================================
RefObjectId RefObjectId::generate()
{
	RefObjectId n;

#ifdef _WIN32
	static_assert(sizeof(GUID) == sizeof(RefObjectId), "Can't cast RefObjectId to GUID, types are not the same size");
	CoCreateGuid(reinterpret_cast<GUID*>(&n));
#elif __APPLE__
	uuid_generate((unsigned char*)(&n));
#endif
	TF_ASSERT(n != RefObjectId::zero() && "Couldn't create GUID");

	return n;
}

//==============================================================================
const RefObjectId& RefObjectId::zero()
{
	return s_zeroRefObjectId;
}

//==============================================================================
bool RefObjectId::fromString(const std::string& s, unsigned int* data)
{
	if (s.empty())
	{
		return false;
	}

	std::string copyS(s);
	char* str = const_cast<char*>(copyS.c_str());
	for (int offset = 0; offset < 4; offset++)
	{
		char* initstr = str;
		data[offset] = strtoul(initstr, &str, 16);

		// strtoul will make these the same if it didn't read anything
		if (initstr == str)
		{
			return false;
		}

		str++;
	}

	return true;
}


//------------------------------------------------------------------------------
uint64_t RefObjectId::getHash() const
{
	uint64_t seed = 0;
	wgt::HashUtilities::combine(seed, getA());
	wgt::HashUtilities::combine(seed, getB());
	wgt::HashUtilities::combine(seed, getC());
	wgt::HashUtilities::combine(seed, getD());
	return seed;
}

BinaryStream& operator<<(BinaryStream& stream, const RefObjectId& v)
{
	std::string id = v;
	stream.serializeBuffer(id.c_str(), id.length());
	return stream;
}

BinaryStream& operator>>(BinaryStream& stream, RefObjectId& v)
{
	ResizingMemoryStream dataStream;
	stream.deserializeBuffer(dataStream);
	if (!stream.fail())
	{
		v = dataStream.buffer();
	}
	return stream;
}

TextStream& operator<<(TextStream& stream, const RefObjectId& v)
{
	std::string id = v;
	FixedMemoryStream dataStream(id.c_str(), id.length());
	stream.serializeString(dataStream);
	return stream;
}

TextStream& operator>>(TextStream& stream, RefObjectId& v)
{
	ResizingMemoryStream dataStream;
	stream.deserializeString(dataStream);
	if (stream.fail())
	{
		return stream;
	}

	v = dataStream.buffer();
	return stream;
}

} // end namespace wgt
