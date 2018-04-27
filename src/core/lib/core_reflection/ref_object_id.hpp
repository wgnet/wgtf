#ifndef REF_OBJECT_ID_HPP
#define REF_OBJECT_ID_HPP

#include <string>
#include "reflection_dll.hpp"
#include "core_variant/variant.hpp"

namespace wgt
{
class REFLECTION_DLL RefObjectId
{
private:
	// NOTE: Structure must be compatible with Microsoft's GUID structure since
	// we cast to GUID in one of functions.
	unsigned int a_;
	unsigned int b_;
	unsigned int c_;
	unsigned int d_;

public:
	RefObjectId();
	RefObjectId(const RefObjectId& other);
	RefObjectId(const std::string& s);
	RefObjectId(unsigned int a, unsigned int b, unsigned int c, unsigned int d) : a_(a), b_(b), c_(c), d_(d)
	{
	}

	unsigned int getA() const
	{
		return a_;
	}
	unsigned int getB() const
	{
		return b_;
	}
	unsigned int getC() const
	{
		return c_;
	}
	unsigned int getD() const
	{
		return d_;
	}

	std::string toString() const
	{
		return *this;
	}
	operator std::string() const;

	std::wstring toWString() const;

	bool operator==(const RefObjectId& rhs) const;
	bool operator!=(const RefObjectId& rhs) const;
	bool operator<(const RefObjectId& rhs) const;

	static RefObjectId generate();
	static const RefObjectId& zero();

	uint64_t getHash() const;
private:
	static bool fromString(const std::string& s, unsigned int* data);
};

REFLECTION_DLL BinaryStream& operator<<(BinaryStream& stream, const RefObjectId& v);
REFLECTION_DLL BinaryStream& operator>>(BinaryStream& stream, RefObjectId& v);
REFLECTION_DLL TextStream& operator<<(TextStream& stream, const RefObjectId& v);
REFLECTION_DLL TextStream& operator>>(TextStream& stream, RefObjectId& v);
} // end namespace wgt

namespace std
{
template <>
struct hash<const wgt::RefObjectId> : public unary_function<const wgt::RefObjectId, uint64_t>
{
	uint64_t operator()(const wgt::RefObjectId& v) const
	{
		return v.getHash();
	}
};
}

META_TYPE_NAME(wgt::RefObjectId, "refObjectId")
#endif // REF_OBJECT_ID_HPP
