#include "active_filters_test_view_model.hpp"
#include "metadata/active_filters_test_view_model.mpp"
#include "core_data_model/simple_active_filters_model.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_data_model/i_item_role.hpp"
#include "testing/data_model_test/test_tree_model.hpp"


namespace wgt
{
//------------------------------------------------------------------------------
// Implementation (PIMPL)
//------------------------------------------------------------------------------
struct ActiveFiltersTestViewModel::Implementation
{
	Implementation( ActiveFiltersTestViewModel& self );
	void init( IDefinitionManager & defManager, IUIFramework & uiFramework );

	ActiveFiltersTestViewModel& self_;
	std::unique_ptr<IActiveFiltersModel> simpleActiveFiltersModel_;
	std::unique_ptr<ITreeModel> sampleDataToFilterOld_;
	std::unique_ptr<AbstractTreeModel> sampleDataToFilterNew_;
};

ActiveFiltersTestViewModel::Implementation::Implementation( 
	ActiveFiltersTestViewModel& self )
	: self_( self )
	, simpleActiveFiltersModel_( nullptr )
	, sampleDataToFilterOld_( new SampleActiveFiltersTreeModel() )
	, sampleDataToFilterNew_( new TestTreeModel() )
{
}

void ActiveFiltersTestViewModel::Implementation::init( 
	IDefinitionManager & defManager, IUIFramework & uiFramework )
{
	auto def = defManager.getDefinition< IActiveFiltersModel >();
	simpleActiveFiltersModel_.reset( new SimpleActiveFiltersModel( "testActiveFilter", defManager, uiFramework ) );
}

//------------------------------------------------------------------------------
// View Model
// Data passed to the QML panel to be used by the control(s).
//------------------------------------------------------------------------------

ActiveFiltersTestViewModel::ActiveFiltersTestViewModel() 
	: impl_( new Implementation( *this ) )
{
}

ActiveFiltersTestViewModel::~ActiveFiltersTestViewModel()
{
	if (impl_ != nullptr)
	{
		delete impl_;
		impl_ = nullptr;
	}
}
	
void ActiveFiltersTestViewModel::init( IDefinitionManager & defManager, IUIFramework & uiFramework )
{
	impl_->init( defManager, uiFramework );
}

IActiveFiltersModel * ActiveFiltersTestViewModel::getSimpleActiveFiltersModel() const
{
	return impl_->simpleActiveFiltersModel_.get();
}

ITreeModel * ActiveFiltersTestViewModel::getSampleDataToFilterOld() const 
{
	return impl_->sampleDataToFilterOld_.get();
}

AbstractTreeModel * ActiveFiltersTestViewModel::getSampleDataToFilterNew() const 
{
	return impl_->sampleDataToFilterNew_.get();
}
//------------------------------------------------------------------------------

struct SampleActiveFiltersTreeItem::Implementation
{
	Implementation( SampleActiveFiltersTreeItem& main, const char* name, const IItem* parent );
	~Implementation();

	SampleActiveFiltersTreeItem& main_;
	const char* name_;
	const IItem* parent_;
};

SampleActiveFiltersTreeItem::Implementation::Implementation(
	SampleActiveFiltersTreeItem& main, const char* name, const IItem* parent )
	: main_( main )
	, name_( name )
	, parent_( parent )
{
}

SampleActiveFiltersTreeItem::Implementation::~Implementation()
{
	delete name_;
}


SampleActiveFiltersTreeItem::SampleActiveFiltersTreeItem( const char* name, const IItem* parent )
	: impl_( new Implementation( *this, name, parent ) )
{
}

SampleActiveFiltersTreeItem::SampleActiveFiltersTreeItem( const SampleActiveFiltersTreeItem& rhs )
	: impl_( new Implementation( *this, rhs.impl_->name_, rhs.impl_->parent_ ) )
{
}

SampleActiveFiltersTreeItem::~SampleActiveFiltersTreeItem()
{
}

SampleActiveFiltersTreeItem& SampleActiveFiltersTreeItem::operator=( const SampleActiveFiltersTreeItem& rhs )
{
	if (this != &rhs)
	{
		impl_.reset( new Implementation(
			*this, rhs.impl_->name_, rhs.impl_->parent_ ) );
	}

	return *this;
}

const IItem* SampleActiveFiltersTreeItem::getParent() const
{
	return impl_->parent_;
}

const char* SampleActiveFiltersTreeItem::getDisplayText( int column ) const
{
	return impl_->name_;
}

ThumbnailData SampleActiveFiltersTreeItem::getThumbnail( int column ) const
{
	return nullptr;
}

Variant SampleActiveFiltersTreeItem::getData( int column, ItemRole::Id roleId ) const
{
	if (roleId == IndexPathRole::roleId_)
	{
		return impl_->name_;
	}

	return Variant();
}

bool SampleActiveFiltersTreeItem::setData( int column, ItemRole::Id roleId, const Variant& data )
{
	return false;
}

//------------------------------------------------------------------------------

struct StringList
{
	StringList()
	{
		position = 0;
		listOfData = "Animations animations_01 animations_02 animations_03 animation_04 animations_05 ";
		listOfData += "Models models_01 models_02 models_03 models_04 models_05 ";
		listOfData += "Objects objects_01 objects_02 objects_03 objects_04 objects_05 ";
		listOfData += "Skins skins_01 skins_02 skins_03 skins_04 skins_05 ";
		listOfData += "Terrain terrain_01 terrain_02 terrain_03 terrain_04 terrain_05 ";
	}

