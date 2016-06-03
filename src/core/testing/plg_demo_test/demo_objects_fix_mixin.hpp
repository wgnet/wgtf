#ifndef DEMO_OBJECTS_FIX_MIXIN_HPP
#define DEMO_OBJECTS_FIX_MIXIN_HPP

#include "demo_objects.hpp"

namespace wgt
{
/**
 * This class exists only to wrap the default DemoObjects 
 * class to work around a bug in Canvas3D where a render call is 
 * queued and the GLContext is destroyed to the hiding the window
 * by adding a new tab.
 * Can be safely removed once the Qt guys fix this.
 */
class DemoObjectsFixMixIn : public DemoObjects
{
public:
	DemoObjectsFixMixIn();

	void componentComplete( Variant userData);
	bool shouldInitializeGL();

private:
	bool valid_;
};
} // end namespace wgt
#endif //DEMO_OBJECTS_HPP
