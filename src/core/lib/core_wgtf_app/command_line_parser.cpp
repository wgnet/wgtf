#include "command_line_parser.hpp"
#include <locale>
#include <codecvt>

namespace wgt
{
//==============================================================================
CommandLineParser::CommandLineParser(int argc, char** argv) : argc_(argc), argv_(argv)
{
}

//==============================================================================
int& CommandLineParser::argc()
{
	return argc_;
}

//==============================================================================
char** CommandLineParser::argv()
{
	return argv_;
}

//==============================================================================
bool CommandLineParser::getFlag(const char* arg) const
{
	auto argLen = ::strlen(arg);
	for (int i = 0; i < argc_; ++i)
	{
		if (argv_[i] && ::strlen(argv_[i]) == argLen && ::strncmp(argv_[i], arg, argLen) == 0)
		{
			return true;
		}
	}
	return false;
}

//==============================================================================
const char* CommandLineParser::getParam(const char* arg) const
{
	auto argLen = ::strlen(arg);
	for (int i = 0; i < argc_ - 1; ++i)
	{
		if (argv_[i] && ::strlen(argv_[i]) == argLen && ::strncmp(argv_[i], arg, argLen) == 0)
		{
			return argv_[i + 1];
		}
	}
	return nullptr;
}

//==============================================================================
std::string CommandLineParser::getParamStr(const char* arg) const
{
	auto param = getParam(arg);
	if (param != nullptr)
	{
		return param;
	}
	return "";
}

//==============================================================================
std::wstring CommandLineParser::getParamStrW(const char* arg) const
{
	auto param = getParam(arg);
	if (param != nullptr)
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
		return conv.from_bytes(param);
	}
	return L"";
}
} // end namespace wgt
