#pragma once

#include <string>

namespace wgt
{

/**
 *	Gets the icon for a given file.
 */
class IFileIconProvider
{
public:
	/**
	 *	Load the icon for the given file and return a URL to the image.
	 *	@param filePath the absolute file name with native directory
	 *		separators.
	 *	@return URL for the loaded icon,
	 *		on an empty string on failure.
	 */
	virtual std::wstring encodeImage(const std::wstring& filePath) = 0;
};
} // end namespace wgt

