#include "component_extension.hpp"

#include "core_variant/type_id.hpp"
#include "core_data_model/i_item_role.hpp"
#include "core_data_model/common_data_roles.hpp"
#include "core_qt_common/qt_framework_common.hpp"
#include "core_qt_common/qml_component_manager.hpp"

#include <QQmlComponent>
#include <QModelIndex>
#include <algorithm>

namespace wgt
{
ITEMROLE(component)
ITEMROLE(componentSupportsAsync)

ComponentExtension::ComponentExtension()
{
	roles_.push_back(ItemRole::componentName);
	roles_.push_back(ItemRole::componentSupportsAsyncName);
}

ComponentExtension::~ComponentExtension()
{
}

QVariant ComponentExtension::data(const QModelIndex& index, ItemRole::Id roleId) const
{
	static const auto latest_version = "2.0";

	if (roleId != ItemRole::componentId &&
		roleId != ItemRole::componentSupportsAsyncId)
	{
		if (roleId == ItemRole::componentSupportsAsyncId)
		{
			return false;
		}
		return QVariant::Invalid;
	}

	const QmlComponentManager* qmlComponentManager = frameworkCommon().qmlComponentManager();
	if (qmlComponentManager == nullptr)
	{
		if (roleId == ItemRole::componentSupportsAsyncId)
		{
			return false;
		}
		return QVariant::Invalid;
	}

	IComponent* component = nullptr;
	auto componentType = extensionData_->data(index, ItemRole::componentTypeId);
	if (componentType.isValid())
	{
		auto componentTypeName = std::string(componentType.toString().toUtf8());
		component = qmlComponentManager->findComponent(componentTypeName.c_str(), latest_version);
	}

	if (component == nullptr)
	{
		auto data = extensionData_->data(index, ItemRole::valueTypeId);
		auto typeName = std::string(data.toString().toUtf8());
		auto typeId = TypeId(typeName.c_str());
		std::function<bool(const ItemRole::Id&)> predicate = [&](const ItemRole::Id& roleId) {
			return extensionData_->data(index, roleId) == true;
		};

		component = qmlComponentManager->findComponent(typeId, predicate, latest_version);
		if (component == nullptr)
		{
			if (roleId == ItemRole::componentSupportsAsyncId)
			{
				return false;
			}
			return QVariant::Invalid;
		}
	}

	if (roleId == ItemRole::componentSupportsAsyncId)
	{
		return qmlComponentManager->supportsAsync(*component);
	}

	auto qmlComponent = qmlComponentManager->toQmlComponent(*component);
	if (qmlComponent == nullptr)
	{
		return QVariant::Invalid;
	}

	return QVariant::fromValue<QObject*>(qmlComponent);
}
} // end namespace wgt
