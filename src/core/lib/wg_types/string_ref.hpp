#ifndef STRING_REF_HPP
#define STRING_REF_HPP

#include <string>

///
/// TODO: Simpler copy of String_Ref, merge this once wg_types is created
///
namespace wgt
{
class StringRef
{
public:
	typedef size_t size_type;

	StringRef( const char * str );
	StringRef( const char * str, size_type length );
	StringRef( const std::string & str );

	//Accessors
	const char* data() const;
	size_type length() const;

	//Operators
	bool operator == ( const StringRef & other ) const;

private:
	const char *	pStart_;
	size_t			length_;
};
} // end namespace wgt
#endif // STRING_REF_HPP
