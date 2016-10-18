#ifndef FILTERED_LIST_MODEL_HPP
#define FILTERED_LIST_MODEL_HPP

#include "i_list_model.hpp"
#include "core_data_model/filtering/i_item_filter.hpp"

#include <functional>
#include <memory>

namespace wgt
{
class FilteredListModel : public IListModel
{
	typedef Signal< void( void ) > SignalVoid;

public:
	FilteredListModel();
	FilteredListModel( const FilteredListModel & rhs );
	virtual ~FilteredListModel();

	FilteredListModel & operator=( const FilteredListModel & rhs );

	virtual IItem * item( size_t index ) const override;
	virtual size_t index( const IItem * item ) const override;
	virtual bool empty() const override;
	virtual size_t size() const override;
	virtual int columnCount() const override;

	virtual Variant getData( int column, ItemRole::Id roleId ) const override;
	virtual bool setData( int column, ItemRole::Id roleId, const Variant & data ) override;

	void setSource( IListModel * source );
	void setFilter( IItemFilter * filter );

	IListModel * getSource();
	const IListModel * getSource() const;

	void refresh( bool waitToFinish = false );

	bool isFiltering() const;

	SignalVoid onFilteringBegin;
	SignalVoid onFilteringEnd;

private:
	struct Implementation;
	std::unique_ptr< Implementation > impl_;
};
} // end namespace wgt
#endif // FILTERED_LIST_MODEL_HPP
