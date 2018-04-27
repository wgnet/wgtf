#include "component_extension_old.hpp"

#include "core_common/assert.hpp"
#include "core_data_model/i_item.hpp"
#include "core_variant/type_id.hpp"
#include "core_qt_common/i_qt_framework.hpp"

#include <QQmlComponent>
#include <QModelIndex>

namespace wgt
{
ComponentExtensionOld::ComponentExtensionOld()
{
}

ComponentExtensionOld::~ComponentExtensionOld()
{
}

QHash<int, QByteArray> ComponentExtensionOld::roleNames() const
{
	QHash<int, QByteArray> roleNames;
	registerRole(ComponentRole::roleName_, roleNames);
	return roleNames;
}

QVariant ComponentExtensionOld::data(const QModelIndex& index, int role) const
{
	ItemRole::Id roleId;
	if (!decodeRole(role, roleId))
	{
		return QVariant(QVariant::Invalid);
	}

	TF_ASSERT(index.isValid());
	auto item = reinterpret_cast<IItem*>(index.internalPointer());
	if (item == nullptr)
	{
		return false;
	}
	auto column = index.column();

	if (roleId == ComponentRole::roleId_)
	{
		auto data = item->getData(column, ValueTypeRole::roleId_);

		std::string typeName;
		data.tryCast(typeName);
		if (typeName.empty())
		{
			return QVariant(QVariant::Invalid);
		}

		auto typeId = TypeId(typeName.c_str());
		std::function<bool(const ItemRole::Id&)> predicate = [&](const ItemRole::Id& role) {
			return item->getData(column, role) == true;
		};
		auto qtFramework = get<IQtFramework>();
		auto component = qtFramework->findComponent(typeId, predicate, "1.0");
		if (component != nullptr)
		{
			auto qmlComponent = qtFramework->toQmlComponent(*component);
			if (qmlComponent != nullptr)
			{
				return QVariant::fromValue<QObject*>(qmlComponent);
			}
		}
	}

	return QVariant(QVariant::Invalid);
}

bool ComponentExtensionOld::setData(const QModelIndex& index, const QVariant& value, int role)
{
	return false;
}
} // end namespace wgt
