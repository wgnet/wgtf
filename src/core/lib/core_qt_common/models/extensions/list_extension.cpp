#include "list_extension.hpp"

namespace wgt
{
ListExtension::ListExtension()
{

}

ListExtension::~ListExtension()
{

}

QHash< int, QByteArray > ListExtension::roleNames() const
{
	return QHash< int, QByteArray >();
}

QItemSelection ListExtension::itemSelection( const QModelIndex & first, const QModelIndex & last ) const
{
	if (!first.isValid() && !last.isValid())
	{
		return QItemSelection();
	}
	if (!first.isValid() && last.isValid())
	{
		return QItemSelection( last, last );
	}
	if (first.isValid() && !last.isValid())
	{
		return QItemSelection( first, first );
	}

	assert( first.column() == 0 && !first.parent().isValid() );
	assert( last.column() == 0 && !last.parent().isValid() );

	auto begin = first;
	auto end = last;

	if (begin.row() > end.row())
	{
		std::swap(begin, end);
	}

	return QItemSelection( begin, end );
}


QModelIndex ListExtension::getNextIndex( const QModelIndex & index ) const
{
	const auto pModel = index.model();
	if (pModel == nullptr)
	{
		return index;
	}
	if (index.row() >= (pModel->rowCount( index.parent() ) - 1))
	{
		return index;
	}
	return pModel->index( index.row() + 1, index.column(), index.parent() );
}


QModelIndex ListExtension::getPreviousIndex( const QModelIndex & index ) const
{
	const auto pModel = index.model();
	if (pModel == nullptr)
	{
		return index;
	}
	if (index.row() <= 0)
	{
		return index;
	}
	return pModel->index( index.row() - 1, index.column(), index.parent() );
}
} // end namespace wgt
