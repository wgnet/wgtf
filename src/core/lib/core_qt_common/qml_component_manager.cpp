#include "qml_component_manager.hpp"

#include "qml_component.hpp"

#include "core_ui_framework/i_component_provider.hpp"

namespace wgt
{
void QmlComponentManager::registerComponent(
	const char* id, const char* version, IComponent& component,
	bool supportsAsync)
{
	auto& components = components_[id];

	auto tokens = ComponentVersion::tokenise(version);
	auto it = components.find(tokens);
	if (it != components.end())
	{
		return;
	}

	components[tokens] = &component;
	if (supportsAsync == false )
	{
		noAsync_.insert(&component);
	}
}

void QmlComponentManager::registerComponentProvider(IComponentProvider& provider)
{
	componentProviders_.push_back(&provider);
}

IComponent* QmlComponentManager::findComponent(const TypeId& typeId,
                                               std::function<bool(const ItemRole::Id&)>& predicate,
                                               const char* version) const
{
	auto tokens = ComponentVersion::tokenise(version);

	for (auto componentProviderIt = componentProviders_.rbegin(); componentProviderIt != componentProviders_.rend();
	     ++componentProviderIt)
	{
		auto id = (*componentProviderIt)->componentId(typeId, predicate);
		if (id == nullptr)
		{
			continue;
		}

		auto componentsIt = components_.find(id);
		if (componentsIt == components_.end())
		{
			continue;
		}

		auto& components = componentsIt->second;

		if (components.empty())
		{
			continue;
		}

		auto componentIt = components.upper_bound(tokens);
		--componentIt;

		return componentIt->second;
	}
	return nullptr;
}

IComponent* QmlComponentManager::findComponent(const char* componentId, const char* version) const
{
	auto tokens = ComponentVersion::tokenise(version);

	auto componentsIt = components_.find(componentId);
	if (componentsIt == components_.end())
	{
		return nullptr;
	}

	auto& components = componentsIt->second;

	if (components.empty())
	{
		return nullptr;
	}

	auto componentIt = components.upper_bound(tokens);
	--componentIt;

	return componentIt->second;
}

QQmlComponent* QmlComponentManager::toQmlComponent(IComponent& component) const
{
	// TODO replace this with a proper UI adapter interface
	auto qmlComponent = dynamic_cast<QmlComponent*>(&component);
	if (qmlComponent != nullptr)
	{
		return qmlComponent->component();
	}

	return nullptr;
}

bool QmlComponentManager::supportsAsync(IComponent & component) const
{
	bool noAsync = noAsync_.find( &component ) != noAsync_.end();
	return !noAsync;
}

}
