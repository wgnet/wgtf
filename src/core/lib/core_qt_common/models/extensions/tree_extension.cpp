#include "tree_extension.hpp"
#include "core_qt_common/models/adapters/child_list_adapter.hpp"
#include "core_qt_common/models/adapters/indexed_adapter.hpp"
#include "core_variant/variant.hpp"
#include "core_qt_common/i_qt_framework.hpp"

namespace wgt
{
ITEMROLE( childModel )
ITEMROLE( hasChildren )
ITEMROLE( expanded )

struct TreeExtension::Implementation
{
	Implementation( TreeExtension& self );
	~Implementation();
	bool expanded( const QModelIndex& index ) const;

	TreeExtension& self_;
	std::vector< IndexedAdapter< ChildListAdapter > > childModels_;
	std::vector< std::unique_ptr< ChildListAdapter > > redundantChildModels_;
};

TreeExtension::Implementation::Implementation( TreeExtension & self )
	: self_( self )
{

}

TreeExtension::Implementation::~Implementation()
{
}


bool TreeExtension::Implementation::expanded( const QModelIndex& index ) const
{
	return self_.dataExt( index, ItemRole::expandedId ).toBool();
}


TreeExtension::TreeExtension()
	: impl_( new Implementation( *this ) )
{
}


TreeExtension::~TreeExtension()
{
}


QHash< int, QByteArray > TreeExtension::roleNames() const
{
	QHash< int, QByteArray > roleNames;
	this->registerRole( ItemRole::childModelName, roleNames );
	this->registerRole( ItemRole::hasChildrenName, roleNames );
	this->registerRole( ItemRole::expandedName, roleNames );
	return roleNames;
}


QVariant TreeExtension::data( const QModelIndex &index, int role ) const
{
	auto model = index.model();
	assert( model != nullptr );

	size_t roleId;
	if (!this->decodeRole( role, roleId ))
	{
		return QVariant( QVariant::Invalid );
	}

	if (roleId == ItemRole::childModelId)
	{
		if (!model->hasChildren(index))
		{
			return QVariant( QVariant::Invalid );
		}

		auto it = std::find( impl_->childModels_.begin(), 
			impl_->childModels_.end(), index );
		if (it != impl_->childModels_.end())
		{
			return QVariant::fromValue< QAbstractItemModel* >( it->data_.get() );
		}
		else
		{
			auto pChildModel = new ChildListAdapter( index );
			impl_->childModels_.emplace_back( index, pChildModel );
			return QVariant::fromValue< QAbstractItemModel* >( pChildModel );
		}
	}
	else if (roleId == ItemRole::hasChildrenId)
	{
		return model->hasChildren( index );
	}
	else if (roleId == ItemRole::expandedId)
	{
		return impl_->expanded( index );
	}

	return QVariant( QVariant::Invalid );
}


bool TreeExtension::setData( 
	const QModelIndex &index, const QVariant &value, int role )
{
	auto model = index.model();
	assert( model != nullptr );

	size_t roleId;
	if (!this->decodeRole( role, roleId ))
	{
		return false;
	}

	if (roleId == ItemRole::expandedId)
	{
		return setDataExt( index, value, roleId );
	}

	return false;
}


void TreeExtension::onLayoutAboutToBeChanged(
	const QList<QPersistentModelIndex> & parents, 
	QAbstractItemModel::LayoutChangeHint hint )
{
	for (auto it = parents.begin(); it != parents.end(); ++it)
	{
		isolateRedundantIndices( 
			*it, impl_->childModels_, impl_->redundantChildModels_ );
	}
}


void TreeExtension::onLayoutChanged(
	const QList<QPersistentModelIndex> & parents, 
	QAbstractItemModel::LayoutChangeHint hint )
{
	impl_->redundantChildModels_.clear();

	QVector< int > roles;
	int role;
	auto res = this->encodeRole( ItemRole::childModelId, role );
	assert( res );
	roles.append( role );
	res = this->encodeRole( ItemRole::hasChildrenId, role );
	assert( res );
	roles.append( role );
	for (auto it = parents.begin(); it != parents.end(); ++it)
	{
		auto model = it->model();
		assert( model != nullptr );

		emit const_cast< QAbstractItemModel * >( model )->dataChanged( *it, *it, roles );
	}
}


void TreeExtension::onRowsAboutToBeRemoved( 
	const QModelIndex& parent, int first, int last )
{
	isolateRedundantIndices( parent, first, last,
		impl_->childModels_, impl_->redundantChildModels_ );
}


void TreeExtension::onRowsRemoved(
	const QModelIndex & parent, int first, int last )
{
	impl_->redundantChildModels_.clear();
}


QItemSelection TreeExtension::itemSelection( const QModelIndex & first, const QModelIndex & last ) const
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

	auto begin = first;
	auto end = last;

	auto parent = QModelIndex();
	{
		// Check if end is a descendant of begin
		auto endTmp = end;
		while (endTmp.isValid())
		{
			if (begin == endTmp)
			{
				parent = begin;
				break;
			}
			endTmp = endTmp.parent();
		}
	}

	if (!parent.isValid())
	{
		// Check if begin is a descendant of end
		auto beginTmp = begin;
		while (beginTmp.isValid())
		{
			if (beginTmp == end)
			{
				std::swap(begin, end);
				parent = begin;
				break;
			}
			beginTmp = beginTmp.parent();
		}
	}

