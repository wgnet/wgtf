#pragma once

#include "core_data_model/i_item_role.hpp"

#include "core_qt_common/private/component_version.hpp"

#include <map>
#include <vector>
#include <set>
#include <memory>
#include <functional>

class QQmlComponent;

namespace wgt
{
class TypeId;
class IComponent;
class IComponentProvider;

class QmlComponentManager
{
public:
	void registerComponent(const char* id, const char* version, IComponent& component, bool supportsAsync );
	void registerComponentProvider(IComponentProvider& provider);

	IComponent* findComponent(const TypeId& typeId, std::function<bool(const ItemRole::Id&)>& predicate,
	                          const char* version) const;
	IComponent* findComponent(const char* componentId, const char* version) const;

	QQmlComponent* toQmlComponent(IComponent& component) const;

	bool supportsAsync(IComponent & component ) const;
private:
	std::map<std::string, std::map<std::vector<int>, IComponent*, ComponentVersion>> components_;
	std::vector<IComponentProvider*> componentProviders_;
	std::set< IComponent * > noAsync_;
};
}
