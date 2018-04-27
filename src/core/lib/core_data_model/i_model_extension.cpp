#include "i_model_extension.hpp"

namespace wgt
{
IModelExtension::IModelExtension() : extensionData_(nullptr)
{
}

IModelExtension::~IModelExtension()
{
}

void IModelExtension::setExtensionData(IExtensionData* extensionData)
{
	extensionData_ = extensionData;
}

const std::vector<std::string>& IModelExtension::roles() const
{
	return roles_;
}

Variant IModelExtension::data(const ModelIndex& index, ItemRole::Id roleId) const
{
	return Variant();
}

bool IModelExtension::setData(const ModelIndex& index, const Variant& value, ItemRole::Id roleId)
{
	return false;
}

Variant IModelExtension::headerData(int section, Orientation orientation, ItemRole::Id roleId) const
{
	return Variant();
}

bool IModelExtension::setHeaderData(int section, Orientation orientation, const Variant& value, ItemRole::Id roleId)
{
	return false;
}

void IModelExtension::onDataChanged(const ModelIndex& topLeft, const ModelIndex& bottomRight,
                                    const std::vector<ItemRole::Id>& roles)
{
}

void IModelExtension::onHeaderDataChanged(Orientation orientation, int first, int last)
{
}

void IModelExtension::onLayoutAboutToBeChanged(const std::vector<ModelIndex>& parents, LayoutHint hint)
{
}

void IModelExtension::onLayoutChanged(const std::vector<ModelIndex>& parents, LayoutHint hint)
{
}

void IModelExtension::onRowsAboutToBeInserted(const ModelIndex& parent, int first, int last)
{
}

void IModelExtension::onRowsInserted(const ModelIndex& parent, int first, int last)
{
}

void IModelExtension::onRowsAboutToBeRemoved(const ModelIndex& parent, int first, int last)
{
}

void IModelExtension::onRowsRemoved(const ModelIndex& parent, int first, int last)
{
}

void IModelExtension::onRowsAboutToBeMoved(const ModelIndex& sourceParent, int sourceFirst, int sourceLast,
                                           const ModelIndex& destinationParent, int destinationRow)
{
}

void IModelExtension::onRowsMoved(const ModelIndex& sourceParent, int sourceFirst, int sourceLast,
                                  const ModelIndex& destinationParent, int destinationRow)
{
}

void IModelExtension::onColumnsAboutToBeInserted(const ModelIndex& parent, int first, int last)
{
}

void IModelExtension::onColumnsInserted(const ModelIndex& parent, int first, int last)
{
}

void IModelExtension::onColumnsAboutToBeRemoved(const ModelIndex& parent, int first, int last)
{
}

void IModelExtension::onColumnsRemoved(const ModelIndex& parent, int first, int last)
{
}

void IModelExtension::onColumnsAboutToBeMoved(const ModelIndex& sourceParent, int sourceFirst, int sourceLast,
                                              const ModelIndex& destinationParent, int destinationRow)
{
}

void IModelExtension::onColumnsMoved(const ModelIndex& sourceParent, int sourceFirst, int sourceLast,
                                     const ModelIndex& destinationParent, int destinationColumn)
{
}
} // end namespace wgt
