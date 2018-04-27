#ifndef STRING_REF_HPP
#define STRING_REF_HPP

#include <string>
#include "hash_utilities.hpp"

///
/// TODO: Simpler copy of String_Ref, merge this once wg_types is created
///
namespace wgt
{
class StringRef
{
public:
	typedef size_t size_type;

	StringRef(const char* str);
	StringRef(const char* str, size_type length);
	StringRef(const std::string& str);

	// Accessors
	const char* data() const;
	size_type length() const;

	// Operators
	bool operator==(const StringRef& other) const;

	bool operator!=(const StringRef& other) const
	{
		return !(*this == other);
	}

private:
	const char* pStart_;
	size_t length_;
};
} // end namespace wgt

namespace std
{
template <>
struct hash<wgt::StringRef> : public unary_function<wgt::StringRef, size_t>
{
public:
	size_t operator()(const wgt::StringRef& v) const
	{
		return static_cast<size_t>(wgt::HashUtilities::compute(v.data(), v.length()));
	}
};
}

#endif // STRING_REF_HPP