	if (!parent.isValid())
	{
		// Check if begin comes before end or vice versa
		auto beginTmp = begin;
		while (beginTmp.isValid())
		{
			auto beginParent = beginTmp.parent();
			auto endTmp = end;
			while (endTmp.isValid())
			{
				auto endParent = endTmp.parent();
				if (beginParent == endParent)
				{
					if (beginTmp.row() > endTmp.row())
					{
						std::swap(begin, end);
					}
					parent = beginParent;
					break;
				}
				endTmp = endParent;
			}
			if (parent.isValid())
			{
				break;
			}
			beginTmp = beginParent;
		}
	}

	// Create an item selection from begin to end
	QItemSelection itemSelection;

	auto it = begin;
	while (true)
	{
		itemSelection.select(it, it);

		if (it == end)
		{
			break;
		}

		// Move next
		const auto next = this->getNextIndex( it );
		assert( it != next );	
		it = next;
	}
	
	return itemSelection;
}


QModelIndex TreeExtension::getNextIndex( const QModelIndex & index ) const
{
	if (!index.isValid())
	{
		return index;
	}
	const auto pModel = index.model();
	if (pModel == nullptr)
	{
		return index;
	}

	// Move to next child
	// > a    - start
	//  | b   - end
	//  | c
	if (impl_->expanded( index ) &&
		pModel->hasChildren( index ))
	{
		const auto child = index.child( 0, index.column() );
		if (child.isValid())
		{
			return child;
		}
	}

	auto it = index;
	while (it.isValid())
	{
		// Move to next sibling
		// > a    - start
		//  | b
		// > c    - end
		const auto parent = it.parent();
		const auto row = it.row() + 1;
		if (row < it.model()->rowCount( parent ))
		{
			const auto sibling = it.sibling( row, index.column() );
			if (sibling.isValid())
			{
				return sibling;
			}
		}

		// Or move to next sibling of parent
		// > a
		//  | b   - start
		// > c    - end
		it = parent;
	}

	// Could not move to next item
	return index;
}


QModelIndex TreeExtension::getPreviousIndex( const QModelIndex & index ) const
{
	if (!index.isValid())
	{
		return index;
	}
	const auto pModel = index.model();
	if (pModel == nullptr)
	{
		return index;
	}

	// Move back to previous sibling
	// > a
	//  | b   - end
	//  | c   - start
	const auto row = index.row() - 1;
	if (row >= 0)
	{
		const auto sibling = index.sibling( row, index.column() );
		if (sibling.isValid())
		{
			// Move forward to last child in the sibling
			// > a
			//  > b
			//    | d
			//    | e - end
			//  | c   - start
			auto it = sibling;
			while (it.isValid())
			{
				// Has children, move to last one
				if (impl_->expanded( it ) &&
					pModel->hasChildren( it ))
				{
					const int lastRow = it.model()->rowCount( it ) - 1;
					const auto lastChild = sibling.child( lastRow, index.column() );
					if (lastChild.isValid())
					{
						it = lastChild;
					}
					else
					{
						// Last child not valid
						return it;
					}
				}
				else
				{
					// Previous row does not have children expanded
					return it;
				}
			}
		}
	}

	// Move to previous parent
	// > a
	//  > b   - end
	//    | d - start
	//    | e
	//  | c   
	const auto parent = index.parent();
	if (parent.isValid())
	{
		return parent;
	}

	// Could not move to previous item
	return index;
}


QModelIndex TreeExtension::getForwardIndex( const QModelIndex & index ) const
{
	if (!index.isValid())
	{
		return index;
	}

	const auto pModel = index.model();
	if (pModel == nullptr)
	{
		return index;
	}

	// Make sure the current item has children
	if (pModel->hasChildren( index ) )
	{
		if (impl_->expanded( index ))
		{
			// Select the first child if the current item is expanded
			const auto child = index.child( 0, index.column() );
			if (child.isValid())
			{
				return child;
			}
		}
		else
		{
			// Expand the current item
			const_cast< TreeExtension * >( this )->setDataExt( index, true, ItemRole::expandedId );

			// Emit the data change
			int role = 0;
			auto res = this->encodeRole( ItemRole::expandedId, role );
			assert( res );
			QVector< int > roles;
			roles.append( role );
			emit const_cast< QAbstractItemModel * >( pModel )->dataChanged( index,
				index,
				roles );

			return index;
		}
	}

	return this->getNextIndex( index );
}


QModelIndex TreeExtension::getBackwardIndex( const QModelIndex & index ) const
{
	if (!index.isValid())
	{
		return index;
	}

	const auto pModel = index.model();
	if (pModel == nullptr)
	{
		return index;
	}

	// Move up to the parent if there are no children or not expanded
	if (pModel->hasChildren( index ) && impl_->expanded( index ))
	{
		// Collapse the current item
		const_cast< TreeExtension * >( this )->setDataExt( index, false, ItemRole::expandedId );

		// Emit the data change
		int role = 0;
		auto res = this->encodeRole( ItemRole::expandedId, role );
		assert( res );
		QVector< int > roles;
		roles.append( role );
		emit const_cast< QAbstractItemModel * >( pModel )->dataChanged( index,
			index,
			roles );

		return index;
	}

	return this->getPreviousIndex( index );
}
} // end namespace wgt
