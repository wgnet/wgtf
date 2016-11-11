#include "i_model_extension.hpp"

namespace wgt
{
IModelExtension::IModelExtension() : extensionData_(nullptr)
{
}

IModelExtension::~IModelExtension()
{
}

void IModelExtension::init(IExtensionData& extensionData)
{
	extensionData_ = &extensionData;
}

const std::vector<std::string>& IModelExtension::roles() const
{
	return roles_;
}

QVariant IModelExtension::data(const QModelIndex& index, ItemRole::Id roleId) const
{
	return QVariant::Invalid;
}

bool IModelExtension::setData(const QModelIndex& index, const QVariant& value, ItemRole::Id roleId)
{
	return false;
}

QVariant IModelExtension::headerData(int section, Qt::Orientation orientation, ItemRole::Id roleId) const
{
	return QVariant::Invalid;
}

bool IModelExtension::setHeaderData(int section, Qt::Orientation orientation, const QVariant& value,
                                    ItemRole::Id roleId)
{
	return false;
}
} // end namespace wgt