#ifndef I_WINDOW_ADAPTER_HPP
#define I_WINDOW_ADAPTER_HPP

#include "core_dependency_system/i_interface.hpp"
#include "i_window_listener.hpp"

namespace wgt
{
class IWindowAdapter
{
public:
	virtual ~IWindowAdapter() {}

	virtual void * nativeWindowId() const = 0;
	virtual void makeFramelessWindow() = 0;
	virtual void addListener( IWindowListener * listener ) = 0;
	virtual void removeListener( IWindowListener * listener ) = 0;
};
} // end namespace wgt
#endif I_WINDOW_ADAPTER_HPP
