#pragma once

#include <core_reflection/reflected_object.hpp>
#include <core_ui_framework/input_handler.hpp>
#include <core_common/assert.hpp>
#include <core_common/signal.hpp>
#include <functional>

namespace wgt
{
class ITool : public InputHandler
{
	DECLARE_REFLECTED;

public:
	virtual ~ITool(){};

	/**
	 * Checks if tool can be activated.
	 * @return True if tool can be activated.
	**/
	virtual bool canActivate() const = 0;

	/**
	 * Called on tool deactivation.
	**/
	virtual void deactivate() = 0;

	/**
	 * Called on tool activation.
	**/
	virtual void activate() = 0;

	/**
	 * Called on IUIApplication::signalUpdate.
	**/
	virtual void update() = 0;

	/**
	 * Called every frame on active tool.
	**/
	virtual void draw() = 0;

	/**
	 * Get the identifier for this tool.
	**/
	virtual uint64_t id() const = 0 { TF_ASSERT(!"Must override id"); return 0; }

	typedef void CanActivateSignature(bool canActivate);
	typedef std::function<CanActivateSignature> CanActivateChangedCallback;
	virtual Connection connectCanActivateChanged(CanActivateChangedCallback callback)
	{
		return Connection();
	}
};
}
