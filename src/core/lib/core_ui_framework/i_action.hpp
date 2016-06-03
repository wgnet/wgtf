#ifndef I_ACTION_HPP
#define I_ACTION_HPP

#include <vector>

namespace wgt
{
class Variant;

class IAction
{
public:
	virtual ~IAction() {}

	virtual const char * text() const = 0;
	virtual const char * icon() const = 0;
	virtual const char * windowId() const = 0;
	virtual const std::vector<std::string>& paths() const = 0;
	virtual const char * shortcut() const = 0;

	virtual bool enabled() const = 0;
	virtual bool checked() const  = 0;
	virtual bool isCheckable() const = 0;
	virtual void execute() = 0;
	
	virtual void setData( const Variant& ) = 0;
	virtual Variant& getData() = 0;
	virtual const Variant& getData() const = 0;
};
} // end namespace wgt
#endif//I_ACTION_HPP
