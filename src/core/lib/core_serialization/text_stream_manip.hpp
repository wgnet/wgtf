#ifndef TEXT_STREAM_MANIP_HPP_INCLUDED
#define TEXT_STREAM_MANIP_HPP_INCLUDED

#include <string>
#include <utility>

#include "serialization_dll.hpp"

namespace wgt
{
class TextStream;

namespace text_stream_manip_details
{
class CStringMatcher
{
public:
	explicit CStringMatcher(const char* pattern) : pattern_(pattern), size_(strlen(pattern))
	{
	}

	CStringMatcher(const char* pattern, size_t size) : pattern_(pattern), size_(size)
	{
	}

	const char* pattern() const
	{
		return pattern_;
	}

	size_t size() const
	{
		return size_;
	}

private:
	const char* pattern_;
	size_t size_;
};

SERIALIZATION_DLL TextStream& operator>>(TextStream& stream, const CStringMatcher& value);

class StringMatcher
{
public:
	explicit StringMatcher(std::string pattern) : pattern_(std::move(pattern))
	{
	}

	const std::string& pattern() const
	{
		return pattern_;
	}

private:
	std::string pattern_;
};

SERIALIZATION_DLL TextStream& operator>>(TextStream& stream, const StringMatcher& value);

class CharMatcher
{
public:
	explicit CharMatcher(char pattern) : pattern_(pattern)
	{
	}

	char pattern() const
	{
		return pattern_;
	}

private:
	char pattern_;
};

SERIALIZATION_DLL TextStream& operator>>(TextStream& stream, const CharMatcher& value);
}

/**
Utility class used to check input stream for exact character(s).
*/
inline text_stream_manip_details::CStringMatcher match(const char* pattern)
{
	return text_stream_manip_details::CStringMatcher(pattern);
}

/**
@overload
*/
inline text_stream_manip_details::CStringMatcher match(const char* pattern, size_t size)
{
	return text_stream_manip_details::CStringMatcher(pattern, size);
}

/**
@overload
*/
inline text_stream_manip_details::CStringMatcher match(const char* begin, const char* end)
{
	return text_stream_manip_details::CStringMatcher(begin, end - begin);
}

/**
@overload
*/
inline text_stream_manip_details::CStringMatcher match(const std::string& pattern)
{
	return text_stream_manip_details::CStringMatcher(pattern.c_str(), pattern.size());
}

/**
@overload
*/
inline text_stream_manip_details::StringMatcher match(std::string&& pattern)
{
	return text_stream_manip_details::StringMatcher(std::move(pattern));
}

/**
@overload
*/
inline text_stream_manip_details::CharMatcher match(char pattern)
{
	return text_stream_manip_details::CharMatcher(pattern);
}

namespace text_stream_manip_details
{
/**
    Utility class used for quoted string serialization.
    */
class QuotedCStr
{
public:
	QuotedCStr(const char* str, size_t size) : c_str_(str), size_(size)
	{
	}

	const char* c_str() const
	{
		return c_str_;
	}

	size_t size() const
	{
		return size_;
	}

private:
	const char* c_str_;
	size_t size_;
};

SERIALIZATION_DLL TextStream& operator<<(TextStream& stream, const QuotedCStr& value);

/**
Utility class used for quoted string (de)serialization.
*/
class QuotedStr
{
public:
	explicit QuotedStr(std::string& str) : str_(&str)
	{
	}

	std::string& str()
	{
		return *str_;
	}

	const std::string& str() const
	{
		return *str_;
	}

private:
	std::string* str_;
};

SERIALIZATION_DLL TextStream& operator<<(TextStream& stream, const QuotedStr& value);
SERIALIZATION_DLL TextStream& operator>>(TextStream& stream, QuotedStr value);
}

/**
Replacement for C++14 std::quoted() function.

Used to (de)serialize string with quotes, which is useful if string may have
arbitrary contents including spaces, newlines, and special chars.
*/
inline text_stream_manip_details::QuotedCStr quoted(const char* str, size_t size)
{
	return text_stream_manip_details::QuotedCStr(str, size);
}

/**
@overload
*/
inline text_stream_manip_details::QuotedCStr quoted(const char* str)
{
	return text_stream_manip_details::QuotedCStr(str, str ? std::strlen(str) : 0);
}

/**
@overload
*/
inline text_stream_manip_details::QuotedCStr quoted(const std::string& str)
{
	return text_stream_manip_details::QuotedCStr(str.c_str(), str.size());
}

/**
@overload
*/
inline text_stream_manip_details::QuotedStr quoted(std::string& str)
{
	return text_stream_manip_details::QuotedStr(str);
}
} // end namespace wgt
#endif
