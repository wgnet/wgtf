#ifndef HISTORY_SELECTION_HANDLER_HPP
#define HISTORY_SELECTION_HANDLER_HPP

#include <vector>
namespace wgt
{
class Variant;
class Collection;

class HistorySelectionHandler
{
public:
	typedef std::vector<unsigned int> Selection;
public:
	HistorySelectionHandler();
	~HistorySelectionHandler();

	void setSelection( const Selection& selectionCollection );
	const Selection & getSelection() const;

private:
	Selection selectionSet_;
};
} // end namespace wgt
#endif //HISTORY_SELECTION_HANDLER_HPP
