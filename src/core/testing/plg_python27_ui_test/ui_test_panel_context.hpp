#pragma once

#include "core_reflection/object_handle.hpp"
#include "core_reflection/reflected_object.hpp"

#include <memory>
#include <string>

namespace wgt
{
class IComponentContext;
class AbstractTreeModel;

/**
 *	Stores variables that can be accessed by QML.
 */ 
class PanelContext
{
public:
	DECLARE_REFLECTED
	PanelContext();

	bool initialize(IComponentContext& context,
	                const char* panelName,
	                const ObjectHandle& pythonObject);
	const std::string & panelName() const;
	AbstractTreeModel* treeModel() const;
	void updateValues();
	void undoUpdateValues(const ObjectHandle&, Variant);
	void redoUpdateValues(const ObjectHandle&, Variant);
	const PanelContext* getSource() const;

private:
	IComponentContext* context_;
	std::string panelName_;
	std::string testScriptDescription_;
	ObjectHandle pythonObject_;
	std::shared_ptr<AbstractTreeModel> treeModel_;
};
} // end namespace wgt
