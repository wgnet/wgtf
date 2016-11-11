#include "string_ref.hpp"
#include <cstring>

namespace wgt
{
//------------------------------------------------------------------------------
StringRef::StringRef(const char* str) : pStart_(str), length_(strlen(pStart_))
{
}

//------------------------------------------------------------------------------
StringRef::StringRef(const char* str, size_type length) : pStart_(str), length_(length)
{
}

//------------------------------------------------------------------------------
StringRef::StringRef(const std::string& str) : pStart_(str.c_str()), length_(str.length())
{
}

//------------------------------------------------------------------------------
const char* StringRef::data() const
{
	return pStart_;
}

//------------------------------------------------------------------------------
StringRef::size_type StringRef::length() const
{
	return length_;
}

//------------------------------------------------------------------------------
bool StringRef::operator==(const StringRef& other) const
{
	if (length_ != other.length_)
	{
		return false;
	}
	return memcmp(pStart_, other.pStart_, length_) == 0;
}
} // end namespace wgt
