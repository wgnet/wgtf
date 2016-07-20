#include "table_extension.hpp"

namespace wgt
{
TableExtension::TableExtension()
{

}

TableExtension::~TableExtension()
{

}


QModelIndex TableExtension::getForwardIndex( const QModelIndex & index ) const
{
	const auto pModel = index.model();
	if (pModel == nullptr)
	{
		return index;
	}
	if (index.column() >= (pModel->columnCount( index.parent() ) - 1))
	{
		return index;
	}
	return pModel->index( index.row(), index.column() + 1, index.parent() );
}


QModelIndex TableExtension::getBackwardIndex( const QModelIndex & index ) const
{
	const auto pModel = index.model();
	if (pModel == nullptr)
	{
		return index;
	}
	if (index.column() <= 0)
	{
		return index;
	}
	return pModel->index( index.row(), index.column() - 1, index.parent() );
}
} // end namespace wgt
