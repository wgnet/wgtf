#ifndef PLUGIN_CONTEXT_MANAGER_HPP
#define PLUGIN_CONTEXT_MANAGER_HPP

#include "core_generic_plugin/interfaces/i_plugin_context_manager.hpp"
#include "core_dependency_system/i_interface.hpp"
#include "core_generic_plugin/interfaces/i_component_context_creator.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"

#include <map>
#include <memory>
#include <string>

namespace wgt
{
class PluginContextManager : public Implements<IPluginContextManager>, public IComponentContextListener
{
	typedef std::vector<IComponentContextCreator*> ContextCreatorCollection;

public:
	PluginContextManager();
	virtual ~PluginContextManager();

	IComponentContext* createContext(const PluginId& id, const std::wstring& path) override;
	IComponentContext* getContext(const PluginId& id) const override;
	IComponentContext* getGlobalContext() const override;
	void destroyContext(const PluginId& id) override;

	virtual void onContextCreatorRegistered(IComponentContextCreator*) override;
	virtual void onContextCreatorDeregistered(IComponentContextCreator*) override;

	void setExecutablePath(const char* path) override;
	const char* getExecutablePath() const override;

private:
	typedef std::vector<IInterface*> InterfaceCollection;
	typedef std::map<IComponentContextCreator*, InterfaceCollection> ContextChildrenCollection;
	ContextChildrenCollection childContexts_;
	std::map<PluginId, std::pair<IComponentContext*, std::wstring>> contexts_;
	std::map<std::string, IComponentContextCreator*> contextCreators_;
	std::unique_ptr<IComponentContext> globalContext_;
	const char* executablepath_;
};
} // end namespace wgt
#endif // PLUGIN_CONTEXT_MANAGER_HPP
