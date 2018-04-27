#pragma once

#include "core_reflection/object_handle.hpp"
#include "core_reflection/reflected_object.hpp"
#include "core_dependency_system/depends.hpp"

#include <memory>
#include <string>

namespace wgt
{
class AbstractTreeModel;
class IDefinitionManager;

/**
 *	Stores variables that can be accessed by QML.
 */
class PanelContext : public Depends<IDefinitionManager>
{
public:
	DECLARE_REFLECTED
	PanelContext();

	bool initialize(const char* panelName, const ObjectHandle& pythonObject);
	const std::string& panelName() const;
	AbstractTreeModel* treeModel() const;
	void updateValues();
	void undoUpdateValues(Variant, Variant);
	void redoUpdateValues(Variant, Variant);
	const PanelContext* getSource() const;

private:
	std::string panelName_;
	std::string testScriptDescription_;
	ObjectHandle pythonObject_;
	std::shared_ptr<AbstractTreeModel> treeModel_;
};
} // end namespace wgt
