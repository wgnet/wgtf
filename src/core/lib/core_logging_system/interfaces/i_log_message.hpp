#pragma once

#include "core_logging/log_level.hpp"
#include <string>

namespace wgt
{
class ILogMessage
{
public:
	virtual ~ILogMessage() = default;

	virtual const char* c_str() const = 0;
	virtual const std::string& str() const = 0;

	virtual LogLevel getLevel() const = 0;
	virtual const char* getLevelString() const = 0;

	virtual std::string getAsHtml() const = 0;

	virtual bool addTag(std::string tag) = 0;
	virtual bool hasTag(const char* needle) const = 0;

};
} // end namespace wgt
