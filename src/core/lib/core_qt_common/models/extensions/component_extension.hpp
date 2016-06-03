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

	QHash< int, QByteArray > roleNames() const override;
	QVariant data( const QModelIndex &index, int role ) const override;
	bool setData( const QModelIndex &index,
		const QVariant &value,
		int role ) override;

private:
	IQtFramework * qtFramework_;
};
} // end namespace wgt
#endif // COMPONENT_EXTENSION_HPP
