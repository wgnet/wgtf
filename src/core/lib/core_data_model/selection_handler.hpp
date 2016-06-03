#ifndef HISTORY_SELECTION_HANDLER_HPP
#define HISTORY_SELECTION_HANDLER_HPP

#include "i_selection_handler.hpp"

// TODO: NGT-849
// Eventually, we need to remove this class
namespace wgt
{
class SelectionHandler
	: public ISelectionHandler
{
public:
    SelectionHandler();
    ~SelectionHandler();

	
	void setSelectedRows( const std::vector< int > & selectionCollection ) override;
	const std::vector< int > & getSelectedRows() const override;
	void setSelectedItems( const std::vector< IItem* > & selectionCollection ) override;
	const std::vector< IItem* > & getSelectedItems() const override;

private:
	std::vector< int > selectedRows_;
	std::vector< IItem* > selectedItems_;
};
} // end namespace wgt
#endif //HISTORY_SELECTION_HANDLER_HPP
