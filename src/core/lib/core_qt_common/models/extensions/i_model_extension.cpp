#include "i_model_extension.hpp"

namespace wgt
{
IModelExtension::IModelExtension()
	: qtFramework_( nullptr )
	, extensionData_( nullptr )
{

}

IModelExtension::~IModelExtension()
{

}

void IModelExtension::init( IExtensionData & extensionData )
{
	extensionData_ = &extensionData;
}

QVariant IModelExtension::data( const QModelIndex &index, int role ) const
{
	size_t roleId;
	if (!decodeRole( role, roleId ))
	{
		return QVariant( QVariant::Invalid );
	}

	return dataExt( index, roleId );
}

bool IModelExtension::setData( const QModelIndex &index, const QVariant &value, int role )
{
	size_t roleId;
	if (!decodeRole( role, roleId ))
	{
		return false;
	}

	return setDataExt( index, value, roleId );
}

QVariant IModelExtension::dataExt( const QModelIndex &index, size_t roleId ) const
{
	assert( extensionData_ != nullptr );

	auto& itemData = extensionData_->getItemData( index );
	return itemData[roleId];
}

bool IModelExtension::setDataExt( const QModelIndex &index, const QVariant &value, size_t roleId )
{
	assert( extensionData_ != nullptr );

	auto& itemData = extensionData_->getItemData( index );
	if (itemData[roleId] == value)
	{
		return false;
	}
	itemData[roleId] = value;

	int role;
	if (encodeRole( roleId, role ))
	{
		QVector< int > roles;
		roles.append( role );
		emit const_cast< QAbstractItemModel * >( index.model() )->dataChanged( index, index, roles );
	}

	return true;
}
} // end namespace wgt