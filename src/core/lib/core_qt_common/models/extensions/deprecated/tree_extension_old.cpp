#include "tree_extension_old.hpp"
#include "selection_extension.hpp"
#include "core_qt_common/models/adapters/child_list_adapter.hpp"
#include "core_qt_common/models/adapters/indexed_adapter.hpp"
#include "core_data_model/i_item.hpp"
#include "core_variant/variant.hpp"
#include "core_logging/logging.hpp"
#include "core_qt_common/i_qt_framework.hpp"
#include "core_ui_framework/i_preferences.hpp"
#include "core_reflection/property_accessor.hpp"
#include <QSettings>

namespace wgt
{
struct TreeExtensionOld::Implementation
{
	Implementation( TreeExtensionOld& self );
	~Implementation();
	void expand( const QModelIndex& index );
	void collapse( const QModelIndex& index );
	bool expanded( const QModelIndex& index ) const;
	bool getIndexPath( const QModelIndex& index, std::string & path ) const;
	void saveStates( const char * id );
	void loadStates( const char * id );

	TreeExtensionOld& self_;
	std::vector< IndexedAdapter< ChildListAdapter > > childModels_;
	std::vector< std::unique_ptr< ChildListAdapter > > redundantChildModels_;
	std::vector< std::string > expandedList_;
	std::vector< IItem* >	memoryExpandedList_;

