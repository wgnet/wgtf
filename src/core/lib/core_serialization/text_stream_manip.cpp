#include "text_stream_manip.hpp"
#include "text_stream.hpp"
#include "fixed_memory_stream.hpp"
#include "resizing_memory_stream.hpp"

namespace wgt
{
namespace text_stream_manip_details
{
TextStream& operator>>(TextStream& stream, const CStringMatcher& value)
{
	if (!stream.beginReadField())
	{
		return stream;
	}

	for (size_t i = 0; i != value.size() && stream.good(); i += 1)
	{
		if (stream.get() != value.pattern()[i])
		{
			// pattern mismatch
			stream.setState(std::ios_base::failbit);
			break;
		}
	}

	return stream;
}

TextStream& operator>>(TextStream& stream, const StringMatcher& value)
{
	stream >> CStringMatcher(value.pattern().c_str(), value.pattern().size());
	return stream;
}

TextStream& operator>>(TextStream& stream, const CharMatcher& value)
{
	if (!stream.beginReadField())
	{
		return stream;
	}

	if (stream.get() != value.pattern())
	{
		// pattern mismatch
		stream.setState(std::ios_base::failbit);
	}

	return stream;
}

TextStream& operator<<(TextStream& stream, const QuotedCStr& value)
{
	FixedMemoryStream src(value.c_str(), value.size());
	stream.serializeString(src);
	return stream;
}

TextStream& operator<<(TextStream& stream, const QuotedStr& value)
{
	FixedMemoryStream src(value.str().c_str(), value.str().size());
	stream.serializeString(src);
	return stream;
}

TextStream& operator>>(TextStream& stream, QuotedStr value)
{
	ResizingMemoryStream dst;
	stream.deserializeString(dst);
	if (!stream.fail())
	{
		value.str() = dst.takeBuffer();
	}
	return stream;
}
}
} // end namespace wgt
