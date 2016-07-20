#ifndef CUSTOM_UNDO_REDO_DATA_HPP
#define CUSTOM_UNDO_REDO_DATA_HPP

#include "undo_redo_data.hpp"

namespace wgt
{
class CommandInstance;

class CustomUndoRedoData : public UndoRedoData
{
public:
	CustomUndoRedoData( CommandInstance & commandInstance );

	void undo() override;
	void redo() override;
	virtual ObjectHandle getCommandDescription() const override;

private:
	CommandInstance & commandInstance_;
};
} // end namespace wgt
#endif // UNDO_REDO_DATA_HPP