	QModelIndex currentIndex_;
	SelectionExtension * selectionExtension_;
	bool blockSelection_;
};

TreeExtensionOld::Implementation::Implementation( TreeExtensionOld & self )
	: self_( self )
	, selectionExtension_( nullptr )
	, blockSelection_( false )
{

}

TreeExtensionOld::Implementation::~Implementation()
{
	memoryExpandedList_.clear();
}

bool TreeExtensionOld::Implementation::getIndexPath( const QModelIndex& index, std::string & path ) const
{
	auto item = reinterpret_cast< IItem * >( index.internalPointer() );
	assert(item != nullptr);
	Variant value = item->getData( 0, IndexPathRole::roleId_ );
	bool isOk = value.tryCast( path );
	if (!isOk || value.isVoid())
	{
		return false;
	}
	return true;
}

void TreeExtensionOld::Implementation::expand( const QModelIndex& index )
{
	std::string indexPath("");
	bool hasPath = getIndexPath( index, indexPath );
	if (hasPath)
	{
		if (!expanded( index ))
		{
			expandedList_.push_back( indexPath );
		}
	}
	else
	{
		if (!expanded( index ))
		{
			auto item = reinterpret_cast< IItem * >( index.internalPointer() );
			assert(item != nullptr);
			memoryExpandedList_.push_back( item );
		}
	}
	
}


void TreeExtensionOld::Implementation::collapse( const QModelIndex& index )
{
	std::string path("");
	bool hasPath = getIndexPath( index, path );
	if (hasPath)
	{
		auto it = std::find( expandedList_.begin(), expandedList_.end(), path );
		if (it != expandedList_.end())
		{
			std::swap( 
				expandedList_[ it - expandedList_.begin() ], 
				expandedList_[ expandedList_.size() - 1 ] );
			expandedList_.pop_back();
		}
	}
	else
	{
		auto item = reinterpret_cast< IItem * >( index.internalPointer() );
		assert(item != nullptr);
		auto it = std::find( memoryExpandedList_.begin(), memoryExpandedList_.end(), item );
		if (it != memoryExpandedList_.end())
		{
			std::swap( 
				memoryExpandedList_[ it - memoryExpandedList_.begin() ], 
				memoryExpandedList_[ memoryExpandedList_.size() - 1 ] );
			memoryExpandedList_.pop_back();
		}
	}
}


bool TreeExtensionOld::Implementation::expanded( const QModelIndex& index ) const
{
	std::string indexPath("");
	bool hasPath = getIndexPath( index, indexPath );
	if (hasPath)
	{
		return 
			std::find( expandedList_.cbegin(), expandedList_.cend(), indexPath ) != 
			expandedList_.cend();
	}
	else
	{
		auto item = reinterpret_cast< IItem * >( index.internalPointer() );
		assert(item != nullptr);
		return 
			std::find( memoryExpandedList_.cbegin(), memoryExpandedList_.cend(), item ) != 
			memoryExpandedList_.cend();
	}
}

void TreeExtensionOld::Implementation::saveStates( const char * id )
{
	if (id == nullptr || id == std::string( "" ))
	{
		NGT_WARNING_MSG( 
			"Tree preference won't save: %s\n", "Please provide unique objectName for WGTreeModel in qml" );
		return;
	}
	auto preferences = self_.qtFramework_->getPreferences();
	if (preferences == nullptr)
	{
		return;
	}
	auto & preference = preferences->getPreference( id );
	auto count = expandedList_.size();
	preference->set( "treeNodeCount", count );
	if (count == 0)
	{
        if (!memoryExpandedList_.empty())
        {
            NGT_WARNING_MSG( 
                "Tree preference won't save for WGTreeModel: %s, %s\n",  id,
                "please provide an unique path string for IndexPathRole of IItem." );
        }
		return;
	}
	int i = 0;
	for (auto item : expandedList_)
	{
		preference->set( std::to_string( i++ ).c_str(), item );
	}
}

void TreeExtensionOld::Implementation::loadStates( const char * id )
{
	if (id == nullptr || id == std::string( "" ))
	{
		return;
	}
	auto preferences = self_.qtFramework_->getPreferences();
	if (preferences == nullptr)
	{
		return;
	}
	auto & preference = preferences->getPreference( id );

	auto accessor = preference->findProperty( "treeNodeCount" );
	if (!accessor.isValid())
	{
		return;
	}

	size_t count = 0;
	bool isOk = preference->get( "treeNodeCount", count );
	assert( isOk );

	std::string value( "" );
	for (size_t i = 0; i < count; ++i)
	{
		bool isOk = preference->get( std::to_string( i ).c_str(), value );
		assert( isOk );
		expandedList_.push_back( value );
	}
}


TreeExtensionOld::TreeExtensionOld()
	: impl_( new Implementation( *this ) )
{
}


TreeExtensionOld::~TreeExtensionOld()
{
}

void TreeExtensionOld::saveStates( const char * modelUniqueName )
{
	impl_->saveStates( modelUniqueName );
}

void TreeExtensionOld::loadStates( const char * modelUniqueName )
{
	impl_->loadStates( modelUniqueName );
}


QHash< int, QByteArray > TreeExtensionOld::roleNames() const
{
	QHash< int, QByteArray > roleNames;
	this->registerRole( ChildModelRole::roleName_, roleNames );
	this->registerRole( HasChildrenRole::roleName_, roleNames );
	this->registerRole( ExpandedRole::roleName_, roleNames );
	this->registerRole( ParentIndexRole::roleName_, roleNames );
	return roleNames;
}


QVariant TreeExtensionOld::data( const QModelIndex &index, int role ) const
{
	auto model = index.model();
	assert( model != nullptr );

	ItemRole::Id roleId;
	if (!this->decodeRole( role, roleId ))
	{
		return QVariant( QVariant::Invalid );
	}

	if (roleId == ChildModelRole::roleId_)
	{
		if (!model->hasChildren(index) ||
			!impl_->expanded( index ))
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
	else if (roleId == HasChildrenRole::roleId_)
	{
		return model->hasChildren( index );
	}
	else if (roleId == ExpandedRole::roleId_)
	{
		return impl_->expanded( index );
	}
	else if (roleId == ParentIndexRole::roleId_)
	{
		QModelIndex parentIndex = model->parent( index );
		return parentIndex;
	}

	return QVariant( QVariant::Invalid );
}


bool TreeExtensionOld::setData( 
	const QModelIndex &index, const QVariant &value, int role )
{
	auto model = index.model();
	assert( model != nullptr );

	ItemRole::Id roleId;
	if (!this->decodeRole( role, roleId ))
	{
		return false;
	}

	if (roleId == ExpandedRole::roleId_)
	{
		// Change the data
		auto expand = value.toBool();
		if (impl_->expanded( index ) == expand)
		{
			return false;
		}

		expand ? impl_->expand( index ) : impl_->collapse( index );

		// Emit the data change
		QVector< int > roles;
		roles.append( role );
		// The child model role is dependent on the expanded role
		auto res = this->encodeRole( ChildModelRole::roleId_, role );
		assert( res );
		roles.append( role );
		emit const_cast< QAbstractItemModel * >( model )->dataChanged( index, index, roles );

		return true;
	}

	return false;
}


void TreeExtensionOld::onLayoutAboutToBeChanged(
	const QList<QPersistentModelIndex> & parents, 
	QAbstractItemModel::LayoutChangeHint hint )
{
	for (auto it = parents.begin(); it != parents.end(); ++it)
	{
		isolateRedundantIndices( 
			*it, impl_->childModels_, impl_->redundantChildModels_ );
	}
}


void TreeExtensionOld::onLayoutChanged(
	const QList<QPersistentModelIndex> & parents, 
	QAbstractItemModel::LayoutChangeHint hint )
{
	impl_->redundantChildModels_.clear();

	QVector< int > roles;
	int role;
	auto res = this->encodeRole( ChildModelRole::roleId_, role );
	assert( res );
	roles.append( role );
	res = this->encodeRole( HasChildrenRole::roleId_, role );
	assert( res );
	roles.append( role );
	for (auto it = parents.begin(); it != parents.end(); ++it)
	{
		auto model = it->model();
		assert( model != nullptr );

		emit const_cast< QAbstractItemModel * >( model )->dataChanged( *it, *it, roles );
	}
}


void TreeExtensionOld::onRowsAboutToBeRemoved( 
	const QModelIndex& parent, int first, int last )
{
	isolateRedundantIndices( parent, first, last,
		impl_->childModels_, impl_->redundantChildModels_ );
}


void TreeExtensionOld::onRowsRemoved(
	const QModelIndex & parent, int first, int last )
{
	impl_->redundantChildModels_.clear();
}


/// Move to previous index
bool TreeExtensionOld::moveUp()
{
	auto model = impl_->currentIndex_.model();
	assert( model != nullptr );

	int prevRow = impl_->currentIndex_.row() - 1;

	if (0 <= prevRow)
	{
		QModelIndex prevIndex = impl_->currentIndex_.sibling( prevRow, 0 );
		int prevIndexsBottomRow = 0;

		do {
			// Previous item's bottom row
			prevIndexsBottomRow = model->rowCount( prevIndex ) - 1;

			impl_->currentIndex_ = prevIndex;

			if (model->hasChildren( impl_->currentIndex_ ))
			{
				// Keep search in child tree if the bottom item has child tree and expanded
				prevIndexsBottomRow = model->rowCount( impl_->currentIndex_ ) - 1;

				prevIndex = impl_->currentIndex_.child( prevIndexsBottomRow, 0 );
			}
		} while (model->hasChildren( impl_->currentIndex_ ) && impl_->expanded( impl_->currentIndex_ ));

		return handleCurrentIndexChanged();
	}
	else
	{
		// We are the first child, move up to the parent
		QModelIndex parent = impl_->currentIndex_.parent();

		if (parent.isValid())
		{
			// Update the current index if the parent is valid
			impl_->currentIndex_ = parent;
			return handleCurrentIndexChanged();
		}
	}

	return false;
}


/// Move to next index
bool TreeExtensionOld::moveDown()
{
	auto model = impl_->currentIndex_.model();
	assert( model != nullptr );

	if (impl_->expanded( impl_->currentIndex_ ))
	{
		// Move to the first child item when the current item is expanded
		impl_->currentIndex_ = impl_->currentIndex_.child( 0, 0 );
		return handleCurrentIndexChanged();
	}
	else
	{
		QModelIndex parent = impl_->currentIndex_.parent();

		int nextRow = impl_->currentIndex_.row() + 1;
		while (parent.isValid())
		{
			if (nextRow < model->rowCount( parent ))
			{
				// Update the current index if the next item is available
				impl_->currentIndex_ = parent.child( nextRow, 0 );
				return handleCurrentIndexChanged();
				break;
			}
			else
			{
				// Reached the bottom, keep searching the parent
				nextRow = parent.row() + 1;
				if (!parent.parent().isValid())
				{
					break;
				}
				parent = parent.parent();
			}
		}

		parent = parent.isValid() ? parent : impl_->currentIndex_;
		if (nextRow < model->rowCount( parent ))
		{
			QModelIndex sibling = parent.sibling( nextRow, impl_->currentIndex_.column() );
			if (sibling.isValid())
			{
				impl_->currentIndex_ = sibling;
				return handleCurrentIndexChanged();
			}
		}
	}

	return false;
}


/// Collapse the current item if it is collapsible or move to the parent
bool TreeExtensionOld::moveLeft()
{
	auto model = impl_->currentIndex_.model();
	assert( model != nullptr );

	// Move up to the parent if there are no children or not expanded
	if (!model->hasChildren( impl_->currentIndex_ ) || !impl_->expanded( impl_->currentIndex_ ))
	{
		// Move up to the parent
		QModelIndex parent = impl_->currentIndex_.parent();

		if (parent.isValid())
		{
			// Update the current index if the parent is valid
			impl_->currentIndex_ = parent;
			return handleCurrentIndexChanged();
		}
	}
	else
	{
		// Collapse the current item
		int expandedRole = -1;
		this->encodeRole( ExpandedRole::roleId_, expandedRole );

		setData( impl_->currentIndex_, QVariant( false ), expandedRole );
	}

	return false;
}


/// Expand the current item if it is expandable or move to the first child
bool TreeExtensionOld::moveRight()
{
	auto model = impl_->currentIndex_.model();
	assert( model != nullptr );

	// Make sure the current item has children
	if (model->hasChildren( impl_->currentIndex_ ) )
	{
		if (impl_->expanded( impl_->currentIndex_ ))
		{
			// Select the first child if the current item is expanded
			impl_->currentIndex_ = impl_->currentIndex_.child( 0, 0 );
			return handleCurrentIndexChanged();
		}
		else
		{
			// Expand the current item
			int expandedRole = -1;
			this->encodeRole( ExpandedRole::roleId_, expandedRole );

			setData( impl_->currentIndex_, QVariant( true ), expandedRole );
		}
	}

	return false;
}


/// Select the current item by manipulating the SelectionExtension (where applicable)
void TreeExtensionOld::selectItem()
{
	if (impl_->selectionExtension_ == nullptr)
	{
		return;
	}

	impl_->selectionExtension_->setSelectedIndex( getCurrentIndex() );
	impl_->selectionExtension_->selectionChanged();
}


bool TreeExtensionOld::handleCurrentIndexChanged()
{
	if (impl_->selectionExtension_ != nullptr)
	{
		impl_->selectionExtension_->setSelectedIndex( getCurrentIndex() );
	}

	emit currentIndexChanged();
	return true;
}


QVariant TreeExtensionOld::getCurrentIndex() const
{
	return QVariant::fromValue( impl_->currentIndex_ );
}


void TreeExtensionOld::setCurrentIndex( const QVariant& index )
{
	QModelIndex idx = index.toModelIndex();
	impl_->currentIndex_ = idx;

	emit currentIndexChanged();
}


bool TreeExtensionOld::getBlockSelection() const
{
	return impl_->blockSelection_;
}


void TreeExtensionOld::setBlockSelection( bool blockSelection )
{
	impl_->blockSelection_ = blockSelection;
	emit blockSelectionChanged();
}


QObject * TreeExtensionOld::getSelectionExtension() const
{
	return impl_->selectionExtension_;
}


void TreeExtensionOld::setSelectionExtension( QObject * selectionExtension )
{
	impl_->selectionExtension_ = qobject_cast< SelectionExtension * >( selectionExtension );
	emit selectionExtensionChanged();
}
} // end namespace wgt
