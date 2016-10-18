#ifndef ACTIVE_FILTERS_TEST_VIEW_MODEL_HPP
#define ACTIVE_FILTERS_TEST_VIEW_MODEL_HPP

#include "core_reflection/reflected_object.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_data_model/i_tree_model.hpp"
#include "core_data_model/i_item.hpp"
#include <memory>
#include "core_data_model/abstract_item_model.hpp"

namespace wgt
{
class IActiveFiltersModel;
class IDefinitionManager;
class IUIFramework;

//------------------------------------------------------------------------------

class ActiveFiltersTestViewModel
{
	DECLARE_REFLECTED
	
public:
	ActiveFiltersTestViewModel();
	~ActiveFiltersTestViewModel();
	
	void init( IDefinitionManager & defManager, IUIFramework & uiFramework );

	IActiveFiltersModel * getSimpleActiveFiltersModel() const;
	ITreeModel * getSampleDataToFilterOld() const;
	AbstractTreeModel * getSampleDataToFilterNew() const;

private:
	struct Implementation;
	Implementation* impl_;
};

//------------------------------------------------------------------------------

class SampleActiveFiltersTreeItem: public IItem
{
public:
	SampleActiveFiltersTreeItem( const char* name, const IItem* parent );
	SampleActiveFiltersTreeItem( const SampleActiveFiltersTreeItem& rhs );
	virtual ~SampleActiveFiltersTreeItem();

	SampleActiveFiltersTreeItem& operator=( const SampleActiveFiltersTreeItem& rhs );

	const IItem* getParent() const;
	virtual const char* getDisplayText( int column ) const;
	virtual ThumbnailData getThumbnail( int column ) const;
	virtual Variant getData( int column, ItemRole::Id roleId ) const;
	virtual bool setData( int column, ItemRole::Id roleId, const Variant& data );

private:
	struct Implementation;
	std::unique_ptr<Implementation> impl_;
};

//------------------------------------------------------------------------------

class SampleActiveFiltersTreeModel: public ITreeModel
{
public:
	SampleActiveFiltersTreeModel();
	SampleActiveFiltersTreeModel( const SampleActiveFiltersTreeModel& rhs );
	virtual ~SampleActiveFiltersTreeModel();

	SampleActiveFiltersTreeModel& operator=( const SampleActiveFiltersTreeModel& rhs );

	virtual IItem* item( size_t index, const IItem* parent ) const override;
	virtual ItemIndex index( const IItem* item ) const override;
	virtual bool empty( const IItem* parent ) const override;
	virtual size_t size( const IItem* parent ) const override;
	virtual int columnCount() const override;

private:
	struct Implementation;
	std::unique_ptr<Implementation> impl_;
};
} // end namespace wgt
#endif // ACTIVE_FILTERS_TEST_VIEW_MODEL_HPP
