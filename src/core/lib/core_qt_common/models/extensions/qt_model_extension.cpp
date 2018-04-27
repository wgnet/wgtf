#include "qt_model_extension.hpp"

#include "core_common/assert.hpp"

namespace wgt
{
QtModelExtension::QtModelExtension() : extensionData_(nullptr)
{
}

QtModelExtension::~QtModelExtension()
{
}

void QtModelExtension::setFramework(QtFrameworkCommon& frameworkCommon)
{
	frameworkCommon_ = &frameworkCommon;
}

void QtModelExtension::setExtensionData(IQtModelExtensionData& extensionData)
{
	extensionData_ = &extensionData;
}

const std::vector<std::string>& QtModelExtension::roles() const
{
	return roles_;
}

QVariant QtModelExtension::data(const QModelIndex& index, ItemRole::Id roleId) const
{
	return QVariant::Invalid;
}

bool QtModelExtension::setData(const QModelIndex& index, const QVariant& value, ItemRole::Id roleId)
{
	return false;
}

QVariant QtModelExtension::headerData(int section, Qt::Orientation orientation, ItemRole::Id roleId) const
{
	return QVariant::Invalid;
}

bool QtModelExtension::setHeaderData(int section, Qt::Orientation orientation, const QVariant& value,
                                     ItemRole::Id roleId)
{
	return false;
}

QtFrameworkCommon& QtModelExtension::frameworkCommon() const
{
	TF_ASSERT(frameworkCommon_ != nullptr);
	return *frameworkCommon_;
}
} // end namespace wgt
