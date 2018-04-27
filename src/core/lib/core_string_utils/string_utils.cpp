#include "string_utils.hpp"

namespace wgt
{
std::string StringUtils::to_string(const std::wstring& str)
{
	static std::wstring_convert<std::codecvt_utf8<wchar_t>> s_WideToStrConverter;
	return s_WideToStrConverter.to_bytes(str);
}

std::wstring StringUtils::to_wstring(const std::string& str)
{
	static std::wstring_convert<std::codecvt_utf8<wchar_t>> s_StrToWideConverter;
	return s_StrToWideConverter.from_bytes(str);
}

void StringUtils::trim_string(std::string& str)
{
	str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](int ch) {
		return !std::isspace(ch);
	}));
	str.erase(std::find_if(str.rbegin(), str.rend(), [](int ch) {
		return !std::isspace(ch);
	}).base(), str.end());
}

bool StringUtils::erase_string(std::string& str, char c)
{
	const auto index = str.find(c);
	if (index != std::string::npos)
	{
		str.erase(index, 1);
		return true;
	}
	return false;
}

bool StringUtils::erase_string(std::string& str, const std::string& substr)
{
	const auto index = str.find(substr);
	if (index != std::string::npos)
	{
		str.erase(index, substr.size());
		return true;
	}
	return false;
}

void StringUtils::sort_strings(std::vector<std::string>& vec)
{
	std::sort(vec.begin(), vec.end(),
		[](const auto& str1, const auto& str2) -> bool
	{
		size_t max = std::min(str1.size(), str2.size());

		for (size_t i = 0; i < max; ++i)
		{
			char char1 = ::tolower(str1[i]);
			char char2 = ::tolower(str2[i]);

			if (char1 == char2)
			{
				continue;
			}

			return char1 < char2;
		}

		return max == str1.length();
	});
}

void StringUtils::replace_string(std::string& str, const std::string& replaceThis, const std::string& withThis)
{
	size_t fromSize = strlen(replaceThis.c_str());
	if (fromSize)
	{
		size_t toSize = strlen(withThis.c_str());
		for (size_t i = 0; i < str.size();)
		{
			size_t found = str.find(replaceThis, i);
			if (found == std::string::npos)
			{
				break;
			}
			str.replace(found, fromSize, withThis);
			i = found + toSize;
		}
	}
}

} // end namespace wgt
