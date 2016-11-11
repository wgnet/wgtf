#ifndef COMMAND_LINE_PARSER_HPP
#define COMMAND_LINE_PARSER_HPP

#include "core_generic_plugin/interfaces/i_command_line_parser.hpp"
#include "core_dependency_system/i_interface.hpp"

#include <string>

namespace wgt
{
class CommandLineParser : public Implements<ICommandLineParser>
{
public:
	CommandLineParser(int argc, char** argv);

	virtual int& argc() override;
	virtual char** argv() override;
	virtual bool getFlag(const char* arg) const override;
	virtual const char* getParam(const char* arg) const override;
	virtual std::string getParamStr(const char* arg) const override;
	virtual std::wstring getParamStrW(const char* arg) const override;

private:
	int argc_;
	char** argv_;
};
} // end namespace wgt
#endif // COMMAND_LINE_PARSER_HPP
