#include "component_extension.hpp"
#include "core_data_model/i_item_role.hpp"
#include "core_data_model/common_data_roles.hpp"
#include "core_qt_common/i_qt_framework.hpp"

#include <QQmlComponent>
#include <QModelIndex>
#include <algorithm>

namespace wgt
{
ITEMROLE(component)

ComponentExtension::ComponentExtension() : qtFramework_(Context::queryInterface<IQtFramework>())
{
	roles_.push_back(ItemRole::componentName);
}

ComponentExtension::~ComponentExtension()
{
}

QVariant ComponentExtension::data(const QModelIndex& index, ItemRole::Id roleId) const
{
	if (roleId != ItemRole::componentId)
	{
		return QVariant::Invalid;
	}

	auto data = extensionData_->data(index, ItemRole::valueTypeId);
	auto typeName = std::string(data.toString().toUtf8());
	auto typeId = TypeId(typeName.c_str());
	std::function<bool(const ItemRole::Id&)> predicate = [&](const ItemRole::Id& roleId) {
		return extensionData_->data(index, roleId) == true;
	};

	auto component = qtFramework_->findComponent(typeId, predicate, "2.0");
	if (component == nullptr)
	{
		return QVariant::Invalid;
	}

	auto qmlComponent = qtFramework_->toQmlComponent(*component);
	if (qmlComponent == nullptr)
	{
		return QVariant::Invalid;
	}

	return QVariant::fromValue<QObject*>(qmlComponent);
}
} // end namespace wgt
