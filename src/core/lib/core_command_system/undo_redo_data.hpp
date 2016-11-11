#ifndef UNDO_REDO_DATA_HPP
#define UNDO_REDO_DATA_HPP

namespace wgt
{
class ObjectHandle;

/**
 *	Interface for undo/redo data that is used by Commands.
 */
class UndoRedoData
{
public:
	virtual ~UndoRedoData(){};

	virtual void undo() = 0;
	virtual void redo() = 0;

	/**
	 *	Get a description of the undo/redo data for display.
	 *	@return a reflected object containing properties, such as
	 *		"Id", "Name", "Type", "PreValue", "PostValue" and "Children".
	 */
	virtual ObjectHandle getCommandDescription() const = 0;
};
} // end namespace wgt
#endif // UNDO_REDO_DATA_HPP
