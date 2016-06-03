#ifndef I_COMMAND_LINE_PARSER_HPP
#define I_COMMAND_LINE_PARSER_HPP

#include <string>

namespace wgt
{
class IInterface;

class ICommandLineParser
{
public:
	virtual ~ICommandLineParser() {}
	virtual int argc() const = 0;
	virtual char** argv() const = 0;
	virtual bool getFlag( const char* arg ) const = 0;
	virtual const char* getParam( const char * arg ) const = 0;
	virtual std::string getParamStr( const char * arg ) const = 0;
	virtual std::wstring getParamStrW( const char * arg ) const = 0;
};
} // end namespace wgt
#endif // I_COMMAND_LINE_PARSER_HPP
