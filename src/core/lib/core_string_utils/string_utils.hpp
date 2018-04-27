#ifndef TOOLS_STRING_UTILS_HPP
#define TOOLS_STRING_UTILS_HPP

#include <algorithm>
#include <codecvt>
#include <vector>
#include <sstream>
#include <cctype>

namespace wgt
{
class StringUtils
{
public:
	template <class TStringType, class TDelimiter>
	static std::vector<std::string> split(const TStringType& str, const TDelimiter& delim)
	{
		std::vector<std::string> elems;
		std::stringstream stream(str);
		std::string item;
		while (std::getline(stream, item, delim))
		{
			elems.push_back(item);
		}
		return elems;
	}

	template <class TCollection, class TDelimiter>
	static std::string join(const TCollection& values, const TDelimiter& delim)
	{
		auto iter = std::begin(values);
		auto sentinel = std::end(values);
		if (iter == sentinel)
			return "";
		std::stringstream stream;
		for (; iter < sentinel - 1; ++iter)
		{
			stream << *iter << delim;
		}
		stream << *iter;
		return stream.str();
	}

	static std::string& to_lower(std::string& str)
	{
		std::transform(str.begin(), str.end(), str.begin(), [](char c) {return static_cast<char>(::tolower(c)); });
		return str;
	}

	/**
	* Converts wide string to standard string
	*/
	static std::string to_string(const std::wstring& str);

	/**
	* Converts string to a wide string
	*/
	static std::wstring to_wstring(const std::string& str);

	/**
	* Trims the front/back of the string of whitespace
	*/
	static void trim_string(std::string& str);

	/**
	* Trims the front/back of the string of whitespace
	*/
	static void trim_string(std::wstring& str)
	{
		str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](int ch) {
			return !std::isspace(ch);
		}));
		str.erase(std::find_if(str.rbegin(), str.rend(), [](int ch) {
			return !std::isspace(ch);
		}).base(), str.end());
	}

	/**
	* Trims the specified character from the front/back of the string
	*/
	static void trim_string(std::wstring& str, wchar_t c)
	{
		left_trim_string(str, c);
		right_trim_string(str, c);
	}

	/**
	* Trims the specified character from the front of the string
	*/
	static void left_trim_string(std::wstring& str, wchar_t c)
	{
		str.erase(str.begin(), std::find_if(str.begin(), str.end(), [c](const wchar_t& ch) {
			return ch != c;
		}));
	}
	
	/**
	* Trims the specified character from the back of the string
	*/
	static void right_trim_string(std::wstring& str, wchar_t c)
	{
		str.erase(std::find_if(str.rbegin(), str.rend(), [c](const wchar_t& ch) {
			return ch != c;
		}).base(), str.end());
	}

	/**
	* Erases the first instance of the substring from the string if it exists
	*/
	static bool erase_string(std::string& str, char c);

	/**
	* Erases the first instance of the substring from the string if it exists
	*/
	static bool erase_string(std::string& str, const std::string& substr);

	/**
	* Sorts a container of strings in ascending case-insensitive order
	*/
	static void sort_strings(std::vector<std::string>& vec);

	/**
	* Replaces a substring within str with another string
	*/
	static void replace_string(std::string& str, const std::string& replaceThis, const std::string& withThis);
};
} // end namespace wgt
#endif // TOOLS_STRING_UTILS_HPP
