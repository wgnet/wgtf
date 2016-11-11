#ifndef I_FILE_UTILITIES_HPP
#define I_FILE_UTILITIES_HPP

namespace wgt
{
class StringRef;

class IFileUtilities
{
protected:
	virtual ~IFileUtilities()
	{
	}

public:
	virtual StringRef getFilename(const StringRef&) = 0;
	virtual StringRef getExtension(const StringRef&) = 0;
	virtual StringRef removeExtension(const StringRef&) = 0;
	virtual bool fileExists(const StringRef& file) = 0;

	virtual std::string resolveFilename(const StringRef& file) = 0;
};
} // end namespace wgt
#endif
