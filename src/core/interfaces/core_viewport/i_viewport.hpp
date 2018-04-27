#pragma once

#include <string>

namespace wgt
{
class IView;
class IComponentContext;

class IViewport
{
public:
	virtual ~IViewport()
	{
	}

	/**
	* Default hint tag for a viewport
	*/
	static const char* viewportTag()
	{
		return "viewport";
	}

	/**
	* @return the id for the viewport
	*/
	virtual std::string getId() const = 0;

	/**
	* @return the viewport name
	*/
	virtual const std::string& getName() const = 0;

	/**
	* On viewport finished loading
	*/
	virtual void onViewportLoaded(IView& view) = 0;

	/**
	* On viewport selected
	*/
	virtual void onSelected(bool selected) = 0;

	/**
	* Resets the viewport
	*/
	virtual void reset() = 0;

	/**
	* Initialises the viewport
	*/
	virtual void initialise(const std::string& id) = 0;

	/**
	* Finalises the viewport
	*/
	virtual void finalise() = 0;

	virtual uintptr_t getNativeWindowHandle()
	{
		return 0;
	}
};
}
