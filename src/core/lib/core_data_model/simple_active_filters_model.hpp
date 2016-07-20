#ifndef SIMPLE_ACTIVE_FILTERS_MODEL_HPP
#define SIMPLE_ACTIVE_FILTERS_MODEL_HPP

#include "i_active_filters_model.hpp"

namespace wgt
{
class IDefinitionManager;
class IUIFramework;

//------------------------------------------------------------------------------
// SimpleActiveFiltersModel
//
// A very simple implementation of an active filters model that may be used
// with the active filters control (WGActiveFilters).
//------------------------------------------------------------------------------

class SimpleActiveFiltersModel : public IActiveFiltersModel
{
public:

	//
	// IActiveFiltersModel Implementation
	//

	SimpleActiveFiltersModel( const char* id, IDefinitionManager & definitionManager, IUIFramework & uiFramework );

	virtual ~SimpleActiveFiltersModel() override;
	
	//
	// IActiveFiltersModel Implementation
	//

	virtual IListModel * getCurrentFilterTerms() const override;
		
	virtual void removeFilterTerm( int index ) override;

	virtual void clearCurrentFilter() override;

	virtual void addFilterTerm( std::string display, std::string value, bool active ) override;

	virtual IListModel * getSavedFilters() const override;

	virtual std::string saveFilter( bool overwrite ) override;

	virtual bool loadFilter( std::string filterId ) override;

	virtual void clearSavedFilters() override;

private:

	//
	// Private Implementation Principle
	//

	struct Impl;
	std::unique_ptr<Impl> impl_;
};
} // end namespace wgt
#endif //SIMPLE_ACTIVE_FILTERS_MODEL_HPP
