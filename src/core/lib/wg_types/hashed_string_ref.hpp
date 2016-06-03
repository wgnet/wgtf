#ifndef HASHED_STRING_REF_HPP
#define HASHED_STRING_REF_HPP

#include <functional>

namespace wgt
{
class HashedStringRef
{
public:
	HashedStringRef( const char * str );

	//Getters
	size_t hash() const;

	//Operators
	bool operator == ( const HashedStringRef & other ) const;

private:
	size_t			hash_;
	const char *	pStart_;
	size_t			length_;
};
} // end namespace wgt

namespace std
{

	template<>
	struct hash< wgt::HashedStringRef >
		: public unary_function< const wgt::HashedStringRef, size_t >
	{
		size_t operator()(const wgt::HashedStringRef & s) const
		{
			return s.hash();
		}
	};

}
#endif // HASHED_STRING_REF_HPP
