#ifndef I_SELECTION_HANDLER_HPP
#define I_SELECTION_HANDLER_HPP

#include <vector>
#include "core_common/signal.hpp"
namespace wgt
{
class IItem;

// TODO: NGT-849
// Eventually, we need to remove this class

class ISelectionHandler
{
	typedef Signal<void(void)> SignalVoid;

public:
	virtual ~ISelectionHandler()
	{
	}

	virtual void setSelectedItems(const std::vector<IItem*>& selectionCollection) = 0;
	virtual const std::vector<IItem*>& getSelectedItems() const = 0;
	virtual void setSelectedRows(const std::vector<int>& selectionCollection) = 0;
	virtual const std::vector<int>& getSelectedRows() const = 0;

	SignalVoid signalPreSelectionChanged;
	SignalVoid signalPostSelectionChanged;
};
} // end namespace wgt
#endif // I_SELECTION_HANDLER_HPP
