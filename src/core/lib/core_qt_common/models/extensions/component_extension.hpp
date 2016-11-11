#ifndef COMPONENT_EXTENSION_HPP
#define COMPONENT_EXTENSION_HPP

#include "i_model_extension.hpp"

namespace wgt
{
class IQtFramework;

class ComponentExtension : public IModelExtension
{
public:
	ComponentExtension();
	virtual ~ComponentExtension();

	QVariant data(const QModelIndex& index, ItemRole::Id roleId) const override;

private:
	IQtFramework* qtFramework_;
};
} // end namespace wgt
#endif // COMPONENT_EXTENSION_HPP
