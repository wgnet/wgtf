#pragma once

#include "core_reflection/reflected_object.hpp"
#include "core_dependency_system/depends.hpp"
#include "core_data_model/abstract_item_model.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_object/object_handle_provider.hpp"
#include <memory>

namespace wgt
{
class Variant;
class IDefinitionManager;
class IEnvManager;

/**
 *	Stores variables that can be accessed by QML.
 */
class TestUIContext : Depends<IDefinitionManager, IEnvManager>
                    , public ObjectHandleProvider<TestUIContext>
{
public:
	DECLARE_REFLECTED
	TestUIContext();
	~TestUIContext();

	void initialise(ObjectHandle model);
	void open(std::string id, ObjectHandle model);
	void close(std::string id);
	void select(std::string id);

	AbstractTreeModel* getTreeModel();
	void reloadEnvironment();
	void toggleReadOnly();
	void updateValues();
	void undoUpdateValues(Variant, Variant);
	void redoUpdateValues(Variant, Variant);

private:
	const IClassDefinition* definition_;
	ObjectHandle selectedModel_;
	std::shared_ptr<AbstractTreeModel> selectedTree_;
	std::map<std::string, ObjectHandle> models_;
	std::string selectedId_;
	bool treeVisible_ = false;
};
} // end namespace wgt
