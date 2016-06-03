#ifndef TEXT_UTILITIES_HPP
#define TEXT_UTILITIES_HPP

#include <string>

namespace wgt
{
namespace TextUtilities
{
	void incrementNumber(
		std::wstring & text, int & currentPos, wchar_t decimalChar = L'.');
	void decrementNumber(
		std::wstring & text, int & currentPos, wchar_t decimalChar = L'.' );
};
} // end namespace wgt
#endif
