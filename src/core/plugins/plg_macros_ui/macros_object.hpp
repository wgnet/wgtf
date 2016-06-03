#ifndef MACROS_OBJECT_HPP
#define MACROS_OBJECT_HPP

#include "core_reflection/object_handle.hpp"

namespace wgt
{
class ICommandManager;
class IListModel;

/**
 *	Wrapper for accessing the current position in the undo/redo list from QML.
 */
class MacrosObject
{
public:
	// ClassDefinition::create() requires a default constructor
	MacrosObject();
	void init( ICommandManager& commandSystem );

	const IListModel * getMacros() const;
	ObjectHandle getSelectedCompoundCommand() const;
	void setSelectedRow( const int index );

private:
	
	ICommandManager* commandSystem_;
	int currentIndex_;
};
} // end namespace wgt
#endif // MACROS_OBJECT_HPP
