#ifndef I_COMMAND_LINE_PARSER_HPP
#define I_COMMAND_LINE_PARSER_HPP

#include <string>

namespace wgt
{
class IInterface;

class ICommandLineParser
{
public:
	virtual ~ICommandLineParser()
	{
	}

	/**
	 *	Note: argc and argv might be changed as Qt removes command line
	 *	arguments that it recognizes.
	 *	@see http://doc.qt.io/qt-5/qapplication.html#QApplication
	 *	@return non-const reference to allow modification.
	 */
	virtual int& argc() = 0;
	/**
	 *	Note: argc and argv might be changed as Qt removes command line
	 *	arguments that it recognizes.
	 *	@see http://doc.qt.io/qt-5/qapplication.html#QApplication
	 *	@return non-const reference to allow modification.
	 */
	virtual char** argv() = 0;
	virtual bool getFlag(const char* arg) const = 0;
	virtual const char* getParam(const char* arg) const = 0;
	virtual std::string getParamStr(const char* arg) const = 0;
	virtual std::wstring getParamStrW(const char* arg) const = 0;
};
} // end namespace wgt
#endif // I_COMMAND_LINE_PARSER_HPP
