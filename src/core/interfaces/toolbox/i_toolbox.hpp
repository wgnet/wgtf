#pragma once

#include <core_common/signal.hpp>

#include <vector>
#include <functional>

namespace wgt
{
class ITool;
template <typename T>
class ObjectHandleT;

/**
 * IToolbox interface manages a collection of viewport tools.
**/
class IToolbox
{
public:
	virtual ~IToolbox()
	{
	}

	/**
	 * Registers tool, ownership is not transferred.
	 * @param tool ObjectHandleT<ITool> with the ITool implementation, nullptr will be ignored.
	**/
	virtual void registerTool(ObjectHandleT<ITool> tool) = 0;

	/**
	 * Deregisters tool.
	 * @param tool ObjectHandleT<ITool> with the ITool implementation.
	**/
	virtual void deregisterTool(ObjectHandleT<ITool> tool) = 0;

	/**
	* Sets the tool that should always be active when no tool is selected. Must already be registered.
	* @param tool ObjectHandleT<ITool> with the ITool implementation.
	* @return True on success (tool is registered)
	**/
	virtual bool setDefaultTool(ObjectHandleT<ITool> tool) = 0;

	/**
	* Returns the default tool.
	* @return Default tool or ObjectHandle initialised to nullptr
	**/
	virtual ObjectHandleT<ITool> getDefaultTool() = 0;

	/**
	 * Sets active tool, it would fail if tool is not registered.
	 * @param tool ObjectHandleT<ITool> with the ITool implementation.
	 * @return True on success.
	**/
	virtual bool setActiveTool(ObjectHandleT<ITool> tool) = 0;

	/**
	 * Returns active tool.
	 * @return Current active tool or nullptr.
	**/
	virtual ObjectHandleT<ITool> getActiveTool() const = 0;

	/**
	* Returns the tool specified by the given id.
	* @return Current active tool or nullptr.
	**/
	virtual ObjectHandleT<ITool> getTool(uint64_t id) const = 0;

	/**
	 * Returns list of all registered tools.
	 * @return vector of pointers to tools.
	**/
	virtual std::vector<ObjectHandleT<ITool>> getAllTools() const = 0;

	/**
	 * Returns default icon path.
	 * @return Default icon url.
	**/
	virtual const char* getDefaultIcon() const = 0;

	/**
	 * Sets default icon path.
	 * @param icon Default icon url.
	**/
	virtual void setDefaultIcon(const char* icon) = 0;

	/**
	 * Switches to toolbox panel.
	**/
	virtual void activatePanel() = 0;

	/**
	 * Inits toolbox.
	**/
	virtual void init() = 0;

	/**
	 * Finalizes toolbox.
	**/
	virtual void fini() = 0;

	typedef void VoidSignature(void);
	typedef std::function<VoidSignature> ToolsetChangedCallback;
	virtual Connection connectToolsetChanged(ToolsetChangedCallback callback)
	{
		return Connection();
	}

	typedef void ActiveToolSignature(ObjectHandleT<ITool> activeTool);
	typedef std::function<ActiveToolSignature> ActiveToolChangedCallback;
	virtual Connection connectActiveToolChanged(ActiveToolChangedCallback callback)
	{
		return Connection();
	}
};
}
