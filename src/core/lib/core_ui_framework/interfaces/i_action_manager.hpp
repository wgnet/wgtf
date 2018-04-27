#pragma once

namespace wgt
{
class IAction;

class IActionManager
{
public:
	virtual ~IActionManager() = default;

	virtual IAction* findAction(const char* id) const = 0;
};
} // end namespace wgt