	std::string next()
	{
		size_t nextPosition = listOfData.find( ' ', position );
		size_t count = nextPosition == std::string::npos ?
			std::string::npos : nextPosition - position;
		std::string temp = listOfData.substr( position, count );
		position = nextPosition == std::string::npos ? 0 : nextPosition + 1;
		return temp;
	}

	std::string listOfData;
	size_t position;
};

struct SampleActiveFiltersTreeModel::Implementation
{
	Implementation( SampleActiveFiltersTreeModel& main );
	~Implementation();

	std::vector<SampleActiveFiltersTreeItem*> getSection( const SampleActiveFiltersTreeItem* parent );
	char* copyString( const std::string& s ) const;
	void generateData( const SampleActiveFiltersTreeItem* parent, size_t level );

	SampleActiveFiltersTreeModel& main_;
	std::unordered_map<const SampleActiveFiltersTreeItem*, std::vector<SampleActiveFiltersTreeItem*>> data_;
	StringList dataSource_;

	static const size_t NUMBER_OF_GROUPS = 5;
	static const size_t NUMBER_OF_LEVELS = 1;
};

SampleActiveFiltersTreeModel::Implementation::Implementation( SampleActiveFiltersTreeModel& main )
	: main_( main )
{
	generateData( nullptr, 0 );
}

SampleActiveFiltersTreeModel::Implementation::~Implementation()
{
	for (auto itr = data_.begin(); itr != data_.end(); ++itr)
	{
		auto items = itr->second;
		size_t max = items.size();

		for (size_t i = 0; i < max; ++i)
		{
			delete items[i];
		}
	}

	data_.clear();
}

std::vector<SampleActiveFiltersTreeItem*> SampleActiveFiltersTreeModel::Implementation::getSection(
	const SampleActiveFiltersTreeItem* parent )
{
	auto itr = data_.find( parent );
	assert( itr != data_.end() );
	return itr->second;
}

char* SampleActiveFiltersTreeModel::Implementation::copyString(
	const std::string& s ) const
{
	char* itemData = new char[s.length() + 1];
	memcpy( itemData, s.data(), s.length() );
	itemData[s.length()] = 0;
	return itemData;
}

void SampleActiveFiltersTreeModel::Implementation::generateData(
	const SampleActiveFiltersTreeItem* parent, size_t level )
{
	for (size_t i = 0; i < NUMBER_OF_GROUPS; ++i)
	{
		std::string dataString = dataSource_.next();
		SampleActiveFiltersTreeItem* item = new SampleActiveFiltersTreeItem(
			copyString( dataString ), parent );
		data_[parent].push_back( item );

		if (level < NUMBER_OF_LEVELS)
		{
			generateData( item, level + 1 );
		}

		data_[item];
	}
}


SampleActiveFiltersTreeModel::SampleActiveFiltersTreeModel()
	: impl_( new Implementation( *this ) )
{
}

SampleActiveFiltersTreeModel::SampleActiveFiltersTreeModel( const SampleActiveFiltersTreeModel& rhs )
	: impl_( new Implementation( *this ) )
{
}

SampleActiveFiltersTreeModel::~SampleActiveFiltersTreeModel()
{
}

SampleActiveFiltersTreeModel& SampleActiveFiltersTreeModel::operator=( const SampleActiveFiltersTreeModel& rhs )
{
	if (this != &rhs)
	{
		impl_.reset( new Implementation( *this ) );
	}

	return *this;
}

IItem* SampleActiveFiltersTreeModel::item( size_t index, const IItem* parent ) const
{
	auto temp = static_cast<const SampleActiveFiltersTreeItem*>( parent );
	return impl_->getSection( temp )[index];
}

ITreeModel::ItemIndex SampleActiveFiltersTreeModel::index( const IItem* item ) const
{
	auto temp = static_cast<const SampleActiveFiltersTreeItem*>( item );
	temp = static_cast<const SampleActiveFiltersTreeItem*>( temp->getParent() );
	ItemIndex index( 0, temp );

	auto items = impl_->getSection( temp );
	auto itr = std::find( items.begin(), items.end(), item );
	assert( itr != items.end() );

	index.first = itr - items.begin();
	return index;
}


bool SampleActiveFiltersTreeModel::empty( const IItem* parent ) const
{
	const auto temp = static_cast< const SampleActiveFiltersTreeItem* >( parent );
	return impl_->getSection( temp ).empty();
}


size_t SampleActiveFiltersTreeModel::size( const IItem* parent ) const
{
	auto temp = static_cast<const SampleActiveFiltersTreeItem*>( parent );
	return impl_->getSection( temp ).size();
}

int SampleActiveFiltersTreeModel::columnCount() const
{
	return 1;
}
} // end namespace wgt
