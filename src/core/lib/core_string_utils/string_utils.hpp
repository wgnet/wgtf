#ifndef TOOLS_STRING_UTILS_HPP
#define TOOLS_STRING_UTILS_HPP

#include <codecvt>
#include <vector>
#include <sstream>

namespace wgt
{
class Utf16to8Facet
	: public std::codecvt_utf8< wchar_t >
{
private:
	Utf16to8Facet() { }
public:
	static Utf16to8Facet * create()
	{
#if ( ( _MSC_VER == 1700 ) && _DEBUG )
		//Lame workaround because the VC2012 compiler assumes that all 
		//allocations come from the CRT
		//https://connect.microsoft.com/VisualStudio/feedback/details/750951/std-locale-implementation-in-crt-assumes-all-facets-to-be-allocated-on-crt-heap-and-crashes-in-destructor-in-debug-mode-if-a-facet-was-allocated-by-a-custom-allocator
		return _NEW_CRT( Utf16to8Facet );
#else
		return new Utf16to8Facet();
#endif
	}
};

class StringUtils
{
public:
	template<class TStringType, class TDelimiter>
	static std::vector<std::string> split(const TStringType& str, const TDelimiter& delim)
	{
		std::vector<std::string> elems;
		std::stringstream stream(str);
		std::string item;
		while ( std::getline(stream, item, delim) ) {
			elems.push_back(item);
		}
		return elems;
	}

	template<class TDelimiter>
	static std::string join(const std::vector<std::string>& strings, const TDelimiter& delim)
	{
		std::stringstream stream;
		for ( auto& str : strings )
		{
			stream << str << delim;
		}
		return stream.str();
	}

};
} // end namespace wgt
#endif //TOOLS_STRING_UTILS_HPP
