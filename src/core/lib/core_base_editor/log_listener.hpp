#include <memory>
#include <vector>
#include <array>
#include "core_logging_system/log_level.hpp"
#include "core_logging_system/log_message.hpp"

namespace wgt
{
class LogListener final
{
public:
	typedef std::vector<LogMessage> LogReport;
	LogListener();
	LogListener(const LogListener&);
	LogListener(LogListener&&) = default;
	LogListener& operator=(const LogListener&);
	LogListener& operator=(LogListener&&) = default;
	~LogListener();
	void startListening(LogLevel logLevel = LogLevel::LOG_ERROR);
	LogReport stopListening();
	bool isListening() const;
private:
	struct Impl;
	std::unique_ptr<Impl> impl_;
};
}