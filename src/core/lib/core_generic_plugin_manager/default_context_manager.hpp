#ifndef DEFAULT_COMPONENT_CONTEXT_HPP
#define DEFAULT_COMPONENT_CONTEXT_HPP

#include "core_common/wg_read_write_lock.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_variant/type_id.hpp"

#include <set>
#include <map>
#include <memory>

namespace wgt
{
class RTTIHelper;

class DefaultComponentContext : public IComponentContext
{
public:
	explicit DefaultComponentContext(const std::wstring& name, IComponentContext* parentContext = NULL);
	~DefaultComponentContext();

	InterfacePtr registerInterfaceImpl(const TypeId&, InterfacePtr pImpl, ContextRegState regState) override;

	bool deregisterInterface(IInterface* typeId) override;

	void* queryInterface(const TypeId&) override;

	void queryInterface(const TypeId&, std::vector<void*>& o_Impls) override;

	ConnectionHolder registerListener(IComponentContextListener& listener) override;

private:
	virtual void onInterfaceRegistered(InterfaceCaster&) override;
	virtual void onInterfaceDeregistered(InterfaceCaster&) override;
	void deregisterListener(IComponentContextListener& listener);

	void onInterfaceRegistered(
		InterfaceCaster& caster,
		std::set< IComponentContextListener * > & called);
	void triggerCallbacks(IComponentContextListener& listener) override;
	wg_read_write_lock lock_;
	using InterfaceMap = std::multimap<const TypeId, std::shared_ptr<RTTIHelper>>;
	InterfaceMap interfaces_;
	std::set<InterfacePtr> registeredInterfaces_;
	IComponentContext* parentContext_;
	using ComponentContextListeners = std::vector<IComponentContextListener*>;
	ComponentContextListeners listeners_;
	std::wstring name_;
	std::shared_ptr<std::function<void(IComponentContextListener&)>> disconnectSig_;
};
} // end namespace wgt
#endif
