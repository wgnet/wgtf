#ifndef I_ACTIVE_FILTERS_MODEL_HPP
#define I_ACTIVE_FILTERS_MODEL_HPP

#include "core_reflection/reflected_object.hpp"
#include "core_reflection/object_handle.hpp"
#include <string>
#include "i_list_model.hpp"

namespace wgt
{
class IListModel;

//------------------------------------------------------------------------------
// ActiveFilterTerm
//
// Represents a single active filter term and its state.
//------------------------------------------------------------------------------

class ActiveFilterTerm
{
	DECLARE_REFLECTED

public:
	ActiveFilterTerm() : display_(""), value_(""), active_(true)
	{
	}
	virtual ~ActiveFilterTerm()
	{
	}

	virtual const std::string& getDisplay() const
	{
		return display_;
	}
	virtual void setDisplay(const std::string& display)
	{
		display_ = display;
	}

	virtual const std::string& getValue() const
	{
		return value_.empty() ? display_ : value_;
	}
	virtual void setValue(const std::string& value)
	{
		value_ = value;
	}

	virtual bool isActive() const
	{
		return active_;
	}
	virtual void setActive(const bool& active)
	{
		active_ = active;
	}

private:
	std::string display_;
	std::string value_;
	bool active_;
};

//------------------------------------------------------------------------------
// SavedFilter
//
// Represents a saved filter's properties as they would be represented in QML.
//------------------------------------------------------------------------------
class SavedActiveFilter
{
	DECLARE_REFLECTED

public:
	SavedActiveFilter()
	{
	}
	virtual ~SavedActiveFilter()
	{
	}

	virtual const std::string& getFilterId() const
	{
		return filterId_;
	}
	virtual void setFilterId(const std::string& value)
	{
		filterId_ = value;
	}

	virtual const std::string& getTerms() const
	{
		return rawTerms_;
	}
	virtual void setTerms(const std::string& rawTerms)
	{
		rawTerms_ = rawTerms;
	}

private:
	std::string filterId_;
	std::string rawTerms_;
};

//------------------------------------------------------------------------------
// IActiveFiltersModel
//
// Represents the data model for the WGActiveFilters control.
//------------------------------------------------------------------------------

class IActiveFiltersModel
{
	DECLARE_REFLECTED

public:
	//-------------------------------------
	// Lifecycle
	//-------------------------------------

	IActiveFiltersModel()
	{
		// Just a temporary implementation until type definition registration
		// allows abstract classes.
	}

	virtual ~IActiveFiltersModel()
	{
	}

	//-------------------------------------
	// Data Model Accessors
	//-------------------------------------

	// Returns the active filter terms
	// Expected: IListModel of ActiveFilterTerm objects
	virtual IListModel* getCurrentFilterTerms() const
	{
		return nullptr;
	}

	// Removes a filter term from the current list
	virtual void removeFilterTerm(int index)
	{
	}

	// Clears the current filter terms
	virtual void clearCurrentFilter()
	{
	}

	// Adds a filter term to the list
	virtual void addFilterTerm(std::string display, std::string value, bool active)
	{
	}

	// Returns saved active filters names
	// Expected: IListModel of saved active filter IDs as saved by the preferences system
	virtual IListModel* getSavedFilters() const
	{
		return nullptr;
	}

	// Saves the current filter terms to the preferences system as a new saved filter entry
	virtual std::string saveFilter(bool overwrite)
	{
		return std::string("");
	}

	// Loads the specified filter by its known ID in the preferences system
	virtual bool loadFilter(std::string filterId)
	{
		return false;
	}

	// Clears out saved filters from the preferences system
	virtual void clearSavedFilters()
	{
	}
};
} // end namespace wgt
#endif // I_ACTIVE_FILTERS_MODEL_HPP
