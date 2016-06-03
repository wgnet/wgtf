#pragma once

#include "core_reflection/object_handle.hpp"
#include "core_reflection/reflected_object.hpp"

#include <memory>
#include <string>

namespace wgt
{
class IComponentContext;
class ITreeModel;


/**
 *	Stores variables that can be accessed by QML.
 */ 
class PanelContext
{
public:
	DECLARE_REFLECTED
	PanelContext();
	~PanelContext();

	const std::string & panelName() const;
	ITreeModel * treeModel() const;
	void updateValues();
	
	IComponentContext * pContext_;
	std::string panelName_;
	ObjectHandle pythonObject_;
	std::unique_ptr< ITreeModel > treeModel_;
};
} // end namespace wgt
