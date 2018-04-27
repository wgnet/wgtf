#ifndef COMPONENT_EXTENSION_HPP
#define COMPONENT_EXTENSION_HPP

#include "qt_model_extension.hpp"

namespace wgt
{
class ComponentExtension : public QtModelExtension
{
public:
	ComponentExtension();
	virtual ~ComponentExtension();

	QVariant data(const QModelIndex& index, ItemRole::Id roleId) const override;
};
} // end namespace wgt
#endif // COMPONENT_EXTENSION_HPP
