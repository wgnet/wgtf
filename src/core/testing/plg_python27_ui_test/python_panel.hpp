#pragma once

#include "core_ui_framework/i_view.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_dependency_system/depends.hpp"


#include <memory>


namespace wgt
{
class IUIFramework;
class IUIApplication;
class IViewCreator;

#define DEPENDS_ON_CLASSES \
	IUIFramework, \
	IUIApplication,\
	IViewCreator


/**
 *	Panel for displaying a Python object as a tree.
 */
class PythonPanel: Depends<DEPENDS_ON_CLASSES>
{
public:
	PythonPanel( IComponentContext & context,
		ObjectHandle & contextObject );
	~PythonPanel();

private:
	/// Create the context object to provide access to the Python references from QML.
	/// Also registers definitions of reflected objects with the reflection system.
	/// @return true if successful, false if not.
	bool createContextObject( const char * panelName,
		ObjectHandle & pythonObject );

	/// Create the panel and add it to the window. (The window exists in a different plugin)
	/// @return true if successful, false if not.
	bool addPanel();


	/// Remove the panel from the window.
	void removePanel();


	IComponentContext& context_;
	std::unique_ptr<IView> pythonView_;
	ObjectHandle contextObject_;
};
} // end namespace wgt
