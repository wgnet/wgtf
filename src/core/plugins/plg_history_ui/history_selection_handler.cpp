#include "history_selection_handler.hpp"
#include "core_variant/variant.hpp"
#include "core_variant/collection.hpp"


namespace wgt
{
//==============================================================================
HistorySelectionHandler::HistorySelectionHandler()
{
}


//==============================================================================
HistorySelectionHandler::~HistorySelectionHandler()
{
}


//==============================================================================
void HistorySelectionHandler::setSelection( const HistorySelectionHandler::Selection & selectionCollection )
{
	selectionSet_ = selectionCollection;
}


//==============================================================================
const HistorySelectionHandler::Selection & HistorySelectionHandler::getSelection() const
{
	return selectionSet_;
}
} // end namespace wgt
