#ifndef UNDO_REDO_DATA_HPP
#define UNDO_REDO_DATA_HPP

#include <memory>

namespace wgt
{
template <typename T> class ManagedObject;
typedef ManagedObject<class GenericObject> CommandDescription;

/**
 *	Interface for undo/redo data that is used by Commands.
 */
class UndoRedoData
{
public:
	virtual ~UndoRedoData(){};

	virtual bool undo() = 0;
	virtual bool redo() = 0;

	/**
	 *	Get a description of the undo/redo data for display.
	 *	@return a reflected object containing properties, such as
	 *		"Id", "Name", "Type", "PreValue", "PostValue" and "Children".
	 */
	virtual CommandDescription getCommandDescription() const = 0;
};
} // end namespace wgt
#endif // UNDO_REDO_DATA_HPP
