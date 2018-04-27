#pragma once

#include "core_ui_framework/input_handler.hpp"
#include "core_variant/variant.hpp"
#include <unordered_map>
#include <functional>

namespace wgt
{
struct IGrabber;

struct IGrabberManager : public InputHandler
{
	virtual ~IGrabberManager() {}
	
	virtual void registerGrabber(IGrabber* grabber,
								 std::function<bool()> iconVisibleFn = nullptr) = 0;

	virtual void deregisterGrabber(IGrabber* grabber) = 0;
	virtual void refreshGrabber(IGrabber* grabber) = 0;

	virtual void draw() = 0;
	virtual void postDraw() = 0;
	virtual void postLoad() = 0;
	virtual void init() = 0;
	virtual void fini() = 0;
};
} // end namespace wgt
