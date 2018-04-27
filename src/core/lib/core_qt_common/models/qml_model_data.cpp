#include "qml_model_data.hpp"

namespace wgt
{
QmlModelData::QmlModelData()
{
}

QmlModelData::~QmlModelData()
{
}

QVariantMap& QmlModelData::roles()
{
	return roles_;
}

void QmlModelData::setRoles(const QVariantMap& roles)
{
	if (roles_ == roles)
	{
		return;
	}

	roles_ = roles;
	rolesChanged();
}
} // end namespace wgt
