#ifndef COMPONENT_EXTENSION_OLD_HPP
#define COMPONENT_EXTENSION_OLD_HPP

#include "i_model_extension_old.hpp"

namespace wgt
{
class IQtFramework;

class ComponentExtensionOld : public IModelExtensionOld
{
public:
	ComponentExtensionOld();
	virtual ~ComponentExtensionOld();

	QHash<int, QByteArray> roleNames() const override;
	QVariant data(const QModelIndex& index, int role) const override;
	bool setData(const QModelIndex& index, const QVariant& value, int role) override;

private:
	IQtFramework* qtFramework_;
};
} // end namespace wgt
#endif // COMPONENT_EXTENSION_HPP
