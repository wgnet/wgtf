#include "test_model.hpp"
#include "core_data_model/i_item_role.hpp"
#include <vector>
#include "wg_types/vector3.hpp"


namespace wgt
{
ListData::ListData()
{

}

ListData::~ListData()
{

}

void ListData::init(IDefinitionManager & defManager, int objCount)
{

	for( int i = 0; i < objCount; i++)
	{
		auto genericObj = GenericObject::create( defManager );
		genericObj->set("name", "GenericObject[" + std::to_string(i+1) + "]");
		genericObj->set("description", "Generic Object");
		genericObj->set("index", i);
		objList_.push_back( genericObj );
	}

	Collection col(objList_);
	listModel_.setSource( col );
}

const IListModel* ListData::getSourceModel() const
{
	return &listModel_;
}


ITEMROLE( name )
ITEMROLE( description )
ITEMROLE( number )

class ListDataModelItem : public AbstractListItem
{
public:
	ListDataModelItem( const char * name, const char * description, int index )
		: name_( name )
		, description_( description )
		, index_( index )
	{

	}

	Variant getData( int column, size_t roleId ) const override
	{
		if (roleId == ItemRole::nameId)
		{
			return name_;
		}
		else if (roleId == ItemRole::descriptionId)
		{
			return description_;
		}
		else if (roleId == ItemRole::numberId)
		{
			return index_;
		}
		return Variant();
	}

	bool setData( int column, size_t roleId, const Variant & data ) override
	{
		return false;
	}
	std::string name_;
	std::string description_;
	int index_;

};

struct ListDataModel::Impl
{
	Impl()
	{
		
	}
	void init( int count )
	{
		std::string name;
		std::string desc = "Test Object";
		for( int i = 0; i < count; i++)
		{
			name = "Object[" + std::to_string(i+1) + "]";
			items_.emplace_back( new ListDataModelItem(name.c_str(), desc.c_str(), i) );
		}
	}
	std::vector< std::unique_ptr< ListDataModelItem > > items_;
};

ListDataModel::ListDataModel()
	: impl_( new Impl() )
{

}

ListDataModel::~ListDataModel()
{

}

void ListDataModel::init( int count )
{
	impl_->init( count );
}

AbstractItem * ListDataModel::item(int row) const
{
	assert( row < static_cast< int >( impl_->items_.size() ) && row >= 0 );
	return impl_->items_[row].get();
}

int ListDataModel::index(const AbstractItem * item) const
{
	auto it = std::find_if( impl_->items_.begin(), impl_->items_.end(),
		[&]( const std::unique_ptr< ListDataModelItem > & listDataModelItem ) { return listDataModelItem.get() == item; } );
	if(it == impl_->items_.end())
	{
		assert( false );
		return -1;
	}
	return static_cast< int >( it - impl_->items_.begin() );
}

int ListDataModel::rowCount() const
{
	return static_cast< int >( impl_->items_.size() );
}

int ListDataModel::columnCount() const
{
	return 1;
}

std::vector< std::string > ListDataModel::roles() const
{
	std::vector< std::string > roles;
	roles.emplace_back( ItemRole::nameName );
	roles.emplace_back( ItemRole::descriptionName );
	roles.emplace_back( ItemRole::numberName );
	return roles;
}

} // end namespace wgt
