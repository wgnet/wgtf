#ifndef UNDO_REDO_DATA_HPP
#define UNDO_REDO_DATA_HPP

namespace wgt
{
class UndoRedoData
{
public:
	virtual ~UndoRedoData() {};

	virtual void undo() = 0;
	virtual void redo() = 0;
};
} // end namespace wgt
#endif // UNDO_REDO_DATA_HPP
