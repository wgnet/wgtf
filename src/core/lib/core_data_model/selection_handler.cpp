#include "selection_handler.hpp"

namespace wgt
{
//==============================================================================
SelectionHandler::SelectionHandler()
{
}

//==============================================================================
SelectionHandler::~SelectionHandler()
{
}

//==============================================================================
void SelectionHandler::setSelectedItems(const std::vector<IItem*>& selectionCollection)
{
	selectedItems_ = selectionCollection;
}

//==============================================================================
const std::vector<IItem*>& SelectionHandler::getSelectedItems() const
{
	return selectedItems_;
}

//==============================================================================
void SelectionHandler::setSelectedRows(const std::vector<int>& selectionCollection)
{
	selectedRows_ = selectionCollection;
}

//==============================================================================
const std::vector<int>& SelectionHandler::getSelectedRows() const
{
	return selectedRows_;
}
} // end namespace wgt
